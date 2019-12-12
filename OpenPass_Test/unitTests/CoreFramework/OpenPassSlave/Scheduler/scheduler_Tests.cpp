#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <list>
#include <iostream>
#include <set>
#include <functional>

#include "fakeWorld.h"
#include "fakeSpawnPointNetwork.h"
#include "fakeEventDetectorNetwork.h"
#include "fakeManipulatorNetwork.h"
#include "fakeEventNetwork.h"

#include "eventDetector.h"
#include "eventDetectorLibrary.h"
#include "scheduler.h"
#include "schedulerTasks.h"

using namespace SimulationSlave::Scheduling;

using testing::NiceMock;
using testing::ReturnRef;
using testing::Return;

template<typename T>
void ExecuteTasks(T tasks)
{
    for(auto& task : tasks)
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

    NiceMock<FakeEventDetector> fakeEventDetector;
    NiceMock<FakeManipulatorNetwork> fakeManipulatorNetwork;

    SimulationSlave::EventDetectorLibrary edl("", nullptr);
    SimulationSlave::EventDetector e1(&fakeEventDetector, &edl);
    SimulationSlave::EventDetector e2(&fakeEventDetector, &edl);

    std::vector<const SimulationSlave::EventDetector*> fakeEventDetectors;
    fakeEventDetectors.push_back(&e1);
    fakeEventDetectors.push_back(&e2);

    NiceMock<FakeEventDetectorNetwork> fakeEventDetectorNetwork;
    ON_CALL(fakeEventDetectorNetwork, GetEventDetectors()).WillByDefault(Return(fakeEventDetectors));

    Scheduler scheduler(&fakeWorld,
                       &fakeSpawnPointNetwork,
                       &fakeEventDetectorNetwork,
                       &fakeManipulatorNetwork,
                       nullptr);

    NiceMock<FakeEventNetwork> fakeEventNetwork;
    RunResult runResult{};
    scheduler.Run(0,
                  300,
                  runResult,
                  &fakeEventNetwork);
}
