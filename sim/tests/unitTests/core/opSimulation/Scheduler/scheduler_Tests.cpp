/********************************************************************************
 * Copyright (c) 2019-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include <functional>
#include <iostream>
#include <list>
#include <set>

#include "eventDetector.h"
#include "eventDetectorLibrary.h"
#include "fakeDataBuffer.h"
#include "fakeEventDetectorNetwork.h"
#include "fakeEventNetwork.h"
#include "fakeManipulatorNetwork.h"
#include "fakeObservationNetwork.h"
#include "fakeSpawnPointNetwork.h"
#include "fakeWorld.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "scheduler.h"
#include "schedulerTasks.h"
#include "runResult.h"

using namespace core::scheduling;

using testing::NiceMock;
using testing::Return;
using testing::ReturnRef;

template <typename T>
void ExecuteTasks(T tasks)
{
    for (auto &task : tasks)
    {
        task.func();
    }
}

bool ExecuteSpawn(int time)
{
    std::cout << "spawning task at " << time << std::endl;
    return true;
}

bool ExecuteEventDetector(int time)
{
    std::cout << "eventDetector task at " << time << std::endl;
    return true;
}

bool ExecuteTrigger(int time)
{
    std::cout << "triggering task at " << time << std::endl;
    return true;
}

bool ExecuteUpdate(int id, int time)
{
    std::cout << "updating task at " << time << " for id: " << id << std::endl;
    return true;
}

TEST(DISABLED_Scheduler, RunWorks)
{
    NiceMock<FakeWorld> fakeWorld;

    NiceMock<FakeSpawnPointNetwork> fakeSpawnPointNetwork;
    NiceMock<FakeDataBuffer> fakeDataBuffer;
    NiceMock<FakeEventDetector> fakeEventDetector;
    NiceMock<FakeManipulatorNetwork> fakeManipulatorNetwork;
    NiceMock<FakeObservationNetwork> fakeObservationNetwork;
    NiceMock<FakeEventDetectorNetwork> fakeEventDetectorNetwork;

    NiceMock<FakeEventNetwork> fakeEventNetwork;
    core::EventDetectorLibrary edl("", nullptr);
    core::EventDetector e1(&fakeEventDetector, &edl);
    core::EventDetector e2(&fakeEventDetector, &edl);

    std::vector<const core::EventDetector *> fakeEventDetectors;
    fakeEventDetectors.push_back(&e1);
    fakeEventDetectors.push_back(&e2);

    ON_CALL(fakeEventDetectorNetwork, GetEventDetectors()).WillByDefault(Return(fakeEventDetectors));

    Scheduler scheduler(fakeWorld, fakeSpawnPointNetwork, fakeEventDetectorNetwork, fakeManipulatorNetwork, fakeObservationNetwork, fakeDataBuffer);

    RunResult runResult{};
    scheduler.Run(0, 300, runResult, fakeEventNetwork);
}
