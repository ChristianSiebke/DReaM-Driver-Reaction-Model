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
#include "Interfaces/agentBlueprintProviderInterface.h"
#include "Interfaces/agentBlueprintInterface.h"

class FakeAgentBlueprintProvider : public AgentBlueprintProviderInterface {
public:
    MOCK_CONST_METHOD1(SampleAgent, AgentBlueprint(const std::string& agentProfileName));
};


