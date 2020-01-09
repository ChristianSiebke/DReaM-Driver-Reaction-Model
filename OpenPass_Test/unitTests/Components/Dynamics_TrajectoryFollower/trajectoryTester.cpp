/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#include "trajectoryTester.h"

TrajectoryTester::TrajectoryTester(const int cycleTime)
{
    fakeBools.insert({"EnforceTrajectory", DontCare<bool>});
    fakeBools.insert({"AutomaticDeactivation", DontCare<bool>});
    ON_CALL(fakeParameters, GetParametersBool()).WillByDefault(ReturnRef(fakeBools));

    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(1));

    trajectoryFollower = std::make_shared<TrajectoryFollowerImplementation>(
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
                             &fakeEventNetwork);
}

TrajectoryTester::TrajectoryTester(const int cycleTime,
                                   SimulationSlave::EventNetworkInterface * eventNetwork)
{
    fakeBools.insert({"EnforceTrajectory", DontCare<bool>});
    fakeBools.insert({"AutomaticDeactivation", DontCare<bool>});
    ON_CALL(fakeParameters, GetParametersBool()).WillByDefault(ReturnRef(fakeBools));

    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(1));

    trajectoryFollower = std::make_shared<TrajectoryFollowerImplementation>(
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
                             eventNetwork);
}

TrajectoryTester::TrajectoryTester(const bool enforceTrajectory, const bool automaticDeactivation, const int cycleTime)
{
    fakeBools.insert({"EnforceTrajectory", enforceTrajectory});
    fakeBools.insert({"AutomaticDeactivation", automaticDeactivation});
    ON_CALL(fakeParameters, GetParametersBool()).WillByDefault(ReturnRef(fakeBools));

    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(1));

    trajectoryFollower = std::make_shared<TrajectoryFollowerImplementation>(
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
                             &fakeEventNetwork);
}

TrajectoryTester::TrajectoryTester(const bool enforceTrajectory,
                                   const bool automaticDeactivation,
                                   FakeWorld *fakeWorld,
                                   const int cycleTime)
{
    fakeBools.insert({"EnforceTrajectory", enforceTrajectory});
    fakeBools.insert({"AutomaticDeactivation", automaticDeactivation});
    ON_CALL(fakeParameters, GetParametersBool()).WillByDefault(ReturnRef(fakeBools));

    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(1));

    trajectoryFollower = std::make_shared<TrajectoryFollowerImplementation>(
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
                             &fakeEventNetwork);
}

TrajectoryTester::TrajectoryTester(const bool enforceTrajectory,
                                   const bool automaticDeactivation,
                                   FakeWorld *fakeWorld,
                                   FakeAgent *fakeAgent,
                                   const int cycleTime)
{
    fakeBools.insert({"EnforceTrajectory", enforceTrajectory});
    fakeBools.insert({"AutomaticDeactivation", automaticDeactivation});
    ON_CALL(fakeParameters, GetParametersBool()).WillByDefault(ReturnRef(fakeBools));

    ON_CALL(*fakeAgent, GetId()).WillByDefault(Return(1));

    trajectoryFollower = std::make_shared<TrajectoryFollowerImplementation>(
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
                             &fakeEventNetwork);
}
