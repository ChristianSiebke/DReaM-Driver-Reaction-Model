/*******************************************************************************
* Copyright (c) 2018, 2019 in-tech GmbH
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

namespace TAG = openpass::importer::xml::slaveConfigImporter::tag;
namespace ATTRIBUTE = openpass::importer::xml::slaveConfigImporter::attribute;

using namespace Importer;
using namespace SimulationCommon;

std::string SlaveConfigImporter::GetLibrary(const QDomElement& root, std::string tag, std::string defaultValue)
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
    for (auto [tagName, defaultValue] : defaultLibraryMapping)
    {
        libs.try_emplace(tagName, SlaveConfigImporter::GetLibrary(libsRoot, tagName, defaultValue));
    }
    return libs;
}

void SlaveConfigImporter::ImportLoggingGroups(QDomElement loggingGroupsElement,
        std::vector<std::string>& loggingGroups)
{
    const auto& loggingGroupElements = loggingGroupsElement.elementsByTagName(TAG::loggingGroup);

    for (auto i = loggingGroupElements.length() - 1; i >= 0; --i)
    {
        const auto& loggingGroupElement = loggingGroupElements.at(i).toElement();

        ThrowIfFalse(!loggingGroupElement.isNull(), "Error parsing LoggingGroup elements");

        std::string groupName = loggingGroupElement.text().toStdString();

        ThrowIfFalse(groupName.length() != 0, "Invalid LoggingGroup name (empty string)");

        loggingGroups.push_back(groupName);
    }
}

void SlaveConfigImporter::ImportExperimentConfig(QDomElement experimentConfigElement,
        ExperimentConfig& experimentConfig)
{
    ThrowIfFalse(ParseInt(experimentConfigElement, "ExperimentID", experimentConfig.experimentId), "ExperimentID not valid.");

    ThrowIfFalse(ParseInt(experimentConfigElement, "NumberOfInvocations", experimentConfig.numberOfInvocations), "NumberOfInvocations not valid.");

    unsigned long randomSeed;
    ThrowIfFalse(ParseULong(experimentConfigElement, "RandomSeed", randomSeed), "RandomSeed not valid.");

    experimentConfig.randomSeed = static_cast<std::uint32_t>(randomSeed);

    bool logCyclicsToCsv;
    if (!ParseBool(experimentConfigElement, "LoggingCyclicsToCsv", logCyclicsToCsv))
    {
        LOG_INTERN(LogLevel::Info) << "Tag LogCyclicsToCsv undefined, falling back to default value false.";
        logCyclicsToCsv = false;
    }
    experimentConfig.logCyclicsToCsv = logCyclicsToCsv;

    // Logging groups
    QDomElement loggingGroupsElement;
    ThrowIfFalse(GetFirstChildElement(experimentConfigElement, TAG::loggingGroups, loggingGroupsElement),
                 "Could not import LoggingGroups.");

    ImportLoggingGroups(loggingGroupsElement, experimentConfig.loggingGroups);

    experimentConfig.libraries = ImportLibraries(experimentConfigElement);
}

void SlaveConfigImporter::ImportScenarioConfig(QDomElement scenarioConfigElement,
        const std::string configurationDir,
        ScenarioConfig& scenarioConfig)
{
    std::string scenarioFilename;
    ThrowIfFalse(ParseString(scenarioConfigElement, "OpenScenarioFile", scenarioFilename),
                 "OpenScenarioFile not valid.");

    scenarioConfig.scenarioPath = Directories::Concat(configurationDir, scenarioFilename);
}

void SlaveConfigImporter::ImportEnvironmentConfig(QDomElement environmentConfigElement,
        EnvironmentConfig& environmentConfig)
{
    //Parse all time of days
    QDomElement timeOfDaysElement;
    ThrowIfFalse(GetFirstChildElement(environmentConfigElement, TAG::timeOfDays, timeOfDaysElement)
                    && ImportProbabilityMap(timeOfDaysElement, "Value", TAG::timeOfDay, environmentConfig.timeOfDays),
                 "Could not import TimeOfDays.");

    //Parse all visibility distances
    QDomElement visibilityDistancesElement;
    ThrowIfFalse(GetFirstChildElement(environmentConfigElement, TAG::visibilityDistances, visibilityDistancesElement)
                    && ImportProbabilityMap(visibilityDistancesElement, "Value", TAG::visibilityDistance, environmentConfig.visibilityDistances),
        "Could not import VisibilityDistances.");

    //Parse all frictions
    QDomElement frictionsElement;
    ThrowIfFalse(GetFirstChildElement(environmentConfigElement, TAG::frictions, frictionsElement)
                    && ImportProbabilityMap(frictionsElement, "Value", TAG::friction, environmentConfig.frictions),
                 "Could not import Frictions.");

    //Parse all weathers
    QDomElement weathersElement;
    ThrowIfFalse(GetFirstChildElement(environmentConfigElement, TAG::weathers, weathersElement)
                    && ImportProbabilityMap(weathersElement, "Value", TAG::weather, environmentConfig.weathers),
                 "Could not import Weathers.");
}

void SlaveConfigImporter::ImportTrafficParameter(QDomElement trafficParameterElement,
        TrafficConfig& trafficConfig)
{
    QDomElement trafficVolumesElement;
    ThrowIfFalse(GetFirstChildElement(trafficParameterElement, TAG::trafficVolumes, trafficVolumesElement)
                    && ImportProbabilityMap(trafficVolumesElement, "Value", TAG::trafficVolume, trafficConfig.trafficVolumes),
                 "Could not import TrafficVolumes.");

    QDomElement platoonRatesElement;
    ThrowIfFalse(GetFirstChildElement(trafficParameterElement, TAG::platoonRates, platoonRatesElement)
                    && ImportProbabilityMap(platoonRatesElement, "Value", TAG::platoonRate, trafficConfig.platoonRates),
                 "Could not import PlatoonRates.");

    QDomElement velocitiesElement;
    ThrowIfFalse(GetFirstChildElement(trafficParameterElement, TAG::velocities, velocitiesElement)
                    && ImportProbabilityMap(velocitiesElement, "Value", TAG::velocity, trafficConfig.velocities),
                 "Could not import Velocities.");

    QDomElement homogenitiesElement;
    ThrowIfFalse(GetFirstChildElement(trafficParameterElement, TAG::homogenities, homogenitiesElement)
                    && ImportProbabilityMap(homogenitiesElement, "Value", TAG::homogenity, trafficConfig.homogenities),
                 "Could not import Homogenities.");
}

void SlaveConfigImporter::ImportLaneParameter(QDomElement trafficConfigElement, TrafficConfig& trafficConfig)
{
    QDomElement regularLaneElement;
    ThrowIfFalse(GetFirstChildElement(trafficConfigElement, TAG::regularLane, regularLaneElement)
                    && ImportProbabilityMap(regularLaneElement, "Name", TAG::agentProfile, trafficConfig.regularLaneAgents),
                 "Could not import RegularLane.");

    QDomElement rightMostLaneElement;
    ThrowIfFalse(GetFirstChildElement(trafficConfigElement, TAG::rightMostLane, rightMostLaneElement)
                    && ImportProbabilityMap(rightMostLaneElement, "Name", TAG::agentProfile, trafficConfig.rightMostLaneAgents),
                 "Could not import RightMostLane.");
}

void SlaveConfigImporter::ImportTrafficConfig(QDomElement trafficConfigElement, TrafficConfig& trafficConfig)
{
    QDomElement trafficParameterElement;
    ThrowIfFalse(GetFirstChildElement(trafficConfigElement, TAG::trafficParameter, trafficParameterElement),
                 "Could not import TrafficParameter.");

    ImportTrafficParameter(trafficParameterElement, trafficConfig);
    ImportLaneParameter(trafficConfigElement, trafficConfig);
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

        //Import profiles catalog
        std::string profilesCatalog;
        ThrowIfFalse(ParseString(documentRoot, "ProfilesCatalog", profilesCatalog), "Could not import Proifles Catalog.");

        slaveConfig.SetProfilesCatalog(Directories::Concat(configurationDir, profilesCatalog));


        //Import experiment config
        QDomElement experimentConfigElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::experimentConfig, experimentConfigElement),
                     "Could not import ExperimentConfig.");

        ImportExperimentConfig(experimentConfigElement, slaveConfig.GetExperimentConfig());

        //Import scenario config
        QDomElement scenarioConfigElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::scenarioConfig, scenarioConfigElement),
                     "Could not import ScenarioConfig.");

        ImportScenarioConfig(scenarioConfigElement, configurationDir, slaveConfig.GetScenarioConfig());

        //Import environment config
        QDomElement environmentConfigElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::environmentConfig, environmentConfigElement),
                     "Could not import EnvironmentConfig.");

        ImportEnvironmentConfig(environmentConfigElement, slaveConfig.GetEnvironmentConfig());

        //Import traffic config
        QDomElement trafficConfigElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::trafficConfig, trafficConfigElement),
                     "Could not import TrafficConfig.");

        ImportTrafficConfig(trafficConfigElement, slaveConfig.GetTrafficConfig());

        return true;
    }
    catch (const std::runtime_error& e)
    {
        LOG_INTERN(LogLevel::Error) << "SlaveConfig import failed: " + std::string(e.what());
        return false;
    }
}
