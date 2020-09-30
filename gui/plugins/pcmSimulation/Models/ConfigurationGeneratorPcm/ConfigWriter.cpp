/*********************************************************************
* Copyright (c) 2017, 2018, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "ConfigWriter.h"
#include "GUI_Definitions.h"

ConfigWriter::ConfigWriter():
    ConfigWriter(baseFolder)
{

}

ConfigWriter::ConfigWriter(const QString &baseFolder):
    baseFolder(baseFolder)
{
    baseDirectory.setPath(baseFolder);
}

const QString ConfigWriter::CreateSlaveConfiguration(const QString &configPath,
                                                   const QString &endTime,
                                                   std::vector<PCM_ParticipantData> &participants,
                                                   std::vector<PCM_InitialValues> &initials,
                                                   std::vector<PCM_Trajectory *> &trajectories,
                                                   const QString &supposedCollisionTime,
                                                   const QString &resultFolderName,
                                                   const QString &pcmCase,
                                                   const int randomSeed)
{
    XmlRunConfig runConfig(0, endTime, 1, "undefined", -1, randomSeed);

    for (size_t iParticipant = 0; iParticipant < participants.size(); iParticipant++)
    {
        XmlSpawnPoint *sp = new XmlSpawnPoint( iParticipant, "SpawnPoint_PCM" );
        sp->AddAgentRef( iParticipant );
        sp->AddParameter(0, XML_PARAMETER_TYPE::double_, "PositionX", initials[iParticipant].GetXpos() );
        sp->AddParameter(1, XML_PARAMETER_TYPE::double_, "PositionY", initials[iParticipant].GetYpos() );
        sp->AddParameter(2, XML_PARAMETER_TYPE::double_, "VelocityX", initials[iParticipant].GetVx() );
        sp->AddParameter(3, XML_PARAMETER_TYPE::double_, "VelocityY", initials[iParticipant].GetVy() );
        sp->AddParameter(4, XML_PARAMETER_TYPE::double_, "AccelerationX", initials[iParticipant].GetAx() );
        sp->AddParameter(5, XML_PARAMETER_TYPE::double_, "AccelerationY", initials[iParticipant].GetAy() );
        sp->AddParameter(6, XML_PARAMETER_TYPE::double_, "YawAngle", initials[iParticipant].GetPsi() );
        runConfig.AddSpawnPoint(sp);

        runConfig.AddAgent( iParticipant, iParticipant, participants[iParticipant] );
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
                                                   const QString &endTime,
                                                   std::vector<PCM_ParticipantData> &participants,
                                                   std::vector<PCM_InitialValues> &initials,
                                                   std::vector<PCM_Trajectory *> &trajectories,
                                                   const QString &supposedCollisionTime,
                                                   const QString &resultFolderName,
                                                   const QString &pcmCase,
                                                   const int randomSeed)
{
    XmlProfilesConfig runConfig(0, endTime, 1, "undefined", -1, randomSeed);

    for (size_t iParticipant = 0; iParticipant < participants.size(); iParticipant++)
    {
        XmlSpawnPoint *sp = new XmlSpawnPoint( iParticipant, "SpawnPoint_PCM" );
        sp->AddAgentRef( iParticipant );
        sp->AddParameter(0, XML_PARAMETER_TYPE::double_, "PositionX", initials[iParticipant].GetXpos() );
        sp->AddParameter(1, XML_PARAMETER_TYPE::double_, "PositionY", initials[iParticipant].GetYpos() );
        sp->AddParameter(2, XML_PARAMETER_TYPE::double_, "VelocityX", initials[iParticipant].GetVx() );
        sp->AddParameter(3, XML_PARAMETER_TYPE::double_, "VelocityY", initials[iParticipant].GetVy() );
        sp->AddParameter(4, XML_PARAMETER_TYPE::double_, "AccelerationX", initials[iParticipant].GetAx() );
        sp->AddParameter(5, XML_PARAMETER_TYPE::double_, "AccelerationY", initials[iParticipant].GetAy() );
        sp->AddParameter(6, XML_PARAMETER_TYPE::double_, "YawAngle", initials[iParticipant].GetPsi() );
        runConfig.AddSpawnPoint(sp);

        runConfig.AddAgent( iParticipant, iParticipant, participants[iParticipant] );
        runConfig.AddModel( iParticipant, iParticipant, participants[iParticipant] );
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

    return WriteProfilesCatalog(runConfig, configPath);
}


const QString ConfigWriter::CreateModelsVehicle(const QString &configPath,
                                                   const QString &endTime,
                                                   std::vector<PCM_ParticipantData> &participants,
                                                   std::vector<PCM_InitialValues> &initials,
                                                   std::vector<PCM_Trajectory *> &trajectories,
                                                   const QString &supposedCollisionTime,
                                                   const QString &resultFolderName,
                                                   const QString &pcmCase,
                                                   const int randomSeed)
{
    XmlModelsConfig runConfig(0, endTime, 1, "undefined", -1, randomSeed);

    for (size_t iParticipant = 0; iParticipant < participants.size(); iParticipant++)
    {
        XmlSpawnPoint *sp = new XmlSpawnPoint( iParticipant, "SpawnPoint_PCM" );
        sp->AddAgentRef( iParticipant );
        sp->AddParameter(0, XML_PARAMETER_TYPE::double_, "PositionX", initials[iParticipant].GetXpos() );
        sp->AddParameter(1, XML_PARAMETER_TYPE::double_, "PositionY", initials[iParticipant].GetYpos() );
        sp->AddParameter(2, XML_PARAMETER_TYPE::double_, "VelocityX", initials[iParticipant].GetVx() );
        sp->AddParameter(3, XML_PARAMETER_TYPE::double_, "VelocityY", initials[iParticipant].GetVy() );
        sp->AddParameter(4, XML_PARAMETER_TYPE::double_, "AccelerationX", initials[iParticipant].GetAx() );
        sp->AddParameter(5, XML_PARAMETER_TYPE::double_, "AccelerationY", initials[iParticipant].GetAy() );
        sp->AddParameter(6, XML_PARAMETER_TYPE::double_, "YawAngle", initials[iParticipant].GetPsi() );
        runConfig.AddSpawnPoint(sp);

        runConfig.AddAgent( iParticipant, iParticipant, participants[iParticipant] );
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

    return WriteModelsVehicle(runConfig, configPath);
}

const QString ConfigWriter::CreateSystemConfiguration(const QString &caseOutputFolder,
                                                      const QString &otherSystemFile,
                                                      const QString &car1SystemFile,
                                                      const QString &car2SystemFile,
                                                      std::vector<PCM_ParticipantData> &participants)
{
    QString systemConfigFile = caseOutputFolder + "/" + FILENAME_SYSTEM_CONIG;

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

    if ((participants.at(0).GetType() == "Car") || (participants.at(0).GetType() == "Truck"))
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
        if ((participants.at(i).GetType() == "Car") || (participants.at(i).GetType() == "Truck"))
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
                                                       std::vector<PCM_Marks *> &marksVec,
                                                       std::vector<PCM_ParticipantData> &participants,
                                                       std::vector<PCM_Trajectory *> &trajectories,
                                                       PCM_Object &object,
                                                       PCM_ViewObject &viewObject,
                                                       PCM_IntendedCourses &intendedCourses,
                                                       PCM_GlobalData &globalData)
{
    Q_UNUSED(intendedCourses);
    XmlScenery sceneryConfig;

    for (size_t i = 0; i < marksVec.size(); i++)
    {
        sceneryConfig.AddMarks(marksVec.at(i));
    }

    sceneryConfig.AddObject(object);
    sceneryConfig.AddViewObject(viewObject);

    sceneryConfig.AddGlobalData(globalData);

    for (size_t i = 0; i < participants.size(); i++)
    {
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
    QString frameworkConfigFile = frameworkConfigPath + "/" + FILENAME_FRAMEWORK_CONIG;
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
    xmlWriter.writeTextElement("logFileMaster",
                               baseDirectory.relativeFilePath(frameworkConfigPath + "/" + FILENAME_OPENPASSMASTER_LOG));
    xmlWriter.writeTextElement("slave", "OpenPassSlave.exe");
    xmlWriter.writeTextElement("libraries", baseDirectory.relativeFilePath(baseFolder) + SUBDIR_LIB_SIM);

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

const QString ConfigWriter::WriteSlaveConfiguration(XmlRunConfig &runConfig,
                                                  const QString &configPath)
{
    // Create the xml with the chosen cases
    QString runConfigFile = configPath + "/" + FILENAME_RUN_CONIG;
    QFile file(runConfigFile);

    // open file
    if (!file.open(QIODevice::WriteOnly))
    {
        // show error message if not able to open file
        std::cout << "Error (ConfigGenerator): could not open "
                  << FILENAME_RUN_CONIG << std::endl;
        return "";
    }

    // if file is successfully opened, create XML
    QXmlStreamWriter xmlWriter(&file);

    xmlWriter.setAutoFormatting(true);

    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement("slaveConfig");
    xmlWriter.writeAttribute("SchemaVersion","0.8.1");

    bool success = runConfig.WriteToXml(&xmlWriter);

    xmlWriter.writeEndElement(); //RunConfiguration

    xmlWriter.writeEndDocument();

    file.close();
    file.flush();

    if (success)
    {
        return runConfigFile;
    }
    else
    {
        return "";
    }
}

const QString ConfigWriter::WriteProfilesCatalog(XmlProfilesConfig &runConfig,
                                                  const QString &configPath)
{
    // Create the xml with the chosen cases
    QString runConfigFile = configPath + "/" + FILENAME_PROFILES_CONIG;
    QFile file(runConfigFile);

    // open file
    if (!file.open(QIODevice::WriteOnly))
    {
        // show error message if not able to open file
        std::cout << "Error (ConfigGenerator): could not open "
                  << FILENAME_PROFILES_CONIG << std::endl;
        return "";
    }

    // if file is successfully opened, create XML
    QXmlStreamWriter xmlWriter(&file);

    xmlWriter.setAutoFormatting(true);

    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement("Profiles");

    xmlWriter.writeAttribute("SchemaVersion","0.4.2");

    bool success = runConfig.WriteToXml(&xmlWriter);

    xmlWriter.writeEndElement(); //RunConfiguration

    xmlWriter.writeEndDocument();

    file.close();
    file.flush();

    if (success)
    {
        return runConfigFile;
    }
    else
    {
        return "";
    }
}

const QString ConfigWriter::WriteModelsVehicle(XmlModelsConfig &runConfig,
                                                  const QString &configPath)
{
    // Create the xml with the chosen cases
    QString runConfigFile = configPath + "/" + FILENAME_MODELS_CONIG;
    QFile file(runConfigFile);

    // open file
    if (!file.open(QIODevice::WriteOnly))
    {
        // show error message if not able to open file
        std::cout << "Error (ConfigGenerator): could not open "
                  << FILENAME_MODELS_CONIG << std::endl;
        return "";
    }

    // if file is successfully opened, create XML
    QXmlStreamWriter xmlWriter(&file);

    xmlWriter.setAutoFormatting(true);

    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement("OpenSCENARIO");

    bool success = runConfig.WriteToXml(&xmlWriter);

    xmlWriter.writeEndElement(); //OpenSCENARIO

    xmlWriter.writeEndDocument();

    file.close();
    file.flush();

    if (success)
    {
        return runConfigFile;
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
    QString sceneryConfigFile = configPath + "/" + FILENAME_SCENERY_CONIG;
    QFile file(sceneryConfigFile);

    //open file
    if (!file.open(QIODevice::WriteOnly))
    {
        // show error message if not able to open file
        std::cout << "Error (ConfigGenerator): could not open "
                  << FILENAME_SCENERY_CONIG << std::endl;
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
    QString sceneryConfigFile = configPath + "/" + FILENAME_PARKING_CONIG;
    QFile file(sceneryConfigFile);

    //open file
    if (!file.open(QIODevice::WriteOnly))
    {
        // show error message if not able to open file
        std::cout << "Error (ConfigGenerator): could not open "
                  << FILENAME_PARKING_CONIG << std::endl;
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
