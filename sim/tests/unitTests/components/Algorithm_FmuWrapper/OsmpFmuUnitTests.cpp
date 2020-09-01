/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "OsmpFmuHandler.h"

using ::testing::Eq;

TEST(OsmpFmuUnitTests, GetTrafficCommandFromOpenScenarioTrajectory)
{
    openScenario::Trajectory trajectory;
    trajectory.points.emplace_back(openScenario::TrajectoryPoint{0.0, 0.1, -0.2, 0.3});
    trajectory.points.emplace_back(openScenario::TrajectoryPoint{5.1, -1.1, 1.2, 1.3});
    trajectory.points.emplace_back(openScenario::TrajectoryPoint{15.2, 2.1, -2.2, -2.3});

    const auto trafficCommand = OsmpFmuHandler::GetTrafficCommandFromOpenScenarioTrajectory(trajectory);
    const auto& trajectoryAction = trafficCommand.action(0).trajectory_action();
    ASSERT_THAT(trajectoryAction.trajectory_point_size(), Eq(3));

    const auto& firstPoint = trajectoryAction.trajectory_point(0);
    ASSERT_THAT(firstPoint.time_stamp().seconds(), Eq(0));
    ASSERT_THAT(firstPoint.time_stamp().nanos(), Eq(0));
    ASSERT_THAT(firstPoint.state().position().x(), Eq(0.1));
    ASSERT_THAT(firstPoint.state().position().y(), Eq(-0.2));
    ASSERT_THAT(firstPoint.state().orientation().yaw(), Eq(0.3));

    const auto& secondPoint = trajectoryAction.trajectory_point(1);
    ASSERT_THAT(secondPoint.time_stamp().seconds(), Eq(5));
    ASSERT_THAT(secondPoint.time_stamp().nanos(), Eq(100000000));
    ASSERT_THAT(secondPoint.state().position().x(), Eq(-1.1));
    ASSERT_THAT(secondPoint.state().position().y(), Eq(1.2));
    ASSERT_THAT(secondPoint.state().orientation().yaw(), Eq(1.3));

    const auto& thirdPoint = trajectoryAction.trajectory_point(2);
    ASSERT_THAT(thirdPoint.time_stamp().seconds(), Eq(15));
    ASSERT_THAT(thirdPoint.time_stamp().nanos(), Eq(200000000));
    ASSERT_THAT(thirdPoint.state().position().x(), Eq(2.1));
    ASSERT_THAT(thirdPoint.state().position().y(), Eq(-2.2));
    ASSERT_THAT(thirdPoint.state().orientation().yaw(), Eq(-2.3));
}
