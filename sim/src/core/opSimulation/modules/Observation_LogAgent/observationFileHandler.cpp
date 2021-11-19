/********************************************************************************
 * Copyright (c) 2017-2020 ITK Engineering GmbH
 *               2018-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

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
#include "include/dataBufferInterface.h"
#include "include/worldInterface.h"
#include "observationFileHandler.h"

ObservationFileHandler::ObservationFileHandler(const DataBufferReadInterface& dataBuffer) :
    dataBuffer{dataBuffer}
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

void ObservationFileHandler::WriteRun([[maybe_unused]] const RunResultInterface& runResult, RunStatistic runStatistic, ObservationCyclics& cyclics, Events& events)
{
    std::stringstream ss;
    ss << COMPONENTNAME << " append log to file: " << tmpPath.toStdString();
    //    LOG(CbkLogLevel::Debug, ss.str());

    // init new run result
    xmlFileStream->writeStartElement(outputTags.RUNRESULT);
    xmlFileStream->writeAttribute(outputAttributes.RUNID, QString::number(runNumber));

    // write RunStatisticsTag
    xmlFileStream->writeStartElement(outputTags.RUNSTATISTICS);

    runStatistic.WriteStatistics(xmlFileStream.get());

    // close RunStatisticsTag
    xmlFileStream->writeEndElement();

    AddEvents(events);
    AddAgents();

    // write CyclicsTag
    xmlFileStream->writeStartElement(outputTags.CYCLICS);

    if (writeCyclicsToCsv)
    {
        QString runPrefix = "";
        if (runNumber < 10)
        {
            runPrefix = "00";
        }
        else if (runNumber < 100)
        {
            runPrefix = "0";
        }
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

void ObservationFileHandler::AddEvents(Events& events)
{
    xmlFileStream->writeStartElement(outputTags.EVENTS);

    for (const auto& event : events)
    {
        xmlFileStream->writeStartElement(outputTags.EVENT);
        xmlFileStream->writeAttribute(outputAttributes.TIME, QString::number(event.time));
        xmlFileStream->writeAttribute(outputAttributes.SOURCE, QString::fromStdString(event.dataRow.key));
        xmlFileStream->writeAttribute(outputAttributes.NAME, QString::fromStdString(event.dataRow.data.name));

        WriteEntities(outputTags.TRIGGERINGENTITIES, event.dataRow.data.triggeringEntities.entities, true);
        WriteEntities(outputTags.AFFECTEDENTITIES, event.dataRow.data.affectedEntities.entities, true);
        WriteParameter(event.dataRow.data.parameter, true);

        xmlFileStream->writeEndElement(); // event
    }

    xmlFileStream->writeEndElement(); // events
}

void ObservationFileHandler::AddAgents()
{
    xmlFileStream->writeStartElement(outputTags.AGENTS);

    const auto agentIds = dataBuffer.GetKeys("Statics/Agents");

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
    xmlFileStream->writeAttribute(outputAttributes.AGENTTYPEGROUPNAME, QString::fromStdString(std::get<std::string>(dataBuffer.GetStatic(keyPrefix + "AgentTypeGroupName").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.AGENTTYPENAME, QString::fromStdString(std::get<std::string>(dataBuffer.GetStatic(keyPrefix + "AgentTypeName").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.VEHICLEMODELTYPE, QString::fromStdString(std::get<std::string>(dataBuffer.GetStatic(keyPrefix + "VehicleModelType").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.DRIVERPROFILENAME, QString::fromStdString(std::get<std::string>(dataBuffer.GetStatic(keyPrefix + "DriverProfileName").at(0))));

    AddVehicleAttributes(agentId);
    AddSensors(agentId);

    xmlFileStream->writeEndElement();
}

void ObservationFileHandler::AddVehicleAttributes(const std::string& agentId)
{
    const std::string keyPrefix = "Agents/" + agentId + "/Vehicle/";

    xmlFileStream->writeStartElement(outputTags.VEHICLEATTRIBUTES);

    xmlFileStream->writeAttribute(outputAttributes.WIDTH, QString::number(std::get<double>(dataBuffer.GetStatic(keyPrefix + "Width").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.LENGTH, QString::number(std::get<double>(dataBuffer.GetStatic(keyPrefix + "Length").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.HEIGHT, QString::number(std::get<double>(dataBuffer.GetStatic(keyPrefix + "Height").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.LONGITUDINALPIVOTOFFSET, QString::number(std::get<double>(dataBuffer.GetStatic(keyPrefix + "LongitudinalPivotOffset").at(0))));

    xmlFileStream->writeEndElement();
}

void ObservationFileHandler::AddSensors(const std::string& agentId)
{
    const std::string keyPrefix = "Statics/Agents/" + agentId + "/Vehicle/Sensors";
    const auto& sensorIds = dataBuffer.GetKeys(keyPrefix);

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
    xmlFileStream->writeAttribute(outputAttributes.TYPE, QString::fromStdString(std::get<std::string>(dataBuffer.GetStatic(sensorKeyPrefix + "Type").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.MOUNTINGPOSITIONLONGITUDINAL, QString::number(std::get<double>(dataBuffer.GetStatic(mountingKeyPrefix + "Position/Longitudinal").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.MOUNTINGPOSITIONLATERAL, QString::number(std::get<double>(dataBuffer.GetStatic(mountingKeyPrefix + "Position/Lateral").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.MOUNTINGPOSITIONHEIGHT, QString::number(std::get<double>(dataBuffer.GetStatic(mountingKeyPrefix + "Position/Height").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.ORIENTATIONYAW, QString::number(std::get<double>(dataBuffer.GetStatic(mountingKeyPrefix + "Orientation/Yaw").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.ORIENTATIONPITCH, QString::number(std::get<double>(dataBuffer.GetStatic(mountingKeyPrefix + "Orientation/Pitch").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.ORIENTATIONROLL, QString::number(std::get<double>(dataBuffer.GetStatic(mountingKeyPrefix + "Orientation/Roll").at(0))));

    xmlFileStream->writeAttribute(outputAttributes.LATENCY, QString::number(std::get<double>(dataBuffer.GetStatic(sensorKeyPrefix + "Parameters/Latency").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.OPENINGANGLEH, QString::number(std::get<double>(dataBuffer.GetStatic(sensorKeyPrefix + "Parameters/OpeningAngleH").at(0))));
    xmlFileStream->writeAttribute(outputAttributes.DETECTIONRANGE, QString::number(std::get<double>(dataBuffer.GetStatic(sensorKeyPrefix + "Parameters/Range").at(0))));

    try
    {
        xmlFileStream->writeAttribute(outputAttributes.OPENINGANGLEV, QString::number(std::get<double>(dataBuffer.GetStatic(sensorKeyPrefix + "Parameters/OpeningAngleV").at(0))));
    }
    catch (const std::out_of_range&)
    {
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

    stream << "Timestep,"
           << "AgentId,"
           << QString::fromStdString(cyclics.GetAgentHeader())
           << '\n';

    const auto& timeSteps = cyclics.GetTimeSteps();
    std::vector<std::string> agentSamplesLines;
    //for (unsigned int timeStepNumber = 0; timeStepNumber < timeSteps.size(); ++timeStepNumber)
    unsigned int timeStepNumber = 0;
    for (const auto timeStep : timeSteps)
    {
        agentSamplesLines = cyclics.GetAgentSamplesLine(timeStepNumber);
        for (size_t agentId = 0; agentId < agentSamplesLines.size(); ++agentId)
        {
            stream << QString::number(timeStep)
                   << ","
                   << QString::fromStdString(std::to_string(agentId))
                   << ","
                   << QString::fromStdString(agentSamplesLines.at(agentId))
                   << '\n';
        }
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
