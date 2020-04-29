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
#include "fakeEventNetwork.h"
#include "fakeAgent.h"

#include "openScenarioActionsImplementation.h"
#include "Common/laneChangeEvent.h"
#include "Common/laneChangeSignal.h"
#include "Common/trajectoryEvent.h"
#include "Common/trajectorySignal.h"
#include "Common/gazeFollowerEvent.h"
#include "Common/gazeFollowerSignal.h"

using ::testing::Return;
using ::testing::Eq;

TEST(OpenScenarioActions_Test, TrajectoryEventForOwnAgent_IsForwardedAsSignal)
{
    constexpr int agentId = 10;
    const openScenario::Trajectory trajectory{{}, "MyTrajectory"};
    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(agentId));
    FakeEventNetwork fakeEventNetwork;
    EventContainer events{std::make_shared<TrajectoryEvent>(0, "", "", agentId, trajectory)};
    ON_CALL(fakeEventNetwork, GetActiveEventCategory(EventDefinitions::EventCategory::SetTrajectory)).WillByDefault(Return(events));

    auto openScenarioActions = OpenScenarioActionsImplementation("",
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 nullptr,
                                                                 nullptr,
                                                                 nullptr,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeAgent,
                                                                 &fakeEventNetwork);

    openScenarioActions.Trigger(0);

    std::shared_ptr<SignalInterface const> signal;
    openScenarioActions.UpdateOutput(0, signal, 0);

    const auto trajectorySignal = std::dynamic_pointer_cast<const TrajectorySignal>(signal);
    ASSERT_TRUE(trajectorySignal);
    ASSERT_THAT(trajectorySignal->componentState, Eq(ComponentState::Acting));
    ASSERT_THAT(trajectorySignal->trajectory.name, Eq("MyTrajectory"));
}

TEST(OpenScenarioActions_Test, TrajectoryEventForOtherAgent_IsIgnored)
{
    constexpr int ownAgentId = 10;
    constexpr int otherAgentId = 11;
    const openScenario::Trajectory trajectory{{}, "MyTractory"};
    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(ownAgentId));
    FakeEventNetwork fakeEventNetwork;
    EventContainer events{std::make_shared<TrajectoryEvent>(0, "", "", otherAgentId, trajectory)};
    ON_CALL(fakeEventNetwork, GetActiveEventCategory(EventDefinitions::EventCategory::SetTrajectory)).WillByDefault(Return(events));

    auto openScenarioActions = OpenScenarioActionsImplementation("",
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 nullptr,
                                                                 nullptr,
                                                                 nullptr,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeAgent,
                                                                 &fakeEventNetwork);

    openScenarioActions.Trigger(0);

    std::shared_ptr<SignalInterface const> signal;
    openScenarioActions.UpdateOutput(0, signal, 0);

    const auto trajectorySignal = std::dynamic_pointer_cast<const TrajectorySignal>(signal);
    ASSERT_TRUE(trajectorySignal);
    ASSERT_THAT(trajectorySignal->componentState, Eq(ComponentState::Disabled));
}

TEST(OpenScenarioActions_Test, LaneChangeEventForOwnAgent_IsForwardedAsSignal)
{
    constexpr int agentId = 10;
    constexpr int deltaLaneId = 2;
    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(agentId));
    FakeEventNetwork fakeEventNetwork;
    EventContainer events{std::make_shared<LaneChangeEvent>(0, "", "", agentId, deltaLaneId)};
    ON_CALL(fakeEventNetwork, GetActiveEventCategory(EventDefinitions::EventCategory::LaneChange)).WillByDefault(Return(events));

    auto openScenarioActions = OpenScenarioActionsImplementation("",
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 nullptr,
                                                                 nullptr,
                                                                 nullptr,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeAgent,
                                                                 &fakeEventNetwork);

    openScenarioActions.Trigger(0);

    std::shared_ptr<SignalInterface const> signal;
    openScenarioActions.UpdateOutput(1, signal, 0);

    const auto laneChangeSignal = std::dynamic_pointer_cast<const LaneChangeSignal>(signal);
    ASSERT_TRUE(laneChangeSignal);
    ASSERT_THAT(laneChangeSignal->componentState, Eq(ComponentState::Acting));
    ASSERT_THAT(laneChangeSignal->deltaLaneId, Eq(deltaLaneId));
}

TEST(OpenScenarioActions_Test, LaneChangeEventForOtherAgent_IsIgnored)
{
    constexpr int ownAgentId = 10;
    constexpr int otherAgentId = 11;
    constexpr int deltaLaneId = 2;
    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(ownAgentId));
    FakeEventNetwork fakeEventNetwork;
    EventContainer events{std::make_shared<LaneChangeEvent>(0, "", "", otherAgentId, deltaLaneId)};
    ON_CALL(fakeEventNetwork, GetActiveEventCategory(EventDefinitions::EventCategory::LaneChange)).WillByDefault(Return(events));

    auto openScenarioActions = OpenScenarioActionsImplementation("",
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 nullptr,
                                                                 nullptr,
                                                                 nullptr,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeAgent,
                                                                 &fakeEventNetwork);

    openScenarioActions.Trigger(0);

    std::shared_ptr<SignalInterface const> signal;
    openScenarioActions.UpdateOutput(1, signal, 0);

    const auto laneChangeSignal = std::dynamic_pointer_cast<const LaneChangeSignal>(signal);
    ASSERT_TRUE(laneChangeSignal);
    ASSERT_THAT(laneChangeSignal->componentState, Eq(ComponentState::Disabled));
}

TEST(OpenScenarioActions_Test, GazeFollowerEventForOwnAgent_IsForwardedAsSignal)
{
    constexpr int agentId = 10;
    std::string fakeGazeActivityState = "fakeGazeActivityState";
    std::string fakeGazeFileName = "fakeGazeFileName";
    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(agentId));
    FakeEventNetwork fakeEventNetwork;
    EventContainer events{std::make_shared<GazeFollowerEvent>(0, "", "", agentId, fakeGazeActivityState, fakeGazeFileName)};
    ON_CALL(fakeEventNetwork, GetActiveEventCategory(EventDefinitions::EventCategory::SetGazeFollower)).WillByDefault(Return(events));

    auto openScenarioActions = OpenScenarioActionsImplementation("",
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 nullptr,
                                                                 nullptr,
                                                                 nullptr,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeAgent,
                                                                 &fakeEventNetwork);

    openScenarioActions.Trigger(0);

    std::shared_ptr<SignalInterface const> signal;
    openScenarioActions.UpdateOutput(2, signal, 0);

    const auto gazeFollowerSignal = std::dynamic_pointer_cast<const GazeFollowerSignal>(signal);
    ASSERT_TRUE(gazeFollowerSignal);
    ASSERT_THAT(gazeFollowerSignal->componentState, Eq(ComponentState::Acting));
    ASSERT_THAT(gazeFollowerSignal->gazeActivityState, fakeGazeActivityState);
    ASSERT_THAT(gazeFollowerSignal->gazeFileName, fakeGazeFileName);
}

TEST(OpenScenarioActions_Test, GazeFollowerEventForOtherAgent_IsIgnored)
{
    constexpr int ownAgentId = 10;
    constexpr int otherAgentId = 11;
    std::string fakeGazeActivityState = "fakeGazeActivityState";
    std::string fakeGazeFileName = "fakeGazeFileName";
    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(ownAgentId));
    FakeEventNetwork fakeEventNetwork;
    EventContainer events{std::make_shared<GazeFollowerEvent>(0, "", "", otherAgentId, fakeGazeActivityState, fakeGazeFileName)};
    ON_CALL(fakeEventNetwork, GetActiveEventCategory(EventDefinitions::EventCategory::SetGazeFollower)).WillByDefault(Return(events));

    auto openScenarioActions = OpenScenarioActionsImplementation("",
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 nullptr,
                                                                 nullptr,
                                                                 nullptr,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeAgent,
                                                                 &fakeEventNetwork);

    openScenarioActions.Trigger(0);

    std::shared_ptr<SignalInterface const> signal;
    openScenarioActions.UpdateOutput(2, signal, 0);

    const auto gazeFollowerSignal = std::dynamic_pointer_cast<const GazeFollowerSignal>(signal);
    ASSERT_TRUE(gazeFollowerSignal);
    ASSERT_THAT(gazeFollowerSignal->componentState, Eq(ComponentState::Disabled));
    ASSERT_THAT(gazeFollowerSignal->gazeActivityState, "");
    ASSERT_THAT(gazeFollowerSignal->gazeFileName, "");
}

