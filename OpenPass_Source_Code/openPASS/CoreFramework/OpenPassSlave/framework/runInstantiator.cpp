/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include <functional>
#include <sstream>

#include "agentFactory.h"
#include "Interfaces/agentBlueprintProviderInterface.h"
#include "agentType.h"
#include "channel.h"
#include "component.h"
#include "CoreFramework/CoreShare/log.h"
#include "Interfaces/observationNetworkInterface.h"
#include "observationModule.h"
#include "runInstantiator.h"
#include "runResult.h"
#include "scheduler.h"
#include "spawnPointNetwork.h"
#include "stochastics.h"
#include "parameters.h"
#include "parameterbuilder.h"
#include "sampler.h"

constexpr char SPAWNER[] = {"Spawner"};

namespace SimulationSlave {

bool RunInstantiator::ExecuteRun()
{
    LOG_INTERN(LogLevel::DebugCore) << std::endl << "### execute run ###";

    stopMutex.lock();
    stopped = false;
    stopMutex.unlock();

    auto& scenario = *configurationContainer.GetScenario();
    auto& scenery = *configurationContainer.GetScenery();
    auto& slaveConfig = *configurationContainer.GetSlaveConfig();
    auto& experimentConfig = slaveConfig.GetExperimentConfig();
    auto& environmentConfig = slaveConfig.GetEnvironmentConfig();

    if (!InitPreRun(scenario, scenery))
    {
        LOG_INTERN(LogLevel::DebugCore) << std::endl << "### initialization failed ###";
        return false;
    }

    Scheduler scheduler(world, spawnPointNetwork, eventDetectorNetwork, manipulatorNetwork, observationNetwork);
    bool scheduler_state { false };

    for (auto invocation = 0; invocation < experimentConfig.numberOfInvocations; invocation++)
    {
        RunResult runResult;

        LOG_INTERN(LogLevel::DebugCore) << std::endl << "### run number: " << invocation << " ###";
        auto seed = static_cast<std::uint32_t>(experimentConfig.randomSeed + invocation);
        if (!InitRun(seed, environmentConfig, runResult))
        {
            LOG_INTERN(LogLevel::DebugCore) << std::endl << "### run initialization failed ###";
            break;
        }

        LOG_INTERN(LogLevel::DebugCore) << std::endl << "### run started ###";
        scheduler_state = scheduler.Run(0, scenario.GetEndTime(), runResult, eventNetwork);
        if (scheduler_state == Scheduler::FAILURE)
        {
            LOG_INTERN(LogLevel::DebugCore) << std::endl << "### run aborted ###";
            break;
        }
        LOG_INTERN(LogLevel::DebugCore) << std::endl << "### run successful ###";

        observationNetwork.FinalizeRun(runResult);
        ClearRun();
    }

    LOG_INTERN(LogLevel::DebugCore) << std::endl << "### end of all runs ###";
    bool observations_state = observationNetwork.FinalizeAll();

    return (scheduler_state && observations_state);
}

bool RunInstantiator::InitPreRun(ScenarioInterface& scenario, SceneryInterface& scenery)
{
    try
    {
        InitializeFrameworkModules(scenario);
        world.CreateScenery(&scenery);
        return true;
    }
    catch(const std::exception& error)
    {
        LOG_INTERN(LogLevel::Error) << std::endl << "### could not init: "  << error.what() << "###";
        return false;
    }

    LOG_INTERN(LogLevel::Error) << std::endl << "### exception caught, which is not of type std::exception! ###";
    return false;
}

void RunInstantiator::InitializeFrameworkModules(ScenarioInterface& scenario)
{
    ThrowIfFalse(stochastics.Instantiate(frameworkModules.stochasticsLibrary),
                 "Failed to instantiate Stochastics");
    ThrowIfFalse(world.Instantiate(),
                 "Failed to instantiate World");
    ThrowIfFalse(eventDetectorNetwork.Instantiate(frameworkModules.eventDetectorLibrary, &scenario, &eventNetwork, &stochastics),
                 "Failed to instantiate EventDetectorNetwork");
    ThrowIfFalse(manipulatorNetwork.Instantiate(frameworkModules.manipulatorLibrary, &scenario, &eventNetwork),
                 "Failed to instantiate ManipulatorNetwork");
    ThrowIfFalse(observationNetwork.Instantiate(frameworkModules.observationLibraries, &stochastics, &world, &eventNetwork, scenario.GetSceneryPath()),
                 "Failed to instantiate ObservationNetwork");
    ThrowIfFalse(observationNetwork.InitAll(),
                 "Failed to initialize ObservationNetwork");
}

void RunInstantiator::InitializeSpawnPointNetwork()
{
    const auto &profileGroups = configurationContainer.GetProfiles()->GetProfileGroups();
    bool existingSpawnProfiles = profileGroups.find(SPAWNER) != profileGroups.end();

    ThrowIfFalse(spawnPointNetwork.Instantiate(frameworkModules.spawnPointLibraries,
                 &agentFactory,
                 &agentBlueprintProvider,
                 &stochastics,
                 configurationContainer.GetScenario(),
                 existingSpawnProfiles ? std::make_optional(profileGroups.at(SPAWNER)) : std::nullopt), "Failed to instantiate SpawnPointNetwork");
}

std::unique_ptr<ParameterInterface> RunInstantiator::SampleWorldParameters(const EnvironmentConfig& environmentConfig, StochasticsInterface* stochastics, const openpass::common::RuntimeInformation& runtimeInformation)
{
    return openpass::parameter::make<SimulationCommon::Parameters>(
        runtimeInformation, openpass::parameter::ParameterSetLevel1 {
            { "TimeOfDay", Sampler::Sample(environmentConfig.timeOfDays, stochastics) },
            { "VisibilityDistance", Sampler::Sample(environmentConfig.visibilityDistances, stochastics) },
            { "Friction", Sampler::Sample(environmentConfig.frictions, stochastics) },
            { "Weather", Sampler::Sample(environmentConfig.weathers, stochastics) }}
    );
}

bool RunInstantiator::InitRun(std::uint32_t seed, const EnvironmentConfig& environmentConfig, RunResult& runResult)
{
    try
    {
        stochastics.InitGenerator(seed);

        worldParameter = SampleWorldParameters(environmentConfig, &stochastics, configurationContainer.GetRuntimeInformation());
        world.ExtractParameter(worldParameter.get());

        observationNetwork.InitRun();
        InitializeSpawnPointNetwork();

        eventNetwork.Initialize(&runResult);
        return true;
    }
    catch(const std::exception& error)
    {
        LOG_INTERN(LogLevel::Error) << std::endl << "### could not init run: "  << error.what() << "###";
        return false;
    }

    LOG_INTERN(LogLevel::Error) << std::endl << "### exception caught, which is not of type std::exception! ###";
    return false;
}

void RunInstantiator::ClearRun()
{
    world.Reset();
    agentFactory.Clear();
    spawnPointNetwork.Clear();
    eventNetwork.Clear();
    eventDetectorNetwork.ResetAll();
}

} // namespace SimulationSlave
