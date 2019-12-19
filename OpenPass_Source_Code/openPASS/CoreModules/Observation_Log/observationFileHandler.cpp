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
#include <QString>

#include "Interfaces/worldInterface.h"
#include "Common/sensorDefinitions.h"
#include "observationFileHandler.h"

void ObservationFileHandler::WriteStartOfFile(const std::string& frameworkVersion)
{
    runNumber = 0;

    // retrieve storage location

    tmpFilename = "simulationOutput.tmp";
    finalFilename = "simulationOutput.xml";

    tmpPath = folder + QDir::separator() + tmpFilename;
    finalPath = folder + QDir::separator() + finalFilename;

    std::stringstream ss;
    ss << COMPONENTNAME << " retrieved storage location: " << finalPath.toStdString();
    //    LOG(CbkLogLevel::Debug, ss.str());

    // setup environment
    QDir dir(folder);
    if (!dir.exists() && !dir.mkpath(folder))
    {
        std::stringstream ss;
        ss << COMPONENTNAME << " could not create folder: " << folder.toStdString();
        //        LOG(CbkLogLevel::Error, ss.str());
        throw std::runtime_error(ss.str());
    }

    if (QFile::exists(tmpPath))
    {
        QFile::remove(tmpPath);
    }

    if (QFile::exists(finalPath))
    {
        QFile::remove(finalPath);
    }

    RemoveCsvCyclics(folder);

    xmlFile = std::make_shared<QFile>(tmpPath);
    if (!xmlFile->open(QIODevice::WriteOnly))
    {
        std::stringstream ss;
        ss << COMPONENTNAME << " could not create file: " << tmpPath.toStdString();
        //        LOG(CbkLogLevel::Error, ss.str());
        throw std::runtime_error(ss.str());
    }

    xmlFileStream = std::make_shared<QXmlStreamWriter>(xmlFile.get());
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

void ObservationFileHandler::WriteRun(const RunResultInterface& runResult, RunStatistic runStatistic,
                                      ObservationCyclics& cyclics, WorldInterface* world, SimulationSlave::EventNetworkInterface* eventNetwork)
{
    Q_UNUSED(runResult);

    std::stringstream ss;
    ss << COMPONENTNAME << " append log to file: " << tmpPath.toStdString();
    //    LOG(CbkLogLevel::Debug, ss.str());

    // init new run result
    xmlFileStream->writeStartElement(outputTags.RUNRESULT);
    xmlFileStream->writeAttribute(outputAttributes.RUNID, QString::number(runNumber));

    // write RunStatisticsTag
    xmlFileStream->writeStartElement(outputTags.RUNSTATISTICS);

    runStatistic.WriteStatistics(xmlFileStream);

    // close RunStatisticsTag
    xmlFileStream->writeEndElement();

    AddEvents(xmlFileStream, eventNetwork);

    AddAgents(xmlFileStream, world);

    // write CyclicsTag
    xmlFileStream->writeStartElement(outputTags.CYCLICS);

    if(writeCyclicsToCsv)
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

        AddReference(xmlFileStream, csvFilename);

        WriteCsvCyclics(csvFilename, cyclics);
    }
    else
    {
        AddHeader(xmlFileStream, cyclics);

        AddSamples(xmlFileStream, cyclics);
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
    // using copy/remove instead of rename, since latter is failing on some systems
    if (xmlFile->copy(finalPath))
    {
        xmlFile->remove();
    }
}

void ObservationFileHandler::AddEventParameters(std::shared_ptr<QXmlStreamWriter> fStream,
        EventParameters eventParameters)
{
    for (auto parameter : eventParameters)
    {
        fStream->writeStartElement(outputTags.EVENTPARAMETER);
        fStream->writeAttribute(outputAttributes.KEY, QString::fromStdString(parameter.key));
        fStream->writeAttribute(outputAttributes.VALUE, QString::fromStdString(parameter.value));

        //Closes EventParameter tag
        fStream->writeEndElement();
    }
}

void ObservationFileHandler::AddEvent(std::shared_ptr<QXmlStreamWriter> fStream, std::shared_ptr<EventInterface> event)
{
    fStream->writeStartElement(outputTags.EVENT);
    fStream->writeAttribute(outputAttributes.ID, QString::number(event->GetId()));
    fStream->writeAttribute(outputAttributes.TIME, QString::number(event->GetEventTime()));
    fStream->writeAttribute(outputAttributes.SOURCE, QString::fromStdString(event->GetSource()));
    fStream->writeAttribute(outputAttributes.NAME, QString::fromStdString(event->GetName()));

    const int& triggeringEventId = event->GetTriggeringEventId();
    if (triggeringEventId >= 0)
    {
        fStream->writeAttribute(outputAttributes.TRIGGERINGEVENTID, QString::number(triggeringEventId));
    }

    AddEventParameters(fStream, event->GetParametersAsString());

    fStream->writeEndElement();
}

void ObservationFileHandler::AddEvents(std::shared_ptr<QXmlStreamWriter> fStream,
                                       SimulationSlave::EventNetworkInterface* eventNetwork)
{
    // write events
    fStream->writeStartElement(outputTags.EVENTS);

    for (const auto& eventList : * (eventNetwork->GetArchivedEvents()))
    {
        for (const auto& event : eventList.second)
        {
            AddEvent(fStream, event);
        }
    }

    for (const auto& eventList : * (eventNetwork->GetActiveEvents()))
    {
        for (const auto& event : eventList.second)
        {
            AddEvent(fStream, event);
        }
    }

    // close EventsTag
    fStream->writeEndElement();
}

void ObservationFileHandler::AddAgents(std::shared_ptr<QXmlStreamWriter> fStream, WorldInterface* world)
{
    //write Agents
    fStream->writeStartElement(outputTags.AGENTS);

    for (const auto& it : world->GetAgents())
    {
        const AgentInterface* agent = it.second;
        AddAgent(fStream, agent);
    }

    for (const auto& it : world->GetRemovedAgents())
    {
        const AgentInterface* agent = it;
        AddAgent(fStream, agent);
    }

    fStream->writeEndElement();
}

void ObservationFileHandler::AddAgent(std::shared_ptr<QXmlStreamWriter> fStream, const AgentInterface* agent)
{
    fStream->writeStartElement(outputTags.AGENT);

    const auto& agentCategory = static_cast<int>(agent->GetAgentCategory());

    fStream->writeAttribute(outputAttributes.ID, QString::number(agent->GetId()));
    fStream->writeAttribute(outputAttributes.AGENTTYPEGROUPNAME,
                            QString::fromStdString(AgentCategoryStrings[agentCategory]));
    fStream->writeAttribute(outputAttributes.AGENTTYPENAME, QString::fromStdString(agent->GetAgentTypeName()));
    fStream->writeAttribute(outputAttributes.VEHICLEMODELTYPE, QString::fromStdString(agent->GetVehicleModelType()));
    fStream->writeAttribute(outputAttributes.DRIVERPROFILENAME, QString::fromStdString(agent->GetDriverProfileName()));

    AddVehicleAttributes(fStream, agent->GetVehicleModelParameters());
    AddSensors(fStream, agent);

    fStream->writeEndElement();
}

void ObservationFileHandler::AddVehicleAttributes(std::shared_ptr<QXmlStreamWriter> fStream, const VehicleModelParameters &vehicleModelParameters)
{
    fStream->writeStartElement(outputTags.VEHICLEATTRIBUTES);

    fStream->writeAttribute(outputAttributes.WIDTH,
                                            QString::number(vehicleModelParameters.width));
    fStream->writeAttribute(outputAttributes.LENGTH,
                                            QString::number(vehicleModelParameters.length));
    fStream->writeAttribute(outputAttributes.HEIGHT,
                                            QString::number(vehicleModelParameters.height));

    const double longitudinalPivotOffset = (vehicleModelParameters.length / 2.0) - vehicleModelParameters.distanceReferencePointToLeadingEdge;
    fStream->writeAttribute(outputAttributes.LONGITUDINALPIVOTOFFSET,
                                            QString::number(longitudinalPivotOffset));

    fStream->writeEndElement();
}

void ObservationFileHandler::AddSensors(std::shared_ptr<QXmlStreamWriter> fStream, const AgentInterface* agent)
{
    const openpass::sensors::Parameters sensorParameters = agent->GetSensorParameters();

    if (ContainsSensor(sensorParameters))
    {
        fStream->writeStartElement(outputTags.SENSORS);

        for (const auto& itSensors : sensorParameters)
        {
            AddSensor(fStream, itSensors);
        }

        fStream->writeEndElement();
    }
}

bool ObservationFileHandler::ContainsSensor(const openpass::sensors::Parameters& sensorParameters) const
{
    return !sensorParameters.empty();
}

void ObservationFileHandler::AddSensor(std::shared_ptr<QXmlStreamWriter> fStream,
                                       const openpass::sensors::Parameter& sensorParameter)
{
    fStream->writeStartElement(outputTags.SENSOR);

    fStream->writeAttribute(outputAttributes.ID, QString::number(sensorParameter.id));

    fStream->writeAttribute(outputAttributes.TYPE, QString::fromStdString(sensorParameter.profile.type));
    fStream->writeAttribute(outputAttributes.MOUNTINGPOSITIONLONGITUDINAL,
                            QString::number(sensorParameter.position.longitudinal));
    fStream->writeAttribute(outputAttributes.MOUNTINGPOSITIONLATERAL,
                            QString::number(sensorParameter.position.lateral));
    fStream->writeAttribute(outputAttributes.MOUNTINGPOSITIONHEIGHT,
                            QString::number(sensorParameter.position.height));
    fStream->writeAttribute(outputAttributes.ORIENTATIONPITCH, QString::number(sensorParameter.position.pitch));
    fStream->writeAttribute(outputAttributes.ORIENTATIONYAW, QString::number(sensorParameter.position.yaw));
    fStream->writeAttribute(outputAttributes.ORIENTATIONROLL, QString::number(sensorParameter.position.roll));

    const auto& parameters = sensorParameter.profile.parameter;

    if (auto latency = openpass::parameter::Get<double>(parameters, "Latency"))
    {
       fStream->writeAttribute(outputAttributes.LATENCY, QString::number(latency.value()));
    }

    if (auto openingAngleH = openpass::parameter::Get<double>(parameters, "OpeningAngleH"))
    {
        fStream->writeAttribute(outputAttributes.OPENINGANGLEH, QString::number(openingAngleH.value()));
    }

    if (auto openingAngleV = openpass::parameter::Get<double>(parameters, "OpeningAngleV"))
    {
        fStream->writeAttribute(outputAttributes.OPENINGANGLEV, QString::number(openingAngleV.value()));
    }

    if (auto detectionRange = openpass::parameter::Get<double>(parameters, "DetectionRange"))
    {
        fStream->writeAttribute(outputAttributes.DETECTIONRANGE, QString::number(detectionRange.value()));
    }

    fStream->writeEndElement(); // Close Sensor Tag for this sensor
}

void ObservationFileHandler::AddHeader(std::shared_ptr<QXmlStreamWriter> fStream, ObservationCyclics& cyclics)
{
    fStream->writeStartElement(outputTags.HEADER);

    fStream->writeCharacters(QString::fromStdString(cyclics.GetHeader()));

    fStream->writeEndElement();
}

void ObservationFileHandler::AddSamples(std::shared_ptr<QXmlStreamWriter> fStream, ObservationCyclics& cyclics)
{
    // write SamplesTag
    fStream->writeStartElement(outputTags.SAMPLES);
    auto timeSteps = cyclics.GetTimeSteps();
    for (unsigned int timeStepNumber = 0; timeStepNumber < timeSteps->size(); ++timeStepNumber)
    {
        fStream->writeStartElement(outputTags.SAMPLE);
        fStream->writeAttribute(outputAttributes.TIME, QString::number(timeSteps->at(timeStepNumber)));
        fStream->writeCharacters(QString::fromStdString(cyclics.GetSamplesLine(timeStepNumber)));

        // close SampleTag
        fStream->writeEndElement();
    }

    // close SamplesTag
    fStream->writeEndElement();
}

void ObservationFileHandler::AddReference(std::shared_ptr<QXmlStreamWriter> fStream, QString filename)
{
    // write CyclicsFileTag
    fStream->writeStartElement(outputTags.CYCLICSFILE);

    fStream->writeCharacters(filename);

    // close CyclicsFileTag
    fStream->writeEndElement();
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

    csvFile = std::make_shared<QFile>(path);
    if (!csvFile->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        std::stringstream ss;
        ss << COMPONENTNAME << " could not create file: " << tmpPath.toStdString();
        //        LOG(CbkLogLevel::Error, ss.str());
        throw std::runtime_error(ss.str());
    }

    QTextStream stream( csvFile.get() );

    stream << "Timestep, " << QString::fromStdString(cyclics.GetHeader()) << '\n';

    auto timeSteps = cyclics.GetTimeSteps();
    for (unsigned int timeStepNumber = 0; timeStepNumber < timeSteps->size(); ++timeStepNumber)
    {
        stream << QString::number(timeSteps->at(timeStepNumber)) << ", " << QString::fromStdString(cyclics.GetSamplesLine(timeStepNumber)) << '\n';
    }

    csvFile->flush();

    csvFile->close();
}
