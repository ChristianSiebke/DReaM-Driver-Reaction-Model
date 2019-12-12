/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  SpawnPointNetwork.h
//! @brief This file implements a container of the spawn points during a
//!        simulation run.
//-----------------------------------------------------------------------------

#pragma once

#include <memory>
#include <map>
#include <list>

#include "Interfaces/worldInterface.h"
#include "Interfaces/agentBlueprintProviderInterface.h"
#include "Interfaces/samplerInterface.h"
#include "Interfaces/scenarioInterface.h"
#include "Interfaces/spawnPointNetworkInterface.h"
#include "Common/runtimeInformation.h"
#include "spawnPointLibraryDefinitions.h"
#include "spawnPoint.h"

namespace SimulationSlave {
class SpawnPointBinding;

class SpawnPointNetwork : public SpawnPointNetworkInterface
{
public:
    SpawnPointNetwork(std::map<std::string, SpawnPointBinding> *spawnPointBindings,
                      WorldInterface* world,
                      const openpass::common::RuntimeInformation& runtimeInformation);
    SpawnPointNetwork(const SpawnPointNetwork&) = delete;
    SpawnPointNetwork(SpawnPointNetwork&&) = delete;
    SpawnPointNetwork& operator=(const SpawnPointNetwork&) = delete;
    SpawnPointNetwork& operator=(SpawnPointNetwork&&) = delete;
    virtual ~SpawnPointNetwork() override = default;

    bool Instantiate(const SpawnPointLibraryInfoCollection& libraryInfos,
                     AgentFactoryInterface* agentFactory,
                     AgentBlueprintProviderInterface* agentBlueprintProvider,
                     const SamplerInterface * const sampler,
                     ScenarioInterface* scenario,
                     const SpawnPointProfiles& spawnPointProfiles) override;

    bool TriggerPreRunSpawnPoints() override;

    bool TriggerRuntimeSpawnPoints(const int timestamp) override;

    std::vector<Agent*> ConsumeNewAgents() override
    {
        std::vector<Agent*> tmpAgents;
        std::swap(tmpAgents, newAgents);
        return tmpAgents;
    }

    void Clear() override;

private:
    std::map<std::string, SpawnPointBinding> * spawnPointBindings;
    WorldInterface* world;
    const openpass::common::RuntimeInformation& runtimeInformation;
    std::multimap<int, std::unique_ptr<SpawnPoint>> preRunSpawnPoints;
    std::multimap<int, std::unique_ptr<SpawnPoint>> runtimeSpawnPoints;
    std::vector<Agent*> newAgents {};
};

} // namespace SimulationSlave

