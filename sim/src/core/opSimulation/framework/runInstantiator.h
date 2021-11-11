/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2016-2018 ITK Engineering GmbH
 *               2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  RunInstantiator.h
//! @brief This file contains the component which triggers the simulation runs.
//-----------------------------------------------------------------------------

#pragma once

#include <QMutex>

#include "common/opExport.h"
#include <map>
#include <string>

#include "frameworkModules.h"

#include "include/agentFactoryInterface.h"
#include "include/configurationContainerInterface.h"
#include "include/dataBufferInterface.h"
#include "include/frameworkModuleContainerInterface.h"
#include "include/observationNetworkInterface.h"
#include "include/parameterInterface.h"
#include "include/stochasticsInterface.h"

namespace core {

class SIMULATIONCOREEXPORT RunInstantiator
{
public:
    RunInstantiator(ConfigurationContainerInterface& configurationContainer,
                    FrameworkModuleContainerInterface& frameworkModuleContainer,
                    FrameworkModules& frameworkModules) :
        configurationContainer(configurationContainer),
        observationNetwork(*frameworkModuleContainer.GetObservationNetwork()),
        agentFactory(*frameworkModuleContainer.GetAgentFactory()),
        agentBlueprintProvider(*frameworkModuleContainer.GetAgentBlueprintProvider()),
        eventNetwork(*frameworkModuleContainer.GetEventNetwork()),
        world(*frameworkModuleContainer.GetWorld()),
        spawnPointNetwork(*frameworkModuleContainer.GetSpawnPointNetwork()),
        stochastics(*frameworkModuleContainer.GetStochastics()),
        eventDetectorNetwork(*frameworkModuleContainer.GetEventDetectorNetwork()),
        manipulatorNetwork(*frameworkModuleContainer.GetManipulatorNetwork()),
        dataBuffer(*frameworkModuleContainer.GetDataBuffer()),
        frameworkModules{frameworkModules}
    {}

    //-----------------------------------------------------------------------------
    //! @brief Executes the run by preparing the stochastics, world and observation
    //!     network instances, then scheduling for each run invocation an update
    //!     on the observation network, finalizing the run invocation and, after all
    //!     invocations have terminated, the observation network itself.
    //!
    //! Executes the run by initializing the stochastics generator with the random
    //! seed from the run configuration, instantiating and initializing the observation
    //! network with the observation instances from the run configuration and the
    //! observation result path from the framework configuration. For each run
    //! invocation:
    //! - configure the world paremeters from the run configuration
    //! - import the scenery file and the world global objects
    //! - init the observation network run
    //! - instantiate the world's spawn point network
    //! - start the scheduling with the observation networks's UpdateTimeStep as
    //!     update callback
    //! - finalize the run invocation using the result on the observation network
    //!
    //! Finally, finalize the observation network and clear teh world.
    //!
    //! @return                             Flag if the update was successful
    //-----------------------------------------------------------------------------
    bool ExecuteRun();

    //-----------------------------------------------------------------------------
    //! Stops the current run.
    //-----------------------------------------------------------------------------
    //void StopRun();

private:
    bool InitPreRun(ScenarioInterface& scenario, SceneryInterface& scenery);
    bool InitRun(std::uint32_t seed, const EnvironmentConfig& environmentConfig, ProfilesInterface &profiles, RunResult& runResult);
    void InitializeFrameworkModules(ScenarioInterface &scenario);
    void InitializeSpawnPointNetwork();
    std::unique_ptr<ParameterInterface> SampleWorldParameters(const EnvironmentConfig& environmentConfig, const ProfileGroup& trafficRules, StochasticsInterface* stochastics, const openpass::common::RuntimeInformation& runtimeInformation);

    void ClearRun();

    QMutex stopMutex;
    bool stopped = true;

    ConfigurationContainerInterface& configurationContainer;
    ObservationNetworkInterface& observationNetwork;
    AgentFactoryInterface& agentFactory;
    AgentBlueprintProviderInterface& agentBlueprintProvider;
    EventNetworkInterface& eventNetwork;
    WorldInterface& world;
    SpawnPointNetworkInterface& spawnPointNetwork;
    StochasticsInterface& stochastics;
    EventDetectorNetworkInterface& eventDetectorNetwork;
    ManipulatorNetworkInterface& manipulatorNetwork;
    DataBufferInterface& dataBuffer;
    FrameworkModules& frameworkModules;

    std::unique_ptr<ParameterInterface> worldParameter;
};

} // namespace core
