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

#include "agent.h"
#include "agentParser.h"
#include "eventNetwork.h"
#include "CoreFramework/CoreShare/log.h"
#include "runResult.h"
#include "scheduler.h"
//-----------------------------------------------------------------------------
/** \file  Scheduler.cpp */
//-----------------------------------------------------------------------------

namespace SimulationSlave {

Scheduler::Scheduler(WorldInterface& world,
                     SpawnPointNetworkInterface& spawnPointNetwork,
                     EventDetectorNetworkInterface& eventDetectorNetwork,
                     ManipulatorNetworkInterface& manipulatorNetwork,
                     ObservationNetworkInterface& observationNetwork) :
    world(world),
    spawnPointNetwork(spawnPointNetwork),
    eventDetectorNetwork(eventDetectorNetwork),
    manipulatorNetwork(manipulatorNetwork),
    observationNetwork(observationNetwork)
{
}

bool Scheduler::Run(
    int startTime,
    int endTime,
    RunResult& runResult,
    EventNetworkInterface& eventNetwork)
{
    if (startTime > endTime)
    {
        LOG_INTERN(LogLevel::Error) << "start time greater than end time";
        return Scheduler::FAILURE;
    }

    currentTime = startTime;

    TaskBuilder taskBuilder(currentTime,
                            runResult,
                            FRAMEWORK_UPDATE_RATE,
                            &world,
                            &spawnPointNetwork,
                            &observationNetwork,
                            &eventDetectorNetwork,
                            &manipulatorNetwork);

    auto bootstrapTasks = taskBuilder.CreateBootstrapTasks();
    auto commonTasks = taskBuilder.CreateCommonTasks();
    auto finalizeRecurringTasks = taskBuilder.CreateFinalizeRecurringTasks();
    auto finalizeTasks = taskBuilder.CreateFinalizeTasks();

    auto taskList = SchedulerTasks(
                   bootstrapTasks,
                   commonTasks,
                   finalizeRecurringTasks,
                   finalizeTasks,
                   FRAMEWORK_UPDATE_RATE);

    if (ExecuteTasks(taskList.GetBootstrapTasks()) == false)
    {
        return Scheduler::FAILURE;
    }

    while (currentTime <= endTime)
    {
        if (!ExecuteTasks(taskList.GetCommonTasks(currentTime)))
        {
            return Scheduler::FAILURE;
        }

        UpdateAgents(taskList, world);

        if (!ExecuteTasks(taskList.ConsumeNonRecurringTasks(currentTime)))
        {
            return Scheduler::FAILURE;
        }

        if (!ExecuteTasks(taskList.GetRecurringTasks(currentTime)))
        {
            return Scheduler::FAILURE;
        }

        currentTime = taskList.GetNextTimestamp(currentTime);

        if (runResult.IsEndCondition())
        {
            LOG_INTERN(LogLevel::DebugCore) << "Scheduler: End of operation (end condition reached)";
            return Scheduler::SUCCESS;
        }

        eventNetwork.ClearActiveEvents();
    }

    if (!ExecuteTasks(taskList.GetFinalizeTasks()))
    {
        return Scheduler::FAILURE;
    }

    LOG_INTERN(LogLevel::DebugCore) << "Scheduler: End of operation (end time reached)";
    return Scheduler::SUCCESS;
}

template<typename T>
bool Scheduler::ExecuteTasks(T tasks)
{
    for (const auto& task : tasks)
    {
        if (task.func() == false)
        {
            return false;
        }
    }
    return true;
}

void Scheduler::UpdateAgents(SchedulerTasks& taskList, WorldInterface& world)
{
    for (const auto& agent : spawnPointNetwork.ConsumeNewAgents())
    {
        ScheduleAgentTasks(taskList, *agent);
    }

    std::list<int> removedAgents;
    for (const auto& agentMap : world.GetAgents())
    {
        AgentInterface* agent = agentMap.second;
        if (!agent->IsValid())
        {
            removedAgents.push_back(agent->GetId());
            world.QueueAgentRemove(agent);
        }
    }
    taskList.DeleteAgentTasks(removedAgents);
}

void Scheduler::ScheduleAgentTasks(SchedulerTasks& taskList, const Agent& agent)
{
    AgentParser agentParser(currentTime);
    agentParser.Parse(agent);

    taskList.ScheduleNewRecurringTasks(agentParser.GetRecurringTasks());
    taskList.ScheduleNewNonRecurringTasks(agentParser.GetNonRecurringTasks());
}

} // namespace SimulationSlave
