/*********************************************************************
* Copyright (c) 2019 in-tech GmbH on behalf of BMW
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "Interfaces/spawnPointNetworkInterface.h"

namespace SimulationSlave {

class FakeSpawnPointNetwork : public SpawnPointNetworkInterface
{
public:
    MOCK_METHOD6(Instantiate,
                 bool(std::string, AgentFactoryInterface*,
                      AgentBlueprintProviderInterface*,
                      ParameterInterface*,
                      const SamplerInterface&,
                      ScenarioInterface*));
    MOCK_METHOD0(Clear,
                 void());
    MOCK_METHOD0(GetSpawnPoint,
                 SpawnPoint * ());
};

}  // namespace SimulationSlave
