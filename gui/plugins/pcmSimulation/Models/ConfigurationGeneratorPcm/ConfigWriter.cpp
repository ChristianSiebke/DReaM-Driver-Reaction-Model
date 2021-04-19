/*********************************************************************
* Copyright (c) 2017, 2018, 2020, 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "ConfigWriter.h"

#include "GUI_Definitions.h"

ConfigWriter::ConfigWriter(const QString &baseFolder)
{
    baseDirectory.setPath(baseFolder);
}

const QString ConfigWriter::CreateSlaveConfiguration(const QString &configPath,
                                                     const PCM_SimulationSet *simSet,
                                                     const QString &resultFolderName,
                                                     const QString &pcmCase,
                                                     const int randomSeed)
{
    QString supposedCollisionTime;
    QString endTime;

    const std::vector<PCM_Trajectory *> &trajectories = simSet->GetTrajectories();

    if (trajectories.size() > 0)
    {
        supposedCollisionTime = QString::number(trajectories.at(0)->GetEndTime());
        endTime = QString::number(supposedCollisionTime.toInt() * 2); //set endTime twice as supposedCollisionTime in order to allow simulation after collision
    }

    XmlSlaveConfig runConfig(0, endTime, 1, "undefined", -1, randomSeed);

    const std::vector<PCM_ParticipantData *> &participants = simSet->GetParticipants();
    const std::vector<PCM_InitialValues *> &initials = simSet->GetInitials();

    for (uint i = 0; i < participants.size(); i++)
    {
        XmlSpawnPoint *sp = new XmlSpawnPoint(i, "SpawnPoint_PCM");
        sp->AddAgentRef(i);
        sp->AddParameter(0, XML_PARAMETER_TYPE::double_, "PositionX", initials.at(i)->GetXpos());
        sp->AddParameter(1, XML_PARAMETER_TYPE::double_, "PositionY", initials.at(i)->GetYpos());
        sp->AddParameter(2, XML_PARAMETER_TYPE::double_, "VelocityX", initials.at(i)->GetVx());
        sp->AddParameter(3, XML_PARAMETER_TYPE::double_, "VelocityY", initials.at(i)->GetVy());
        sp->AddParameter(4, XML_PARAMETER_TYPE::double_, "AccelerationX", initials.at(i)->GetAx());
        sp->AddParameter(5, XML_PARAMETER_TYPE::double_, "AccelerationY", initials.at(i)->GetAy());
        sp->AddParameter(6, XML_PARAMETER_TYPE::double_, "YawAngle", initials.at(i)->GetPsi());
        runConfig.AddSpawnPoint(sp);

        runConfig.AddAgent(i, i, participants.at(i));
    }

    XmlObservation *observation = new XmlObservation(0, "Evaluation_Pcm");

    int idCounter = 0;
    observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::int_, "Supposed collision time",
                              supposedCollisionTime);

    for (size_t i = 0; i < trajectories.size(); i++)
    {
        if (i == 0)
        {
            observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::intVector, "WayPoints_Time", trajectories.at(i)->GetTimeVecString());
        }

        observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::doubleVector,
                                  "WayPoints_X" + QString::number(i), trajectories.at(i)->GetXPosVecString());
        observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::doubleVector,
                                  "WayPoints_Y" + QString::number(i), trajectories.at(i)->GetYPosVecString());
        observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::doubleVector,
                                  "WayPoints_uVel" + QString::number(i), trajectories.at(i)->GetUVelVecString());
        observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::doubleVector,
                                  "WayPoints_vVel" + QString::number(i), trajectories.at(i)->GetVVelVecString());
        observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::doubleVector,
                                  "WayPoints_YawAngle" + QString::number(i), trajectories.at(i)->GetPsiVecString());
    }

    observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::string_, "resultFolderName",
                              baseDirectory.relativeFilePath(resultFolderName));
    observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::string_, "pcmCaseId",
                              pcmCase);
    observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::string_, "pcmCaseIndex",
                              QString::number(-1));

    runConfig.AddObservation(observation);

    XmlObservation *observerCollision = new XmlObservation(1, "Observation_Collision");
    observerCollision->AddParameter(0, XML_PARAMETER_TYPE::double_, "endVelocity", "1.0");

    runConfig.AddObservation(observerCollision);

    XmlObservation *observerScopeLogger = new XmlObservation(2, "Observation_ScopeLogger");
    runConfig.AddObservation(observerScopeLogger);

    return WriteSlaveConfiguration(runConfig, configPath);
}

const QString ConfigWriter::CreateProfilesCatalog(const QString &configPath,
                                                  const PCM_SimulationSet *simSet,
                                                  const QString &resultFolderName,
                                                  const QString &pcmCase,
                                                  const int randomSeed)
{
    QString supposedCollisionTime;
    QString endTime;

    const std::vector<PCM_Trajectory *> &trajectories = simSet->GetTrajectories();

    if (trajectories.size() > 0)
    {
        supposedCollisionTime = QString::number(trajectories.at(0)->GetEndTime());
        endTime = QString::number(supposedCollisionTime.toInt() * 2); //set endTime twice as supposedCollisionTime in order to allow simulation after collision
    }

    XmlProfilesConfig profilesConfig(0, endTime, 1, "undefined", -1, randomSeed);

    const std::vector<PCM_ParticipantData *> &participants = simSet->GetParticipants();
    const std::vector<PCM_InitialValues *> &initials = simSet->GetInitials();

    for (uint i = 0; i < participants.size(); i++)
    {
        XmlSpawnPoint *sp = new XmlSpawnPoint(i, "SpawnPoint_PCM");
        sp->AddAgentRef(i);
        sp->AddParameter(0, XML_PARAMETER_TYPE::double_, "PositionX", initials.at(i)->GetXpos());
        sp->AddParameter(1, XML_PARAMETER_TYPE::double_, "PositionY", initials.at(i)->GetYpos());
        sp->AddParameter(2, XML_PARAMETER_TYPE::double_, "VelocityX", initials.at(i)->GetVx());
        sp->AddParameter(3, XML_PARAMETER_TYPE::double_, "VelocityY", initials.at(i)->GetVy());
        sp->AddParameter(4, XML_PARAMETER_TYPE::double_, "AccelerationX", initials.at(i)->GetAx());
        sp->AddParameter(5, XML_PARAMETER_TYPE::double_, "AccelerationY", initials.at(i)->GetAy());
        sp->AddParameter(6, XML_PARAMETER_TYPE::double_, "YawAngle", initials.at(i)->GetPsi());
        profilesConfig.AddSpawnPoint(sp);

        profilesConfig.AddModelId(i);
    }

    XmlObservation *observation = new XmlObservation(0, "Evaluation_Pcm");

    int idCounter = 0;
    observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::int_, "Supposed collision time",
                              supposedCollisionTime);

    for (size_t i = 0; i < trajectories.size(); i++)
    {
        if (i == 0)
        {
            observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::intVector, "WayPoints_Time", trajectories.at(i)->GetTimeVecString());
        }

        observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::doubleVector,
                                  "WayPoints_X" + QString::number(i), trajectories.at(i)->GetXPosVecString());
        observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::doubleVector,
                                  "WayPoints_Y" + QString::number(i), trajectories.at(i)->GetYPosVecString());
        observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::doubleVector,
                                  "WayPoints_uVel" + QString::number(i), trajectories.at(i)->GetUVelVecString());
        observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::doubleVector,
                                  "WayPoints_vVel" + QString::number(i), trajectories.at(i)->GetVVelVecString());
        observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::doubleVector,
                                  "WayPoints_YawAngle" + QString::number(i), trajectories.at(i)->GetPsiVecString());
    }

    observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::string_, "resultFolderName",
                              baseDirectory.relativeFilePath(resultFolderName));
    observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::string_, "pcmCaseId",
                              pcmCase);
    observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::string_, "pcmCaseIndex",
                              QString::number(-1));

    profilesConfig.AddObservation(observation);

    XmlObservation *observerCollision = new XmlObservation(1, "Observation_Collision");
    observerCollision->AddParameter(0, XML_PARAMETER_TYPE::double_, "endVelocity", "1.0");

    profilesConfig.AddObservation(observerCollision);

    XmlObservation *observerScopeLogger = new XmlObservation(2, "Observation_ScopeLogger");
    profilesConfig.AddObservation(observerScopeLogger);

    return WriteProfilesCatalog(profilesConfig, configPath);
}

const QString ConfigWriter::CreateModelsVehicle(const QString &configPath,
                                                const PCM_SimulationSet *simSet,
                                                const QString &resultFolderName,
                                                const QString &pcmCase,
                                                const int randomSeed)
{
    QString supposedCollisionTime;
    QString endTime;

    const std::vector<PCM_Trajectory *> &trajectories = simSet->GetTrajectories();

    if (trajectories.size() > 0)
    {
        supposedCollisionTime = QString::number(trajectories.at(0)->GetEndTime());
        endTime = QString::number(supposedCollisionTime.toInt() * 2); //set endTime twice as supposedCollisionTime in order to allow simulation after collision
    }

    XmlModelsConfig modelsConfig(0, endTime, 1, "undefined", -1, randomSeed);

    const std::vector<PCM_ParticipantData *> &participants = simSet->GetParticipants();
    const std::vector<PCM_InitialValues *> &initials = simSet->GetInitials();

    for (uint i = 0; i < participants.size(); i++)
    {
        XmlSpawnPoint *sp = new XmlSpawnPoint(i, "SpawnPoint_PCM");
        sp->AddAgentRef(i);
        sp->AddParameter(0, XML_PARAMETER_TYPE::double_, "PositionX", initials.at(i)->GetXpos());
        sp->AddParameter(1, XML_PARAMETER_TYPE::double_, "PositionY", initials.at(i)->GetYpos());
        sp->AddParameter(2, XML_PARAMETER_TYPE::double_, "VelocityX", initials.at(i)->GetVx());
        sp->AddParameter(3, XML_PARAMETER_TYPE::double_, "VelocityY", initials.at(i)->GetVy());
        sp->AddParameter(4, XML_PARAMETER_TYPE::double_, "AccelerationX", initials.at(i)->GetAx());
        sp->AddParameter(5, XML_PARAMETER_TYPE::double_, "AccelerationY", initials.at(i)->GetAy());
        sp->AddParameter(6, XML_PARAMETER_TYPE::double_, "YawAngle", initials.at(i)->GetPsi());
        modelsConfig.AddSpawnPoint(sp);

        modelsConfig.AddAgent(i, i, participants.at(i));
    }

    XmlObservation *observation = new XmlObservation(0, "Evaluation_Pcm");

    int idCounter = 0;
    observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::int_, "Supposed collision time",
                              supposedCollisionTime);

    for (size_t i = 0; i < trajectories.size(); i++)
    {
        if (i == 0)
        {
            observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::intVector, "WayPoints_Time", trajectories.at(i)->GetTimeVecString());
        }

        observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::doubleVector,
                                  "WayPoints_X" + QString::number(i), trajectories.at(i)->GetXPosVecString());
        observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::doubleVector,
                                  "WayPoints_Y" + QString::number(i), trajectories.at(i)->GetYPosVecString());
        observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::doubleVector,
                                  "WayPoints_uVel" + QString::number(i), trajectories.at(i)->GetUVelVecString());
        observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::doubleVector,
                                  "WayPoints_vVel" + QString::number(i), trajectories.at(i)->GetVVelVecString());
        observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::doubleVector,
                                  "WayPoints_YawAngle" + QString::number(i), trajectories.at(i)->GetPsiVecString());
    }

    observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::string_, "resultFolderName",
                              baseDirectory.relativeFilePath(resultFolderName));
    observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::string_, "pcmCaseId",
                              pcmCase);
    observation->AddParameter(idCounter++, XML_PARAMETER_TYPE::string_, "pcmCaseIndex",
                              QString::number(-1));

    modelsConfig.AddObservation(observation);

    XmlObservation *observerCollision = new XmlObservation(1, "Observation_Collision");
    observerCollision->AddParameter(0, XML_PARAMETER_TYPE::double_, "endVelocity", "1.0");

    modelsConfig.AddObservation(observerCollision);

    XmlObservation *observerScopeLogger = new XmlObservation(2, "Observation_ScopeLogger");
    modelsConfig.AddObservation(observerScopeLogger);

    return WriteModelsVehicle(modelsConfig, configPath);
}

const QString ConfigWriter::CreateSystemConfiguration(const QString &caseOutputFolder,
                                                      const QString &otherSystemFile,
                                                      const QString &car1SystemFile,
                                                      const QString &car2SystemFile,
                                                      const PCM_SimulationSet *simSet)
{
    QString systemConfigFile = caseOutputFolder + "/" + FILENAME_SYSTEM_CONFIG;

    QString agent_OtherSystemFile = "Systems/agent_NoDynamic.xml";
    if (otherSystemFile != "")
    {
        agent_OtherSystemFile = otherSystemFile;
    }

    QString agent_Car1SystemFile = "Systems/agent_TwoTrackModel.xml";
    if (car1SystemFile != "")
    {
        agent_Car1SystemFile = car1SystemFile;
    }

    QString agent_Car2SystemFile = "Systems/agent_TwoTrackModel.xml";
    if (car2SystemFile != "")
    {
        agent_Car2SystemFile = car2SystemFile;
    }
    else
    {
        agent_Car2SystemFile = agent_Car1SystemFile;
    }

    const std::vector<PCM_ParticipantData *> &participants = simSet->GetParticipants();

    if (participants.size() < 1)
    {
        return "";
    }

    if (QFile::exists(systemConfigFile))
    {
        QFile::remove(systemConfigFile);
    }

    QString firstAgentFile;
    bool firstAgentFileIsSet = false; //only the first agent which is a car will get agent_Car1SystemFile

    if ((participants.at(0)->GetType() == "car") || (participants.at(0)->GetType() == "truck"))
    {
        firstAgentFile = agent_Car1SystemFile;
        firstAgentFileIsSet = true;
    }
    else
    {
        firstAgentFile = agent_OtherSystemFile;
    }

    QDomDocument systemDocument;
    if (!XmlMergeHelper::loadDomDocument(firstAgentFile, systemDocument))
    {
        return "";
    }

    QDomElement systemRoot = systemDocument.documentElement();
    XmlMergeHelper::setIdOfSystem(systemRoot, 0);

    for (size_t i = 1; i < participants.size(); i++)
    {
        QString agentFile;
        if ((participants.at(i)->GetType() == "car") || (participants.at(i)->GetType() == "truck"))
        {
            if (firstAgentFileIsSet)
            {
                agentFile = agent_Car2SystemFile;
            }
            else
            {
                agentFile = agent_Car1SystemFile;
            }
        }
        else
        {
            agentFile = agent_OtherSystemFile;
        }

        QDomDocument agentDocument;
        if (!XmlMergeHelper::loadDomDocument(agentFile, agentDocument))
        {
            return "";
        }
        QDomElement agentRoot = agentDocument.documentElement();
        XmlMergeHelper::setIdOfSystem(agentRoot, i);
        systemRoot.appendChild(agentRoot.firstChildElement("system"));
    }

    std::locale::global(std::locale("C"));

    QFile systemXmlFile(systemConfigFile); // automatic object will be closed on destruction
    if (!systemXmlFile.open(QIODevice::WriteOnly))
    {
        std::cout << "Error (ConfigGenerator): could not open xmlFile: " << systemConfigFile.toStdString()
                  <<
                  std::endl;
        return "";
    }

    QTextStream stream(&systemXmlFile);
    stream << systemDocument.toString();

    systemXmlFile.close();

    return systemConfigFile;
}

const QString ConfigWriter::CreateSceneryConfiguration(const QString &configPath,
                                                       const PCM_SimulationSet *simSet)
{
    XmlScenery sceneryConfig;

    const std::vector<const PCM_Marks *> *marksVec = simSet->GetPcmData()->GetMarksVec();

    for (size_t i = 0; i < marksVec->size(); i++)
    {
        sceneryConfig.AddMarks(marksVec->at(i));
    }

    sceneryConfig.SetObject(simSet->GetPcmData()->GetObject());
    sceneryConfig.SetViewObject(simSet->GetPcmData()->GetViewObject());

    sceneryConfig.SetGlobalData(simSet->GetPcmData()->GetGlobalData());

    for (size_t i = 0; i < simSet->GetParticipants().size(); i++)
    {
        const std::vector<PCM_Trajectory *> trajectories = simSet->GetTrajectories();
        if (i < trajectories.size())
        {
            sceneryConfig.AddTrajectory(i, trajectories.at(i));
        }
    }

    return WriteSceneryConfiguration(sceneryConfig, configPath);
}

const QString ConfigWriter::CreateParkingConfiguration(const QString &configPath)
{
    return WriteParkingConfiguration(configPath);
}

const QString ConfigWriter::CreateFrameworkConfiguration(const QString frameworkConfigPath,
                                                         QList<QMap<QString, QString> > configList,
                                                         const int logLevel)
{
    QString frameworkConfigFile = frameworkConfigPath + "/" + FILENAME_FRAMEWORK_CONFIG;
    QFile file(frameworkConfigFile);

    // open file
    if (!file.open(QIODevice::WriteOnly))
    {
        // show error message if not able to open file
        std::cout << "Error (ConfigGenerator): "
                  "could not open masterConfiguration.xml" << std::endl;
        return "";
    }

    // if file is successfully opened, create XML
    QXmlStreamWriter xmlWriter(&file);

    xmlWriter.setAutoFormatting(true);

    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement("masterConfig");
    xmlWriter.writeTextElement("logLevel", QString::number(logLevel));
    //    xmlWriter.writeTextElement("logFileMaster",
    //                               baseDirectory.relativeFilePath(frameworkConfigPath + "/" + FILENAME_OPENPASSMASTER_LOG));
    //    xmlWriter.writeTextElement("slave", "OpenPassSlave.exe");
    //    xmlWriter.writeTextElement("libraries", baseDirectory.relativeFilePath(baseFolder) + SUBDIR_LIB_SIM);

    xmlWriter.writeStartElement("slaveConfigs");

    for (QMap<QString, QString> configSet : configList)
    {
        xmlWriter.writeStartElement("slaveConfig");
        QMapIterator<QString, QString> configSetIterator(configSet);
        while (configSetIterator.hasNext())
        {
            configSetIterator.next();
            xmlWriter.writeTextElement(configSetIterator.key(), configSetIterator.value());
        }
        xmlWriter.writeEndElement(); // slaveConfig
    }

    xmlWriter.writeEndElement(); // slaveConfigs
    xmlWriter.writeEndElement(); // masterConfig

    xmlWriter.writeEndDocument();

    file.close();
    file.flush();

    return frameworkConfigFile;
}

const QString ConfigWriter::WriteSlaveConfiguration(XmlSlaveConfig &slaveConfig,
                                                    const QString &configPath)
{
    // Create the xml with the chosen cases
    QString slaveConfigFile = configPath + "/" + FILENAME_SLAVE_CONFIG;
    QFile file(slaveConfigFile);

    // open file
    if (!file.open(QIODevice::WriteOnly))
    {
        // show error message if not able to open file
        std::cout << "Error (ConfigGenerator): could not open "
                  << FILENAME_SLAVE_CONFIG << std::endl;
        return "";
    }

    // if file is successfully opened, create XML
    QXmlStreamWriter xmlWriter(&file);

    xmlWriter.setAutoFormatting(true);

    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement("slaveConfig");
    xmlWriter.writeAttribute("SchemaVersion","0.8.1");

    bool success = slaveConfig.WriteToXml(&xmlWriter);

    xmlWriter.writeEndElement(); //SlaveConfig

    xmlWriter.writeEndDocument();

    file.close();
    file.flush();

    if (success)
    {
        return slaveConfigFile;
    }
    else
    {
        return "";
    }
}

const QString ConfigWriter::WriteProfilesCatalog(XmlProfilesConfig &profilesConfig,
                                                 const QString &configPath)
{
    // Create the xml with the chosen cases
    QString profilesConfigFile = configPath + "/" + FILENAME_PROFILES_CONFIG;
    QFile file(profilesConfigFile);

    // open file
    if (!file.open(QIODevice::WriteOnly))
    {
        // show error message if not able to open file
        std::cout << "Error (ConfigGenerator): could not open "
                  << FILENAME_PROFILES_CONFIG << std::endl;
        return "";
    }

    // if file is successfully opened, create XML
    QXmlStreamWriter xmlWriter(&file);

    xmlWriter.setAutoFormatting(true);

    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement("Profiles");

    xmlWriter.writeAttribute("SchemaVersion", "0.4.3");

    bool success = profilesConfig.WriteToXml(&xmlWriter);

    xmlWriter.writeEndElement(); //Profiles

    xmlWriter.writeEndDocument();

    file.close();
    file.flush();

    if (success)
    {
        return profilesConfigFile;
    }
    else
    {
        return "";
    }
}

const QString ConfigWriter::WriteModelsVehicle(XmlModelsConfig &modelsConfig,
                                               const QString &configPath)
{
    // Create the xml with the chosen cases
    QString modelsConfigFile = configPath + "/" + FILENAME_MODELS_CONFIG;
    QFile file(modelsConfigFile);

    // open file
    if (!file.open(QIODevice::WriteOnly))
    {
        // show error message if not able to open file
        std::cout << "Error (ConfigGenerator): could not open "
                  << FILENAME_MODELS_CONFIG << std::endl;
        return "";
    }

    // if file is successfully opened, create XML
    QXmlStreamWriter xmlWriter(&file);

    xmlWriter.setAutoFormatting(true);

    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement("OpenSCENARIO");

    bool success = modelsConfig.WriteToXml(&xmlWriter);

    xmlWriter.writeEndElement(); //OpenSCENARIO

    xmlWriter.writeEndDocument();

    file.close();
    file.flush();

    if (success)
    {
        return modelsConfigFile;
    }
    else
    {
        return "";
    }
}

const QString ConfigWriter::WriteSceneryConfiguration(XmlScenery &sceneryConfig,
                                                      const QString &configPath)
{
    // write the xml agent file
    QString sceneryConfigFile = configPath + "/" + FILENAME_SCENERY_CONFIG;
    QFile file(sceneryConfigFile);

    //open file
    if (!file.open(QIODevice::WriteOnly))
    {
        // show error message if not able to open file
        std::cout << "Error (ConfigGenerator): could not open "
                  << FILENAME_SCENERY_CONFIG << std::endl;
        return "";
    }

    // if file is successfully opened, create XML
    QXmlStreamWriter xmlWriter(&file);

    xmlWriter.setAutoFormatting(true);

    xmlWriter.writeStartDocument();

    sceneryConfig.WriteToXml(&xmlWriter);

    xmlWriter.writeEndDocument();

    file.close();
    file.flush();

    return sceneryConfigFile;
}

const QString ConfigWriter::WriteParkingConfiguration(const QString &configPath)
{
    // write the xml agent file
    QString sceneryConfigFile = configPath + "/" + FILENAME_PARKING_CONFIG;
    QFile file(sceneryConfigFile);

    //open file
    if (!file.open(QIODevice::WriteOnly))
    {
        // show error message if not able to open file
        std::cout << "Error (ConfigGenerator): could not open "
                  << FILENAME_PARKING_CONFIG << std::endl;
        return "";
    }

    // if file is successfully opened, create XML
    QXmlStreamWriter xmlWriter(&file);

    xmlWriter.setAutoFormatting(true);

    xmlWriter.writeStartDocument();

    QXmlStreamAttributes attrib;
    attrib.append("name","Parking");
    attrib.append("north","10000");
    attrib.append("east","10000");
    attrib.append("south","-10000");
    attrib.append("west","-10000");
    attrib.append("revMajor","1");
    attrib.append("revMinor","1");
    attrib.append("date","2020-01-01T00:00:00");
    attrib.append("version","1");

    xmlWriter.writeStartElement("OpenDRIVE");

    xmlWriter.writeStartElement("header");
    xmlWriter.writeAttributes(attrib);
    xmlWriter.writeEndElement(); // header

    xmlWriter.writeStartElement("road");
    xmlWriter.writeAttribute("name","unnamed");
    xmlWriter.writeAttribute("junction","-1");
    xmlWriter.writeAttribute("length","10000");
    xmlWriter.writeAttribute("id","1");
    xmlWriter.writeEmptyElement("link");
    xmlWriter.writeStartElement("planView");
    xmlWriter.writeStartElement("geometry");
    xmlWriter.writeAttribute("s","0");
    xmlWriter.writeAttribute("hdg","0");
    xmlWriter.writeAttribute("x","-5000");
    xmlWriter.writeAttribute("y","5000");
    xmlWriter.writeAttribute("length","10000");
    xmlWriter.writeEmptyElement("line");
    xmlWriter.writeEndElement(); // geometry
    xmlWriter.writeEndElement(); // planView
    xmlWriter.writeStartElement("elevationProfile");
    xmlWriter.writeStartElement("elevation");
    xmlWriter.writeAttribute("a","0");
    xmlWriter.writeAttribute("s","0");
    xmlWriter.writeAttribute("b","0");
    xmlWriter.writeAttribute("c","0");
    xmlWriter.writeAttribute("d","0");
    xmlWriter.writeEndElement(); // elevation
    xmlWriter.writeEndElement(); // elevationProfile
    xmlWriter.writeStartElement("lateralProfile");
    xmlWriter.writeStartElement("superelevation");
    xmlWriter.writeAttribute("a","0");
    xmlWriter.writeAttribute("s","0");
    xmlWriter.writeAttribute("b","0");
    xmlWriter.writeAttribute("c","0");
    xmlWriter.writeAttribute("d","0");
    xmlWriter.writeEndElement(); // superelevation
    xmlWriter.writeStartElement("crossfall");
    xmlWriter.writeAttribute("side","both");
    xmlWriter.writeAttribute("a","0");
    xmlWriter.writeAttribute("s","0");
    xmlWriter.writeAttribute("b","0");
    xmlWriter.writeAttribute("c","0");
    xmlWriter.writeAttribute("d","0");
    xmlWriter.writeEndElement(); // crossfall
    xmlWriter.writeEndElement(); // lateralProfile


    xmlWriter.writeStartElement("lanes");
    xmlWriter.writeStartElement("laneSection");
    xmlWriter.writeAttribute("s","0");
    xmlWriter.writeStartElement("right");
    xmlWriter.writeStartElement("lane");
    xmlWriter.writeAttribute("level","1");
    xmlWriter.writeAttribute("type","driving");
    xmlWriter.writeAttribute("id","-2");
    xmlWriter.writeStartElement("width");
    xmlWriter.writeAttribute("a","10000");
    xmlWriter.writeAttribute("b","0");
    xmlWriter.writeAttribute("c","0");
    xmlWriter.writeAttribute("d","0");
    xmlWriter.writeAttribute("sOffset","0");
    xmlWriter.writeEndElement(); // width
    xmlWriter.writeStartElement("roadMark");
    xmlWriter.writeAttribute("color","standard");
    xmlWriter.writeAttribute("width","0.12");
    xmlWriter.writeAttribute("weight","standard");
    xmlWriter.writeAttribute("sOffset","0");
    xmlWriter.writeAttribute("type","broken");
    xmlWriter.writeAttribute("laneChange","both");
    xmlWriter.writeEndElement(); // roadMark
    xmlWriter.writeEndElement(); // lane
    xmlWriter.writeStartElement("lane");
    xmlWriter.writeAttribute("level","1");
    xmlWriter.writeAttribute("type","driving");
    xmlWriter.writeAttribute("id","-1");
    xmlWriter.writeStartElement("width");
    xmlWriter.writeAttribute("a","0");
    xmlWriter.writeAttribute("b","0");
    xmlWriter.writeAttribute("c","0");
    xmlWriter.writeAttribute("d","0");
    xmlWriter.writeAttribute("sOffset","0");
    xmlWriter.writeEndElement(); // width
    xmlWriter.writeStartElement("roadMark");
    xmlWriter.writeAttribute("color","standard");
    xmlWriter.writeAttribute("width","0.12");
    xmlWriter.writeAttribute("weight","standard");
    xmlWriter.writeAttribute("sOffset","0");
    xmlWriter.writeAttribute("type","broken");
    xmlWriter.writeAttribute("laneChange","both");
    xmlWriter.writeEndElement(); // roadMark
    xmlWriter.writeEndElement(); // lane
    xmlWriter.writeEndElement(); // right
    xmlWriter.writeStartElement("center");
    xmlWriter.writeStartElement("lane");
    xmlWriter.writeAttribute("level","1");
    xmlWriter.writeAttribute("type","border");
    xmlWriter.writeAttribute("id","0");
    xmlWriter.writeStartElement("roadMark");
    xmlWriter.writeAttribute("color","standard");
    xmlWriter.writeAttribute("width","0.12");
    xmlWriter.writeAttribute("weight","standard");
    xmlWriter.writeAttribute("sOffset","0");
    xmlWriter.writeAttribute("type","solid");
    xmlWriter.writeAttribute("laneChange","both");
    xmlWriter.writeEndElement(); // roadMark
    xmlWriter.writeEndElement(); // lane
    xmlWriter.writeEndElement(); // center
    xmlWriter.writeEndElement(); // laneSection
    xmlWriter.writeEndElement(); // lanes

    xmlWriter.writeStartElement("type");
    xmlWriter.writeAttribute("s","0");
    xmlWriter.writeAttribute("type","unknown");
    xmlWriter.writeEndElement(); // type

    xmlWriter.writeEndElement(); // road

    xmlWriter.writeEndElement(); // OpenDRIVE

    xmlWriter.writeEndDocument();

    file.close();
    file.flush();

    return sceneryConfigFile;
}
