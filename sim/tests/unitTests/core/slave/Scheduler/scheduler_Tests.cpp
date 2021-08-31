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

using namespace openpass::scheduling;

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
    SimulationSlave::EventDetectorLibrary edl("", nullptr);
    SimulationSlave::EventDetector e1(&fakeEventDetector, &edl);
    SimulationSlave::EventDetector e2(&fakeEventDetector, &edl);

    std::vector<const SimulationSlave::EventDetector *> fakeEventDetectors;
    fakeEventDetectors.push_back(&e1);
    fakeEventDetectors.push_back(&e2);

    ON_CALL(fakeEventDetectorNetwork, GetEventDetectors()).WillByDefault(Return(fakeEventDetectors));

    Scheduler scheduler(fakeWorld, fakeSpawnPointNetwork, fakeEventDetectorNetwork, fakeManipulatorNetwork, fakeObservationNetwork, fakeDataBuffer);

    RunResult runResult{};
    scheduler.Run(0, 300, runResult, fakeEventNetwork);
}
