#include <vector>

#include "Interfaces/scenarioInterface.h"
#include "eventDetector.h"
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

using namespace SimulationSlave;
using namespace openpass::scheduling;

TEST(TaskBuilder, CommonTaskCreation_Works)
{
    NiceMock<FakeEventDetector> fakeEventDetector;
    NiceMock<FakeManipulatorNetwork> fakeManipulatorNetwork;

    EventDetectorLibrary edl("", nullptr);
    SimulationSlave::EventDetector e1(&fakeEventDetector, &edl);
    SimulationSlave::EventDetector e2(&fakeEventDetector, &edl);

    std::vector<const SimulationSlave::EventDetector *> fakeEventDetectors;
    fakeEventDetectors.push_back(&e1);
    fakeEventDetectors.push_back(&e2);

    NiceMock<FakeEventDetectorNetwork> fakeEventDetectorNetwork;
    ON_CALL(fakeEventDetectorNetwork, GetEventDetectors()).WillByDefault(Return(fakeEventDetectors));
    int currentTime = 0;

    NiceMock<FakeWorld> fakeWorld;
    RunResult runResult{};
    TaskBuilder taskBuilder(currentTime,
                            runResult,
                            100,
                            &fakeWorld,
                            nullptr,
                            nullptr,
                            &fakeEventDetectorNetwork,
                            &fakeManipulatorNetwork);

    auto commonTasks = taskBuilder.CreateCommonTasks();
    ASSERT_THAT(commonTasks, SizeIs(Gt(size_t(0))));
    ASSERT_THAT(commonTasks, Contains(Field(&TaskItem::taskType, Eq(TaskType::Spawning))));
    ASSERT_THAT(commonTasks, Contains(Field(&TaskItem::taskType, Eq(TaskType::EventDetector))));
    ASSERT_THAT(commonTasks, Not(Contains(Field(&TaskItem::taskType, Eq(TaskType::Manipulator)))));
}

TEST(TaskBuilder, FinalizeRecurringTaskCreation_Works)
{
    NiceMock<FakeEventDetectorNetwork> fakeEventDetectorNetwork;
    NiceMock<FakeManipulatorNetwork> fakeManipulatorNetwork;

    int currentTime = 0;

    NiceMock<FakeWorld> fakeWorld;
    RunResult runResult{};
    TaskBuilder taskBuilder(currentTime,
                            runResult,
                            100,
                            &fakeWorld,
                            nullptr,
                            nullptr,
                            &fakeEventDetectorNetwork,
                            &fakeManipulatorNetwork);

    auto finalizeTasks = taskBuilder.CreateFinalizeRecurringTasks();
    ASSERT_THAT(finalizeTasks, SizeIs(Gt(size_t(0))));
}
