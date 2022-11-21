/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "TrafficSimulationModel.h"

TrafficSimulationModel::TrafficSimulationModel(QObject *const parent) :
    TrafficSimulationInterface(parent), experiment(new ExperimentModel(this)), environment(new EnvironmentModel(this)), scenario(new ScenarioModel(this)), traffic(new TrafficModel(this)), profilesCatalogue("")
{
}

ExperimentInterface *TrafficSimulationModel::getExperiment() const
{
    return experiment;
}

EnvironmentInterface *TrafficSimulationModel::getEnvironment() const
{
    return environment;
}

ScenarioInterface *TrafficSimulationModel::getScenario() const
{
    return scenario;
}

TrafficInterface *TrafficSimulationModel::getTraffic() const
{
    return traffic;
}

TrafficSimulationInterface::ProfilesCatalogue TrafficSimulationModel::getProfilesCatalogue() const
{
    return profilesCatalogue;
}

void TrafficSimulationModel::setProfilesCatalogue(const ProfilesCatalogue &_profilesCatalogue)
{
    profilesCatalogue = _profilesCatalogue;
}
