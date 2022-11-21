/********************************************************************************
 * Copyright (c) 2019-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/WorldAnalyzer.h"

#include "fakeAgent.h"
#include "fakeWorld.h"
#include "fakeStream.h"
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

    static const auto NO_AGENTS_IN_RANGE = AgentInterfaces{};
    static const auto NO_OBJECTS_IN_RANGE = std::vector<const WorldObjectInterface*>{};

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

TEST(WorldAnalyzer, GetValidLaneSpawningRanges_NoScenarioAgentsAllDrivingLanes_ReturnsFullRange)
{
    const RoadId roadId{"ROADID"};
    const auto [roadGraph, vertex] = GetSingleVertexRoadGraph(RouteElement{roadId, true});
    const LaneId laneId{-1};
    const SPosition sStart{0};
    const SPosition sEnd{100};
    const LaneSection fakeLaneSection {sStart, sEnd, {laneId}};
    const LaneSections fakeLaneSections {fakeLaneSection};
    const Ranges expectedValidRanges {{sStart, sEnd}};
    const RouteQueryResult<double> endOfLaneResult {{vertex, sEnd}};

    const LaneTypes validLaneTypes = {LaneType::Driving};

    std::unique_ptr<FakeLaneStream> laneStream = std::make_unique<FakeLaneStream>();
    EXPECT_CALL(*laneStream, GetAgentsInRange(StreamPosition{sStart, 0},
                                            StreamPosition{sEnd, 0}))
            .WillOnce(Return(NO_AGENTS_IN_RANGE));
    EXPECT_CALL(*laneStream, GetLaneTypes()).WillOnce(Return(std::vector<std::pair<double, LaneType>>{std::make_pair(0, LaneType::Driving)}));

    FakeWorld fakeWorld;
    WorldAnalyzer worldAnalyzer(&fakeWorld,
                                [](const auto&) {});

    const auto& actualResult = worldAnalyzer.GetValidLaneSpawningRanges(std::move(laneStream), sStart, sEnd, validLaneTypes);
    EXPECT_THAT(actualResult,
                expectedValidRanges);
}

TEST(WorldAnalyzer, GetValidLaneSpawningRanges_NoScenarioAgentsMixedLaneTypes_ReturnsDrivingLaneRanges)
{
    const RoadId roadId{"ROADID"};
    const auto [roadGraph, vertex] = GetSingleVertexRoadGraph(RouteElement{roadId, true});
    const LaneId laneId{-1};
    const SPosition sStart{10};
    const SPosition sEnd{100};
    const SPosition sLaneType1{0};
    const SPosition sLaneType2{50};
    const SPosition sLaneType3{90};
    const LaneSection fakeLaneSection {sStart, sEnd, {laneId}};
    const LaneSections fakeLaneSections {fakeLaneSection};
    const Ranges expectedValidRanges {{sStart, sLaneType2}, {sLaneType3, sEnd}};
    const RouteQueryResult<double> endOfLaneResult {{vertex, sEnd}};

    const LaneTypes validLaneTypes = {LaneType::Driving};

    std::unique_ptr<FakeLaneStream> laneStream = std::make_unique<FakeLaneStream>();
    ON_CALL(*laneStream, GetAgentsInRange(_,_))
            .WillByDefault(Return(NO_AGENTS_IN_RANGE));
    std::vector<std::pair<double, LaneType>> laneTypes{{sLaneType1, LaneType::Driving},{sLaneType2, LaneType::None}, {sLaneType3, LaneType::Driving}};
    EXPECT_CALL(*laneStream, GetLaneTypes()).WillOnce(Return(laneTypes));

    FakeWorld fakeWorld;
    WorldAnalyzer worldAnalyzer(&fakeWorld,
                                [](const auto&) {});

    const auto& actualResult = worldAnalyzer.GetValidLaneSpawningRanges(std::move(laneStream), sStart, sEnd, validLaneTypes);
    EXPECT_THAT(actualResult,
                expectedValidRanges);
}

struct GetValidLaneSpawningRanges_OneAgent_Data
{
    const SPosition sStart;
    const SPosition sEnd;

    const AgentCategory agentCategory;
    const Range scenarioAgentBounds;

    const std::vector<Range> expectedValidRanges;
};

class GetValidLaneSpawningRanges_OneAgent : public::testing::TestWithParam<GetValidLaneSpawningRanges_OneAgent_Data>
{
public:
    GetValidLaneSpawningRanges_OneAgent()
    {}

    FakeAgent fakeAgent;
    FakeWorld fakeWorld;
    std::unique_ptr<FakeLaneStream> laneStream = std::make_unique<FakeLaneStream>();
};

TEST_P(GetValidLaneSpawningRanges_OneAgent, GetValidLaneSpawningRanges)
{
    const auto data = GetParam();
    const LaneTypes validLaneTypes = {LaneType::Driving};

    EXPECT_CALL(*laneStream, GetAgentsInRange(StreamPosition{data.sStart, 0},
                                            StreamPosition{data.sEnd, 0}))
            .WillOnce(Return(AgentInterfaces{&fakeAgent}));

    EXPECT_CALL(fakeAgent, GetAgentCategory())
            .WillRepeatedly(Return(data.agentCategory));

    EXPECT_CALL(*laneStream, GetStreamPosition(&fakeAgent, MeasurementPoint::Rear))
            .WillRepeatedly(Return(StreamPosition{data.scenarioAgentBounds.first, 0.}));

    EXPECT_CALL(*laneStream, GetStreamPosition(&fakeAgent, MeasurementPoint::Front))
            .WillRepeatedly(Return(StreamPosition{data.scenarioAgentBounds.second, 0.}));

    EXPECT_CALL(*laneStream, GetLaneTypes()).WillOnce(Return(std::vector<std::pair<double, LaneType>>{std::make_pair(0, LaneType::Driving)}));

    WorldAnalyzer worldAnalyzer{&fakeWorld,
                                [](const auto&) {}};

    const auto& actualResult = worldAnalyzer.GetValidLaneSpawningRanges(std::move(laneStream), data.sStart, data.sEnd, validLaneTypes);

    Ranges expectedValidLaneRanges {};

    for (const auto& range : data.expectedValidRanges)
    {
        expectedValidLaneRanges.emplace_back(range.first, range.second);
    }

    EXPECT_THAT(actualResult, UnorderedElementsAreArray(expectedValidLaneRanges));
}

INSTANTIATE_TEST_CASE_P(WorldAnalyzer_AltersValidSpawnRangeCorrectly, GetValidLaneSpawningRanges_OneAgent,
    ::testing::Values(
                                              // sStart | sEnd |      agentCategory     |      bounds      |      expectedValidRanges |
        // single common agent does not impact valid ranges
        GetValidLaneSpawningRanges_OneAgent_Data{0,   100,   AgentCategory::Common,           {5, 10},                   {{0, 100}}},
        // single ego agent renders agent bounds invalid as spawning points
        GetValidLaneSpawningRanges_OneAgent_Data{0,   100,      AgentCategory::Ego,           {5, 10},                   {{0, 4.999}, {10.001, 100}}},
        // single scenario agent renders agent bounds invalid as spawning points
        GetValidLaneSpawningRanges_OneAgent_Data{0,   100, AgentCategory::Scenario,           {5, 10},                   {{0, 4.999}, {10.001, 100}}},
        // a range entirely encapsulated by a single agent is invalid
        GetValidLaneSpawningRanges_OneAgent_Data{25,    30, AgentCategory::Scenario,         {20, 35},                    {}},
        // a single ego agent outside the range leave full range valid
        GetValidLaneSpawningRanges_OneAgent_Data{0,   100,      AgentCategory::Ego,         {105,110},                   {{0, 100}}},
        // a single scenario agent outside the range leave full range valid
        GetValidLaneSpawningRanges_OneAgent_Data{0,   100, AgentCategory::Scenario,         {105,110},                   {{0, 100}}}
    ));

struct GetValidLaneSpawningRanges_TwoAgents_Data
{
    const SPosition sStart;
    const SPosition sEnd;

    const AgentCategory firstAgentCategory;
    const Range firstScenarioAgentBounds;

    const AgentCategory secondAgentCategory;
    const Range secondScenarioAgentBounds;

    const std::vector<Range> expectedValidRanges;
};

class GetValidLaneSpawningRanges_TwoAgents : public::testing::TestWithParam<GetValidLaneSpawningRanges_TwoAgents_Data>
{
public:
    GetValidLaneSpawningRanges_TwoAgents()
    {}

    FakeAgent firstFakeAgent;
    FakeAgent secondFakeAgent;
    FakeWorld fakeWorld;
    std::unique_ptr<FakeLaneStream> laneStream = std::make_unique<FakeLaneStream>();
};

TEST_P(GetValidLaneSpawningRanges_TwoAgents, GetValidLaneSpawningRanges)
{
    const auto data = GetParam();

    EXPECT_CALL(*laneStream, GetAgentsInRange(StreamPosition{data.sStart, 0},
                                            StreamPosition{data.sEnd, 0}))
            .WillOnce(Return(AgentInterfaces{&firstFakeAgent, &secondFakeAgent}));

    EXPECT_CALL(firstFakeAgent, GetAgentCategory())
            .WillRepeatedly(Return(data.firstAgentCategory));

    EXPECT_CALL(*laneStream, GetStreamPosition(&firstFakeAgent, MeasurementPoint::Rear))
            .WillRepeatedly(Return(StreamPosition{data.firstScenarioAgentBounds.first, 0.}));

    EXPECT_CALL(*laneStream, GetStreamPosition(&firstFakeAgent, MeasurementPoint::Front))
            .WillRepeatedly(Return(StreamPosition{data.firstScenarioAgentBounds.second, 0.}));

    EXPECT_CALL(secondFakeAgent, GetAgentCategory())
            .WillRepeatedly(Return(data.secondAgentCategory));

    EXPECT_CALL(*laneStream, GetStreamPosition(&secondFakeAgent, MeasurementPoint::Rear))
            .WillRepeatedly(Return(StreamPosition{data.secondScenarioAgentBounds.first, 0.}));

    EXPECT_CALL(*laneStream, GetStreamPosition(&secondFakeAgent, MeasurementPoint::Front))
            .WillRepeatedly(Return(StreamPosition{data.secondScenarioAgentBounds.second, 0.}));

    EXPECT_CALL(*laneStream, GetLaneTypes()).WillOnce(Return(std::vector<std::pair<double, LaneType>>{std::make_pair(0, LaneType::Driving)}));

    WorldAnalyzer worldAnalyzer{&fakeWorld,
                                [](const auto&) {}};

    const LaneTypes validLaneTypes = {LaneType::Driving};

    const auto& actualResult = worldAnalyzer.GetValidLaneSpawningRanges(std::move(laneStream), data.sStart, data.sEnd, validLaneTypes);

    Ranges expectedValidLaneRanges {};

    for (const auto& range : data.expectedValidRanges)
    {
        expectedValidLaneRanges.emplace_back(range.first, range.second);
    }

    EXPECT_THAT(actualResult, UnorderedElementsAreArray(expectedValidLaneRanges));
}

INSTANTIATE_TEST_CASE_P(WorldAnalyzer_AltersValidSpawnRangeCorrectly, GetValidLaneSpawningRanges_TwoAgents,
    ::testing::Values(
                                               //sStart | sEnd |    firstAgentCategory  | firstScenarioAgentBounds | secondAgentCategory    | secondAgentBounds | shouldHaveValidRanges | expectedValidRanges
        // only common agents -- full range should be valid
        GetValidLaneSpawningRanges_TwoAgents_Data{0,   100,   AgentCategory::Common,         DontCare<Range>(),   AgentCategory::Common,  DontCare<Range>(),                   {{0, 100}}},
        // only a scenario agent and a common agent (the reverse will act as a one agent situation with the agent having category common)
        // only range outside of agent bounds should be valid
        GetValidLaneSpawningRanges_TwoAgents_Data{0,   100,      AgentCategory::Ego,                   {5, 10},   AgentCategory::Common,           {20, 25},                   {{0, 4.999}, {10.001, 100}}}, // the first value of further ranges are padded with .01 to avoid re-detecting the same agent
        GetValidLaneSpawningRanges_TwoAgents_Data{0,   100, AgentCategory::Scenario,                   {5, 10},   AgentCategory::Common,           {20, 25},                   {{0, 4.999}, {10.001, 100}}},
        // two internal scenario agents
        // spawn range before rear of nearest agent and after front of furthest agent is valid
        GetValidLaneSpawningRanges_TwoAgents_Data{0,   100,      AgentCategory::Ego,                   {5, 10}, AgentCategory::Scenario,           {25, 50},                   {{0, 4.999}, {50.001, 100}}},
        GetValidLaneSpawningRanges_TwoAgents_Data{0,   100, AgentCategory::Scenario,                   {5, 10}, AgentCategory::Scenario,           {25, 50},                   {{0, 4.999}, {50.001, 100}}},
        // two external scenario agents encapsualting spawn range -- no valid spawn range
        GetValidLaneSpawningRanges_TwoAgents_Data{10,   100,      AgentCategory::Ego,                    {0, 5}, AgentCategory::Scenario,           {100, 105},                 {}},
        GetValidLaneSpawningRanges_TwoAgents_Data{10,   100, AgentCategory::Scenario,                    {0, 5}, AgentCategory::Scenario,           {100, 105},                 {}},
        // one internal scenario agent and one external (beyond range) -- partial valid spawn range
        GetValidLaneSpawningRanges_TwoAgents_Data{10,   100, AgentCategory::Scenario,                    {0, 5}, AgentCategory::Scenario,           {50, 55},                   {{55.001, 100}}},
        // one internal scenario agent and one external (before range) -- partial valid spawn range
        GetValidLaneSpawningRanges_TwoAgents_Data{0,     75, AgentCategory::Scenario,                  {50, 55}, AgentCategory::Scenario,         {100, 105},                   {{0, 49.999}}},
        // two external agents (outside of range - beyond) -- full valid range
        GetValidLaneSpawningRanges_TwoAgents_Data{0,   100,       AgentCategory::Ego,                {105, 110}, AgentCategory::Scenario,         {125, 130},                   {{0, 100}}},
        // two external agents (outside of range - before) -- full valid range
        GetValidLaneSpawningRanges_TwoAgents_Data{100,   200, AgentCategory::Scenario,                   {0, 5}, AgentCategory::Scenario,           {55, 60},                   {{100, 200}}}
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

    FakeAgent fakeAgent;
    FakeWorld fakeWorld;

    std::unique_ptr<FakeLaneStream> laneStream = std::make_unique<FakeLaneStream>();

    EXPECT_CALL(*laneStream, GetObjectsInRange(_,_))
            .WillOnce(Return(std::vector<const WorldObjectInterface*>{&fakeAgent}));

    EXPECT_CALL(*laneStream, GetStreamPosition(&fakeAgent, MeasurementPoint::Rear))
            .WillOnce(Return(StreamPosition{data.opponentRearDistanceFromStartOfRoad, 0.}));
    EXPECT_CALL(fakeAgent, GetVelocity())
            .WillOnce(Return(data.opponentVelocity))
            .WillOnce(Return(data.opponentVelocity));

    const auto emptyLoggingCallback = [](const std::string&) {};
    WorldAnalyzer worldAnalyzer(&fakeWorld,
                                emptyLoggingCallback);
    const auto actualAdjustedVelocity = worldAnalyzer.CalculateSpawnVelocityToPreventCrashing(std::move(laneStream),
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
    std::unique_ptr<FakeLaneStream> laneStream = std::make_unique<FakeLaneStream>();
};

TEST_P(SpawnWillCauseCrashTests, PredictsCrashesAccurately)
{
    const auto data = GetParam();

    const SPosition sPosition{data.spawnPosition};
    const double agentFrontLength{.5};
    const double agentRearLength{.5};
    const double velocity{data.spawnVelocity};

    if (!data.objectExistsInSearchDirection)
    {
        EXPECT_CALL(*laneStream, GetObjectsInRange(_,_))
                .WillOnce(Return(NO_OBJECTS_IN_RANGE));
    }
    else
    {
        std::vector<const WorldObjectInterface*> objectsInRange{&fakeObjectInLane};
        EXPECT_CALL(*laneStream, GetObjectsInRange(_,_))
                .WillOnce(Return(objectsInRange));

        EXPECT_CALL(fakeObjectInLane, GetVelocity())
                .WillOnce(Return(data.objectVelocity));

        if (data.direction == Direction::FORWARD)
        {
            EXPECT_CALL(*laneStream, GetStreamPosition(&fakeObjectInLane, MeasurementPoint::Rear))
                    .WillOnce(Return(StreamPosition{data.objectPosition, 0.}));
        }
        else
        {
            EXPECT_CALL(*laneStream, GetStreamPosition(&fakeObjectInLane, MeasurementPoint::Front))
                    .WillOnce(Return(StreamPosition{data.objectPosition, 0.}));
        }
    }

    const auto emptyLoggingCallback = [](const std::string&) {};
    WorldAnalyzer worldAnalyzer(&fakeWorld,
                                emptyLoggingCallback);
    const auto actualSpawnWillCauseCrashResult = worldAnalyzer.SpawnWillCauseCrash(std::move(laneStream),
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
