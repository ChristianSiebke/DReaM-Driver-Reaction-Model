/********************************************************************************
 * Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "sceneryDynamics.h"

SceneryDynamics::SceneryDynamics()
{
}

openScenario::EnvironmentAction SceneryDynamics::GetEnvironment() const
{
    return environment;
}

void SceneryDynamics::SetEnvironment(const openScenario::EnvironmentAction &environment)
{
    this->environment = environment;
}

std::vector<openScenario::TrafficSignalController> SceneryDynamics::GetTrafficSignalControllers() const
{
    return trafficSignalControllers;
}

void SceneryDynamics::AddTrafficSignalController(const openScenario::TrafficSignalController &controller)
{
    trafficSignalControllers.push_back(controller);
}
