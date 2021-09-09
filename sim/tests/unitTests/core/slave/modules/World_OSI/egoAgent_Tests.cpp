/*******************************************************************************
* Copyright (c) 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "egoAgent.h"
#include "fakeAgent.h"
#include "fakeWorld.h"

using ::testing::NiceMock;
using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::Eq;
using ::testing::ElementsAre;
using ::testing::SizeIs;

TEST(EgoAgent_Test, GetDistanceToEndOfLane)
{
    NiceMock<FakeAgent> fakeAgent;
    NiceMock<FakeWorld> fakeWorld;

    ObjectPosition agentPosition{{},{{"Road1", GlobalRoadPosition{"Road1", -2, 12, 0, 0}}},{}};
    ON_CALL(fakeAgent, GetObjectPosition()).WillByDefault(ReturnRef(agentPosition));
    std::vector<std::string> roads{"Road1"};
    ON_CALL(fakeAgent, GetRoads(_)).WillByDefault(Return(roads));

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"Road1", true}, roadGraph);
    RoadGraphVertex target = add_vertex(RouteElement{"Road2", true}, roadGraph);
    add_edge(root, target, roadGraph);

    RouteQueryResult<double> distances{{0, 123}};
    ON_CALL(fakeWorld, GetDistanceToEndOfLane(_,_,-1,12,100)).WillByDefault(Return(distances));

    EgoAgent egoAgent {&fakeAgent, &fakeWorld};
    egoAgent.SetRoadGraph(std::move(roadGraph), root, target);

    const auto result = egoAgent.GetDistanceToEndOfLane(100, 1);
    ASSERT_THAT(result, Eq(123));
}

TEST(EgoAgent_Test, GetObjectsInRange)
{
    NiceMock<FakeAgent> fakeAgent;
    NiceMock<FakeWorld> fakeWorld;

    ObjectPosition agentPosition{{},{{"Road1", GlobalRoadPosition{"Road1", -2, 12, 0, 0}}},{}};
    ON_CALL(fakeAgent, GetObjectPosition()).WillByDefault(ReturnRef(agentPosition));
    std::vector<std::string> roads{"Road1"};
    ON_CALL(fakeAgent, GetRoads(_)).WillByDefault(Return(roads));

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"Road1", true}, roadGraph);
    RoadGraphVertex target = add_vertex(RouteElement{"Road2", true}, roadGraph);
    add_edge(root, target, roadGraph);

    FakeAgent otherAgent;
    RouteQueryResult<std::vector<const WorldObjectInterface*>> objects{{0, {&otherAgent}}};
    ON_CALL(fakeWorld, GetObjectsInRange(_,_,-1,12,100, 100)).WillByDefault(Return(objects));

    EgoAgent egoAgent {&fakeAgent, &fakeWorld};
    egoAgent.SetRoadGraph(std::move(roadGraph), root, target);

    const auto result = egoAgent.GetObjectsInRange(100, 100, 1);
    ASSERT_THAT(result, ElementsAre(&otherAgent));
}

TEST(EgoAgent_Test, GetAgentsInRange)
{
    NiceMock<FakeAgent> fakeAgent;
    NiceMock<FakeWorld> fakeWorld;

    ObjectPosition agentPosition{{},{{"Road1", GlobalRoadPosition{"Road1", -2, 12, 0, 0}}},{}};
    ON_CALL(fakeAgent, GetObjectPosition()).WillByDefault(ReturnRef(agentPosition));
    std::vector<std::string> roads{"Road1"};
    ON_CALL(fakeAgent, GetRoads(_)).WillByDefault(Return(roads));

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"Road1", true}, roadGraph);
    RoadGraphVertex target = add_vertex(RouteElement{"Road2", true}, roadGraph);
    add_edge(root, target, roadGraph);

    FakeAgent otherAgent;
    RouteQueryResult<std::vector<const AgentInterface*>> objects{{0, {&otherAgent}}};
    ON_CALL(fakeWorld, GetAgentsInRange(_,_,-1,12,100, 100)).WillByDefault(Return(objects));

    EgoAgent egoAgent {&fakeAgent, &fakeWorld};
    egoAgent.SetRoadGraph(std::move(roadGraph), root, target);

    const auto result = egoAgent.GetAgentsInRange(100, 100, 1);
    ASSERT_THAT(result, ElementsAre(&otherAgent));
}

TEST(EgoAgent_Test, GetTrafficSignsInRange)
{
    NiceMock<FakeAgent> fakeAgent;
    NiceMock<FakeWorld> fakeWorld;

    ObjectPosition agentPosition{{},{{"Road1", GlobalRoadPosition{"Road1", -2, 12, 0, 0}}},{}};
    ON_CALL(fakeAgent, GetObjectPosition()).WillByDefault(ReturnRef(agentPosition));
    std::vector<std::string> roads{"Road1"};
    ON_CALL(fakeAgent, GetRoads(_)).WillByDefault(Return(roads));

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"Road1", true}, roadGraph);
    RoadGraphVertex target = add_vertex(RouteElement{"Road2", true}, roadGraph);
    add_edge(root, target, roadGraph);


    RouteQueryResult<std::vector<CommonTrafficSign::Entity>> objects{{0, {CommonTrafficSign::Entity{}}}};
    ON_CALL(fakeWorld, GetTrafficSignsInRange(_,_,-1,12,100)).WillByDefault(Return(objects));

    EgoAgent egoAgent {&fakeAgent, &fakeWorld};
    egoAgent.SetRoadGraph(std::move(roadGraph), root, target);

    const auto result = egoAgent.GetTrafficSignsInRange(100, 1);
    ASSERT_THAT(result, SizeIs(1));
}

TEST(EgoAgent_Test, GetReferencePointPosition_FirstRoad)
{
    NiceMock<FakeAgent> fakeAgent;
    NiceMock<FakeWorld> fakeWorld;

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"Road1", true}, roadGraph);
    RoadGraphVertex target = add_vertex(RouteElement{"Road2", true}, roadGraph);
    add_edge(root, target, roadGraph);

    GlobalRoadPosition referencePoint{"Road1", -2, 2.0, 3.0, 0.1};
    ObjectPosition position{{{"Road1", referencePoint}},{{"Road1", referencePoint}},{}};
    ON_CALL(fakeAgent, GetObjectPosition()).WillByDefault(ReturnRef(position));
    std::vector<std::string> roads{"Road1"};
    ON_CALL(fakeAgent, GetRoads(_)).WillByDefault(Return(roads));

    EgoAgent egoAgent {&fakeAgent, &fakeWorld};
    egoAgent.SetRoadGraph(std::move(roadGraph), root, target);

    const auto result = egoAgent.GetReferencePointPosition();

    ASSERT_THAT(result.has_value(), Eq(true));
    ASSERT_THAT(result.value().laneId, Eq(-2));
    ASSERT_THAT(result.value().roadPosition.s, Eq(2.0));
    ASSERT_THAT(result.value().roadPosition.t, Eq(3.0));
    ASSERT_THAT(result.value().roadPosition.hdg, Eq(0.1));
}

TEST(EgoAgent_Test, GetReferencePointPosition_SecondRoad)
{
    NiceMock<FakeAgent> fakeAgent;
    NiceMock<FakeWorld> fakeWorld;

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"Road1", true}, roadGraph);
    RoadGraphVertex target = add_vertex(RouteElement{"Road2", true}, roadGraph);
    add_edge(root, target, roadGraph);

    GlobalRoadPosition referencePoint{"Road2", -2, 2.0, 3.0, 0.1};
    ObjectPosition position{{{"Road2", referencePoint}},{{"Road2", referencePoint}},{}};
    ON_CALL(fakeAgent, GetObjectPosition()).WillByDefault(ReturnRef(position));
    std::vector<std::string> roads{"Road2"};
    ON_CALL(fakeAgent, GetRoads(MeasurementPoint::Front)).WillByDefault(Return(roads));

    EgoAgent egoAgent {&fakeAgent, &fakeWorld};
    egoAgent.SetRoadGraph(std::move(roadGraph), root, target);
    egoAgent.Update();

    const auto result = egoAgent.GetReferencePointPosition();

    ASSERT_THAT(result.has_value(), Eq(true));
    ASSERT_THAT(result.value().laneId, Eq(-2));
    ASSERT_THAT(result.value().roadPosition.s, Eq(2.0));
    ASSERT_THAT(result.value().roadPosition.t, Eq(3.0));
    ASSERT_THAT(result.value().roadPosition.hdg, Eq(0.1));
}

TEST(EgoAgent_Test, GetReferencePointPosition_NotOnRoute)
{
    NiceMock<FakeAgent> fakeAgent;
    NiceMock<FakeWorld> fakeWorld;

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"Road1", true}, roadGraph);
    RoadGraphVertex target = add_vertex(RouteElement{"Road2", true}, roadGraph);
    add_edge(root, target, roadGraph);

    GlobalRoadPosition referencePoint{"Offroad", -2, 2.0, 3.0, 0.1};
    ObjectPosition position{{{"Offroad", referencePoint}},{},{}};
    ON_CALL(fakeAgent, GetObjectPosition()).WillByDefault(ReturnRef(position));

    EgoAgent egoAgent {&fakeAgent, &fakeWorld};
    egoAgent.SetRoadGraph(std::move(roadGraph), root, target);

    const auto result = egoAgent.GetReferencePointPosition();

    ASSERT_THAT(result.has_value(), Eq(false));
}

class EgoAgent_GetAlternatives_Test : public ::testing::Test
{
public:
    EgoAgent_GetAlternatives_Test()
    {
        add_edge(root, node1, roadGraph);
        add_edge(root, node2, roadGraph);
        add_edge(node2, node21, roadGraph);
        add_edge(node2, node22, roadGraph);
        add_edge(root, node3, roadGraph);
        ON_CALL(fakeAgent, GetObjectPosition()).WillByDefault(ReturnRef(agentPosition));
        ON_CALL(fakeAgent, GetRoads(_)).WillByDefault(Return(roads));
        RouteQueryResult<double> distances {{node1, 100},
                                            {node21, 150},
                                            {node22, 200},
                                            {node3, 50}};
        ON_CALL(fakeWorld, GetDistanceToEndOfLane(_,_,-3,12,500)).WillByDefault(Return(distances));

        RouteQueryResult<std::vector<const WorldObjectInterface*>> objects{{node1, {&opponent1, &opponent2}},
                                                                           {node21, {}},
                                                                           {node22, {}},
                                                                           {node3, {&opponent1}}};
        ON_CALL(fakeWorld, GetObjectsInRange(_,_,-3,12,100,500)).WillByDefault(Return(objects));

        egoAgent.SetRoadGraph(std::move(roadGraph), root, node21);
    }

    NiceMock<FakeAgent> fakeAgent;
    NiceMock<FakeWorld> fakeWorld;
    EgoAgent egoAgent {&fakeAgent, &fakeWorld};
    ObjectPosition agentPosition{{},{{"Road1", GlobalRoadPosition{"Road1", -2, 12, 0, 0}}},{}};
    std::vector<std::string> roads{"Road1"};
    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"Road1", true}, roadGraph);
    RoadGraphVertex node1 = add_vertex(roadGraph);
    RoadGraphVertex node2 = add_vertex(roadGraph);
    RoadGraphVertex node21 = add_vertex(roadGraph);
    RoadGraphVertex node22 = add_vertex(roadGraph);
    RoadGraphVertex node3 = add_vertex(roadGraph);
    FakeAgent opponent1;
    FakeAgent opponent2;
};

TEST_F(EgoAgent_GetAlternatives_Test, GetAlternativesFilterByDistance)
{
    const auto alternatives = egoAgent.GetAlternatives(
                                  Predicate<DistanceToEndOfLane>{[](const auto& distance){return distance < 160;}},
                                  DistanceToEndOfLaneParameter{500, -1});

    ASSERT_THAT(alternatives, ElementsAre(0, 1, 3));
}

TEST_F(EgoAgent_GetAlternatives_Test, GetAlternativesFilterByDistanceAndObjects)
{
    const auto alternatives = egoAgent.GetAlternatives(
                                  Predicate<DistanceToEndOfLane, ObjectsInRange>{[](const auto& distance, const auto& objects){return distance < 160 && objects.value.empty();}},
                                  DistanceToEndOfLaneParameter{500, -1},
                                  ObjectsInRangeParameter{100, 500, -1});

    ASSERT_THAT(alternatives, ElementsAre(1));
}

TEST_F(EgoAgent_GetAlternatives_Test, GetAlternativesFilterAndSortByDistance)
{
    const auto alternatives = egoAgent.GetAlternatives(
                                  Predicate<DistanceToEndOfLane>{[](const auto& distance){return distance < 160;}},
                                  Compare<DistanceToEndOfLane>{[](const auto& distances){return distances.first < distances.second;}},
                                  DistanceToEndOfLaneParameter{500, -1});

    ASSERT_THAT(alternatives, ElementsAre(3, 0, 1));
}

TEST_F(EgoAgent_GetAlternatives_Test, GetAlternativesFilterAndSortByDistanceAndObjects)
{
    const auto alternatives = egoAgent.GetAlternatives(
                                  Predicate<DistanceToEndOfLane, ObjectsInRange>{[](const auto& distance, const auto& objects){return distance < 160 && !objects.value.empty();}},
                                  Compare<DistanceToEndOfLane, ObjectsInRange>{[](const auto& distances, const auto& objects){return objects.first.value.size() < objects.second.value.size();}},
                                  DistanceToEndOfLaneParameter{500, -1},
                                  ObjectsInRangeParameter{100, 500, -1});

    ASSERT_THAT(alternatives, ElementsAre(3, 0));
}

TEST(EgoAgent_Test, GetWorldPosition_SameRoadInOdDirection)
{
    NiceMock<FakeAgent> fakeAgent;
    NiceMock<FakeWorld> fakeWorld;

    ObjectPosition agentPosition{{{"Road1", GlobalRoadPosition{"Road1", -2, 12, 1, 0.2}}},{{"Road1", GlobalRoadPosition{"Road1", -2, 12, 1, 0.2}}},{}};
    ON_CALL(fakeAgent, GetObjectPosition()).WillByDefault(ReturnRef(agentPosition));

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"Road1", true}, roadGraph);
    RoadGraphVertex target = add_vertex(RouteElement{"Road2", true}, roadGraph);
    add_edge(root, target, roadGraph);

    ON_CALL(fakeWorld, GetRoadLength("Road1")).WillByDefault(Return(100.0));
    ON_CALL(fakeWorld, GetLaneWidth("Road1", _, _)).WillByDefault(Return(4.0));
    Position worldPosition{1, 2, 0.5, 0.0};
    RoadPosition expectedRoadPosition {62.0, -3.5, 0.3};
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(expectedRoadPosition,"Road1")).WillOnce(Return(worldPosition));
    std::vector<std::string> roads{"Road1"};
    ON_CALL(fakeAgent, GetRoads(MeasurementPoint::Front)).WillByDefault(Return(roads));

    EgoAgent egoAgent {&fakeAgent, &fakeWorld};
    egoAgent.SetRoadGraph(std::move(roadGraph), root, target);

    auto result = egoAgent.GetWorldPosition(50.0, 1.5, 0.3);
    ASSERT_THAT(result.xPos, Eq(1));
    ASSERT_THAT(result.yPos, Eq(2));
}

TEST(EgoAgent_Test, GetWorldPosition_SameRoadAgainstOdDirection)
{
    NiceMock<FakeAgent> fakeAgent;
    NiceMock<FakeWorld> fakeWorld;

    ObjectPosition agentPosition{{{"Road1", GlobalRoadPosition{"Road1", 2, 12, -1, 0.2}}},{{"Road1", GlobalRoadPosition{"Road1", -2, 12, 1, 0.2}}},{}};
    ON_CALL(fakeAgent, GetObjectPosition()).WillByDefault(ReturnRef(agentPosition));

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"Road1", false}, roadGraph);
    RoadGraphVertex target = add_vertex(RouteElement{"Road2", true}, roadGraph);
    add_edge(root, target, roadGraph);

    ON_CALL(fakeWorld, GetRoadLength("Road1")).WillByDefault(Return(100.0));
    ON_CALL(fakeWorld, GetLaneWidth("Road1", _, _)).WillByDefault(Return(4.0));
    Position worldPosition{1, 2, 0.5, 0.0};
    RoadPosition expectedRoadPosition {2.0, 3.5, 0.3 - M_PI};
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(expectedRoadPosition,"Road1")).WillOnce(Return(worldPosition));
    std::vector<std::string> roads{"Road1"};
    ON_CALL(fakeAgent, GetRoads(MeasurementPoint::Front)).WillByDefault(Return(roads));

    EgoAgent egoAgent {&fakeAgent, &fakeWorld};
    egoAgent.SetRoadGraph(std::move(roadGraph), root, target);

    auto result = egoAgent.GetWorldPosition(10.0, 1.5, 0.3);
    ASSERT_THAT(result.xPos, Eq(1));
    ASSERT_THAT(result.yPos, Eq(2));
}

TEST(EgoAgent_Test, GetWorldPosition_NextRoadBothInOdDirection)
{
    NiceMock<FakeAgent> fakeAgent;
    NiceMock<FakeWorld> fakeWorld;

    ObjectPosition agentPosition{{{"Road1", GlobalRoadPosition{"Road1", -2, 12, 1, 0.2}}},{{"Road1", GlobalRoadPosition{"Road1", -2, 12, 1, 0.2}}},{}};
    ON_CALL(fakeAgent, GetObjectPosition()).WillByDefault(ReturnRef(agentPosition));

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"Road1", true}, roadGraph);
    RoadGraphVertex target = add_vertex(RouteElement{"Road2", true}, roadGraph);
    add_edge(root, target, roadGraph);

    ON_CALL(fakeWorld, GetRoadLength("Road1")).WillByDefault(Return(100.0));
    ON_CALL(fakeWorld, GetRoadLength("Road2")).WillByDefault(Return(160.0));
    ON_CALL(fakeWorld, GetLaneWidth("Road1", _, _)).WillByDefault(Return(4.0));
    Position worldPosition{1, 2, 0.5, 0.0};
    RoadPosition expectedRoadPosition {62.0, -3.5, 0.3};
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(expectedRoadPosition,"Road2")).WillOnce(Return(worldPosition));
    std::vector<std::string> roads{"Road1"};
    ON_CALL(fakeAgent, GetRoads(MeasurementPoint::Front)).WillByDefault(Return(roads));

    EgoAgent egoAgent {&fakeAgent, &fakeWorld};
    egoAgent.SetRoadGraph(std::move(roadGraph), root, target);

    auto result = egoAgent.GetWorldPosition(150.0, 1.5, 0.3);
    ASSERT_THAT(result.xPos, Eq(1));
    ASSERT_THAT(result.yPos, Eq(2));
}

TEST(EgoAgent_Test, GetWorldPosition_NextRoadBothAgainstOdDirection)
{
    NiceMock<FakeAgent> fakeAgent;
    NiceMock<FakeWorld> fakeWorld;

    ObjectPosition agentPosition{{{"Road1", GlobalRoadPosition{"Road1", 2, 12, -1, 0.2}}},{{"Road1", GlobalRoadPosition{"Road1", -2, 12, 1, 0.2}}},{}};
    ON_CALL(fakeAgent, GetObjectPosition()).WillByDefault(ReturnRef(agentPosition));

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"Road1", false}, roadGraph);
    RoadGraphVertex target = add_vertex(RouteElement{"Road2", false}, roadGraph);
    add_edge(root, target, roadGraph);

    ON_CALL(fakeWorld, GetRoadLength("Road1")).WillByDefault(Return(100.0));
    ON_CALL(fakeWorld, GetRoadLength("Road2")).WillByDefault(Return(160.0));
    ON_CALL(fakeWorld, GetLaneWidth("Road1", _, _)).WillByDefault(Return(4.0));
    Position worldPosition{1, 2, 0.5, 0.0};
    RoadPosition expectedRoadPosition {122.0, 3.5, 0.3 - M_PI};
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(expectedRoadPosition,"Road2")).WillOnce(Return(worldPosition));
    std::vector<std::string> roads{"Road1"};
    ON_CALL(fakeAgent, GetRoads(MeasurementPoint::Front)).WillByDefault(Return(roads));

    EgoAgent egoAgent {&fakeAgent, &fakeWorld};
    egoAgent.SetRoadGraph(std::move(roadGraph), root, target);

    auto result = egoAgent.GetWorldPosition(50.0, 1.5, 0.3);
    ASSERT_THAT(result.xPos, Eq(1));
    ASSERT_THAT(result.yPos, Eq(2));
}

TEST(EgoAgent_Test, GetWorldPosition_NextRoadInThenAgainstOdDirection)
{
    NiceMock<FakeAgent> fakeAgent;
    NiceMock<FakeWorld> fakeWorld;

    ObjectPosition agentPosition{{{"Road1", GlobalRoadPosition{"Road1", -2, 12, 1, 0.2}}},{{"Road1", GlobalRoadPosition{"Road1", -2, 12, 1, 0.2}}},{}};
    ON_CALL(fakeAgent, GetObjectPosition()).WillByDefault(ReturnRef(agentPosition));

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"Road1", true}, roadGraph);
    RoadGraphVertex target = add_vertex(RouteElement{"Road2", false}, roadGraph);
    add_edge(root, target, roadGraph);

    ON_CALL(fakeWorld, GetRoadLength("Road1")).WillByDefault(Return(100.0));
    ON_CALL(fakeWorld, GetRoadLength("Road2")).WillByDefault(Return(160.0));
    ON_CALL(fakeWorld, GetLaneWidth("Road1", _, _)).WillByDefault(Return(4.0));
    Position worldPosition{1, 2, 0.5, 0.0};
    RoadPosition expectedRoadPosition {98.0, 3.5, 0.3 - M_PI};
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(expectedRoadPosition,"Road2")).WillOnce(Return(worldPosition));
    std::vector<std::string> roads{"Road1"};
    ON_CALL(fakeAgent, GetRoads(MeasurementPoint::Front)).WillByDefault(Return(roads));

    EgoAgent egoAgent {&fakeAgent, &fakeWorld};
    egoAgent.SetRoadGraph(std::move(roadGraph), root, target);

    auto result = egoAgent.GetWorldPosition(150.0, 1.5, 0.3);
    ASSERT_THAT(result.xPos, Eq(1));
    ASSERT_THAT(result.yPos, Eq(2));
}


TEST(EgoAgent_Test, GetWorldPosition_NextRoadAgainstThenInOdDirection)
{
    NiceMock<FakeAgent> fakeAgent;
    NiceMock<FakeWorld> fakeWorld;

    ObjectPosition agentPosition{{{"Road1", GlobalRoadPosition{"Road1", 2, 12, -1, 0.2}}},{{"Road1", GlobalRoadPosition{"Road1", -2, 12, 1, 0.2}}},{}};
    ON_CALL(fakeAgent, GetObjectPosition()).WillByDefault(ReturnRef(agentPosition));

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"Road1", false}, roadGraph);
    RoadGraphVertex target = add_vertex(RouteElement{"Road2", true}, roadGraph);
    add_edge(root, target, roadGraph);

    ON_CALL(fakeWorld, GetRoadLength("Road1")).WillByDefault(Return(100.0));
    ON_CALL(fakeWorld, GetRoadLength("Road2")).WillByDefault(Return(160.0));
    ON_CALL(fakeWorld, GetLaneWidth("Road1", _, _)).WillByDefault(Return(4.0));
    Position worldPosition{1, 2, 0.5, 0.0};
    RoadPosition expectedRoadPosition {38.0, -3.5, 0.3};
    EXPECT_CALL(fakeWorld, RoadCoord2WorldCoord(expectedRoadPosition,"Road2")).WillOnce(Return(worldPosition));
    std::vector<std::string> roads{"Road1"};
    ON_CALL(fakeAgent, GetRoads(MeasurementPoint::Front)).WillByDefault(Return(roads));

    EgoAgent egoAgent {&fakeAgent, &fakeWorld};
    egoAgent.SetRoadGraph(std::move(roadGraph), root, target);

    auto result = egoAgent.GetWorldPosition(50.0, 1.5, 0.3);
    ASSERT_THAT(result.xPos, Eq(1));
    ASSERT_THAT(result.yPos, Eq(2));
}

struct GetLaneIdFromRelative_Data
{
    bool inOdDirection;
    int ownLaneId;
    int relativeLane;
    int expectedLaneId;
};

class GetLaneIdFromRelativeTest : public::testing::TestWithParam<GetLaneIdFromRelative_Data>
{};

TEST_P(GetLaneIdFromRelativeTest, CalculatesCorrectLaneId)
{
    const auto& data = GetParam();

    NiceMock<FakeAgent> fakeAgent;
    NiceMock<FakeWorld> fakeWorld;

    ObjectPosition agentPosition{{},{{"Road1", GlobalRoadPosition{"Road1", data.ownLaneId, 0, 0, 0}}},{}};
    ON_CALL(fakeAgent, GetObjectPosition()).WillByDefault(ReturnRef(agentPosition));
    std::vector<std::string> roads{"Road1"};
    ON_CALL(fakeAgent, GetRoads(MeasurementPoint::Front)).WillByDefault(Return(roads));
    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"Road1", data.inOdDirection}, roadGraph);

    EgoAgent egoAgent {&fakeAgent, &fakeWorld};
    egoAgent.SetRoadGraph(std::move(roadGraph), root, root);

    auto result = egoAgent.GetLaneIdFromRelative(data.relativeLane);

    ASSERT_THAT(result, Eq(data.expectedLaneId));
}

INSTANTIATE_TEST_SUITE_P(InOdDirection, GetLaneIdFromRelativeTest, ::testing::Values(
GetLaneIdFromRelative_Data{true, -1, 0, -1},
GetLaneIdFromRelative_Data{true, -1, 1, 1},
GetLaneIdFromRelative_Data{true, -1, 2, 2},
GetLaneIdFromRelative_Data{true, -1, -1, -2},
GetLaneIdFromRelative_Data{true, -2, 1, -1},
GetLaneIdFromRelative_Data{true, -2, 2, 1},
GetLaneIdFromRelative_Data{true, 1, 0, 1},
GetLaneIdFromRelative_Data{true, 1, 1, 2},
GetLaneIdFromRelative_Data{true, 1, -1, -1},
GetLaneIdFromRelative_Data{true, 2, -3, -2}
));

INSTANTIATE_TEST_SUITE_P(AgainstOdDirection, GetLaneIdFromRelativeTest, ::testing::Values(
GetLaneIdFromRelative_Data{false, -1, 0, -1},
GetLaneIdFromRelative_Data{false, -1, 1, -2},
GetLaneIdFromRelative_Data{false, -1, -1, 1},
GetLaneIdFromRelative_Data{false, -1, -2, 2},
GetLaneIdFromRelative_Data{false, -2, -1, -1},
GetLaneIdFromRelative_Data{false, -2, -2, 1},
GetLaneIdFromRelative_Data{false, 1, 0, 1},
GetLaneIdFromRelative_Data{false, 1, 1, -1},
GetLaneIdFromRelative_Data{false, 1, -1, 2},
GetLaneIdFromRelative_Data{false, 2, 3, -2}
));
