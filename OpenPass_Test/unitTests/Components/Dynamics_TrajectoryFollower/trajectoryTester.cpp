/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#include "trajectoryTester.h"

template<class T>
TrajectoryTester<T>::TrajectoryTester(const int cycleTime)
{
    fakeBools.insert({"EnforceTrajectory", DontCare<bool>});
    fakeBools.insert({"AutomaticDeactivation", DontCare<bool>});
    ON_CALL(fakeParameters, GetParametersBool()).WillByDefault(ReturnRef(fakeBools));

    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(1));

    EXPECT_CALL(fakeEventNetwork, GetActiveEventCategory(EventDefinitions::EventCategory::ComponentStateChange))
                                    .WillOnce(Return(fakeActivatingEventContainer))
                                    .WillRepeatedly(Return(fakeEmptyEventContainer));

    trajectoryFollower = std::make_shared<T>(
                "trajectoryFollower",
                false,
                DontCare<int>(),
                DontCare<int>(),
                DontCare<int>(),
                cycleTime,
                nullptr,
                nullptr,
                &fakeParameters,
                &fakeObservations,
                nullptr,
                &fakeAgent,
                &fakeTrajectory,
                &fakeEventNetwork);
}

template<class T>
TrajectoryTester<T>::TrajectoryTester(const int cycleTime,
                                                               SimulationSlave::EventNetworkInterface * eventNetwork)
{
    fakeBools.insert({"EnforceTrajectory", DontCare<bool>});
    fakeBools.insert({"AutomaticDeactivation", DontCare<bool>});
    ON_CALL(fakeParameters, GetParametersBool()).WillByDefault(ReturnRef(fakeBools));

    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(1));

    trajectoryFollower = std::make_shared<T>(
                "trajectoryFollower",
                false,
                DontCare<int>(),
                DontCare<int>(),
                DontCare<int>(),
                cycleTime,
                nullptr,
                nullptr,
                &fakeParameters,
                &fakeObservations,
                nullptr,
                &fakeAgent,
                &fakeTrajectory,
                eventNetwork);
}

template<class T>
TrajectoryTester<T>::TrajectoryTester(const bool enforceTrajectory, const bool automaticDeactivation, const int cycleTime)
{
    fakeBools.insert({"EnforceTrajectory", enforceTrajectory});
    fakeBools.insert({"AutomaticDeactivation", automaticDeactivation});
    ON_CALL(fakeParameters, GetParametersBool()).WillByDefault(ReturnRef(fakeBools));

    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(1));

    EXPECT_CALL(fakeEventNetwork, GetActiveEventCategory(EventDefinitions::EventCategory::ComponentStateChange))
                                    .WillOnce(Return(fakeActivatingEventContainer))
                                    .WillRepeatedly(Return(fakeEmptyEventContainer));

    trajectoryFollower = std::make_shared<T>(
                "trajectoryFollower",
                false,
                DontCare<int>(),
                DontCare<int>(),
                DontCare<int>(),
                cycleTime,
                nullptr,
                nullptr,
                &fakeParameters,
                &fakeObservations,
                nullptr,
                &fakeAgent,
                &fakeTrajectory,
                &fakeEventNetwork);
}

template<class T>
TrajectoryTester<T>::TrajectoryTester(const bool enforceTrajectory,
                 const bool automaticDeactivation,
                 WorldCoordinateTrajectory *fakeCoordinates,
                 const int cycleTime)
{
    fakeBools.insert({"EnforceTrajectory", enforceTrajectory});
    fakeBools.insert({"AutomaticDeactivation", automaticDeactivation});
    ON_CALL(fakeParameters, GetParametersBool()).WillByDefault(ReturnRef(fakeBools));

    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(1));

    ON_CALL(fakeTrajectory, GetWorldCoordinates()).WillByDefault(Return(fakeCoordinates));

    EXPECT_CALL(fakeEventNetwork, GetActiveEventCategory(EventDefinitions::EventCategory::ComponentStateChange))
                                    .WillOnce(Return(fakeActivatingEventContainer))
                                    .WillRepeatedly(Return(fakeEmptyEventContainer));

    trajectoryFollower = std::make_shared<T>(
                "trajectoryFollower",
                false,
                DontCare<int>(),
                DontCare<int>(),
                DontCare<int>(),
                cycleTime,
                nullptr,
                nullptr,
                &fakeParameters,
                &fakeObservations,
                nullptr,
                &fakeAgent,
                &fakeTrajectory,
                &fakeEventNetwork);
}

template<class T>
TrajectoryTester<T>::TrajectoryTester(const bool enforceTrajectory,
                                   const bool automaticDeactivation,
                                   FakeTrajectory *fakeTrajectory,
                                   const int cycleTime)
{
    fakeBools.insert({"EnforceTrajectory", enforceTrajectory});
    fakeBools.insert({"AutomaticDeactivation", automaticDeactivation});
    ON_CALL(fakeParameters, GetParametersBool()).WillByDefault(ReturnRef(fakeBools));

    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(1));

    EXPECT_CALL(fakeEventNetwork, GetActiveEventCategory(EventDefinitions::EventCategory::ComponentStateChange))
                                    .WillOnce(Return(fakeActivatingEventContainer))
                                    .WillRepeatedly(Return(fakeEmptyEventContainer));

    trajectoryFollower = std::make_shared<T>(
                "trajectoryFollower",
                false,
                DontCare<int>(),
                DontCare<int>(),
                DontCare<int>(),
                cycleTime,
                nullptr,
                nullptr,
                &fakeParameters,
                &fakeObservations,
                nullptr,
                &fakeAgent,
                fakeTrajectory,
                &fakeEventNetwork);
}

template<class T>
TrajectoryTester<T>::TrajectoryTester(const bool enforceTrajectory,
                                   const bool automaticDeactivation,
                                   FakeTrajectory *fakeTrajectory,
                                   FakeWorld *fakeWorld,
                                   const int cycleTime)
{
    fakeBools.insert({"EnforceTrajectory", enforceTrajectory});
    fakeBools.insert({"AutomaticDeactivation", automaticDeactivation});
    ON_CALL(fakeParameters, GetParametersBool()).WillByDefault(ReturnRef(fakeBools));

    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(1));

    EXPECT_CALL(fakeEventNetwork, GetActiveEventCategory(EventDefinitions::EventCategory::ComponentStateChange))
                                    .WillOnce(Return(fakeActivatingEventContainer))
                                    .WillRepeatedly(Return(fakeEmptyEventContainer));

    trajectoryFollower = std::make_shared<T>(
                "trajectoryFollower",
                false,
                DontCare<int>(),
                DontCare<int>(),
                DontCare<int>(),
                cycleTime,
                nullptr,
                fakeWorld,
                &fakeParameters,
                &fakeObservations,
                nullptr,
                &fakeAgent,
                fakeTrajectory,
                &fakeEventNetwork);
}

template<class T>
TrajectoryTester<T>::TrajectoryTester(const bool enforceTrajectory,
                                      const bool automaticDeactivation,
                                      FakeTrajectory *fakeTrajectory,
                                      FakeWorld *fakeWorld,
                                      FakeAgent *fakeAgent,
                                      const int cycleTime)
{
    fakeBools.insert({"EnforceTrajectory", enforceTrajectory});
    fakeBools.insert({"AutomaticDeactivation", automaticDeactivation});
    ON_CALL(fakeParameters, GetParametersBool()).WillByDefault(ReturnRef(fakeBools));

    EXPECT_CALL(fakeEventNetwork, GetActiveEventCategory(EventDefinitions::EventCategory::ComponentStateChange))
                                    .WillOnce(Return(fakeActivatingEventContainer))
                                    .WillRepeatedly(Return(fakeEmptyEventContainer));

    ON_CALL(*fakeAgent, GetId()).WillByDefault(Return(1));

    trajectoryFollower = std::make_shared<T>(
                "trajectoryFollower",
                false,
                DontCare<int>(),
                DontCare<int>(),
                DontCare<int>(),
                cycleTime,
                nullptr,
                fakeWorld,
                &fakeParameters,
                &fakeObservations,
                nullptr,
                fakeAgent,
                fakeTrajectory,
                &fakeEventNetwork);
}

template class TrajectoryTester<AbsoluteWorldCoordinateTrajectoryFollower>;
template class TrajectoryTester<RoadCoordinateTrajectoryFollower>;
template class TrajectoryTester<TestTrajectoryFollowerCommonBaseImpl>;
