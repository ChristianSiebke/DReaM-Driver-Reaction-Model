/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "ConfigGeneratorPcm.h"

#include "GUI_Definitions.h"
#include "sceneryImporterPCM.h"
#include "scenarioImporterPCM.h"
#include "vehicleModelImporter.h"

ConfigGenerator::ConfigGenerator() :
    ConfigGenerator(baseFolder)
{
}

ConfigGenerator::ConfigGenerator(const QString &baseFolder)
{
    this->baseFolder = baseFolder;
    configWriter = new ConfigWriter(baseFolder);
}

ConfigGenerator::~ConfigGenerator()
{
    delete configWriter;
}

bool ConfigGenerator::GenerateConfigs(const QString &pcmCase,
                                      const QString &caseOutputFolderName,
                                      const QString &otherSystemFile,
                                      const QString &car1SystemFile,
                                      const QString &car2SystemFile,
                                      const int randomSeed,
                                      const PCM_SimulationSet *simulationSet)
{
    if (pcmCase == "")
    {
        return false;
    }

    QString caseOutputFolder = baseFolder + "/" + caseOutputFolderName;
    QString caseResultFolder = caseOutputFolder + "/" + DIRNAME_CASE_RESULTS;
    QString caseConfigFolder = caseOutputFolder + "/" + FILENAME_OPENPASSSIMULATION_CONFIGS;

    if (!CheckDirs(caseResultFolder))
    {
        return false;
    }
    if (!CheckDirs(caseConfigFolder))
    {
        return false;
    }

    const QString systemConfigFile = configWriter->CreateSystemConfiguration(caseConfigFolder,
                                                                             otherSystemFile,
                                                                             car1SystemFile,
                                                                             car2SystemFile,
                                                                             simulationSet);
    if (systemConfigFile == "")
    {
        return false;
    }

    const QString runConfigFile = configWriter->CreateSimulationConfiguration(caseConfigFolder,
                                                                         simulationSet,
                                                                         caseResultFolder,
                                                                         pcmCase,
                                                                         randomSeed);
    if (runConfigFile == "")
    {
        return false;
    }

    const QString profilesCatalogFile = configWriter->CreateProfilesCatalog(caseConfigFolder,
                                                                            simulationSet,
                                                                            caseResultFolder,
                                                                            pcmCase,
                                                                            randomSeed);
    if (profilesCatalogFile == "")
    {
        return false;
    }

    const QString modelsVehicleFile = configWriter->CreateModelsVehicle(caseConfigFolder,
                                                                        simulationSet,
                                                                        caseResultFolder,
                                                                        pcmCase,
                                                                        randomSeed);
    if (modelsVehicleFile == "")
    {
        return false;
    }

    const QString sceneryConfigFile = configWriter->CreateSceneryConfiguration(caseConfigFolder,
                                                                               simulationSet);
    if (sceneryConfigFile == "")
    {
        return false;
    }

    const QString parkingConfigFile = configWriter->CreateParkingConfiguration(caseConfigFolder);
    if (parkingConfigFile == "")
    {
        return false;
    }

    QString scenarioFile = caseConfigFolder + "/" + FILENAME_SCENARIO_CONFIG;
    OpenScenarioWriter openScenarioWriter;
    const QString scenarioConfigFile = openScenarioWriter.CreateScenarioFile(scenarioFile,
                                                                             simulationSet);

    if (scenarioConfigFile == "")
    {
        return false;
    }

    AddConfigSet(caseOutputFolder);

    return true;
}

const QString ConfigGenerator::GenerateFrameworkConfig(const int logLevel)
{
    return configWriter->CreateFrameworkConfiguration(baseFolder + "/..", configSetList, logLevel);
}

void ConfigGenerator::AddConfigSet(QString resultFolderName)
{
    QMap<QString, QString> configSet;

    configSet.insert("logFileSimulation", resultFolderName + "/" + FILENAME_OPENPASSSIMULATION_LOG);
    configSet.insert("configurations", resultFolderName + "/" + FILENAME_OPENPASSSIMULATION_CONFIGS);
    configSet.insert("results", resultFolderName + "/" + DIRNAME_CASE_RESULTS);

    configSetList.append(configSet);
}

void ConfigGenerator::Clear()
{
    configSetList.clear();
}

bool ConfigGenerator::CheckDirs(const QString myDir)
{
    QDir myDear(myDir);
    if (myDear.exists())
    {
        if (!myDear.removeRecursively())
        {
            return false;
        }
    }
    if (!myDear.mkpath(myDear.absolutePath()))
    {
        return false;
    }
    return true;
}
