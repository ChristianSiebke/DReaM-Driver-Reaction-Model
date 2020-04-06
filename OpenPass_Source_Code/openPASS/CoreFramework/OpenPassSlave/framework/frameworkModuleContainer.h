/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  FrameworkModuleContainer.h
*   \brief This file stores all core framework modules
*/
//-----------------------------------------------------------------------------

#pragma once

#include "Interfaces/agentBlueprintInterface.h"
#include "Interfaces/configurationContainerInterface.h"
#include "Interfaces/frameworkModuleContainerInterface.h"
#include "agentBlueprintProvider.h"
#include "agentFactory.h"
#include "dataStore.h"
#include "dataStoreInterface/dataStoreBinding.h"
#include "directories.h"
#include "eventDetectorInterface/eventDetectorBinding.h"
#include "eventDetectorNetwork.h"
#include "eventNetwork.h"
#include "frameworkModules.h"
#include "manipulatorInterface/manipulatorBinding.h"
#include "manipulatorNetwork.h"
#include "modelBinding.h"
#include "observationInterface/observationBinding.h"
#include "observationNetwork.h"
#include "spawnPointInterface/spawnPointBinding.h"
#include "spawnPointNetwork.h"
#include "stochasticsInterface/stochastics.h"
#include "stochasticsInterface/stochasticsBinding.h"
#include "worldInterface/world.h"

namespace SimulationSlave {

class FrameworkModuleContainer final : public FrameworkModuleContainerInterface
{
public:
    FrameworkModuleContainer(FrameworkModules frameworkModules,
                             ConfigurationContainerInterface *configurationContainer,
                             const openpass::common::RuntimeInformation &runtimeInformation,
                             CallbackInterface *callbacks);

    AgentBlueprintProviderInterface *GetAgentBlueprintProvider() override;
    AgentFactoryInterface *GetAgentFactory() override;
    DataStoreInterface *GetDataStore() override;
    EventDetectorNetworkInterface *GetEventDetectorNetwork() override;
    EventNetworkInterface *GetEventNetwork() override;
    ManipulatorNetworkInterface *GetManipulatorNetwork() override;
    ObservationNetworkInterface *GetObservationNetwork() override;
    SpawnPointNetworkInterface *GetSpawnPointNetwork() override;
    StochasticsInterface *GetStochastics() override;
    WorldInterface *GetWorld() override;

private:
    DataStoreBinding dataStoreBinding;
    DataStore dataStore;

    StochasticsBinding stochasticsBinding;
    Stochastics stochastics;

    WorldBinding worldBinding;
    World world;

    std::map<std::string, ObservationBinding> observationBindings;
    ObservationNetwork observationNetwork;

    EventDetectorBinding eventDetectorBinding;
    EventDetectorNetwork eventDetectorNetwork;

    ManipulatorBinding manipulatorBinding;
    ManipulatorNetwork manipulatorNetwork;

    ModelBinding modelBinding;

    AgentFactory agentFactory;

    AgentBlueprintProvider agentBlueprintProvider;

    EventNetwork eventNetwork;

    std::map<std::string, SpawnPointBinding> spawnPointBindings;
    SpawnPointNetwork spawnPointNetwork;
};

} //namespace SimulationSlave
