/*******************************************************************************
* Copyright (c) 2018, 2019 in-tech GmbH
*               2017 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  ObservationFileHandler.cpp */
//-----------------------------------------------------------------------------

#include <sstream>

#include <QDir>
#include <QFile>
#include <QString>
#include <QTemporaryFile>

#include "common/openPassTypes.h"
#include "common/openPassUtils.h"
#include "common/sensorDefinitions.h"
#include "include/dataStoreInterface.h"
#include "include/worldInterface.h"
#include "observationFileHandler.h"

ObservationFileHandler::ObservationFileHandler(const DataStoreReadInterface& dataStore) :
    dataStore{dataStore}
{
}

void ObservationFileHandler::WriteStartOfFile(const std::string& frameworkVersion)
{
    runNumber = 0;

    // setup environment
    QDir dir(folder);
    if (!dir.exists() && !dir.mkpath(folder))
    {
        std::stringstream ss;
        ss << COMPONENTNAME << " could not create folder: " << folder.toStdString();
        //        LOG(CbkLogLevel::Error, ss.str());
        throw std::runtime_error(ss.str());
    }

    if (QFile::exists(finalPath))
    {
        QFile::remove(finalPath);
    }

    RemoveCsvCyclics(folder);

    xmlFile = std::make_unique<QTemporaryFile>(folder + "/simulationOutput_XXXXXX.tmp");
    xmlFile->setAutoRemove(false);
    xmlFile->fileName();   // required for setAutoRemove to be applied, see https://doc.qt.io/qt-5/qtemporaryfile.html#setAutoRemove

    if (!xmlFile->open())
    {
        std::stringstream ss;
        ss << COMPONENTNAME << ": could not create file: " << xmlFile->fileName().toStdString();
        throw std::runtime_error(ss.str());
    }

    xmlFileStream = std::make_unique<QXmlStreamWriter>(xmlFile.get());
    xmlFileStream->setAutoFormatting(true);
    xmlFileStream->writeStartDocument();
    xmlFileStream->writeStartElement(outputTags.SIMULATIONOUTPUT);
    xmlFileStream->writeAttribute(outputAttributes.FRAMEWORKVERSION, QString::fromStdString(frameworkVersion));
    xmlFileStream->writeAttribute(outputAttributes.SCHEMAVERSION, outputFileVersion);
    xmlFileStream->writeStartElement(outputTags.SCENERYFILE);
    xmlFileStream->writeCharacters(QString::fromStdString(sceneryFile));
    xmlFileStream->writeEndElement();
    xmlFileStream->writeStartElement(outputTags.RUNRESULTS);
}

void ObservationFileHandler::WriteRun([[maybe_unused]] const RunResultInterface& runResult, RunStatistic runStatistic, ObservationCyclics& cyclics)
{
    std::stringstream ss;
    ss << COMPONENTNAME << " append log to file: " << tmpPath.toStdString();
    //    LOG(CbkLogLevel::Debug, ss.str());

    // init new run result
    xmlFileStream->writeStartElement(outputTags.RUNRESULT);
    xmlFileStream->writeAttribute(outputAttributes.RUNID, QString::number(runNumber));

    // write RunStatisticsTag
    xmlFileStream->writeStartElement(outputTags.RUNSTATISTICS);

    const auto agentIds = dataStore.GetKeys("Statics/Agents");

    for (const auto& agentId : agentIds)
    {
        const auto tdtResult = dataStore.GetCyclic(std::nullopt, std::stoi(agentId), "TotalDistanceTraveled");
        const auto last_tdt_row = (*((*tdtResult).end() - 1)).get();

        if (last_tdt_row.entityId == 0)
        {
            runStatistic.EgoDistanceTraveled = std::get<double>(last_tdt_row.value);
        }

        runStatistic.TotalDistanceTraveled += std::get<double>(last_tdt_row.value);
    }

    runStatistic.WriteStatistics(xmlFileStream.get());

    // close RunStatisticsTag
    xmlFileStream->writeEndElement();

    AddEvents();
    AddAgents();

    // write CyclicsTag
    xmlFileStream->writeStartElement(outputTags.CYCLICS);

    QString runPrefix = "";
    if (runNumber < 10)
    {
        runPrefix = "00";
    }
    else if (runNumber < 100)
    {
        runPrefix = "0";
    }

    if (writeCyclicsToCsv)
    {
        QString csvFilename = "Cyclics_Run_" + runPrefix + QString::number(runNumber) + ".csv";

        AddReference(csvFilename);

        WriteCsvCyclics(csvFilename, cyclics);
    }
    else
    {
        AddHeader(cyclics);
        AddSamples(cyclics);
    }

    // close CyclicsTag
    xmlFileStream->writeEndElement();

    // close RunResultTag
    xmlFileStream->writeEndElement();

    ++runNumber;
}

void ObservationFileHandler::WriteEndOfFile()
{
    // close RunResultsTag
    xmlFileStream->writeEndElement();

    // close SimulationOutputTag
    xmlFileStream->writeEndElement();

    xmlFileStream->writeEndDocument();
    xmlFile->flush();
    xmlFile->close();

    // finalize results
    xmlFile->rename(finalPath);
}

void ObservationFileHandler::AddEvents()
{
    xmlFileStream->writeStartElement(outputTags.EVENTS);

    const auto events = dataStore.GetAcyclic(std::nullopt, std::nullopt, "*");

    for (const AcyclicRow& event : *events)
    {
        // necessary, as events and entities are currently both handled by acyclics and we don't have filters yet
        if (event.key == "Entities") continue;

        xmlFileStream->writeStartElement(outputTags.EVENT);
        xmlFileStream->writeAttribute(outputAttributes.TIME, QString::number(event.timestamp));
        xmlFileStream->writeAttribute(outputAttributes.SOURCE, QString::fromStdString(event.key));
        xmlFileStream->writeAttribute(outputAttributes.NAME, QString::fromStdString(event.data.name));

        WriteEntities(outputTags.TRIGGERINGENTITIES, event.data.triggeringEntities.entities, true);
        WriteEntities(outputTags.AFFECTEDENTITIES, event.data.affectedEntities.entities, true);
        WriteParameter(event.data.parameter, true);

        xmlFileStream->writeEndElement(); // event
    }

    xmlFileStream->writeEndElement(); // events
}

void ObservationFileHandler::AddAgents()
{
    xmlFileStream->writeStartElement(outputTags.AGENTS);

    const auto agentIds = dataStore.GetKeys("Statics/Agents");

    for (const auto& agentId : agentIds)
    {
        AddAgent(agentId);
    }

    xmlFileStream->writeEndElement();
}

void ObservationFileHandler::AddAgent(const std::string& agentId)
{
    const std::string keyPrefix = "Agents/" + agentId + "/";

    xmlFileStream->writeStartElement(outputTags.AGENT);

    xmlFileStream->writeAttribute(outputAttributes.ID, QString::fromStdString(agentId));
    xmlFileStream->writeAttribute(outputAttributes.AGENTTYPEGROUPNAME, QString::fromStdString(std::get<std::string>(dataStore.GetStatic(keyPrefix + "AgentTypeGroupName").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.AGENTTYPENAME, QString::fromStdString(std::get<std::string>(dataStore.GetStatic(keyPrefix + "AgentTypeName").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.VEHICLEMODELTYPE, QString::fromStdString(std::get<std::string>(dataStore.GetStatic(keyPrefix + "VehicleModelType").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.DRIVERPROFILENAME, QString::fromStdString(std::get<std::string>(dataStore.GetStatic(keyPrefix + "DriverProfileName").at(0))));

    AddVehicleAttributes(agentId);
    AddSensors(agentId);

    xmlFileStream->writeEndElement();
}

void ObservationFileHandler::AddVehicleAttributes(const std::string& agentId)
{
    const std::string keyPrefix = "Agents/" + agentId + "/Vehicle/";

    xmlFileStream->writeStartElement(outputTags.VEHICLEATTRIBUTES);

    xmlFileStream->writeAttribute(outputAttributes.WIDTH, QString::number(std::get<double>(dataStore.GetStatic(keyPrefix + "Width").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.LENGTH, QString::number(std::get<double>(dataStore.GetStatic(keyPrefix + "Length").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.HEIGHT, QString::number(std::get<double>(dataStore.GetStatic(keyPrefix + "Height").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.LONGITUDINALPIVOTOFFSET, QString::number(std::get<double>(dataStore.GetStatic(keyPrefix + "LongitudinalPivotOffset").at(0))));

    xmlFileStream->writeEndElement();
}

void ObservationFileHandler::AddSensors(const std::string& agentId)
{
    const std::string keyPrefix = "Statics/Agents/" + agentId + "/Vehicle/Sensors";
    const auto& sensorIds = dataStore.GetKeys(keyPrefix);

    if (sensorIds.empty())
    {
        return;
    }

    xmlFileStream->writeStartElement(outputTags.SENSORS);

    for (const auto& sensorId : sensorIds)
    {
        AddSensor(agentId, sensorId);
    }

    xmlFileStream->writeEndElement();
}

bool ObservationFileHandler::ContainsSensor(const openpass::sensors::Parameters& sensorParameters) const
{
    return !sensorParameters.empty();
}

void ObservationFileHandler::AddSensor(const std::string& agentId, const::std::string& sensorId)
{
    const std::string sensorKeyPrefix = "Agents/" + agentId + "/Vehicle/Sensors/" + sensorId + "/";
    const std::string mountingKeyPrefix = sensorKeyPrefix + "Mounting/";

    xmlFileStream->writeStartElement(outputTags.SENSOR);

    xmlFileStream->writeAttribute(outputAttributes.ID, QString::fromStdString(sensorId));
    xmlFileStream->writeAttribute(outputAttributes.TYPE, QString::fromStdString(std::get<std::string>(dataStore.GetStatic(sensorKeyPrefix + "Type").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.MOUNTINGPOSITIONLONGITUDINAL, QString::number(std::get<double>(dataStore.GetStatic(mountingKeyPrefix + "Position/Longitudinal").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.MOUNTINGPOSITIONLATERAL, QString::number(std::get<double>(dataStore.GetStatic(mountingKeyPrefix + "Position/Lateral").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.MOUNTINGPOSITIONHEIGHT, QString::number(std::get<double>(dataStore.GetStatic(mountingKeyPrefix + "Position/Height").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.ORIENTATIONYAW, QString::number(std::get<double>(dataStore.GetStatic(mountingKeyPrefix + "Orientation/Yaw").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.ORIENTATIONPITCH, QString::number(std::get<double>(dataStore.GetStatic(mountingKeyPrefix + "Orientation/Pitch").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.ORIENTATIONROLL, QString::number(std::get<double>(dataStore.GetStatic(mountingKeyPrefix + "Orientation/Roll").at(0))));

    const auto parameters = dataStore.GetKeys("Statics/" + sensorKeyPrefix + "Parameters");
    for (const auto& parameter : parameters)
    {
        xmlFileStream->writeAttribute(QString::fromStdString(parameter), QString::number(std::get<double>(dataStore.GetStatic(sensorKeyPrefix + "Parameters/" + parameter).at(0))));
    }

    xmlFileStream->writeEndElement();
}

void ObservationFileHandler::AddHeader(ObservationCyclics& cyclics)
{
    xmlFileStream->writeStartElement(outputTags.HEADER);
    xmlFileStream->writeCharacters(QString::fromStdString(cyclics.GetHeader()));
    xmlFileStream->writeEndElement();
}

void ObservationFileHandler::AddSamples(ObservationCyclics& cyclics)
{
    // write SamplesTag
    xmlFileStream->writeStartElement(outputTags.SAMPLES);
    const auto& timeSteps = cyclics.GetTimeSteps();

    unsigned int timeStepNumber = 0;
    for (const auto timeStep : timeSteps)
    {
        xmlFileStream->writeStartElement(outputTags.SAMPLE);
        xmlFileStream->writeAttribute(outputAttributes.TIME, QString::number(timeStep));
        xmlFileStream->writeCharacters(QString::fromStdString(cyclics.GetSamplesLine(timeStepNumber)));

        // close SampleTag
        xmlFileStream->writeEndElement();

        ++timeStepNumber;
    }

    // close SamplesTag
    xmlFileStream->writeEndElement();
}

void ObservationFileHandler::AddReference(QString filename)
{
    // write CyclicsFileTag
    xmlFileStream->writeStartElement(outputTags.CYCLICSFILE);

    xmlFileStream->writeCharacters(filename);

    // close CyclicsFileTag
    xmlFileStream->writeEndElement();
}

void ObservationFileHandler::RemoveCsvCyclics(QString directory)
{
    QDirIterator it(directory, QStringList() << "Cyclics_Run*.csv", QDir::Files, QDirIterator::NoIteratorFlags);
    while (it.hasNext())
    {
        it.next();
        QFileInfo fileInfo = it.fileInfo();
        if (fileInfo.baseName().startsWith("Cyclics_Run_") && fileInfo.suffix() == "csv")
        {
            QFile::remove(fileInfo.filePath());
        }
    }
}

void ObservationFileHandler::WriteCsvCyclics(QString filename, ObservationCyclics& cyclics)
{
    QString path = folder + QDir::separator() + filename;

    csvFile = std::make_unique<QFile>(path);
    if (!csvFile->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        std::stringstream ss;
        ss << COMPONENTNAME << " could not create file: " << tmpPath.toStdString();
        //        LOG(CbkLogLevel::Error, ss.str());
        throw std::runtime_error(ss.str());
    }

    QTextStream stream(csvFile.get());

    stream << "Timestep, " << QString::fromStdString(cyclics.GetHeader()) << '\n';

    const auto& timeSteps = cyclics.GetTimeSteps();
    unsigned int timeStepNumber = 0;
    for (const auto timeStep : timeSteps)
    {
        stream << QString::number(timeStep) << ", " << QString::fromStdString(cyclics.GetSamplesLine(timeStepNumber)) << '\n';
        ++timeStepNumber;
    }

    csvFile->flush();

    csvFile->close();
}

void ObservationFileHandler::WriteEntities(const QString tag, const openpass::type::EntityIds &entities, bool mandatory)
{
    if (!entities.empty())
    {
        xmlFileStream->writeStartElement(tag);
        for (const auto &entity : entities)
        {
            xmlFileStream->writeStartElement(output::tag::ENTITY);
            xmlFileStream->writeAttribute(output::attribute::ID, QString::number(entity));
            xmlFileStream->writeEndElement();
        }
        xmlFileStream->writeEndElement();
    }
    else if (mandatory)
    {
        xmlFileStream->writeEmptyElement(tag);
    }
}

void ObservationFileHandler::WriteParameter(const openpass::type::FlatParameter &parameters, bool mandatory)
{
    constexpr auto tag = output::tag::PARAMETERS;
    if (!parameters.empty())
    {
        xmlFileStream->writeStartElement(tag);

        // No structured binding on purpose:
        // see https://stackoverflow.com/questions/46114214/lambda-implicit-capture-fails-with-variable-declared-from-structured-binding
        for (const auto &p : parameters)
        {
            auto parameterWriter = [&](const std::string &value) {
                xmlFileStream->writeStartElement(output::tag::PARAMETER);
                xmlFileStream->writeAttribute(output::attribute::KEY, QString::fromStdString(p.first));
                xmlFileStream->writeAttribute(output::attribute::VALUE, QString::fromStdString(value));
                xmlFileStream->writeEndElement();
            };

            std::visit(openpass::utils::FlatParameter::to_string(parameterWriter), p.second);
        }
        xmlFileStream->writeEndElement();
    }
    else if (mandatory)
    {
        xmlFileStream->writeEmptyElement(tag);
    }
}
