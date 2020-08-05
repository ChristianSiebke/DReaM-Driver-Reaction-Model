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

#include "observationFileHandler.h"

void ObservationFileHandler::WriteStartOfFile()
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
        std::list<std::pair<std::string, std::string>> eventParameters)
{
    for (auto parameterPair : eventParameters)
    {
        fStream->writeStartElement(outputTags.EVENTPARAMETER);
        fStream->writeAttribute(outputAttributes.KEY, QString::fromStdString(parameterPair.first));
        fStream->writeAttribute(outputAttributes.VALUE, QString::fromStdString(parameterPair.second));

        //Closes EventParameter tag
        fStream->writeEndElement();
    }
}

void ObservationFileHandler::AddEvent(std::shared_ptr<QXmlStreamWriter> fStream, std::shared_ptr<EventInterface> event)
{
    const std::string eventType = GetEventString(event->GetEventType());

    fStream->writeStartElement(outputTags.EVENT);
    fStream->writeAttribute(outputAttributes.ID, QString::number(event->GetId()));
    fStream->writeAttribute(outputAttributes.TIME, QString::number(event->GetEventTime()));
    fStream->writeAttribute(outputAttributes.SOURCE, QString::fromStdString(event->GetSource()));
    fStream->writeAttribute(outputAttributes.TYPE, QString::fromStdString(eventType));

    const int& triggeringEventId = event->GetTriggeringEventId();
    if (triggeringEventId >= 0)
    {
        fStream->writeAttribute(outputAttributes.TRIGGERINGEVENTID, QString::number(triggeringEventId));
    }

    AddEventParameters(fStream, event->GetEventParametersAsString());

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

    AddSensors(fStream, agent);

    fStream->writeEndElement();
}

void ObservationFileHandler::AddSensors(std::shared_ptr<QXmlStreamWriter> fStream, const AgentInterface* agent)
{
    const std::list<SensorParameter> sensorParameters = agent->GetSensorParameters();

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

bool ObservationFileHandler::ContainsSensor(const std::list<SensorParameter>& sensorParameters) const
{
    return !sensorParameters.empty();
}

void ObservationFileHandler::AddSensor(std::shared_ptr<QXmlStreamWriter> fStream,
                                       const SensorParameter& sensorParameter)
{
    fStream->writeStartElement(outputTags.SENSOR);

    fStream->writeAttribute(outputAttributes.ID, QString::number(sensorParameter.id));

    fStream->writeAttribute(outputAttributes.TYPE, QString::fromStdString(sensorParameter.sensorProfile.type));
    fStream->writeAttribute(outputAttributes.MOUNTINGPOSITIONLONGITUDINAL,
                            QString::number(sensorParameter.sensorPosition.longitudinal));
    fStream->writeAttribute(outputAttributes.MOUNTINGPOSITIONLATERAL,
                            QString::number(sensorParameter.sensorPosition.lateral));
    fStream->writeAttribute(outputAttributes.MOUNTINGPOSITIONHEIGHT,
                            QString::number(sensorParameter.sensorPosition.height));
    fStream->writeAttribute(outputAttributes.ORIENTATIONPITCH, QString::number(sensorParameter.sensorPosition.pitch));
    fStream->writeAttribute(outputAttributes.ORIENTATIONYAW, QString::number(sensorParameter.sensorPosition.yaw));
    fStream->writeAttribute(outputAttributes.ORIENTATIONROLL, QString::number(sensorParameter.sensorPosition.roll));

    const auto& sensorProfile = sensorParameter.sensorProfile.parameters;

    fStream->writeAttribute(outputAttributes.LATENCY, QString::number(sensorProfile->GetParametersDouble().at("Latency")));

    if (sensorProfile->GetParametersDouble().count("OpeningAngleH"))
    {
        fStream->writeAttribute(outputAttributes.OPENINGANGLEH,
                                QString::number(sensorProfile->GetParametersDouble().at("OpeningAngleH")));
    }
    if (sensorProfile->GetParametersDouble().count("OpeningAngleV"))
    {
        fStream->writeAttribute(outputAttributes.OPENINGANGLEV,
                                QString::number(sensorProfile->GetParametersDouble().at("OpeningAngleV")));
    }
    if (sensorProfile->GetParametersDouble().count("DetectionRange"))
    {
        fStream->writeAttribute(outputAttributes.DETECTIONRANGE,
                                QString::number(sensorProfile->GetParametersDouble().at("DetectionRange")));
    }

    //fStream->writeAttribute(SensorFailureProbabilityAttribute, QString::number(sensorParameters.failureProbability));
    //fStream->writeAttribute(SensorLatencyAttribute, QString::number(sensorParameters.latency));

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

std::string ObservationFileHandler::GetEventString(EventDefinitions::EventType eventType)
{
    int keyIndex = static_cast<int>(eventType);
    return EventDefinitions::EventTypeStrings[keyIndex];
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

    stream << "frame,"
           << "id,"
           << QString::fromStdString(cyclics.GetAgentHeader())
           << '\n';

    auto timeSteps = cyclics.GetTimeSteps();
    std::vector<std::string> agentSamplesLines;
    for (unsigned int timeStepNumber = 0; timeStepNumber < timeSteps->size(); ++timeStepNumber)
    {
        agentSamplesLines = cyclics.GetAgentSamplesLine(timeStepNumber);
        for (size_t agentId = 0; agentId < agentSamplesLines.size(); ++agentId)
        {
            stream << QString::number(timeSteps->at(timeStepNumber))
                   << ","
                   << QString::fromStdString(std::to_string(agentId))
                   << ","
                   << QString::fromStdString(agentSamplesLines.at(agentId))
                   << '\n';
        }
    }

    csvFile->flush();

    csvFile->close();
}
