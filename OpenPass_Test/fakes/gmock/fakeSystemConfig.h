/*********************************************************************
* Copyright (c) 2019 in-tech
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "Interfaces/systemConfigInterface.h"

class FakeSystemConfig : public SystemConfigInterface
{
public:
    MOCK_METHOD0(GetSystems,
                 std::map<int, std::shared_ptr< SimulationSlave::AgentTypeInterface>>& ());
    MOCK_METHOD1(SetSystems,
                 void (std::map<int, std::shared_ptr< SimulationSlave::AgentTypeInterface>> systems));
    MOCK_METHOD0(AddModelParameters,
                 ParameterInterface* ());
    MOCK_METHOD1(AddModelParameters,
                 void (std::shared_ptr<ParameterInterface> modelParameters));
};
