/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "gmock/gmock.h"

#include "Interfaces/agentFactoryInterface.h"


class FakeAgentFactory : public SimulationSlave::AgentFactoryInterface
{
public:
    MOCK_METHOD0(ResetIds, void());
    MOCK_METHOD0(Clear, void());
    MOCK_METHOD1(AddAgent, SimulationSlave::Agent*(AgentBlueprintInterface*));
};
