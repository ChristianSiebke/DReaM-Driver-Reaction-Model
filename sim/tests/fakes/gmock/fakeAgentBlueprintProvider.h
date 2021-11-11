/********************************************************************************
 * Copyright (c) 2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "include/agentBlueprintProviderInterface.h"
#include "include/agentBlueprintInterface.h"

class FakeAgentBlueprintProvider : public AgentBlueprintProviderInterface {
public:
    MOCK_CONST_METHOD2(SampleAgent, AgentBlueprint(const std::string& agentProfileName, const openScenario::Parameters& assignedParameters));
};


