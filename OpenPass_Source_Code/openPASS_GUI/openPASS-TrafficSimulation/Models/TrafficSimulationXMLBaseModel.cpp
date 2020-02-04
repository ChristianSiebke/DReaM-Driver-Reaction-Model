/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "TrafficSimulationXMLBaseModel.h"

QString const TrafficSimulationXMLBaseModel::KeyRoot = QStringLiteral("slaveConfig");
QString const TrafficSimulationXMLBaseModel::KeyProfilesCatalogue = QStringLiteral("ProfilesCatalog");

QString const TrafficSimulationXMLBaseModel::KeyExperiment = QStringLiteral("ExperimentConfig");
QString const TrafficSimulationXMLBaseModel::KeyExperimentID = QStringLiteral("ExperimentID");
QString const TrafficSimulationXMLBaseModel::KeyExperimentNumberInvocations = QStringLiteral("NumberOfInvocations");
QString const TrafficSimulationXMLBaseModel::KeyExperimentSeed = QStringLiteral("RandomSeed");
QString const TrafficSimulationXMLBaseModel::KeyExperimentLibraries = QStringLiteral("Libraries");
QString const TrafficSimulationXMLBaseModel::KeyExperimentLoggingGroups = QStringLiteral("LoggingGroups");
QString const TrafficSimulationXMLBaseModel::KeyExperimentLoggingGroup = QStringLiteral("LoggingGroup");

QList<QString> const TrafficSimulationXMLBaseModel::KeyListExperiment =
{
    TrafficSimulationXMLBaseModel::KeyExperimentID,
    TrafficSimulationXMLBaseModel::KeyExperimentNumberInvocations,
    TrafficSimulationXMLBaseModel::KeyExperimentSeed,
    TrafficSimulationXMLBaseModel::KeyExperimentLibraries,
    TrafficSimulationXMLBaseModel::KeyExperimentLoggingGroups,
};





QString const TrafficSimulationXMLBaseModel::KeyEnvironment = QStringLiteral("EnvironmentConfig");
QString const TrafficSimulationXMLBaseModel::KeyTimeOfDayMap = QStringLiteral("TimeOfDays");
QString const TrafficSimulationXMLBaseModel::KeyTimeOfDay = QStringLiteral("TimeOfDay");
QString const TrafficSimulationXMLBaseModel::KeyVisibilityDistanceMap = QStringLiteral("VisibilityDistances");
QString const TrafficSimulationXMLBaseModel::KeyVisibilityDistance = QStringLiteral("VisibilityDistance");
QString const TrafficSimulationXMLBaseModel::KeyFrictionMap = QStringLiteral("Frictions");
QString const TrafficSimulationXMLBaseModel::KeyFriction = QStringLiteral("Friction");
QString const TrafficSimulationXMLBaseModel::KeyWeatherMap = QStringLiteral("Weathers");
QString const TrafficSimulationXMLBaseModel::KeyWeather = QStringLiteral("Weather");

QList<QString> const TrafficSimulationXMLBaseModel::KeyListEnvironment =
{
    TrafficSimulationXMLBaseModel::KeyTimeOfDayMap,
    TrafficSimulationXMLBaseModel::KeyVisibilityDistanceMap,
    TrafficSimulationXMLBaseModel::KeyFrictionMap,
    TrafficSimulationXMLBaseModel::KeyWeatherMap,
};





QString const TrafficSimulationXMLBaseModel::KeyEgo = QStringLiteral("EgoConfig");
QString const TrafficSimulationXMLBaseModel::KeyEgoAgent = QStringLiteral("EgoAgentProfile");

QList<QString> const TrafficSimulationXMLBaseModel::KeyListEgo =
{
    TrafficSimulationXMLBaseModel::KeyEgoAgent,
};



QString const TrafficSimulationXMLBaseModel::KeyScenario = QStringLiteral("ScenarioConfig");
QString const TrafficSimulationXMLBaseModel::KeyScenarioFile = QStringLiteral("OpenScenarioFile");
QString const TrafficSimulationXMLBaseModel::KeyScenarioAgents = QStringLiteral("ScenarioAgentProfiles");
QString const TrafficSimulationXMLBaseModel::KeyScenarioAgent = QStringLiteral("ScenarioAgentProfile");


QList<QString> const TrafficSimulationXMLBaseModel::KeyListScenario =
{
    TrafficSimulationXMLBaseModel::KeyScenarioFile,
};



QString const TrafficSimulationXMLBaseModel::KeyTraffic= QStringLiteral("TrafficConfig");
QString const TrafficSimulationXMLBaseModel::KeyTrafficParameters= QStringLiteral("TrafficParameter");
QString const TrafficSimulationXMLBaseModel::KeyTrafficRegularLane= QStringLiteral("RegularLane");
QString const TrafficSimulationXMLBaseModel::KeyTrafficRightmostLane= QStringLiteral("RightMostLane");

QString const TrafficSimulationXMLBaseModel::KeyTrafficAgent= QStringLiteral("AgentProfile");
QString const TrafficSimulationXMLBaseModel::KeyTrafficVolumes= QStringLiteral("TrafficVolumes");
QString const TrafficSimulationXMLBaseModel::KeyTrafficVolume= QStringLiteral("TrafficVolume");
QString const TrafficSimulationXMLBaseModel::KeyTrafficPlatoonRates= QStringLiteral("PlatoonRates");
QString const TrafficSimulationXMLBaseModel::KeyTrafficPlatoonRate= QStringLiteral("PlatoonRate");
QString const TrafficSimulationXMLBaseModel::KeyTrafficVelocities= QStringLiteral("Velocities");
QString const TrafficSimulationXMLBaseModel::KeyTrafficVelocity= QStringLiteral("Velocity");
QString const TrafficSimulationXMLBaseModel::KeyTrafficHomogeneities= QStringLiteral("Homogenities");
QString const TrafficSimulationXMLBaseModel::KeyTrafficHomogeneity= QStringLiteral("Homogenity");


QList<QString> const TrafficSimulationXMLBaseModel::KeyListTraffic =
{
    TrafficSimulationXMLBaseModel::KeyTrafficParameters,
    TrafficSimulationXMLBaseModel::KeyTrafficRegularLane,
    TrafficSimulationXMLBaseModel::KeyTrafficRightmostLane,
};


QList<QString> const TrafficSimulationXMLBaseModel::KeyListTrafficParameters =
{
    TrafficSimulationXMLBaseModel::KeyTrafficVolumes,
    TrafficSimulationXMLBaseModel::KeyTrafficPlatoonRates,
    TrafficSimulationXMLBaseModel::KeyTrafficVelocities,
    TrafficSimulationXMLBaseModel::KeyTrafficHomogeneities,
};

