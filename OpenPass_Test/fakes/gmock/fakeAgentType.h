/*********************************************************************
* Copyright (c) 2018 - 2019 in-tech
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "Interfaces/agentTypeInterface.h"

namespace SimulationSlave {

class FakeAgentType : public AgentTypeInterface {
 public:
  MOCK_METHOD1(AddChannel,
      bool(int id));
  MOCK_METHOD1(AddComponent,
      bool(std::shared_ptr<ComponentType> component));
  MOCK_CONST_METHOD0(GetChannels,
      const std::list<int>&());
  MOCK_CONST_METHOD0(GetComponents,
      const std::map<std::string, std::shared_ptr<ComponentType>>&());
};

}  // namespace SimulationSlave
