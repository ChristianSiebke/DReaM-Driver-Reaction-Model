/*********************************************************************
* Copyright (c) 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "ConfigSetreaderPcm.h"

#include "GUI_Definitions.h"
#include "pcm_data.h"
#include "scenarioImporterPCM.h"
#include "sceneryImporterPCM.h"
#include "vehicleModelImporter.h"

ConfigSetReaderPcm::ConfigSetReaderPcm(const QString &configFolder) :
    configFolder(configFolder)
{
}

PCM_SimulationSet *ConfigSetReaderPcm::Read()
{
    PCM_Data *pcmData = new PCM_Data();
    std::map<int, PCM_Trajectory *> trajectoryMap;

    // get participants data from sceneryConfiguration.xml of previous results
    QString prevSceneryConfigFilePath = configFolder + '/' + FILENAME_SCENERY_CONFIG;
    SceneryImporterPCM sceneryImporterPCM;
    if (!sceneryImporterPCM.Import(prevSceneryConfigFilePath.toStdString(),
                                   *pcmData, trajectoryMap))
    {
        return nullptr;
    }

    // get participants data from scenario.xosc of previous results
    QString prevScenarioConfigFilePath = configFolder + '/' + FILENAME_SCENARIO_CONFIG;
    ScenarioImporterPCM scenarioImporterPCM;
    if (!scenarioImporterPCM.Import(prevScenarioConfigFilePath.toStdString(), trajectoryMap))
    {
        return nullptr;
    }

    //get participants data from vehicleModelCatalog.xosc
    QString prevVehicleModelCatalogFilePath = configFolder + '/' + FILENAME_MODELS_CONFIG;
    VehicleModelImporter vehicleModelImporter;
    std::map<int, PCM_ParticipantData *> particpantsMap;
    if (!vehicleModelImporter.Import(prevVehicleModelCatalogFilePath.toStdString(), particpantsMap))
    {
        return nullptr;
    }

    std::vector<PCM_ParticipantData *> participants;
    for (auto &particpantsIterator : particpantsMap)
    {
        participants.push_back(particpantsIterator.second);
    }

    std::vector<PCM_InitialValues *> initials;
    std::vector<PCM_Trajectory *> trajectories;

    for (auto &trajectoryIterator : trajectoryMap)
    {
        trajectories.push_back(trajectoryIterator.second);
    }

    // retrieve initial values from trajectories
    for (const PCM_Trajectory *trajectory : trajectories)
    {
        initials.push_back(
            new PCM_InitialValues(QString::number(trajectory->GetXPosVec()->at(0)),
                                  QString::number(trajectory->GetYPosVec()->at(0)),
                                  QString::number(trajectory->GetVVelVec()->at(0)),
                                  QString::number(trajectory->GetUVelVec()->at(0)),
                                  QString::number(0),
                                  QString::number(0),
                                  QString::number(trajectory->GetPsiVec()->at(0))));
    }

    return new PCM_SimulationSet(participants,
                                 initials,
                                 trajectories,
                                 pcmData);
}
