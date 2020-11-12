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

#ifdef USE_EXTENDED_OSI
TEST(OsmpFmuUnitTests, GetTrafficCommandFromOpenScenarioTrajectory_FollowTrajectoryAction)
{
    openScenario::Trajectory trajectory;
    trajectory.points.emplace_back(openScenario::TrajectoryPoint{0.0, 0.1, -0.2, 0.3});
    trajectory.points.emplace_back(openScenario::TrajectoryPoint{5.1, -1.1, 1.2, 1.3});
    trajectory.points.emplace_back(openScenario::TrajectoryPoint{15.2, 2.1, -2.2, -2.3});
    openScenario::TrajectoryTimeReference timeReference{"", 0.0, 0.0};
    trajectory.timeReference = timeReference;

    osi3::TrafficCommand trafficCommand;
    OsmpFmuHandler::AddTrafficCommandActionFromOpenScenarioTrajectory(trafficCommand.add_action(), trajectory);

    const auto& trajectoryAction = trafficCommand.action(0).follow_trajectory_action();
    ASSERT_THAT(trajectoryAction.trajectory_point_size(), Eq(3));

    const auto& firstPoint = trajectoryAction.trajectory_point(0);
    ASSERT_THAT(firstPoint.timestamp().seconds(), Eq(0));
    ASSERT_THAT(firstPoint.timestamp().nanos(), Eq(0));
    ASSERT_THAT(firstPoint.position().x(), Eq(0.1));
    ASSERT_THAT(firstPoint.position().y(), Eq(-0.2));
    ASSERT_THAT(firstPoint.orientation().yaw(), Eq(0.3));

    const auto& secondPoint = trajectoryAction.trajectory_point(1);
    ASSERT_THAT(secondPoint.timestamp().seconds(), Eq(5));
    ASSERT_THAT(secondPoint.timestamp().nanos(), Eq(100000000));
    ASSERT_THAT(secondPoint.position().x(), Eq(-1.1));
    ASSERT_THAT(secondPoint.position().y(), Eq(1.2));
    ASSERT_THAT(secondPoint.orientation().yaw(), Eq(1.3));

    const auto &thirdPoint = trajectoryAction.trajectory_point(2);
    ASSERT_THAT(thirdPoint.timestamp().seconds(), Eq(15));
    ASSERT_THAT(thirdPoint.timestamp().nanos(), Eq(200000000));
    ASSERT_THAT(thirdPoint.position().x(), Eq(2.1));
    ASSERT_THAT(thirdPoint.position().y(), Eq(-2.2));
    ASSERT_THAT(thirdPoint.orientation().yaw(), Eq(-2.3));
}

TEST(OsmpFmuUnitTests, GetTrafficCommandFromOpenScenarioTrajectory_FollowPathAction)
{
    openScenario::Trajectory trajectory;
    trajectory.points.emplace_back(openScenario::TrajectoryPoint{0.0, 0.1, -0.2, 0.3});
    trajectory.points.emplace_back(openScenario::TrajectoryPoint{5.1, -1.1, 1.2, 1.3});
    trajectory.points.emplace_back(openScenario::TrajectoryPoint{15.2, 2.1, -2.2, -2.3});

    const auto trafficCommand = OsmpFmuHandler::GetTrafficCommandFromOpenScenarioTrajectory(trajectory);
    const auto& pathAction = trafficCommand.action(0).follow_path_action();
    ASSERT_THAT(pathAction.path_point_size(), Eq(3));

    const auto& firstPoint = pathAction.path_point(0);
    ASSERT_THAT(firstPoint.position().x(), Eq(0.1));
    ASSERT_THAT(firstPoint.position().y(), Eq(-0.2));
    ASSERT_THAT(firstPoint.orientation().yaw(), Eq(0.3));

    const auto& secondPoint = pathAction.path_point(1);
    ASSERT_THAT(secondPoint.position().x(), Eq(-1.1));
    ASSERT_THAT(secondPoint.position().y(), Eq(1.2));
    ASSERT_THAT(secondPoint.orientation().yaw(), Eq(1.3));

    const auto &thirdPoint = pathAction.path_point(2);
    ASSERT_THAT(thirdPoint.position().x(), Eq(2.1));
    ASSERT_THAT(thirdPoint.position().y(), Eq(-2.2));
    ASSERT_THAT(thirdPoint.orientation().yaw(), Eq(-2.3));
}

TEST(OsmpFmuUnitTests, GetTrafficCommandFromOpenScenarioPosition)
{
    constexpr double x = 3.14, y = 42.0;
    openScenario::Position position = openScenario::WorldPosition{3.14, 42.0};

    osi3::TrafficCommand trafficCommand;
    OsmpFmuHandler::AddTrafficCommandActionFromOpenScenarioPosition(trafficCommand.add_action(), position, nullptr, nullptr);

    const auto &action = trafficCommand.action(0);
    auto hasAcquireGlobalPositionAction = action.has_acquire_global_position_action();
    ASSERT_TRUE(hasAcquireGlobalPositionAction);
    const auto &positionAction = action.acquire_global_position_action();
    ASSERT_TRUE(positionAction.has_position());
    ASSERT_EQ(positionAction.position().x(), x);
    ASSERT_EQ(positionAction.position().y(), y);
    ASSERT_FALSE(positionAction.position().has_z());
    ASSERT_FALSE(positionAction.has_orientation());
    ASSERT_FALSE(positionAction.orientation().has_roll());
    ASSERT_FALSE(positionAction.orientation().has_pitch());
    ASSERT_FALSE(positionAction.orientation().has_yaw());
    ASSERT_FALSE(positionAction.has_action_header());
}
#endif
