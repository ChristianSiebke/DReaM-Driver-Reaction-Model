/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2021 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "scheduler.h"

#include "common/log.h"
#include "agent.h"
#include "agentParser.h"
#include "eventNetwork.h"
#include "runResult.h"
#include "taskBuilder.h"

namespace openpass::scheduling {

using namespace SimulationSlave;

Scheduler::Scheduler(WorldInterface &world,
                     SpawnPointNetworkInterface &spawnPointNetwork,
                     EventDetectorNetworkInterface &eventDetectorNetwork,
                     ManipulatorNetworkInterface &manipulatorNetwork,
                     ObservationNetworkInterface &observationNetwork,
                     DataBufferInterface &dataInterface) :
    world(world),
    spawnPointNetwork(spawnPointNetwork),
    eventDetectorNetwork(eventDetectorNetwork),
    manipulatorNetwork(manipulatorNetwork),
    observationNetwork(observationNetwork),
    dataInterface(dataInterface)
{
}

bool Scheduler::Run(
    int startTime,
    int endTime,
    RunResult &runResult,
    EventNetworkInterface &eventNetwork)
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
                            &manipulatorNetwork,
                            &dataInterface);

    auto bootstrapTasks = taskBuilder.CreateBootstrapTasks();
    auto spawningTasks = taskBuilder.CreateSpawningTasks();
    auto preAgentTasks = taskBuilder.CreatePreAgentTasks();
    auto synchronizeTasks = taskBuilder.CreateSynchronizeTasks();
    auto finalizeTasks = taskBuilder.CreateFinalizeTasks();

    auto taskList = SchedulerTasks(
        bootstrapTasks,
        spawningTasks,
        preAgentTasks,
        synchronizeTasks,
        finalizeTasks,
        FRAMEWORK_UPDATE_RATE);

    if (ExecuteTasks(taskList.GetBootstrapTasks()) == false)
    {
        return Scheduler::FAILURE;
    }

    while (currentTime <= endTime)
    {
        if (!ExecuteTasks(taskList.GetSpawningTasks(currentTime)))
        {
            return Scheduler::FAILURE;
        }

        UpdateAgents(taskList, world);

        if (!ExecuteTasks(taskList.GetPreAgentTasks(currentTime)) ||
            !ExecuteTasks(taskList.ConsumeNonRecurringAgentTasks(currentTime)) ||
            !ExecuteTasks(taskList.GetRecurringAgentTasks(currentTime)) ||
            !ExecuteTasks(taskList.GetSynchronizeTasks(currentTime)))
        {
            return Scheduler::FAILURE;
        }

        currentTime = taskList.GetNextTimestamp(currentTime);

        if (runResult.IsEndCondition())
        {
            LOG_INTERN(LogLevel::DebugCore) << "Scheduler: End of operation (end condition reached)";
            return Scheduler::SUCCESS;
        }

        eventNetwork.Clear();
    }

    if (!ExecuteTasks(taskList.GetFinalizeTasks()))
    {
        return Scheduler::FAILURE;
    }

    LOG_INTERN(LogLevel::DebugCore) << "Scheduler: End of operation (end time reached)";
    return Scheduler::SUCCESS;
}

template <typename T>
bool Scheduler::ExecuteTasks(T tasks)
{
    for (const auto &task : tasks)
    {
        if (task.func() == false)
        {
            return false;
        }
    }
    return true;
}

void Scheduler::UpdateAgents(SchedulerTasks &taskList, WorldInterface &world)
{
    for (const auto &agent : spawnPointNetwork.ConsumeNewAgents())
    {
        agent->LinkSchedulerTime(&currentTime);
        ScheduleAgentTasks(taskList, *agent);
    }

    std::vector<int> removedAgents;
    for (const auto &agent: world.GetRemovedAgentsInPreviousTimestep())
    {
        removedAgents.push_back(agent->GetId());
    }
    taskList.DeleteAgentTasks(removedAgents);
}

void Scheduler::ScheduleAgentTasks(SchedulerTasks &taskList, const Agent &agent)
{
    AgentParser agentParser(currentTime);
    agentParser.Parse(agent);

    taskList.ScheduleNewRecurringTasks(agentParser.GetRecurringTasks());
    taskList.ScheduleNewNonRecurringTasks(agentParser.GetNonRecurringTasks());
}

} // namespace openpass::scheduling
