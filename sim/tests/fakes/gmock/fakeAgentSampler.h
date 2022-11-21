/********************************************************************************
 * Copyright (c) 2019-2020 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include "gmock/gmock.h"
#include "Interfaces/agentSamplerInterface.h"

class FakeAgentSampler : public AgentSamplerInterface
{
 public:
  MOCK_METHOD3(SampleAgent,
      bool(AgentBlueprintInterface*,
           LaneCategory,
           unsigned int));
};


