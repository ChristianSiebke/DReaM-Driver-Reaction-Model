/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  TaskBuilder.cpp */
//-----------------------------------------------------------------------------

#include "taskBuilder.h"

#include <algorithm>

#include "eventDetector.h"
#include "manipulator.h"

using namespace SimulationSlave;
namespace openpass::scheduling {

TaskBuilder::TaskBuilder(const int &currentTime,
                         RunResult &runResult,
                         const int frameworkUpdateRate,
                         WorldInterface *const world,
                         SpawnPointNetworkInterface *const spawnPointNetwork,
                         ObservationNetworkInterface *const observationNetwork,
                         EventDetectorNetworkInterface *const eventDetectorNetwork,
                         ManipulatorNetworkInterface *const manipulatorNetwork,
                         DataBufferInterface * const dataInterface) :
    currentTime{currentTime},
    runResult{runResult},
    frameworkUpdateRate{frameworkUpdateRate},
    world{world},
    spawnPointNetwork{spawnPointNetwork},
    observationNetwork{observationNetwork},
    eventDetectorNetwork{eventDetectorNetwork},
    manipulatorNetwork{manipulatorNetwork},
    dataInterface{dataInterface}
{
    BuildEventDetectorTasks();
    BuildManipulatorTasks();
}

std::list<TaskItem> TaskBuilder::CreateBootstrapTasks()
{
    return {
        SpawningTaskItem(frameworkUpdateRate, [&] { return spawnPointNetwork->TriggerPreRunSpawnZones(); }),
    };
}

std::list<TaskItem> TaskBuilder::CreateSpawningTasks()
{
    return {
        SpawningTaskItem(frameworkUpdateRate, [&] { return spawnPointNetwork->TriggerRuntimeSpawnPoints(currentTime); }),
        SyncWorldTaskItem(ScheduleAtEachCycle, [&] { dataInterface->ClearTimeStep(); })};
}

std::list<TaskItem> TaskBuilder::CreatePreAgentTasks()
{
    std::list<TaskItem> items{
        SyncWorldTaskItem(ScheduleAtEachCycle, [&] { world->PublishGlobalData(currentTime); })};

    std::copy(std::begin(eventDetectorTasks), std::end(eventDetectorTasks), std::back_inserter(items));
    std::copy(std::begin(manipulatorTasks), std::end(manipulatorTasks), std::back_inserter(items));

    return items;
}

std::list<TaskItem> TaskBuilder::CreateSynchronizeTasks()
{
    return {
        ObservationTaskItem(ScheduleAtEachCycle, [&] { return observationNetwork->UpdateTimeStep(currentTime, runResult); }),
        SyncWorldTaskItem(ScheduleAtEachCycle, [&] { world->SyncGlobalData(currentTime); })};
}

std::list<TaskItem> TaskBuilder::CreateFinalizeTasks()
{
    std::list<TaskItem> items{};

    std::copy(std::begin(eventDetectorTasks), std::end(eventDetectorTasks), std::back_inserter(items));
    std::copy(std::begin(manipulatorTasks), std::end(manipulatorTasks), std::back_inserter(items));

    return items;
}

void TaskBuilder::BuildEventDetectorTasks()
{
    for (const auto &eventDetector : eventDetectorNetwork->GetEventDetectors())
    {
        auto impl = eventDetector->GetImplementation();
        eventDetectorTasks.emplace_back(EventDetectorTaskItem(ScheduleAtEachCycle, [this, impl] { impl->Trigger(this->currentTime); }));
    }
}

void TaskBuilder::BuildManipulatorTasks()
{
    for (const auto &manipulator : manipulatorNetwork->GetManipulators())
    {
        auto impl = manipulator->GetImplementation();
        manipulatorTasks.emplace_back(ManipulatorTaskItem(ScheduleAtEachCycle, [this, impl] { impl->Trigger(this->currentTime); }));
    }
}

} // namespace openpass::scheduling
