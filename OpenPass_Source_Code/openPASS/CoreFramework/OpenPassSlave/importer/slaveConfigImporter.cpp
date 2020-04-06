/*******************************************************************************
* Copyright (c) 2018, 2019, 2020 in-tech GmbH
*               2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  SlaveConfigImporter.cpp */
//-----------------------------------------------------------------------------

#include <cmath>

#include "slaveConfigImporter.h"
#include "CoreFramework/CoreShare/log.h"
#include "importerLoggingHelper.h"

namespace TAG = openpass::importer::xml::slaveConfigImporter::tag;
namespace ATTRIBUTE = openpass::importer::xml::slaveConfigImporter::attribute;

using namespace Importer;
using namespace SimulationCommon;

std::string SlaveConfigImporter::GetLibrary(const QDomElement& root, const std::string& tag, const std::string& defaultValue)
{
    std::string parsedName;
    if (!Parse(root, tag, parsedName))
    {
        LOG_INTERN(LogLevel::Info) << "Library " + tag << " undefined, falling back to default value " + defaultValue;
        return defaultValue;
    }
    return parsedName;
}

ExperimentConfig::Libraries SlaveConfigImporter::ImportLibraries(QDomElement rootElement)
{
    QDomElement libsRoot;
    if (!SimulationCommon::GetFirstChildElement(rootElement, TAG::libraries, libsRoot))
    {
        LOG_INTERN(LogLevel::Warning) << "No libraries found. Falling back to default values";
        return defaultLibraryMapping;
    }

    ExperimentConfig::Libraries libs;
    for (const auto& [tagName, defaultValue] : defaultLibraryMapping)
    {
        libs.try_emplace(tagName, SlaveConfigImporter::GetLibrary(libsRoot, tagName, defaultValue));
    }
    return libs;
}

void SlaveConfigImporter::ImportExperiment(QDomElement experimentElement,
        ExperimentConfig& experimentConfig)
{
    ThrowIfFalse(ParseInt(experimentElement, "ExperimentID", experimentConfig.experimentId),
                 experimentElement, "ExperimentID not valid.");

    ThrowIfFalse(ParseInt(experimentElement, "NumberOfInvocations", experimentConfig.numberOfInvocations),
                 experimentElement, "NumberOfInvocations not valid.");

    unsigned long randomSeed;
    ThrowIfFalse(ParseULong(experimentElement, "RandomSeed", randomSeed),
                 experimentElement, "RandomSeed not valid.");

    experimentConfig.randomSeed = static_cast<std::uint32_t>(randomSeed);

    experimentConfig.libraries = ImportLibraries(experimentElement);
}

void SlaveConfigImporter::ImportScenario(QDomElement scenarioElement,
        const std::string configurationDir,
        ScenarioConfig& scenarioConfig)
{
    std::string scenarioFilename;
    ThrowIfFalse(ParseString(scenarioElement, "OpenScenarioFile", scenarioFilename),
                 scenarioElement, "OpenScenarioFile not valid.");

    scenarioConfig.scenarioPath = openpass::core::Directories::Concat(configurationDir, scenarioFilename);
}

void SlaveConfigImporter::ImportEnvironment(QDomElement environmentElement,
        EnvironmentConfig& environmentConfig)
{
    //Parse all time of days
    QDomElement timeOfDaysElement;
    ThrowIfFalse(GetFirstChildElement(environmentElement, TAG::timeOfDays, timeOfDaysElement),
                 environmentElement, "Tag " + std::string(TAG::timeOfDays) + " is missing.");
    ThrowIfFalse(ImportProbabilityMap(timeOfDaysElement, "Value", TAG::timeOfDay, environmentConfig.timeOfDays),
                 timeOfDaysElement, "Could not import Probabilities.");

    //Parse all visibility distances
    QDomElement visibilityDistancesElement;
    ThrowIfFalse(GetFirstChildElement(environmentElement, TAG::visibilityDistances, visibilityDistancesElement),
                 environmentElement, "Tag " + std::string(TAG::visibilityDistances) + " is missing.");
    ThrowIfFalse(ImportProbabilityMap(visibilityDistancesElement, "Value", TAG::visibilityDistance, environmentConfig.visibilityDistances),
                 visibilityDistancesElement, "Could not import Probabilities.");

    //Parse all frictions
    QDomElement frictionsElement;
    ThrowIfFalse(GetFirstChildElement(environmentElement, TAG::frictions, frictionsElement),
                 environmentElement, "Tag " + std::string(TAG::frictions) + " is missing.");
    ThrowIfFalse(ImportProbabilityMap(frictionsElement, "Value", TAG::friction, environmentConfig.frictions),
                 frictionsElement, "Could not import Probabilities.");

    //Parse all weathers
    QDomElement weathersElement;
    ThrowIfFalse(GetFirstChildElement(environmentElement, TAG::weathers, weathersElement),
                 environmentElement, "Tag " + std::string(TAG::weathers) + " is missing.");
    ThrowIfFalse(ImportProbabilityMap(weathersElement, "Value", TAG::weather, environmentConfig.weathers),
                 weathersElement, "Could not import Probabilities.");
}

void SlaveConfigImporter::ImportSpawners(const QDomElement &spawnersElement,
                                                  SpawnPointLibraryInfoCollection& spawnPointsInfo)
{
    QDomElement spawnPointElement;
    ThrowIfFalse(GetFirstChildElement(spawnersElement, TAG::spawner, spawnPointElement),
                 spawnersElement, "Tag " + std::string(TAG::spawner) + " is missing.");

    while (!spawnPointElement.isNull())
    {
        SpawnPointLibraryInfo spawnPointInfo;

        ThrowIfFalse(ParseString(spawnPointElement, TAG::library, spawnPointInfo.libraryName),
                     spawnPointElement, "Tag " + std::string(TAG::library) + " is missing.");

        std::string type;
        ThrowIfFalse(ParseString(spawnPointElement, TAG::type, type),
                     spawnPointElement, "Tag " + std::string(TAG::type) + " is missing.");
        const auto& spawnPointTypeIter = std::find_if(spawnPointTypeMapping.cbegin(),
                                                      spawnPointTypeMapping.cend(),
                                                      [&type](const auto& spawnPointTypePair) -> bool
                                                      {
                                                          return spawnPointTypePair.second == type;
                                                      });
        ThrowIfFalse(spawnPointTypeIter != spawnPointTypeMapping.cend(),
                     spawnPointElement, "SpawnPoint Type invalid");
        spawnPointInfo.type = spawnPointTypeIter->first;

        ThrowIfFalse(ParseInt(spawnPointElement, TAG::priority, spawnPointInfo.priority),
                     spawnPointElement, "Tag " + std::string(TAG::priority) + " is missing.");

        std::string spawnPointProfile;
        if(ParseString(spawnPointElement, "Profile", spawnPointProfile))
        {
            spawnPointInfo.profileName.emplace(spawnPointProfile);
        }

        spawnPointsInfo.emplace_back(spawnPointInfo);

        spawnPointElement = spawnPointElement.nextSiblingElement(TAG::spawner);
    }
}

void SlaveConfigImporter::ImportObservations(const QDomElement& observationsElement, ObservationInstanceCollection& observations)
{
    QDomElement observationElement;
    ThrowIfFalse(GetFirstChildElement(observationsElement, TAG::observation, observationElement),
                 observationsElement, "Tag " + std::string(TAG::observation) + " is missing.");

    while (!observationElement.isNull())
    {
        ObservationInstance observationInstance;

        ThrowIfFalse(ParseString(observationElement, TAG::library, observationInstance.libraryName),
                     observationElement, "Tag " + std::string(TAG::library) + " is missing.");

        QDomElement parametersElement;
        ThrowIfFalse(GetFirstChildElement(observationElement, TAG::parameters, parametersElement),
                     observationsElement, "Tag " + std::string(TAG::parameters) + " is missing.");
        observationInstance.parameters = openpass::parameter::Import(parametersElement, observationsElement);

        observations.emplace_back(observationInstance);

        observationElement = observationElement.nextSiblingElement(TAG::observation);
    }
}

bool SlaveConfigImporter::Import(const std::string& configurationDir,
                                 const std::string& slaveConfigFile,
                                 Configuration::SlaveConfig& slaveConfig)
{
    try
    {
        std::locale::global(std::locale("C"));

        QFile xmlFile(slaveConfigFile.c_str()); // automatic object will be closed on destruction
        ThrowIfFalse(xmlFile.open(QIODevice::ReadOnly), "an error occurred during slave configuration import");

        QByteArray xmlData(xmlFile.readAll());
        QDomDocument document;
        ThrowIfFalse(document.setContent(xmlData), "invalid xml file format of file " + slaveConfigFile);

        QDomElement documentRoot = document.documentElement();
        if (documentRoot.isNull())
        {
            return false;
        }

        std::string configVersion;
        ParseAttributeString(documentRoot, ATTRIBUTE::schemaVersion, configVersion);
        ThrowIfFalse(configVersion.compare(supportedConfigVersion) == 0, "SlaveConfig version not suppored. Supported version is " + std::string(supportedConfigVersion));

        std::string profilesCatalog;
        ThrowIfFalse(ParseString(documentRoot, "ProfilesCatalog", profilesCatalog), "Could not import Proifles Catalog.");

        slaveConfig.SetProfilesCatalog(openpass::core::Directories::Concat(configurationDir, profilesCatalog));


        auto& experimentConfig = slaveConfig.GetExperimentConfig();
        QDomElement experimentConfigElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::experiment, experimentConfigElement),
                     "Tag " + std::string(TAG::experiment) + " is missing.");
        ImportExperiment(experimentConfigElement, experimentConfig);

        QDomElement scenarioConfigElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::scenario, scenarioConfigElement),
                     "Tag " + std::string(TAG::scenario) + " is missing.");
        ImportScenario(scenarioConfigElement, configurationDir, slaveConfig.GetScenarioConfig());

        QDomElement environmentConfigElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::environment, environmentConfigElement),
                     "Tag " + std::string(TAG::environment) + " is missing.");
        ImportEnvironment(environmentConfigElement, slaveConfig.GetEnvironmentConfig());

        QDomElement observationConfigElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::observations, observationConfigElement),
                     "Tag " + std::string(TAG::observations) + " is missing.");
        ImportObservations(observationConfigElement, slaveConfig.GetObservationConfig());

        QDomElement spawnPointsConfigElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::spawners, spawnPointsConfigElement),
                     "Tag " + std::string(TAG::spawners) + " is missing.");
        ImportSpawners(spawnPointsConfigElement, slaveConfig.GetSpawnPointsConfig());

        return true;
    }
    catch (const std::runtime_error& e)
    {
        LOG_INTERN(LogLevel::Error) << "SlaveConfig import failed: " + std::string(e.what());
        return false;
    }
}
