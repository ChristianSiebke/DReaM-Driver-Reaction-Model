/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
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

#include <opExport.h>
#include "worldInterface/world.h"
#include "Interfaces/frameworkModuleContainerInterface.h"

#include "agentFactory.h"
#include "agentBlueprintProvider.h"
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
#include "sampler.h"
#include "spawnPointInterface/spawnPointBinding.h"
#include "spawnPointNetwork.h"
#include "stochasticsInterface/stochastics.h"
#include "stochasticsInterface/stochasticsBinding.h"

#include "Interfaces/agentBlueprintInterface.h"
#include "Interfaces/configurationContainerInterface.h"

namespace SimulationSlave {

//-----------------------------------------------------------------------------
/** \brief This class instantiates and stores all core framework modules
*   \details
*/
//-----------------------------------------------------------------------------
class CORESLAVEEXPORT FrameworkModuleContainer : public FrameworkModuleContainerInterface
{
public:
    FrameworkModuleContainer(FrameworkModules frameworkModules,
                             ConfigurationContainerInterface* configurationContainer,
                             CallbackInterface* callbacks);

    virtual ~FrameworkModuleContainer() override = default;

    AgentFactoryInterface* GetAgentFactory() override;

    EventDetectorNetworkInterface* GetEventDetectorNetwork() override;

    EventNetworkInterface* GetEventNetwork() override;

    ManipulatorNetworkInterface* GetManipulatorNetwork() override;

    ObservationNetworkInterface* GetObservationNetwork() override;

    const SamplerInterface& GetSampler() const override;

    SpawnPointNetworkInterface* GetSpawnPointNetwork() override;

    StochasticsInterface* GetStochastics() override;

    WorldInterface* GetWorld() override;

    AgentBlueprintProviderInterface* GetAgentBlueprintProvider() override;

private:
    StochasticsBinding stochasticsBinding;
    Stochastics stochastics;

    WorldBinding worldBinding;
    World world;

    SpawnPointBinding spawnPointBinding;
    SpawnPointNetwork spawnPointNetwork;

    ObservationBinding observationBinding;
    ObservationNetwork observationNetwork;

    EventDetectorBinding eventDetectorBinding;
    EventDetectorNetwork eventDetectorNetwork;

    ManipulatorBinding manipulatorBinding;
    ManipulatorNetwork manipulatorNetwork;

    ModelBinding modelBinding;

    AgentFactory agentFactory;

    const Sampler sampler;

    AgentBlueprintProvider agentBlueprintProvider;

    EventNetwork eventNetwork;
};

} //namespace SimulationSlave
