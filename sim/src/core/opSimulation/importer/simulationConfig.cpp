/********************************************************************************
 * Copyright (c) 2018-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** \file  simulationConfig.cpp */
//-----------------------------------------------------------------------------

#include "simulationConfig.h"

namespace Configuration
{

ExperimentConfig &SimulationConfig::GetExperimentConfig()
{
    return experimentConfig;
}

ScenarioConfig &SimulationConfig::GetScenarioConfig()
{
    return scenarioConfig;
}

SpawnPointLibraryInfoCollection& SimulationConfig::GetSpawnPointsConfig()
{
    return spawnPointsConfig;
}

ObservationInstanceCollection& SimulationConfig::GetObservationConfig()
{
    return observationConfig;
}

EnvironmentConfig &SimulationConfig::GetEnvironmentConfig()
{
    return environmentConfig;
}

const std::string &SimulationConfig::GetProfilesCatalog() const
{
    return profilesCatalog;
}

void SimulationConfig::SetProfilesCatalog(std::string profilesCatalog)
{
    this->profilesCatalog = profilesCatalog;
}

} //namespace Configuration
