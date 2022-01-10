/********************************************************************************
 * Copyright (c) 2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "include/sceneryDynamicsInterface.h"

class FakeSceneryDynamics : public SceneryDynamicsInterface
{
public:
    MOCK_CONST_METHOD0(GetEnvironment, openScenario::EnvironmentAction ());
    MOCK_CONST_METHOD0(GetTrafficSignalControllers, std::vector<openScenario::TrafficSignalController> ());
};
