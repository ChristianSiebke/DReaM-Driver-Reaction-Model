/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*               2020 BMW AG
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
#include "fakeWorld.h"

#include "openScenarioActionsImplementation.h"
#include "oscActionsCalculation.h"
#include "Common/laneChangeEvent.h"
#include "Common/trajectoryEvent.h"
#include "Common/trajectorySignal.h"
#include "Common/gazeFollowerEvent.h"
#include "Common/gazeFollowerSignal.h"

using ::testing::Return;
using ::testing::_;
using ::testing::Eq;
using ::testing::DoubleNear;

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

{
    constexpr int agentId = 10;
    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(agentId));
    FakeEventNetwork fakeEventNetwork;
    LaneChangeParameter laneChange{};
    EventContainer events{std::make_shared<LaneChangeEvent>(0, "", "", agentId, laneChange)};
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
    openScenarioActions.UpdateOutput(0, signal, 0);

    const auto laneChangeSignal = std::dynamic_pointer_cast<const TrajectorySignal>(signal);
    ASSERT_TRUE(laneChangeSignal);
    ASSERT_THAT(laneChangeSignal->componentState, Eq(ComponentState::Acting));
}

TEST(OpenScenarioActions_Test, LaneChangeEventForOtherAgent_IsIgnored)
{
    constexpr int ownAgentId = 10;
    constexpr int otherAgentId = 11;
    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(ownAgentId));
    FakeEventNetwork fakeEventNetwork;
    LaneChangeParameter laneChange{};
    EventContainer events{std::make_shared<LaneChangeEvent>(0, "", "", otherAgentId, laneChange)};
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
    openScenarioActions.UpdateOutput(0, signal, 0);

    const auto laneChangeSignal = std::dynamic_pointer_cast<const TrajectorySignal>(signal);
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

struct OscActionsCalculation_LaneChangeData
{
    double deltaS;
    double deltaT;
    double deltaTime;
};

class OscActionCalculation_LaneChangeTest : public ::testing::TestWithParam<OscActionsCalculation_LaneChangeData>
{

};

TEST_P(OscActionCalculation_LaneChangeTest, CorrectStartAndEndConstraints)
{
    auto data = GetParam();
    GlobalRoadPosition startPosition{"Road", -2, 10.,11.,0.5};

    FakeWorld fakeWorld;
    ON_CALL(fakeWorld, RoadCoord2WorldCoord(_,"Road")).WillByDefault([](auto roadCoord, std::string)
    {return Position{roadCoord.s + 1.0, roadCoord.t + 2.0, roadCoord.hdg + 0.1, 0.0};});

    OscActionsCalculation calculation{&fakeWorld};

    auto trajectoryResult = calculation.CalculateSinusiodalLaneChange(data.deltaS, data.deltaT, data.deltaTime, 0.1, startPosition, 1.5);

    const auto& startPoint = trajectoryResult.points.front();
    ASSERT_THAT(startPoint.time, DoubleNear(1.5, 1e-3));
    ASSERT_THAT(startPoint.x, DoubleNear(11.0, 1e-3));
    ASSERT_THAT(startPoint.y, DoubleNear(13.0, 1e-3));
    ASSERT_THAT(startPoint.yaw, DoubleNear(0.1, 1e-3));

    const auto& endPoint = trajectoryResult.points.back();
    ASSERT_THAT(endPoint.time, DoubleNear(1.5 + data.deltaTime, 1e-3));
    ASSERT_THAT(endPoint.x, DoubleNear(11.0 + data.deltaS, 1e-3));
    ASSERT_THAT(endPoint.y, DoubleNear(13.0 + data.deltaT, 1e-3));
    ASSERT_THAT(endPoint.yaw, DoubleNear(0.1, 1e-3));
}

TEST_P(OscActionCalculation_LaneChangeTest, ShapeIsSineCurve)
{
    auto data = GetParam();
    GlobalRoadPosition startPosition{"Road", -2, 10.,11.,0.5};

    FakeWorld fakeWorld;
    ON_CALL(fakeWorld, RoadCoord2WorldCoord(_,"Road")).WillByDefault([](auto roadCoord, std::string)
    {return Position{roadCoord.s + 1.0, roadCoord.t + 2.0, roadCoord.hdg + 0.1, 0.0};});

    OscActionsCalculation calculation{&fakeWorld};

    auto trajectoryResult = calculation.CalculateSinusiodalLaneChange(data.deltaS, data.deltaT, data.deltaTime, 0.1, startPosition, 1.5);

    for (const auto point : trajectoryResult.points)
    {
        const auto s = point.x - 11.0;
        const auto t = point.y - 13.0;
        ASSERT_THAT(t, DoubleNear(data.deltaT * 0.5 * (1 - std::cos(s * M_PI / data.deltaS)), 1e-3));
    }
}

 INSTANTIATE_TEST_CASE_P(OscActionCalculation_LaneChangeTest, OscActionCalculation_LaneChangeTest, testing::Values(
                             OscActionsCalculation_LaneChangeData{10.0, 4.0, 2.0},
                             OscActionsCalculation_LaneChangeData{20.0, -3.0, 10.0},
                             OscActionsCalculation_LaneChangeData{5.0, 5.0, 0.5}
                             ));
