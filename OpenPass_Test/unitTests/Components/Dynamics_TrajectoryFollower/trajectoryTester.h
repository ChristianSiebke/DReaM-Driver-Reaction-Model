/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#pragma once

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "dontCare.h"

#include "fakeEventNetwork.h"
#include "fakeParameter.h"
#include "fakeTrajectory.h"
#include "fakeAgent.h"
#include "fakeWorld.h"
#include "fakeObservation.h"

#include "trajectoryFollowerCommonBase.h"
#include "absoluteWorldCoordinateTrajectoryFollower.h"
#include "roadCoordinateTrajectoryFollower.h"
#include "Common/componentStateChangeEvent.h"

using ::testing::NiceMock;
using ::testing::ReturnRef;
using ::testing::Return;
using ::testing::DontCare;

template <class T>
class TrajectoryTester
{
public:
    static const int CC_LOCAL_LINK_ID = 83;

    std::vector<int> fakeActingAgents {1};
    std::shared_ptr<ComponentChangeEvent> fakeEvent = std::make_shared<ComponentChangeEvent>(0,
                                                                                             DontCare<std::string>(),
                                                                                             DontCare<std::string>(),
                                                                                             EventDefinitions::EventType::ComponentStateChange,
                                                                                             DontCare<std::vector<int>>(),
                                                                                             fakeActingAgents,
                                                                                             "Dynamics_TrajectoryFollower",
                                                                                             "Acting");

    EventContainer fakeActivatingEventContainer = {fakeEvent};
    EventContainer fakeEmptyEventContainer = {};

    NiceMock<FakeEventNetwork> fakeEventNetwork;

    std::map<std::string, bool> fakeBools;
    NiceMock<FakeParameter> fakeParameters;

    NiceMock<FakeTrajectory> fakeTrajectory;

    NiceMock<FakeAgent> fakeAgent;

    NiceMock<FakeObservation> fakeObservation;
    std::map<int, ObservationInterface*> fakeObservations{{0, &fakeObservation}};

    // this can probably be more generic
    // std::shared_ptr<AbsoluteWorldCoordinateTrajectoryFollower> trajectoryFollower;
    std::shared_ptr<T> trajectoryFollower;

    TrajectoryTester(const int cycleTime = 100);

    TrajectoryTester(const int cycleTime,
                     SimulationSlave::EventNetworkInterface * eventNetwork);

    TrajectoryTester(const bool enforceTrajectory,
                     const bool automaticDeactivation,
                     const int cycleTime = 100);

    TrajectoryTester(const bool enforceTrajectory,
                     const bool automaticDeactivation,
                     WorldCoordinateTrajectory *fakeCoordinates,
                     const int cycleTime = 100);

    TrajectoryTester(const bool enforceTrajectory,
                     const bool automaticDeactivation,
                     FakeTrajectory *fakeTrajectory,
                     const int cycleTime = 100);

    TrajectoryTester(const bool enforceTrajectory,
                     const bool automaticDeactivation,
                     FakeTrajectory *fakeTrajectory,
                     FakeWorld *fakeWorld,
                     const int cycleTime = 100);

    TrajectoryTester(const bool enforceTrajectory,
                     const bool automaticDeactivation,
                     FakeTrajectory *fakeTrajectory,
                     FakeWorld *fakeWorld,
                     FakeAgent *fakeAgent,
                     const int cycleTime = 100);
};

class TestTrajectoryFollowerCommonBaseImpl : public TrajectoryFollowerCommonBase
{
public:
    TestTrajectoryFollowerCommonBaseImpl(std::string componentName,
                                         bool isInit,
                                         int priority,
                                         int offsetTime,
                                         int responseTime,
                                         int cycleTime,
                                         StochasticsInterface *stochastics,
                                         WorldInterface *world,
                                         const ParameterInterface *parameters,
                                         const std::map<int, ObservationInterface*> *observations,
                                         const CallbackInterface *callbacks,
                                         AgentInterface *agent,
                                         [[ maybe_unused ]]TrajectoryInterface *trajectory,
                                         SimulationSlave::EventNetworkInterface * const eventNetwork) :
        TrajectoryFollowerCommonBase(componentName,
                                     isInit,
                                     priority,
                                     offsetTime,
                                     responseTime,
                                     cycleTime,
                                     stochastics,
                                     world,
                                     parameters,
                                     observations,
                                     callbacks,
                                     agent,
                                     eventNetwork)
    {}

    virtual void CalculateNextTimestep([[ maybe_unused ]]int time)
    {}

    ComponentState GetComponentState() const
    {
        return GetState();
    }
};
