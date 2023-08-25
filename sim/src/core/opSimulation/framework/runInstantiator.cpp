/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "runInstantiator.h"

#include <functional>
#include <sstream>

#include <QCommandLineParser>
#include <qcoreapplication.h>

#include "../../../tudresden/components/DriverReactionModel/AgentStateRecorder/AgentStateRecorder.h" //DReaM
#include "../../../tudresden/components/GlobalObserver/GlobalObserverMain/GlobalObserver_main.h"     //DReaM
#include "../../../tudresden/components/GlobalObserver/Analytics/AnalysisDataRecorder.h"             //DReaM
#include "agentFactory.h"
#include "agentType.h"
#include "bindings/dataBuffer.h"
#include "bindings/stochastics.h"
#include "channel.h"
#include "common/log.h"
#include "component.h"
#include "core/opSimulation/framework/commandLineParser.h"
#include "include/agentBlueprintProviderInterface.h"
#include "include/eventDetectorNetworkInterface.h"
#include "include/manipulatorNetworkInterface.h"
#include "include/observationNetworkInterface.h"
#include "modelElements/parameters.h"
#include "observationModule.h"
#include "parameterbuilder.h"
#include "sampler.h"
#include "scheduler/runResult.h"
#include "scheduler/scheduler.h"
#include "spawnPointNetwork.h"

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
    //--- TU Dresden
    GlobalObserver::Main::SetProfiles(configurationContainer.GetProfiles());
    auto arguments = QCoreApplication::arguments();
    auto parsedArguments = CommandLineParser::Parse(arguments);
    std::string scenarioConfigPath =
        QCoreApplication::applicationDirPath().toStdString() + "\\" + parsedArguments.configsPath + "\\" + "Scenario.xosc";
    GlobalObserver::Main::SetScenarioConfigPath(scenarioConfigPath);
    std::string scenarioResultsPath = QCoreApplication::applicationDirPath().toStdString() + "\\" +
                                      CommandLineParser::Parse(QCoreApplication::arguments()).resultsPath + "\\";
    GlobalObserver::AnalysisDataRecorder::SetScenarioConfigPath(scenarioResultsPath);
    //--
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

        LOG_INTERN(LogLevel::Error) << std::endl
                                        << "### run number: " << invocation << " ###";
        auto seed = static_cast<std::uint32_t>(experimentConfig.randomSeed + invocation);
        if (!InitRun(seed, environmentConfig, profiles, runResult))
        {
            LOG_INTERN(LogLevel::Error) << std::endl
                                            << "### run initialization failed ###";
            break;
        }

        LOG_INTERN(LogLevel::DebugCore) << std::endl
                                        << "### run started ###";
        scheduler_state = scheduler.Run(0, scenario.GetEndTime(), runResult, eventNetwork);
        if (scheduler_state == core::scheduling::Scheduler::FAILURE)
        {
            LOG_INTERN(LogLevel::Error) << std::endl
                                            << "### run aborted ###";
            break;
        }
        LOG_INTERN(LogLevel::DebugCore) << std::endl
                                        << "### run successful ###";
        AgentStateRecorder::AgentStateRecorder::BufferRuns(invocation);
        AgentStateRecorder::AgentStateRecorder::ResetAgentStateRecorder(); // DReaM agents record
        GlobalObserver::Main::Reset();

        GlobalObserver::AnalysisDataRecorder::SetRunId(invocation); // DReaM: hand over run id
        GlobalObserver::AnalysisDataRecorder::Reset();              // DReaM agents record

        observationNetwork.FinalizeRun(runResult);
        ClearRun();
        std::cout << "Run:" << invocation << " terminated successful" << std::endl;
    }
    AgentStateRecorder::AgentStateRecorder::WriteOutputFile(); // DReaM: write output files
    GlobalObserver::AnalysisDataRecorder::WriteOutput();

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
