/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "frameworkModuleContainer.h"

#include "frameworkModules.h"

namespace SimulationSlave {

FrameworkModuleContainer::FrameworkModuleContainer(
    FrameworkModules frameworkModules,
    ConfigurationContainerInterface *configurationContainer,
    const openpass::common::RuntimeInformation &runtimeInformation,
    CallbackInterface *callbacks) :
    dataStoreBinding(frameworkModules.dataStoreLibrary, runtimeInformation, callbacks),
    dataStore(&dataStoreBinding),
    stochasticsBinding(callbacks),
    stochastics(&stochasticsBinding),
    worldBinding(frameworkModules.worldLibrary, callbacks, &stochastics),
    world(&worldBinding),
    observationNetwork(&observationBindings),
    eventDetectorBinding(callbacks),
    eventDetectorNetwork(&eventDetectorBinding, &world),
    manipulatorBinding(callbacks),
    manipulatorNetwork(&manipulatorBinding, &world),
    modelBinding(frameworkModules.libraryDir, runtimeInformation, callbacks),
    agentFactory(&modelBinding, &world, &stochastics, &observationNetwork, &eventNetwork, &dataStore),
    agentBlueprintProvider(configurationContainer, stochastics),
    eventNetwork(&dataStore),
    spawnPointNetwork(&spawnPointBindings, &world, runtimeInformation)
{
    for (const auto &libraryInfo : frameworkModules.spawnPointLibraries)
    {
        spawnPointBindings.emplace(libraryInfo.libraryName, SpawnPointBinding(callbacks));
    }

    for (const auto& libraryInfo : frameworkModules.observationLibraries)
    {
        observationBindings.emplace(libraryInfo.libraryName, ObservationBinding(runtimeInformation, callbacks));
    }
}

AgentBlueprintProviderInterface *FrameworkModuleContainer::GetAgentBlueprintProvider()
{
    return &agentBlueprintProvider;
}

AgentFactoryInterface *FrameworkModuleContainer::GetAgentFactory()
{
    return &agentFactory;
}

DataStoreInterface *FrameworkModuleContainer::GetDataStore()
{
    return &dataStore;
}

EventDetectorNetworkInterface *FrameworkModuleContainer::GetEventDetectorNetwork()
{
    return &eventDetectorNetwork;
}

EventNetworkInterface *FrameworkModuleContainer::GetEventNetwork()
{
    return &eventNetwork;
}

ManipulatorNetworkInterface *FrameworkModuleContainer::GetManipulatorNetwork()
{
    return &manipulatorNetwork;
}

ObservationNetworkInterface *FrameworkModuleContainer::GetObservationNetwork()
{
    return &observationNetwork;
}

SpawnPointNetworkInterface *FrameworkModuleContainer::GetSpawnPointNetwork()
{
    return &spawnPointNetwork;
}

StochasticsInterface *FrameworkModuleContainer::GetStochastics()
{
    return &stochastics;
}

WorldInterface *FrameworkModuleContainer::GetWorld()
{
    return &world;
}

} //namespace SimulationSlave
