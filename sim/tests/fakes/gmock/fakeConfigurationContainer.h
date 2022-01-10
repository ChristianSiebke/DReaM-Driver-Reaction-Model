/********************************************************************************
 * Copyright (c) 2018 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "include/configurationContainerInterface.h"

class FakeConfigurationContainer : public ConfigurationContainerInterface
{
public:
    MOCK_METHOD0(ImportAllConfigurations,
                 bool());
    MOCK_METHOD0(GetSystemConfigBlueprint,
                 std::shared_ptr<SystemConfigInterface> ());
    MOCK_METHOD0(GetSimulationConfig,
                 SimulationConfigInterface * ());
    MOCK_METHOD0(GetScenery,
                 SceneryInterface * ());
    MOCK_METHOD0(GetScenario,
                 ScenarioInterface * ());
    MOCK_METHOD0(GetVehicleModels,
                 VehicleModelsInterface * ());
    MOCK_METHOD0(GetProfiles,
                 ProfilesInterface * ());
    MOCK_METHOD0(GetSystemConfigs,
                 std::map<std::string, std::shared_ptr<SystemConfigInterface>>& ());
};
