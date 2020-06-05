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
