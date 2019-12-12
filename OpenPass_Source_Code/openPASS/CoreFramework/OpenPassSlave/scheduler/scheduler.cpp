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

Scheduler::Scheduler(WorldInterface* world,
                     SpawnPointNetworkInterface* spawnPointNetwork,
                     EventDetectorNetworkInterface* eventDetectorNetwork,
                     ManipulatorNetworkInterface* manipulatorNetwork,
                     ObservationNetworkInterface* observationNetwork) :
    world(world),
    spawnPointNetwork(spawnPointNetwork),
    eventDetectorNetwork(eventDetectorNetwork),
    manipulatorNetwork(manipulatorNetwork),
    observationNetwork(observationNetwork)
{
}

SchedulerReturnState Scheduler::Run(
    int startTime,
    int endTime,
    RunResult& runResult,
    EventNetworkInterface* eventNetwork)
{
    if (startTime > endTime)
    {
        LOG_INTERN(LogLevel::Error) << "start time greater than end time";
        return SchedulerReturnState::AbortSimulation;
    }
    currentTime = startTime;

    TaskBuilder taskBuilder(currentTime,
                            runResult,
                            frameworkUpdateRate,
                            world,
                            spawnPointNetwork,
                            observationNetwork,
                            eventDetectorNetwork,
                            manipulatorNetwork);

    auto bootstrapTasks = taskBuilder.CreateBootstrapTasks();
    auto commonTasks = taskBuilder.CreateCommonTasks();
    auto finalizeRecurringTasks = taskBuilder.CreateFinalizeRecurringTasks();
    auto finalizeTasks = taskBuilder.CreateFinalizeTasks();

    taskList = std::unique_ptr<SchedulerTasks>(new SchedulerTasks(
                   bootstrapTasks,
                   commonTasks,
                   finalizeRecurringTasks,
                   finalizeTasks,
                   frameworkUpdateRate));

    if (ExecuteTasks(taskList->GetBootstrapTasks()) == false)
    {
        return SchedulerReturnState::AbortInvocation;
    }

    while (currentTime <= endTime)
    {
        if (!ExecuteTasks(taskList->GetCommonTasks(currentTime)))
        {
            return SchedulerReturnState::AbortSimulation;
        }

        UpdateAgents(world);

        if (!ExecuteTasks(taskList->ConsumeNonRecurringTasks(currentTime)))
        {
            return SchedulerReturnState::AbortSimulation;
        }

        if (!ExecuteTasks(taskList->GetRecurringTasks(currentTime)))
        {
            return SchedulerReturnState::AbortSimulation;
        }

        currentTime = taskList->GetNextTimestamp(currentTime);

        if (runResult.IsEndCondition())
        {
            LOG_INTERN(LogLevel::DebugCore) << "Scheduler: End of operation (end condition reached)";
            return SchedulerReturnState::NoError;
        }

        eventNetwork->ClearActiveEvents();
    }

    if (!ExecuteTasks(taskList->GetFinalizeTasks()))
    {
        return SchedulerReturnState::AbortSimulation;
    }

    LOG_INTERN(LogLevel::DebugCore) << "Scheduler: End of operation (end time reached)";
    return SchedulerReturnState::NoError;
}

template<typename T>
bool Scheduler::ExecuteTasks(T tasks)
{
    for (const auto& task : tasks)
    {
        if (task.func() == false)
        {
            failedTaskItem = &task;
            return false;
        }
    }
    return true;
}

void Scheduler::UpdateAgents(WorldInterface* world)
{
    for (const auto& agent : spawnPointNetwork->ConsumeNewAgents())
    {
        ScheduleAgentTasks(*agent);
    }

    std::list<int> removedAgents;
    for (const auto& agentMap : world->GetAgents())
    {
        AgentInterface* agent = agentMap.second;
        if (!agent->IsValid())
        {
            removedAgents.push_back(agent->GetId());
            world->QueueAgentRemove(agent);
        }
    }
    taskList->DeleteAgentTasks(removedAgents);
}

void Scheduler::ScheduleAgentTasks(const Agent& agent)
{
    AgentParser agentParser(currentTime);
    agentParser.Parse(agent);

    taskList->ScheduleNewRecurringTasks(agentParser.GetRecurringTasks());
    taskList->ScheduleNewNonRecurringTasks(agentParser.GetNonRecurringTasks());
}

} // namespace SimulationSlave
