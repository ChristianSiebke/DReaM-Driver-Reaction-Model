/*********************************************************************
 * Copyright (c) 2018, 2019 in-tech
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 **********************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "Interfaces/manipulatorNetworkInterface.h"

class FakeManipulatorNetwork : public ManipulatorNetworkInterface
{
public:
    MOCK_METHOD3(Instantiate,
                 bool(std::string, ScenarioInterface *, SimulationSlave::EventNetworkInterface *));
    MOCK_METHOD0(Clear,
                 void());
    MOCK_METHOD0(GetManipulators,
                 std::vector<const SimulationSlave::Manipulator*>());
};
