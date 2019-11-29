/*********************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "fakeWorld.h"
#include "fakeSection.h"
#include "fakeLane.h"
#include "fakeWorldData.h"
#include "fakeLaneManager.h"
#include "fakeMovingObject.h"
#include "fakeAgent.h"

#include "agentManager_Testhelper.h"
#include "Generators/laneGeometryElementGenerator.h"

#include "AgentAdapter.h"
#include "WorldDataQuery.h"

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::IsNull;
using ::testing::_;
using ::testing::Eq;
using ::testing::Ge;
using ::testing::Le;
using ::testing::Lt;
using ::testing::AllOf;
using ::testing::DoubleEq;

/// \test Check what happens, if no agents are available
TEST(GetAgentInFront, NoAgentInFront_ReturnsDefault)
{
    AgentManager agentManager;
    auto ego = agentManager.Create(0.0, 5.0);

    FakeLaneManager laneManager(1, 1, 3.0, {1000}, "TestRoadId");
    //    laneManager.SetLength(0, 0, 1000);
    laneManager.AddWorldObject(0, 0, ego.movingObject);

    OWL::Interfaces::Lanes laneAssignments { laneManager.lanes[0][0] };
    ON_CALL(ego.movingObject, GetLaneAssignments()).WillByDefault(ReturnRef(laneAssignments));

    ASSERT_THAT(ego.agent.GetAgentInFront(0), Eq(nullptr));
}

TEST(GetAgentInFront, AgentLookingForward_CallsForwardSearch)
{
    //TODO: Add agents direction

    AgentManager agentManager;
    auto ego = agentManager.Create(0.0, 5.0);
    FakeAgent mockAgent;

    Fakes::Lane fakeLane;
    ON_CALL(ego.movingObject, GetDistance(_,_)).WillByDefault(Return(5.0));

    EXPECT_CALL(agentManager.fakeWorld, GetNextAgentInLane(_,_, -1, 5.0, true)).WillOnce(Return(&mockAgent));

    ASSERT_THAT(ego.agent.GetAgentInFront(-1), Eq(&mockAgent));
}

TEST(GetDistanceToEndOfLane, AgentLookingForward_CallsForwardSearch)
{
    //TODO: Add agents direction
    //TODO: How to test with a set mainLaneId? -> private member of test object

    AgentManager agentManager;
    auto ego = agentManager.Create(0.0, 5.0);

    ON_CALL(ego.movingObject, GetDistance(_,_)).WillByDefault(Return(5.0));

    EXPECT_CALL(agentManager.fakeWorld, GetDistanceToEndOfLane(_,_, _, 5.0, 1000)).WillOnce(Return(100));

    ASSERT_THAT(ego.agent.GetDistanceToEndOfLane(1000, -1), Eq(100));
}

/// \test Check that agents behind the ego are not considered
TEST(GetAgentInFront, OneAgentBehind_ReturnsDefault)
{
    AgentManager agentManager;

    auto ego = agentManager.Create(100.0, 105.0);
    auto behindOpponent = agentManager.Create(50.0, 55.0);

    FakeLaneManager laneManager(1, 1, 3.0, {1000}, "TestRoadId");
    //    laneManager.SetLength(0, 0, 1000);
    laneManager.AddWorldObject(0, 0, ego.movingObject);
    laneManager.AddWorldObject(0, 0, behindOpponent.movingObject);

    OWL::Interfaces::Lanes laneAssignments { laneManager.lanes[0][0] };
    ON_CALL(ego.movingObject, GetLaneAssignments()).WillByDefault(ReturnRef(laneAssignments));

    ASSERT_THAT(ego.agent.GetAgentInFront(0), Eq(nullptr));
}

//TODO: Change to Expectation against the mock!
/// \test Check that agents are evaluated w.r.t. to their distance
TEST(DISABLED_GetAgentInFront, TwoAgentsOnSameLane_ReturnsCloserAgent)
{
    FakeLaneManager laneManager(1, 1, 3.0, {1000}, "TestRoadId");
    AgentManager agentManager;

    auto ego = agentManager.Create(0.0, 5.0);
    auto closeOpponent = agentManager.Create(50.0, 55.0);
    auto distantOpponent = agentManager.Create(100.0, 105.0);

    //    laneManager.SetLength(0, 0, 1000);
    laneManager.AddWorldObject(0, 0, ego.movingObject);
    laneManager.AddWorldObject(0, 0, closeOpponent.movingObject);
    laneManager.AddWorldObject(0, 0, distantOpponent.movingObject);

    OWL::Interfaces::Lanes laneAssignments { laneManager.lanes[0][0] };
    ON_CALL(ego.movingObject, GetLaneAssignments()).WillByDefault(ReturnRef(laneAssignments));

    auto result = ego.agent.GetAgentInFront(0);
    ASSERT_THAT(result, Eq(&closeOpponent.agent));
}

///////////////////////////////////////////////////////////////////////////////

/// \test Check that search continues in connected lane
TEST(DISABLED_GetAgentInFront, OneAgentOnNextLane_ReturnsAgent)
{
    FakeLaneManager laneManager(2, 1, 3.0, {100, 100}, "TestRoadId");

    AgentManager agentManager;

    auto ego = agentManager.Create(0.0, 5.0);
    auto opponent = agentManager.Create(100.0, 105.0);

    laneManager.AddWorldObject(0, 0, ego.movingObject);
    laneManager.AddWorldObject(1, 0, opponent.movingObject);

    OWL::Interfaces::Lanes laneAssignments { laneManager.lanes[0][0] }; // todo: move to AgentManager

    auto result = ego.agent.GetAgentInFront(0);
    ASSERT_THAT(result, Eq(&opponent.agent));
}

/// \test Make sure that the event horizon is a realtive measure w.r.t. the requesting agent
TEST(DISABLED_GetAgentInFront, EgoAndOpponentRightAfterEventHorizon_ReturnsOpponent)
{
    FakeLaneManager laneManager(2, 1, 3.0, {OWL::EVENTHORIZON, 100}, "TestRoadId");
    AgentManager agentManager;

    auto ego = agentManager.Create(OWL::EVENTHORIZON, OWL::EVENTHORIZON + 5);
    auto opponent = agentManager.Create(OWL::EVENTHORIZON + 10, OWL::EVENTHORIZON + 15);

    laneManager.AddWorldObject(1, 0, ego.movingObject);
    laneManager.AddWorldObject(1, 0, opponent.movingObject);

    OWL::Interfaces::Lanes laneAssignments { laneManager.lanes[0][0] };
    ON_CALL(ego.movingObject, GetLaneAssignments()).WillByDefault(ReturnRef(laneAssignments));

    auto result = ego.agent.GetAgentInFront(0);
    ASSERT_THAT(result, Eq(&opponent.agent));
}

/// \test Check that search does not continue infinitly
TEST(GetAgentInFront, OneAgentButOnLaneExceedingEventHorizon_ReturnsDefault)
{
    FakeLaneManager laneManager(2, 1, 3.0, {OWL::EVENTHORIZON, 100}, "TestRoadId");

    AgentManager agentManager;

    auto ego = agentManager.Create(0.0, 5.0);
    auto opponent = agentManager.Create(100.0, 105.0);

    laneManager.AddWorldObject(0, 0, ego.movingObject);
    laneManager.AddWorldObject(1, 0, opponent.movingObject);

    OWL::Interfaces::Lanes laneAssignments { laneManager.lanes[0][0] };
    ON_CALL(ego.movingObject, GetLaneAssignments()).WillByDefault(ReturnRef(laneAssignments));

    auto result = ego.agent.GetAgentInFront(0);
    ASSERT_THAT(result, Eq(nullptr));
}

TEST(MovingObject_Tests, SetAndGetReferencePointPosition_ReturnsCorrectPosition)
{
    OWL::Primitive::AbsPosition position;
    position.x = 100.0;
    position.y = 150.0;
    position.z = 10.0;
    osi3::MovingObject osiObject;
    OWL::Implementation::MovingObject movingObject(&osiObject, nullptr);
    movingObject.SetLength(8.0);
    movingObject.SetDistanceReferencPointToLeadingEdge(7.0);
    movingObject.SetYaw(0.5);
    movingObject.SetReferencePointPosition(position);
    OWL::Primitive::AbsPosition resultPosition = movingObject.GetReferencePointPosition();
    ASSERT_THAT(resultPosition.x, DoubleEq(position.x));
    ASSERT_THAT(resultPosition.y, DoubleEq(position.y));
    ASSERT_THAT(resultPosition.z, DoubleEq(position.z));
}

TEST(MovingObject_Tests, SetAndGetReferencePointPositionWithYawChangeInBetween_ReturnsCorrectPosition)
{
    OWL::Primitive::AbsPosition position;
    position.x = 100.0;
    position.y = 150.0;
    position.z = 10.0;
    osi3::MovingObject osiObject;
    OWL::Implementation::MovingObject movingObject(&osiObject, nullptr);
    movingObject.SetLength(8.0);
    movingObject.SetDistanceReferencPointToLeadingEdge(7.0);
    movingObject.SetYaw(0.5);
    movingObject.SetReferencePointPosition(position);
    movingObject.SetYaw(0.7);
    OWL::Primitive::AbsPosition resultPosition = movingObject.GetReferencePointPosition();
    ASSERT_THAT(resultPosition.x, DoubleEq(position.x));
    ASSERT_THAT(resultPosition.y, DoubleEq(position.y));
    ASSERT_THAT(resultPosition.z, DoubleEq(position.z));
}

TEST(MovingObject_Tests, SetReferencePointPosition_SetsCorrectPositionOnOSIObject)
{
    OWL::Primitive::AbsPosition position;
    position.x = 100.0;
    position.y = 150.0;
    position.z = 10.0;
    osi3::MovingObject osiObject;
    OWL::Implementation::MovingObject movingObject(&osiObject, nullptr);
    movingObject.SetLength(8.0);
    movingObject.SetDistanceReferencPointToLeadingEdge(6.0);
    movingObject.SetYaw(M_PI * 0.25);
    movingObject.SetReferencePointPosition(position);
    auto resultPosition = osiObject.base().position();
    ASSERT_THAT(resultPosition.x(), DoubleEq(position.x + std::sqrt(2)));
    ASSERT_THAT(resultPosition.y(), DoubleEq(position.y + std::sqrt(2)));
    ASSERT_THAT(resultPosition.z(), DoubleEq(position.z));
}
