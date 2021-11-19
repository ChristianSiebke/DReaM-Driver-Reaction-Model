/********************************************************************************
 * Copyright (c) 2018-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <unordered_map>

#include "include/agentTypeInterface.h"
#include "include/simulationConfigInterface.h"
#include "include/profilesInterface.h"
#include "include/sceneryInterface.h"
#include "include/scenarioInterface.h"
#include "include/systemConfigInterface.h"
#include "include/vehicleModelsInterface.h"

class ConfigurationContainerInterface
{
public:
    virtual ~ConfigurationContainerInterface() = default;

    virtual bool ImportAllConfigurations() = 0;
    virtual std::shared_ptr<SystemConfigInterface> GetSystemConfigBlueprint() = 0;
    virtual SimulationConfigInterface* GetSimulationConfig() = 0;
    virtual ProfilesInterface* GetProfiles() = 0;
    virtual SceneryInterface* GetScenery() = 0;
    virtual ScenarioInterface* GetScenario() = 0;
    virtual std::map<std::string, std::shared_ptr<SystemConfigInterface>>& GetSystemConfigs() = 0;
    virtual VehicleModelsInterface* GetVehicleModels() = 0;
    virtual const openpass::common::RuntimeInformation& GetRuntimeInformation() const = 0;
};
