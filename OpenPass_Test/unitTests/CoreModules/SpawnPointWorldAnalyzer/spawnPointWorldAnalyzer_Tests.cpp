/*********************************************************************
* Copyright (c) 2019, 2020 in-tech
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "Common/WorldAnalyzer.h"

#include "fakeAgent.h"
#include "fakeWorld.h"
#include "dontCare.h"

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::Ne;
using ::testing::Ge;
using ::testing::Gt;
using ::testing::Le;
using ::testing::Lt;
using ::testing::AllOf;
using ::testing::DontCare;
using ::testing::DoubleEq;
using ::testing::_;
using ::testing::UnorderedElementsAreArray;

namespace
{
    static constexpr auto SEARCH_FORWARDS = true;
    static constexpr auto SEARCH_BACKWARDS = false;
    static constexpr auto MAX_SEARCH_DISTANCE = std::numeric_limits<double>::max();
    static constexpr auto DOUBLE_INFINITY = std::numeric_limits<double>::infinity();

    static const auto NO_AGENTS_IN_RANGE = RouteQueryResult<std::vector<const AgentInterface*>>{{0, {}}};
    static const auto NO_OBJECTS_IN_RANGE = RouteQueryResult<std::vector<const WorldObjectInterface*>>{{0, {}}};

    static inline std::pair<RoadGraph, RoadGraphVertex> GetSingleVertexRoadGraph(const RouteElement& routeElement)
    {
        RoadGraph roadGraph;
        auto vertex = add_vertex(routeElement, roadGraph);
        return {roadGraph, vertex};
    }
}

bool operator== (const RoadGraph& lhs, const RoadGraph& rhs)
{
    return get(RouteElement(), lhs, 0) == get(RouteElement(), rhs, 0);
}

TEST(WorldAnalyzer, GetValidLaneSpawningRanges_NoScenarioAgents_ReturnsFullRange)
{
    const RoadId roadId{"ROADID"};
    const auto [roadGraph, vertex] = GetSingleVertexRoadGraph(RouteElement{roadId, true});
    const LaneId laneId{-1};
    const SPosition sStart{0};
    const SPosition sEnd{100};
    const ValidLaneSpawningRanges expectedValidRanges{{sStart, sEnd}};

    FakeWorld fakeWorld;
    EXPECT_CALL(fakeWorld, GetAgentsInRange(roadGraph,
                                            vertex,
                                            laneId,
                                            sStart,
                                            DOUBLE_INFINITY,
                                            DOUBLE_INFINITY))
            .WillOnce(Return(NO_AGENTS_IN_RANGE));

    WorldAnalyzer worldAnalyzer(&fakeWorld,
                                [](const auto&) {});
    EXPECT_THAT(worldAnalyzer.GetValidLaneSpawningRanges(roadId, laneId, sStart, sEnd),
                expectedValidRanges);
}

struct GetValidLaneSpawningRanges_OneAgent_Data
{
    const RoadId roadId;
    const LaneId laneId;
    const SPosition sStart;
    const SPosition sEnd;

    const AgentCategory agentCategory;
    const Range scenarioAgentBounds;

    const bool shouldHaveValidRanges;
    const ValidLaneSpawningRanges expectedValidRanges;
};

class GetValidLaneSpawningRanges_OneAgent : public::testing::TestWithParam<GetValidLaneSpawningRanges_OneAgent_Data>
{
public:
    GetValidLaneSpawningRanges_OneAgent()
    {}

    FakeAgent fakeAgent;
    FakeWorld fakeWorld;
};

TEST_P(GetValidLaneSpawningRanges_OneAgent, GetValidLaneSpawningRanges)
{
    const auto data = GetParam();
    const auto [roadGraph, vertex] = GetSingleVertexRoadGraph(RouteElement{data.roadId, true});

    EXPECT_CALL(fakeWorld, GetAgentsInRange(roadGraph,
                                            vertex,
                                            data.laneId,
                                            data.sStart,
                                            DOUBLE_INFINITY,
                                            DOUBLE_INFINITY))
            .WillOnce(Return(RouteQueryResult<std::vector<const AgentInterface*>>{{vertex, {&fakeAgent}}}));

    EXPECT_CALL(fakeAgent, GetAgentCategory())
            .WillRepeatedly(Return(data.agentCategory));

    ObjectPosition fakeAgentRoadPosition{{{data.roadId, GlobalRoadPosition{data.roadId, data.laneId, data.scenarioAgentBounds.first, 0, 0}}},{},{}};
    EXPECT_CALL(fakeAgent, GetObjectPosition())
            .WillRepeatedly(ReturnRef(fakeAgentRoadPosition));

    VehicleModelParameters fakeAgentVehicleModelParameters;
    fakeAgentVehicleModelParameters.length = data.scenarioAgentBounds.second - data.scenarioAgentBounds.first;
    fakeAgentVehicleModelParameters.distanceReferencePointToLeadingEdge = fakeAgentVehicleModelParameters.length;
    EXPECT_CALL(fakeAgent, GetVehicleModelParameters())
            .WillRepeatedly(Return(fakeAgentVehicleModelParameters));
    EXPECT_CALL(fakeAgent, GetRoads(MeasurementPoint::Reference))
            .WillRepeatedly(Return(std::vector<std::string>{data.roadId}));

    WorldAnalyzer worldAnalyzer{&fakeWorld,
                                [](const auto&) {}};
    const auto actualValidRanges = worldAnalyzer.GetValidLaneSpawningRanges(data.roadId,
                                                                            data.laneId,
                                                                            data.sStart,
                                                                            data.sEnd);
    EXPECT_THAT(actualValidRanges.has_value(), data.shouldHaveValidRanges);
    if (data.shouldHaveValidRanges && actualValidRanges.has_value())
    {
        EXPECT_THAT(actualValidRanges.value(), UnorderedElementsAreArray(data.expectedValidRanges));
    }
}

INSTANTIATE_TEST_CASE_P(WorldAnalyzer_AltersValidSpawnRangeCorrectly, GetValidLaneSpawningRanges_OneAgent,
    ::testing::Values(
                                              // roadId  | laneId | sStart | sEnd |      agentCategory     |      bounds      | shouldHaveValidRanges |     expectedValidRanges |
        // single common agent does not impact valid ranges
        GetValidLaneSpawningRanges_OneAgent_Data{"ROADID",      -1,       0,   100,   AgentCategory::Common,           {5, 10},                   true, {{0, 100}}},
        // single ego agent renders agent bounds invalid as spawning points
        GetValidLaneSpawningRanges_OneAgent_Data{"ROADID",      -1,       0,   100,      AgentCategory::Ego,           {5, 10},                   true, {{0, 5.0}, {10.0, 100}}},
        // single scenario agent renders agent bounds invalid as spawning points
        GetValidLaneSpawningRanges_OneAgent_Data{"ROADID",      -1,       0,   100, AgentCategory::Scenario,           {5, 10},                   true, {{0, 5}, {10.0, 100}}},
        // a range entirely encapsulated by a single agent is invalid
        GetValidLaneSpawningRanges_OneAgent_Data{"ROADID",      -1,      25,    30, AgentCategory::Scenario,           {20,35},                  false, DontCare<ValidLaneSpawningRanges>()},
        // a single ego agent outside the range leave full range valid
        GetValidLaneSpawningRanges_OneAgent_Data{"ROADID",      -1,       0,   100,      AgentCategory::Ego,         {105,110},                   true, {{0, 100}}},
        // a single scenario agent outside the range leave full range valid
        GetValidLaneSpawningRanges_OneAgent_Data{"ROADID",      -1,       0,   100, AgentCategory::Scenario,         {105,110},                   true, {{0, 100}}}
    ));

struct GetValidLaneSpawningRanges_TwoAgents_Data
{
    const RoadId roadId;
    const LaneId laneId;
    const SPosition sStart;
    const SPosition sEnd;

    const AgentCategory firstAgentCategory;
    const Range firstScenarioAgentBounds;

    const AgentCategory secondAgentCategory;
    const Range secondScenarioAgentBounds;

    const bool shouldHaveValidRanges;
    const ValidLaneSpawningRanges expectedValidRanges;
};

class GetValidLaneSpawningRanges_TwoAgents : public::testing::TestWithParam<GetValidLaneSpawningRanges_TwoAgents_Data>
{
public:
    GetValidLaneSpawningRanges_TwoAgents()
    {}

    FakeAgent firstFakeAgent;
    FakeAgent secondFakeAgent;
    FakeWorld fakeWorld;
};

TEST_P(GetValidLaneSpawningRanges_TwoAgents, GetValidLaneSpawningRanges)
{
    const auto data = GetParam();

    EXPECT_CALL(firstFakeAgent, GetAgentCategory())
            .WillRepeatedly(Return(data.firstAgentCategory));

    ObjectPosition fakeAgentRoadPosition{{{data.roadId, GlobalRoadPosition{data.roadId, data.laneId, data.firstScenarioAgentBounds.first, 0, 0}}},{},{}};
    EXPECT_CALL(firstFakeAgent, GetObjectPosition())
            .WillRepeatedly(ReturnRef(fakeAgentRoadPosition));

    VehicleModelParameters fakeAgentVehicleModelParameters;
    fakeAgentVehicleModelParameters.length = data.firstScenarioAgentBounds.second - data.firstScenarioAgentBounds.first;
    fakeAgentVehicleModelParameters.distanceReferencePointToLeadingEdge = fakeAgentVehicleModelParameters.length;
    EXPECT_CALL(firstFakeAgent, GetVehicleModelParameters())
            .WillRepeatedly(Return(fakeAgentVehicleModelParameters));

    EXPECT_CALL(firstFakeAgent, GetRoads(MeasurementPoint::Reference))
            .WillRepeatedly(Return(std::vector<std::string>{data.roadId}));

    EXPECT_CALL(secondFakeAgent, GetAgentCategory())
        .WillRepeatedly(Return(data.secondAgentCategory));

    ObjectPosition secondFakeAgentRoadPosition{{{data.roadId, GlobalRoadPosition{data.roadId, data.laneId, data.secondScenarioAgentBounds.first, 0, 0}}},{},{}};
    EXPECT_CALL(secondFakeAgent, GetObjectPosition())
            .WillRepeatedly(ReturnRef(secondFakeAgentRoadPosition));

    VehicleModelParameters secondFakeAgentVehicleModelParameters;
    secondFakeAgentVehicleModelParameters.length = data.secondScenarioAgentBounds.second - data.secondScenarioAgentBounds.first;
    secondFakeAgentVehicleModelParameters.distanceReferencePointToLeadingEdge = secondFakeAgentVehicleModelParameters.length;
    EXPECT_CALL(secondFakeAgent, GetVehicleModelParameters())
            .WillRepeatedly(Return(secondFakeAgentVehicleModelParameters));

    EXPECT_CALL(secondFakeAgent, GetRoads(MeasurementPoint::Reference))
            .WillRepeatedly(Return(std::vector<std::string>{data.roadId}));

    const auto [roadGraph, vertex] = GetSingleVertexRoadGraph(RouteElement{data.roadId, true});

    RouteQueryResult<std::vector<const AgentInterface*>> fakeAgentsInRange{{vertex, {&firstFakeAgent, &secondFakeAgent}}};
    EXPECT_CALL(fakeWorld, GetAgentsInRange(roadGraph,
                                            vertex,
                                            data.laneId,
                                            data.sStart,
                                            DOUBLE_INFINITY,
                                            DOUBLE_INFINITY))
            .WillOnce(Return(fakeAgentsInRange));

    WorldAnalyzer worldAnalyzer{&fakeWorld,
                                [](const auto&) {}};
    const auto actualValidRanges = worldAnalyzer.GetValidLaneSpawningRanges(data.roadId,
                                                                            data.laneId,
                                                                            data.sStart,
                                                                            data.sEnd);
    EXPECT_THAT(actualValidRanges.has_value(), data.shouldHaveValidRanges);
    if (data.shouldHaveValidRanges && actualValidRanges.has_value())
    {
        EXPECT_THAT(actualValidRanges.value(), UnorderedElementsAreArray(data.expectedValidRanges));
    }
}

INSTANTIATE_TEST_CASE_P(WorldAnalyzer_AltersValidSpawnRangeCorrectly, GetValidLaneSpawningRanges_TwoAgents,
    ::testing::Values(
                                               // roadId  | laneId | sStart | sEnd |    firstAgentCategory  | firstScenarioAgentBounds | secondAgentCategory    | secondAgentBounds | shouldHaveValidRanges | expectedValidRanges
        // only common agents -- full range should be valid
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,       0,   100,   AgentCategory::Common,         DontCare<Range>(),   AgentCategory::Common,  DontCare<Range>(),                   true, {{0, 100}}},
        // only a scenario agent and a common agent (the reverse will act as a one agent situation with the agent having category common)
        // only range outside of agent bounds should be valid
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,       0,   100,      AgentCategory::Ego,                   {5, 10},   AgentCategory::Common,           {20, 25},                   true, {{0, 5}, {10.0, 100}}}, // the first value of further ranges are padded with .01 to avoid re-detecting the same agent
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,       0,   100, AgentCategory::Scenario,                   {5, 10},   AgentCategory::Common,           {20, 25},                   true, {{0, 5}, {10.0, 100}}},
        // two internal scenario agents
        // spawn range before rear of nearest agent and after front of furthest agent is valid
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,       0,   100,      AgentCategory::Ego,                   {5, 10}, AgentCategory::Scenario,           {25, 50},                   true, {{0, 5}, {50.0, 100}}},
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,       0,   100, AgentCategory::Scenario,                   {5, 10}, AgentCategory::Scenario,           {25, 50},                   true, {{0, 5}, {50.0, 100}}},
        // two external scenario agents encapsualting spawn range -- no valid spawn range
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,      10,   100,      AgentCategory::Ego,                    {0, 5}, AgentCategory::Scenario,         {105, 110},                  false, DontCare<ValidLaneSpawningRanges>()},
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,      10,   100, AgentCategory::Scenario,                    {0, 5}, AgentCategory::Scenario,         {105, 110},                  false, DontCare<ValidLaneSpawningRanges>()},
        // one internal scenario agent and one external (beyond range) -- partial valid spawn range
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,      10,   100, AgentCategory::Scenario,                    {0, 5}, AgentCategory::Scenario,           {50, 55},                   true, {{55.0, 100}}},
        // one internal scenario agent and one external (before range) -- partial valid spawn range
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,      0,     75, AgentCategory::Scenario,                  {50, 55}, AgentCategory::Scenario,         {100, 105},                   true, {{0, 50}}},
        // two external agents (outside of range - beyond) -- full valid range
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,      0,   100,       AgentCategory::Ego,                {105, 110}, AgentCategory::Scenario,         {125, 130},                   true, {{0, 100}}},
        // two external agents (outside of range - before) -- full valid range
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,      100,   200, AgentCategory::Scenario,                   {0, 5}, AgentCategory::Scenario,           {55, 60},                   true, {{100, 200}}}
    ));

struct CalculateSpawnVelocityToPreventCrashing_Data
{
    const double opponentRearDistanceFromStartOfRoad;
    const double opponentVelocity;

    const double intendedSpawnPosition;
    const double agentFrontLength;
    const double agentRearLength;
    const double intendedVelocity;
    const double expectedAdjustedVelocity;
};

class CalculateSpawnVelocityToPreventCrashingTests : public::testing::TestWithParam<CalculateSpawnVelocityToPreventCrashing_Data>
{
public:
    CalculateSpawnVelocityToPreventCrashingTests()
    {}

    FakeWorld fakeWorld;
    FakeAgent fakeAgent;
};

TEST_P(CalculateSpawnVelocityToPreventCrashingTests, AdjustsVelocityToPreventCrashing)
{
    const auto data = GetParam();
    const RoadId roadId = "ROADID";
    const auto [roadGraph, vertex] = GetSingleVertexRoadGraph(RouteElement{roadId, true});
    const LaneId laneId = -1;

    FakeAgent fakeAgent;
    FakeWorld fakeWorld;

    RouteQueryResult<std::vector<const AgentInterface*>> agentsInRange{{vertex, {&fakeAgent}}};
    EXPECT_CALL(fakeWorld, GetAgentsInRange(roadGraph,
                                            vertex,
                                            laneId,
                                            _,
                                            0,
                                            _))
            .WillOnce(Return(agentsInRange));

    EXPECT_CALL(fakeAgent, GetDistanceToStartOfRoad(MeasurementPoint::Rear, roadId))
            .WillOnce(Return(data.opponentRearDistanceFromStartOfRoad));
    EXPECT_CALL(fakeAgent, GetVelocity())
            .WillOnce(Return(data.opponentVelocity))
            .WillOnce(Return(data.opponentVelocity));

    const auto emptyLoggingCallback = [](const std::string&) {};
    WorldAnalyzer worldAnalyzer(&fakeWorld,
                                emptyLoggingCallback);
    const auto actualAdjustedVelocity = worldAnalyzer.CalculateSpawnVelocityToPreventCrashing(roadId,
                                                                                              laneId,
                                                                                              data.intendedSpawnPosition,
                                                                                              data.agentFrontLength,
                                                                                              data.agentRearLength,
                                                                                              data.intendedVelocity);

    EXPECT_THAT(actualAdjustedVelocity, DoubleEq(data.expectedAdjustedVelocity));
}

INSTANTIATE_TEST_CASE_P(WorldAnalyzer_CalculateSpawnVelocityToPreventCrashing, CalculateSpawnVelocityToPreventCrashingTests,
    ::testing::Values(
                                                  // opponentRearDistanceFromStartOfRoad |  opponentVelocity | intendedSpawnPosition | agentFrontLength | agentRearLength | intendedVelocity | expectedAdjustedVelocity |
        CalculateSpawnVelocityToPreventCrashing_Data{                                  60,                 20,                   49.5,               0.5,              0.5,                30,                      25.0}
));

struct SpawnWillCauseCrash_Data
{
    const SPosition spawnPosition;
    const double spawnVelocity;
    const Direction direction;
    const bool objectExistsInSearchDirection;
    const SPosition objectPosition;
    const double objectVelocity;
    const bool expectedSpawnWillCauseCrashResult;
};

class SpawnWillCauseCrashTests : public::testing::TestWithParam<SpawnWillCauseCrash_Data>
{
public:
    SpawnWillCauseCrashTests()
    {}

    FakeWorld fakeWorld;
    FakeAgent fakeObjectInLane;
};

TEST_P(SpawnWillCauseCrashTests, PredictsCrashesAccurately)
{
    const auto data = GetParam();

    const RoadId roadId{"ROADID"};
    const auto [roadGraph, vertex] = GetSingleVertexRoadGraph(RouteElement{roadId, true});
    const LaneId laneId{-1};
    const SPosition sPosition{data.spawnPosition};
    const double agentFrontLength{.5};
    const double agentRearLength{.5};
    const double velocity{data.spawnVelocity};
    const bool searchDirection = (data.direction == Direction::FORWARD);

    if (!data.objectExistsInSearchDirection)
    {
        EXPECT_CALL(fakeWorld, GetObjectsInRange(roadGraph,
                                                 vertex,
                                                 laneId,
                                                 sPosition,
                                                 searchDirection ? 0 : std::numeric_limits<double>::infinity(),
                                                 searchDirection ? std::numeric_limits<double>::infinity() : 0))
                .WillOnce(Return(NO_OBJECTS_IN_RANGE));
    }
    else
    {
        RouteQueryResult<std::vector<const WorldObjectInterface*>> objectsInRange{{vertex, {&fakeObjectInLane}}};
        EXPECT_CALL(fakeWorld, GetObjectsInRange(roadGraph,
                                                 vertex,
                                                 laneId,
                                                 sPosition,
                                                 searchDirection ? 0 : std::numeric_limits<double>::infinity(),
                                                 searchDirection ? std::numeric_limits<double>::infinity() : 0))
                .WillOnce(Return(objectsInRange));

        EXPECT_CALL(fakeObjectInLane, GetVelocity())
                .WillOnce(Return(data.objectVelocity));

        if (data.direction == Direction::FORWARD)
        {
            EXPECT_CALL(fakeObjectInLane, GetDistanceToStartOfRoad(MeasurementPoint::Rear, roadId))
                    .WillOnce(Return(data.objectPosition));
        }
        else
        {
            EXPECT_CALL(fakeObjectInLane, GetDistanceToStartOfRoad(MeasurementPoint::Front, roadId))
                    .WillOnce(Return(data.objectPosition));
        }
    }

    const auto emptyLoggingCallback = [](const std::string&) {};
    WorldAnalyzer worldAnalyzer(&fakeWorld,
                                emptyLoggingCallback);
    const auto actualSpawnWillCauseCrashResult = worldAnalyzer.SpawnWillCauseCrash(roadId,
                                                                                   laneId,
                                                                                   sPosition,
                                                                                   agentFrontLength,
                                                                                   agentRearLength,
                                                                                   velocity,
                                                                                   data.direction);

    EXPECT_THAT(actualSpawnWillCauseCrashResult, data.expectedSpawnWillCauseCrashResult);
}

INSTANTIATE_TEST_CASE_P(WorldAnalyzer_SpawnWillCauseCrash, SpawnWillCauseCrashTests,
    ::testing::Values(
                            //      spawnPosition     |   spawnVelocity   |      direction     | objectExistsInSearchDirection |    objectPosition    |   objectVelocity  | expectedSpawnWillCauseCrashResult
        SpawnWillCauseCrash_Data{DontCare<SPosition>(), DontCare<double>(),  Direction::FORWARD,                          false, DontCare<SPosition>(), DontCare<double>(), false},
        SpawnWillCauseCrash_Data{                    0,               30.0,  Direction::FORWARD,                           true,                  75.0,                0.0, true},
        SpawnWillCauseCrash_Data{                 75.0,                0.0, Direction::BACKWARD,                           true,                   0.0,               30.0, true}
));
