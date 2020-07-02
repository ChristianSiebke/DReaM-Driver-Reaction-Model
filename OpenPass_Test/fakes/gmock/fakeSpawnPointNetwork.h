/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "Interfaces/spawnPointNetworkInterface.h"

namespace SimulationSlave {

class FakeSpawnPointNetwork : public SpawnPointNetworkInterface
{
public:
    MOCK_METHOD6(Instantiate,
                 bool(const SpawnPointLibraryInfoCollection&,
                      AgentFactoryInterface*,
                      AgentBlueprintProviderInterface*,
                      StochasticsInterface*,
                      ScenarioInterface*,
                      const std::optional<ProfileGroup>&));
    MOCK_METHOD0(TriggerPreRunSpawnPoints,
                 bool());
    MOCK_METHOD1(TriggerRuntimeSpawnPoints,
                 bool(const int));
    MOCK_METHOD0(ConsumeNewAgents,
                 std::vector<Agent*>());
    MOCK_METHOD0(Clear,
                 void());
};

}  // namespace SimulationSlave
