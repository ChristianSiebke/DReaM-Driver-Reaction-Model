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

#include "CoreFramework/CoreShare/log.h"
#include "parameterbuilder.h"
#include "agentFactory.h"
#include "spawnPoint.h"
#include "spawnPointNetwork.h"
#include "spawnPointBinding.h"

namespace SimulationSlave {

SpawnPointNetwork::SpawnPointNetwork(std::map<std::string, SpawnPointBinding>* spawnPointBindings,
                                     WorldInterface* world,
                                     const openpass::common::RuntimeInformation& runtimeInformation) :
    spawnPointBindings{spawnPointBindings},
    world{world},
    runtimeInformation {runtimeInformation}
{}

void SpawnPointNetwork::Clear()
{
    preRunSpawnPoints.clear();
    runtimeSpawnPoints.clear();
}

bool SpawnPointNetwork::Instantiate(const SpawnPointLibraryInfoCollection& libraryInfos,
                                    AgentFactoryInterface *agentFactory,
                                    AgentBlueprintProviderInterface *agentBlueprintProvider,
                                    const SamplerInterface * const sampler,
                                    ScenarioInterface* scenario,
                                    const SpawnPointProfiles& spawnPointProfiles)
{
    for (const auto& libraryInfo : libraryInfos)
    {
        const auto bindingIter = spawnPointBindings->find(libraryInfo.libraryName);
        if (bindingIter == spawnPointBindings->end())
        {
            return false;
        }

        auto binding = &bindingIter->second;
        SpawnPointDependencies dependencies(agentFactory, world, agentBlueprintProvider, sampler);

        if (libraryInfo.profileName.has_value())
        {
            try
            {
                auto parameter = openpass::parameter::make<SimulationCommon::Parameters>(
                            runtimeInformation,
                            spawnPointProfiles.at(libraryInfo.profileName.value()));

                dependencies.parameters = parameter.get(); // get spawnpoint access to the parameters
                binding->SetParameter(std::move(parameter)); // the transfer ownership away from network
            }
            catch (const std::out_of_range&)
            {
                LOG_INTERN(LogLevel::Error) << "Could not load spawn point profile \"" << libraryInfo.profileName.value() << "\"";
                return false;
            }
        }
        else
        {
            dependencies.scenario = scenario;
        }

        auto spawnPoint = binding->Instantiate(libraryInfo.libraryName, dependencies);
        if (!spawnPoint)
        {
            LOG_INTERN(LogLevel::DebugCore) << "could not initialize spawn point with library path\"" << libraryInfo.libraryName << "\"";
            return false;
        }

        switch (libraryInfo.type)
        {
        case SpawnPointType::PreRun:
            preRunSpawnPoints.emplace(libraryInfo.priority, std::move(spawnPoint));
            break;
        case SpawnPointType::Runtime:
            runtimeSpawnPoints.emplace(libraryInfo.priority, std::move(spawnPoint));
            break;
        }
    }

    return true;
}

bool SpawnPointNetwork::TriggerPreRunSpawnPoints()
{
    try {
        std::for_each(preRunSpawnPoints.crbegin(), preRunSpawnPoints.crend(),
        [&](auto const& element)
        {
            const auto spawnedAgents = element.second->GetImplementation()->Trigger();
            newAgents.insert(newAgents.cend(), spawnedAgents.cbegin(), spawnedAgents.cend());
        });
    }
    catch (const std::runtime_error& error)
    {
        LOG_INTERN(LogLevel::Error) << error.what();
        return false;
    }

    return true;
}

bool SpawnPointNetwork::TriggerRuntimeSpawnPoints([[maybe_unused]]const int timestamp)
{
    try {
        std::for_each(runtimeSpawnPoints.crbegin(), runtimeSpawnPoints.crend(),
        [&](auto const& element)
        {
            const auto spawnedAgents = element.second->GetImplementation()->Trigger();
            newAgents.insert(newAgents.cend(), spawnedAgents.cbegin(), spawnedAgents.cend());
        });
    }
    catch (const std::runtime_error& error)
    {
        LOG_INTERN(LogLevel::Error) << error.what();
        return false;
    }

    return true;
}

} // namespace SimulationSlave
