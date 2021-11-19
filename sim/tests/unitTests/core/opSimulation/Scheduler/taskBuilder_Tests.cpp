/********************************************************************************
 * Copyright (c) 2019-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include <vector>

#include "include/scenarioInterface.h"
#include "eventDetector.h"
#include "fakeDataBuffer.h"
#include "fakeEventDetectorNetwork.h"
#include "fakeManipulatorNetwork.h"
#include "fakeSpawnPointNetwork.h"
#include "fakeWorld.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "taskBuilder.h"

using ::testing::_;
using ::testing::Contains;
using ::testing::Eq;
using ::testing::Field;
using ::testing::Gt;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Not;
using ::testing::Return;
using ::testing::SizeIs;

using namespace core;
using namespace core::scheduling;

TEST(TaskBuilder, SpawningTaskCreation_Works)
{
    NiceMock<FakeManipulatorNetwork> fakeManipulatorNetwork;
    NiceMock<FakeEventDetectorNetwork> fakeEventDetectorNetwork;
    int currentTime = 0;

    NiceMock<FakeWorld> fakeWorld;
    NiceMock<FakeDataBuffer> fakeDataBuffer;
    RunResult runResult{};
    TaskBuilder taskBuilder(currentTime,
                            runResult,
                            100,
                            &fakeWorld,
                            nullptr,
                            nullptr,
                            &fakeEventDetectorNetwork,
                            &fakeManipulatorNetwork,
                            &fakeDataBuffer);

    auto commonTasks = taskBuilder.CreateSpawningTasks();
    ASSERT_THAT(commonTasks, SizeIs(Gt(size_t(0))));
    ASSERT_THAT(commonTasks, Contains(Field(&TaskItem::taskType, Eq(TaskType::Spawning))));
}

TEST(TaskBuilder, PreAgentTaskCreation_Works)
{
    NiceMock<FakeEventDetector> fakeEventDetector;
    NiceMock<FakeManipulatorNetwork> fakeManipulatorNetwork;

    EventDetectorLibrary edl("", nullptr);
    core::EventDetector e1(&fakeEventDetector, &edl);
    core::EventDetector e2(&fakeEventDetector, &edl);

    std::vector<const core::EventDetector *> fakeEventDetectors;
    fakeEventDetectors.push_back(&e1);
    fakeEventDetectors.push_back(&e2);

    NiceMock<FakeEventDetectorNetwork> fakeEventDetectorNetwork;
    ON_CALL(fakeEventDetectorNetwork, GetEventDetectors()).WillByDefault(Return(fakeEventDetectors));
    int currentTime = 0;

    NiceMock<FakeWorld> fakeWorld;
    NiceMock<FakeDataBuffer> fakeDataBuffer;
    RunResult runResult{};
    TaskBuilder taskBuilder(currentTime,
                            runResult,
                            100,
                            &fakeWorld,
                            nullptr,
                            nullptr,
                            &fakeEventDetectorNetwork,
                            &fakeManipulatorNetwork,
                            &fakeDataBuffer);

    auto commonTasks = taskBuilder.CreatePreAgentTasks();
    ASSERT_THAT(commonTasks, SizeIs(Gt(size_t(0))));
    ASSERT_THAT(commonTasks, Contains(Field(&TaskItem::taskType, Eq(TaskType::SyncGlobalData))));
    ASSERT_THAT(commonTasks, Contains(Field(&TaskItem::taskType, Eq(TaskType::EventDetector))));
    ASSERT_THAT(commonTasks, Not(Contains(Field(&TaskItem::taskType, Eq(TaskType::Manipulator)))));
}

TEST(TaskBuilder, SynchronizeTaskCreation_Works)
{
    NiceMock<FakeEventDetectorNetwork> fakeEventDetectorNetwork;
    NiceMock<FakeManipulatorNetwork> fakeManipulatorNetwork;

    int currentTime = 0;

    NiceMock<FakeWorld> fakeWorld;
    NiceMock<FakeDataBuffer> fakeDataBuffer;
    RunResult runResult{};
    TaskBuilder taskBuilder(currentTime,
                            runResult,
                            100,
                            &fakeWorld,
                            nullptr,
                            nullptr,
                            &fakeEventDetectorNetwork,
                            &fakeManipulatorNetwork,
                            &fakeDataBuffer);

    auto finalizeTasks = taskBuilder.CreateSynchronizeTasks();
    ASSERT_THAT(finalizeTasks, SizeIs(Gt(size_t(0))));
}
