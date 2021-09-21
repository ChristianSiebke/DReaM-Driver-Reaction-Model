/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020, 2021 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "runInstantiator.h"

#include <functional>
#include <sstream>

#include "common/log.h"
#include "include/agentBlueprintProviderInterface.h"
#include "include/observationNetworkInterface.h"
#include "include/eventDetectorNetworkInterface.h"
#include "include/manipulatorNetworkInterface.h"
#include "agentFactory.h"
#include "agentType.h"
#include "channel.h"
#include "component.h"
#include "bindings/dataBuffer.h"
#include "observationModule.h"
#include "modelElements/parameters.h"
#include "scheduler/runResult.h"
#include "scheduler/scheduler.h"
#include "spawnPointNetwork.h"
#include "bindings/stochastics.h"
#include "parameterbuilder.h"
#include "sampler.h"

constexpr char SPAWNER[] = {"Spawner"};

namespace core {

bool RunInstantiator::ExecuteRun()
{
    LOG_INTERN(LogLevel::DebugCore) << std::endl
                                    << "### execute run ###";

    stopMutex.lock();
    stopped = false;
    stopMutex.unlock();

    auto &scenario = *configurationContainer.GetScenario();
    auto &scenery = *configurationContainer.GetScenery();
    auto &simulationConfig = *configurationContainer.GetSimulationConfig();
    auto &profiles = *configurationContainer.GetProfiles();
    auto &experimentConfig = simulationConfig.GetExperimentConfig();
    auto &environmentConfig = simulationConfig.GetEnvironmentConfig();

    if (!InitPreRun(scenario, scenery))
    {
        LOG_INTERN(LogLevel::DebugCore) << std::endl
                                        << "### initialization failed ###";
        return false;
    }

    dataBuffer.PutStatic("SceneryFile", scenario.GetSceneryPath(), true);
    ThrowIfFalse(observationNetwork.InitAll(), "Failed to initialize ObservationNetwork");

    core::scheduling::Scheduler scheduler(world, spawnPointNetwork, eventDetectorNetwork, manipulatorNetwork, observationNetwork, dataBuffer);
    bool scheduler_state{false};

    for (auto invocation = 0; invocation < experimentConfig.numberOfInvocations; invocation++)
    {
        RunResult runResult;

        LOG_INTERN(LogLevel::DebugCore) << std::endl
                                        << "### run number: " << invocation << " ###";
        auto seed = static_cast<std::uint32_t>(experimentConfig.randomSeed + invocation);
        if (!InitRun(seed, environmentConfig, profiles, runResult))
        {
            LOG_INTERN(LogLevel::DebugCore) << std::endl
                                            << "### run initialization failed ###";
            break;
        }

        LOG_INTERN(LogLevel::DebugCore) << std::endl
                                        << "### run started ###";
        scheduler_state = scheduler.Run(0, scenario.GetEndTime(), runResult, eventNetwork);
        if (scheduler_state == core::scheduling::Scheduler::FAILURE)
        {
            LOG_INTERN(LogLevel::DebugCore) << std::endl
                                            << "### run aborted ###";
            break;
        }
        LOG_INTERN(LogLevel::DebugCore) << std::endl
                                        << "### run successful ###";

        observationNetwork.FinalizeRun(runResult);
        ClearRun();
    }

    LOG_INTERN(LogLevel::DebugCore) << std::endl
                                    << "### end of all runs ###";
    bool observations_state = observationNetwork.FinalizeAll();

    return (scheduler_state && observations_state);
}

bool RunInstantiator::InitPreRun(ScenarioInterface& scenario, SceneryInterface& scenery)
{
    try
    {
        InitializeFrameworkModules(scenario);
        world.CreateScenery(&scenery, scenario.GetSceneryDynamics());
        return true;
    }
    catch (const std::exception &error)
    {
        LOG_INTERN(LogLevel::Error) << std::endl
                                    << "### could not init: " << error.what() << "###";
        return false;
    }

    LOG_INTERN(LogLevel::Error) << std::endl
                                << "### exception caught, which is not of type std::exception! ###";
    return false;
}

void RunInstantiator::InitializeFrameworkModules(ScenarioInterface& scenario)
{
    ThrowIfFalse(dataBuffer.Instantiate(),
                 "Failed to instantiate DataBuffer");
    ThrowIfFalse(stochastics.Instantiate(frameworkModules.stochasticsLibrary),
                 "Failed to instantiate Stochastics");
    ThrowIfFalse(world.Instantiate(),
                 "Failed to instantiate World");
    ThrowIfFalse(eventDetectorNetwork.Instantiate(frameworkModules.eventDetectorLibrary, &scenario, &eventNetwork, &stochastics),
                 "Failed to instantiate EventDetectorNetwork");
    ThrowIfFalse(manipulatorNetwork.Instantiate(frameworkModules.manipulatorLibrary, &scenario, &eventNetwork),
                 "Failed to instantiate ManipulatorNetwork");
    ThrowIfFalse(observationNetwork.Instantiate(frameworkModules.observationLibraries, &stochastics, &world, &eventNetwork, scenario.GetSceneryPath(), &dataBuffer),
                 "Failed to instantiate ObservationNetwork");
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
                                               existingSpawnProfiles ? std::make_optional(profileGroups.at(SPAWNER)) : std::nullopt),
                 "Failed to instantiate SpawnPointNetwork");
}

std::unique_ptr<ParameterInterface> RunInstantiator::SampleWorldParameters(const EnvironmentConfig& environmentConfig, const ProfileGroup& trafficRules, StochasticsInterface* stochastics, const openpass::common::RuntimeInformation& runtimeInformation)
{
    auto trafficRule = helper::map::query(trafficRules, environmentConfig.trafficRules);
    ThrowIfFalse(trafficRule.has_value(), "No traffic rule set with name " + environmentConfig.trafficRules + " defined in ProfilesCatalog");
    auto parameters = trafficRule.value();
    parameters.emplace_back("TimeOfDay", Sampler::Sample(environmentConfig.timeOfDays, stochastics));
    parameters.emplace_back("VisibilityDistance", Sampler::Sample(environmentConfig.visibilityDistances, stochastics));
    parameters.emplace_back("Friction", Sampler::Sample(environmentConfig.frictions, stochastics));
    parameters.emplace_back("Weather", Sampler::Sample(environmentConfig.weathers, stochastics));

    return openpass::parameter::make<SimulationCommon::Parameters>(runtimeInformation, parameters);
}
bool RunInstantiator::InitRun(std::uint32_t seed, const EnvironmentConfig &environmentConfig, ProfilesInterface& profiles, RunResult &runResult)
{
    try
    {
        stochastics.InitGenerator(seed);

        auto trafficRules = helper::map::query(profiles.GetProfileGroups(), "TrafficRules");
        ThrowIfFalse(trafficRules.has_value(), "No traffic rules defined in ProfilesCatalog");
        worldParameter = SampleWorldParameters(environmentConfig, trafficRules.value(), &stochastics, configurationContainer.GetRuntimeInformation());
        world.ExtractParameter(worldParameter.get());

        observationNetwork.InitRun();
        InitializeSpawnPointNetwork();

        eventNetwork.Initialize(&runResult);
        return true;
    }
    catch (const std::exception &error)
    {
        LOG_INTERN(LogLevel::Error) << std::endl
                                    << "### could not init run: " << error.what() << "###";
        return false;
    }

    LOG_INTERN(LogLevel::Error) << std::endl
                                << "### exception caught, which is not of type std::exception! ###";
    return false;
}

void RunInstantiator::ClearRun()
{
    world.Reset();
    agentFactory.Clear();
    spawnPointNetwork.Clear();
    eventNetwork.Clear();
    eventDetectorNetwork.ResetAll();
    dataBuffer.ClearRun();
}

} // namespace core
