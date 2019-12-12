/*********************************************************************
* Copyright (c) 2019 in-tech
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
#include "fakeSampler.h"
#include "dontCare.h"

using ::testing::Return;
using ::testing::Ne;
using ::testing::DontCare;
using ::testing::DoubleEq;
using ::testing::_;

namespace
{
    static constexpr auto SEARCH_FORWARDS = true;
    static constexpr auto SEARCH_BACKWARDS = false;
    static constexpr auto NO_AGENT = nullptr;
    static constexpr auto MAX_SEARCH_DISTANCE = std::numeric_limits<double>::max();
}

TEST(WorldAnalyzer, GetValidLaneSpawningRanges_NoScenarioAgents_ReturnsFullRange)
{
    const RoadId roadId{"ROADID"};
    const Route routeForRoadId{roadId};
    const LaneId laneId{-1};
    const SPosition sStart{0};
    const SPosition sEnd{100};
    const ValidLaneSpawningRanges expectedValidRanges{{sStart, sEnd}};

    FakeWorld fakeWorld;
    EXPECT_CALL(fakeWorld, GetNextAgentInLane(routeForRoadId,
                                              roadId,
                                              laneId,
                                              sEnd,
                                              SEARCH_BACKWARDS,
                                              MAX_SEARCH_DISTANCE))
            .WillOnce(Return(NO_AGENT));

    WorldAnalyzer worldAnalyzer(&fakeWorld,
                                [](const auto&) {});
    EXPECT_THAT(worldAnalyzer.GetValidLaneSpawningRanges(routeForRoadId, roadId, laneId, sStart, sEnd),
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
    const Route routeForRoadId{data.roadId};

    EXPECT_CALL(fakeWorld, GetNextAgentInLane(routeForRoadId,
                                              data.roadId,
                                              data.laneId,
                                              data.sEnd,
                                              SEARCH_BACKWARDS,
                                              MAX_SEARCH_DISTANCE))
            .WillOnce(Return(&fakeAgent));
    EXPECT_CALL(fakeWorld, GetNextAgentInLane(routeForRoadId,
                                              data.roadId,
                                              data.laneId,
                                              data.scenarioAgentBounds.first,
                                              SEARCH_BACKWARDS,
                                              MAX_SEARCH_DISTANCE))
            .WillOnce(Return(NO_AGENT));

    EXPECT_CALL(fakeAgent, GetAgentCategory())
            .WillRepeatedly(Return(data.agentCategory));

    RoadPosition fakeAgentRoadPosition;
    fakeAgentRoadPosition.s = data.scenarioAgentBounds.first;
    EXPECT_CALL(fakeAgent, GetRoadPosition())
            .WillRepeatedly(Return(fakeAgentRoadPosition));

    VehicleModelParameters fakeAgentVehicleModelParameters;
    fakeAgentVehicleModelParameters.length = data.scenarioAgentBounds.second - data.scenarioAgentBounds.first;
    fakeAgentVehicleModelParameters.distanceReferencePointToLeadingEdge = fakeAgentVehicleModelParameters.length;
    EXPECT_CALL(fakeAgent, GetVehicleModelParameters())
            .WillRepeatedly(Return(fakeAgentVehicleModelParameters));
    EXPECT_CALL(fakeAgent, GetRoadId(MeasurementPoint::Reference))
            .WillRepeatedly(Return(data.roadId));
    EXPECT_CALL(fakeAgent, GetMainLaneId(MeasurementPoint::Reference))
            .WillRepeatedly(Return(data.laneId));

    if (data.agentCategory != AgentCategory::Common)
    {
        EXPECT_CALL(fakeWorld, GetNextAgentInLane(routeForRoadId,
                                                  data.roadId,
                                                  data.laneId,
                                                  data.sStart,
                                                  SEARCH_FORWARDS,
                                                  MAX_SEARCH_DISTANCE))
                .WillOnce(Return(NO_AGENT));
    }

    WorldAnalyzer worldAnalyzer{&fakeWorld,
                                [](const auto&) {}};
    const auto actualValidRanges = worldAnalyzer.GetValidLaneSpawningRanges(routeForRoadId,
                                                                            data.roadId,
                                                                            data.laneId,
                                                                            data.sStart,
                                                                            data.sEnd);
    EXPECT_THAT(actualValidRanges.has_value(), data.shouldHaveValidRanges);
    if (data.shouldHaveValidRanges && actualValidRanges.has_value())
    {
        EXPECT_THAT(actualValidRanges->size(), data.expectedValidRanges.size());
        // expect that for every actual valid range, an expected valid range is found
        std::for_each(std::begin(*actualValidRanges),
                      std::end(*actualValidRanges),
                      [&data](const auto& range) -> void
        {
            const auto foundIter = std::find(std::begin(data.expectedValidRanges),
                                             std::end(data.expectedValidRanges),
                                             range);
            EXPECT_THAT(foundIter, Ne(std::end(data.expectedValidRanges)));
        });
    }
}

INSTANTIATE_TEST_CASE_P(WorldAnalyzer_AltersValidSpawnRangeCorrectly, GetValidLaneSpawningRanges_OneAgent,
    ::testing::Values(
                                              // roadId  | laneId | sStart | sEnd |      agentCategory     |      bounds      | shouldHaveValidRanges |     expectedValidRanges |
        // single common agent does not impact valid ranges
        GetValidLaneSpawningRanges_OneAgent_Data{"ROADID",      -1,       0,   100,   AgentCategory::Common,           {5, 10},                   true, {{0, 100}}},
        // single ego agent renders agent bounds invalid as spawning points
        GetValidLaneSpawningRanges_OneAgent_Data{"ROADID",      -1,       0,   100,      AgentCategory::Ego,           {5, 10},                   true, {{0, 5.0}, {10.01, 100}}},
        // single scenario agent renders agent bounds invalid as spawning points
        GetValidLaneSpawningRanges_OneAgent_Data{"ROADID",      -1,       0,   100, AgentCategory::Scenario,           {5, 10},                   true, {{0, 5}, {10.01, 100}}},
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
    const Route routeForRoadId{data.roadId};

    EXPECT_CALL(fakeWorld, GetNextAgentInLane(routeForRoadId,
                                              data.roadId,
                                              data.laneId,
                                              data.sEnd,
                                              SEARCH_BACKWARDS,
                                              MAX_SEARCH_DISTANCE))
            .WillOnce(Return(&firstFakeAgent));

    EXPECT_CALL(fakeWorld, GetNextAgentInLane(routeForRoadId,
                                              data.roadId,
                                              data.laneId,
                                              data.firstScenarioAgentBounds.first,
                                              SEARCH_BACKWARDS,
                                              MAX_SEARCH_DISTANCE))
            .WillOnce(Return(NO_AGENT));

    EXPECT_CALL(firstFakeAgent, GetAgentCategory())
            .WillRepeatedly(Return(data.firstAgentCategory));

    RoadPosition fakeAgentRoadPosition;
    fakeAgentRoadPosition.s = data.firstScenarioAgentBounds.first;
    EXPECT_CALL(firstFakeAgent, GetRoadPosition())
            .WillRepeatedly(Return(fakeAgentRoadPosition));

    VehicleModelParameters fakeAgentVehicleModelParameters;
    fakeAgentVehicleModelParameters.length = data.firstScenarioAgentBounds.second - data.firstScenarioAgentBounds.first;
    fakeAgentVehicleModelParameters.distanceReferencePointToLeadingEdge = fakeAgentVehicleModelParameters.length;
    EXPECT_CALL(firstFakeAgent, GetVehicleModelParameters())
            .WillRepeatedly(Return(fakeAgentVehicleModelParameters));

    EXPECT_CALL(firstFakeAgent, GetRoadId(MeasurementPoint::Reference))
            .WillRepeatedly(Return(data.roadId));
    EXPECT_CALL(firstFakeAgent, GetMainLaneId(MeasurementPoint::Reference))
            .WillRepeatedly(Return(data.laneId));

    if (data.firstAgentCategory != AgentCategory::Common)
    {
        EXPECT_CALL(secondFakeAgent, GetAgentCategory())
            .WillRepeatedly(Return(data.secondAgentCategory));

        RoadPosition secondFakeAgentRoadPosition;
        secondFakeAgentRoadPosition.s = data.secondScenarioAgentBounds.first;
        EXPECT_CALL(secondFakeAgent, GetRoadPosition())
                .WillRepeatedly(Return(secondFakeAgentRoadPosition));

        VehicleModelParameters secondFakeAgentVehicleModelParameters;
        secondFakeAgentVehicleModelParameters.length = data.secondScenarioAgentBounds.second - data.secondScenarioAgentBounds.first;
        secondFakeAgentVehicleModelParameters.distanceReferencePointToLeadingEdge = secondFakeAgentVehicleModelParameters.length;
        EXPECT_CALL(secondFakeAgent, GetVehicleModelParameters())
                .WillRepeatedly(Return(secondFakeAgentVehicleModelParameters));

        EXPECT_CALL(fakeWorld, GetNextAgentInLane(routeForRoadId,
                                                  data.roadId,
                                                  data.laneId,
                                                  data.sStart,
                                                  SEARCH_FORWARDS,
                                                  MAX_SEARCH_DISTANCE))
                .WillOnce(Return(&secondFakeAgent));

        EXPECT_CALL(fakeWorld, GetNextAgentInLane(routeForRoadId,
                                                  data.roadId,
                                                  data.laneId,
                                                  DoubleEq(data.secondScenarioAgentBounds.second + 0.01),
                                                  SEARCH_FORWARDS,
                                                  MAX_SEARCH_DISTANCE))
                .WillOnce(Return(NO_AGENT));

        EXPECT_CALL(secondFakeAgent, GetRoadId(MeasurementPoint::Reference))
                .WillRepeatedly(Return(data.roadId));
        EXPECT_CALL(secondFakeAgent, GetMainLaneId(MeasurementPoint::Reference))
                .WillRepeatedly(Return(data.laneId));
    }

    WorldAnalyzer worldAnalyzer{&fakeWorld,
                                [](const auto&) {}};
    const auto actualValidRanges = worldAnalyzer.GetValidLaneSpawningRanges(routeForRoadId,
                                                                            data.roadId,
                                                                            data.laneId,
                                                                            data.sStart,
                                                                            data.sEnd);
    EXPECT_THAT(actualValidRanges.has_value(), data.shouldHaveValidRanges);
    if (data.shouldHaveValidRanges && actualValidRanges.has_value())
    {
        EXPECT_THAT(actualValidRanges->size(), data.expectedValidRanges.size());
        // expect that for every actual valid range, an expected valid range is found
        std::for_each(std::begin(*actualValidRanges),
                      std::end(*actualValidRanges),
                      [&data](const auto& range) -> void
        {
            const auto foundIter = std::find(std::begin(data.expectedValidRanges),
                                             std::end(data.expectedValidRanges),
                                             range);
            EXPECT_THAT(foundIter, Ne(std::end(data.expectedValidRanges)));
        });
    }
}

INSTANTIATE_TEST_CASE_P(WorldAnalyzer_AltersValidSpawnRangeCorrectly, GetValidLaneSpawningRanges_TwoAgents,
    ::testing::Values(
                                               // roadId  | laneId | sStart | sEnd |    firstAgentCategory  | firstScenarioAgentBounds | secondAgentCategory    | secondAgentBounds | shouldHaveValidRanges | expectedValidRanges
        // only common agents -- full range should be valid
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,       0,   100,   AgentCategory::Common,         DontCare<Range>(),   AgentCategory::Common,  DontCare<Range>(),                   true, {{0, 100}}},
        // only a scenario agent and a common agent (the reverse will act as a one agent situation with the agent having category common)
        // only range outside of agent bounds should be valid
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,       0,   100,      AgentCategory::Ego,                   {5, 10},   AgentCategory::Common,             {0, 5},                   true, {{0, 5}, {10.01, 100}}}, // the first value of further ranges are padded with .01 to avoid re-detecting the same agent
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,       0,   100, AgentCategory::Scenario,                   {5, 10},   AgentCategory::Common,  DontCare<Range>(),                   true, {{0, 5}, {10.01, 100}}},
        // two internal scenario agents
        // spawn range before rear of nearest agent and after front of furthest agent is valid
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,       0,   100,      AgentCategory::Ego,                   {5, 10}, AgentCategory::Scenario,           {25, 50},                   true, {{0, 5}, {50.01, 100}}},
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,       0,   100, AgentCategory::Scenario,                   {5, 10}, AgentCategory::Scenario,           {25, 50},                   true, {{0, 5}, {50.01, 100}}},
        // two external scenario agents encapsualting spawn range -- no valid spawn range
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,      10,   100,      AgentCategory::Ego,                    {0, 5}, AgentCategory::Scenario,         {105, 110},                  false, DontCare<ValidLaneSpawningRanges>()},
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,      10,   100, AgentCategory::Scenario,                    {0, 5}, AgentCategory::Scenario,         {105, 110},                  false, DontCare<ValidLaneSpawningRanges>()},
        // one internal scenario agent and one external (beyond range) -- partial valid spawn range
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,      10,   100, AgentCategory::Scenario,                    {0, 5}, AgentCategory::Scenario,           {50, 55},                   true, {{55.01, 100}}},
        // one internal scenario agent and one external (before range) -- partial valid spawn range
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,      0,     75, AgentCategory::Scenario,                  {50, 55}, AgentCategory::Scenario,         {100, 105},                   true, {{0, 50}}},
        // two external agents (outside of range - beyond) -- full valid range
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,      0,   100,       AgentCategory::Ego,                {105, 110}, AgentCategory::Scenario,         {125, 130},                   true, {{0, 100}}},
        // two external agents (outside of range - before) -- full valid range
        GetValidLaneSpawningRanges_TwoAgents_Data{"ROADID",      -1,      100,   200, AgentCategory::Scenario,                   {0, 5}, AgentCategory::Scenario,           {55, 60},                   true, {{100, 200}}}
    ));

struct GetNextSpawnPositionTests_Data
{
    const std::pair<double, double> bounds;
    const bool agentExistsInRange;
    const double firstAgentInLanePosition;
    const double firstAgentRearLength;
    const double intendedVelocity;
    const double gapInSeconds;
    const bool expectingValidSpawnPosition;
    const double expectedSpawnPosition;
};

class GetNextSpawnPositionTests : public::testing::TestWithParam<GetNextSpawnPositionTests_Data>
{
public:
    GetNextSpawnPositionTests()
    {}

    FakeWorld fakeWorld;
    FakeAgent fakeAgent;
};

TEST_P(GetNextSpawnPositionTests, GetNextSpawnPosition)
{
    const auto data = GetParam();

    const RoadId roadId{"ROADID"};
    const Route routeForRoadId{roadId};
    const LaneId laneId{-1};
    const double agentFrontLength{0.5};
    const double agentRearLength{0.5};

    if (!data.agentExistsInRange)
    {
        EXPECT_CALL(fakeWorld, GetNextObjectInLane(routeForRoadId,
                                                  roadId,
                                                  laneId,
                                                  data.bounds.first,
                                                  SEARCH_FORWARDS,
                                                  data.bounds.second + data.intendedVelocity * data.gapInSeconds))
                .WillOnce(Return(NO_AGENT));

        EXPECT_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(routeForRoadId,
                                                             roadId,
                                                             laneId,
                                                             0,
                                                             MAX_SEARCH_DISTANCE))
                .WillOnce(Return(MAX_SEARCH_DISTANCE));

        EXPECT_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(routeForRoadId,
                                                             roadId,
                                                             laneId,
                                                             data.bounds.second,
                                                             data.bounds.second))
                .WillOnce(Return(MAX_SEARCH_DISTANCE));

    }

    else
    {
        EXPECT_CALL(fakeWorld, GetNextObjectInLane(routeForRoadId,
                                                  roadId,
                                                  laneId,
                                                  data.bounds.first,
                                                  SEARCH_FORWARDS,
                                                  data.bounds.second + data.intendedVelocity * data.gapInSeconds))
                .WillOnce(Return(&fakeAgent));

        EXPECT_CALL(fakeAgent, GetDistanceToStartOfRoad())
                .WillRepeatedly(Return(data.firstAgentInLanePosition));

        // create an arbitrary agent length such that the first agent rear and front length are equal.
        const auto agentLength = data.firstAgentRearLength * 2;
        EXPECT_CALL(fakeAgent, GetLength())
                .WillOnce(Return(agentLength));
        EXPECT_CALL(fakeAgent, GetDistanceReferencePointToLeadingEdge())
                .WillOnce(Return(data.firstAgentRearLength));
    }

    WorldAnalyzer worldAnalyzer(&fakeWorld,
                                [](const auto&) {});
    const auto nextSpawnPosition = worldAnalyzer.GetNextSpawnPosition(routeForRoadId,
                                                                      roadId,
                                                                      laneId,
                                                                      data.bounds,
                                                                      agentFrontLength,
                                                                      agentRearLength,
                                                                      data.intendedVelocity,
                                                                      data.gapInSeconds,
                                                                      Direction::FORWARD);

    EXPECT_THAT(nextSpawnPosition.has_value(), data.expectingValidSpawnPosition);
    if (data.expectingValidSpawnPosition)
    {
        EXPECT_THAT(nextSpawnPosition.value(), data.expectedSpawnPosition);
    }
}
INSTANTIATE_TEST_CASE_P(WorldAnalyzer_GetNextSpawnPosition, GetNextSpawnPositionTests,
    ::testing::Values(
                                   //  bounds  | agentExistsInRange | firstAgentInLanePosition | firstAgentRearLength | intendedVelocity | gapInSeconds |expectingValidSpawnPosition | expectedSpawnPosition
        // no agents yet spawned in range -- spawn first at end of range (minus front length)
        GetNextSpawnPositionTests_Data{{0, 100},               false,        DontCare<double>(),    DontCare<double>(),                30,             1,                        true, 99.5}, // no agent in lane - last possible position in range minus agentFrontLength
        // agent spawned in lane -- spawn behind with calculated minimum gap
        GetNextSpawnPositionTests_Data{{0, 500},                true,                       100,                   0.5,                30,             1,                        true, 67},  // agent at end of lane (position 100) - separation is minimum of gapInSeconds * velocity + rearLength of front agent + frontLength of spawnedAgent
                                                                                                                                                                                             // OR MINIMUM_SEPARATION_BUFFER (2.0) + rearLength of front agent + frontLength of spawned agent + MINIMUM_GAP_IN_SECONDS (1.0) * velocity
        // agent spawned in lane -- spawn behind with calculated minimum gap
        GetNextSpawnPositionTests_Data{{0, 500},                true,                       100,                   0.5,                30,             2,                        true, 39},
        // agent spawned in lane with no room left for new agent -- no valid spawn position
        GetNextSpawnPositionTests_Data{{0, 100},                true,                         1,                   0.5,                30,             1,                       false, DontCare<double>()}
));

struct CalculateSpawnVelocityToPreventCrashing_Data
{
    const bool objectExistsInLane;
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
    const Route routeForRoadId{roadId};
    const LaneId laneId = -1;
    const double intendedVehicleFrontPosition = data.intendedSpawnPosition + data.agentFrontLength;

    FakeAgent fakeAgent;
    FakeWorld fakeWorld;

    if (data.objectExistsInLane)
    {
        EXPECT_CALL(fakeWorld, GetNextObjectInLane(routeForRoadId,
                                                   roadId,
                                                   laneId,
                                                   _,
                                                   SEARCH_FORWARDS))
                .WillOnce(Return(&fakeAgent));

        EXPECT_CALL(fakeAgent, GetDistanceToStartOfRoad(MeasurementPoint::Rear))
                .WillOnce(Return(data.opponentRearDistanceFromStartOfRoad));
        EXPECT_CALL(fakeAgent, GetVelocity())
                .WillOnce(Return(data.opponentVelocity));
        EXPECT_CALL(fakeAgent, GetDistanceToStartOfRoad(MeasurementPoint::Front))
                .WillOnce(Return(MAX_SEARCH_DISTANCE));
    }
    else
    {
        EXPECT_CALL(fakeWorld, GetNextObjectInLane(routeForRoadId,
                                                   roadId,
                                                   laneId,
                                                   _,
                                                   SEARCH_FORWARDS))
                .WillOnce(Return(nullptr));
        EXPECT_CALL(fakeWorld, GetDistanceToEndOfDrivingLane(routeForRoadId,
                                                             roadId,
                                                             laneId,
                                                             intendedVehicleFrontPosition,
                                                             _))
                .WillOnce(Return(MAX_SEARCH_DISTANCE));
    }

    const auto emptyLoggingCallback = [](const std::string&) {};
    WorldAnalyzer worldAnalyzer(&fakeWorld,
                                emptyLoggingCallback);
    const auto actualAdjustedVelocity = worldAnalyzer.CalculateSpawnVelocityToPreventCrashing(routeForRoadId,
                                                                                              roadId,
                                                                                              laneId,
                                                                                              data.intendedSpawnPosition,
                                                                                              data.agentFrontLength,
                                                                                              data.agentRearLength,
                                                                                              data.intendedVelocity);

    EXPECT_THAT(actualAdjustedVelocity, DoubleEq(data.expectedAdjustedVelocity));
}

INSTANTIATE_TEST_CASE_P(WorldAnalyzer_CalculateSpawnVelocityToPreventCrashing, CalculateSpawnVelocityToPreventCrashingTests,
    ::testing::Values(
                                                  // objectExistsInLane | opponentRearDistanceFromStartOfRoad |  opponentVelocity | intendedSpawnPosition | agentFrontLength | agentRearLength | intendedVelocity | expectedAdjustedVelocity |
        CalculateSpawnVelocityToPreventCrashing_Data{              false,                   DontCare<double>(), DontCare<double>(),                     50,               0.5,              0.5,                30,                        30},
        CalculateSpawnVelocityToPreventCrashing_Data{               true,                                   60,                 20,                     50,               0.5,              0.5,                30,                        30 - (60 / 3.6)}
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
    const Route route{roadId};
    const LaneId laneId{-1};
    const SPosition sPosition{data.spawnPosition};
    const double agentFrontLength{.5};
    const double agentRearLength{.5};
    const double velocity{data.spawnVelocity};
    const bool searchDirection = (data.direction == Direction::FORWARD);

    if (!data.objectExistsInSearchDirection)
    {
        EXPECT_CALL(fakeWorld, GetNextObjectInLane(route,
                                                   roadId,
                                                   laneId,
                                                   sPosition,
                                                   searchDirection))
                .WillOnce(Return(nullptr));
    }
    else
    {
        EXPECT_CALL(fakeWorld, GetNextObjectInLane(route,
                                                   roadId,
                                                   laneId,
                                                   sPosition,
                                                   searchDirection))
                .WillOnce(Return(&fakeObjectInLane));

        EXPECT_CALL(fakeObjectInLane, GetVelocity())
                .WillOnce(Return(data.objectVelocity));

        if (data.direction == Direction::FORWARD)
        {
            EXPECT_CALL(fakeObjectInLane, GetDistanceToStartOfRoad(MeasurementPoint::Rear))
                    .WillOnce(Return(data.objectPosition));
        }
        else
        {
            EXPECT_CALL(fakeObjectInLane, GetDistanceToStartOfRoad(MeasurementPoint::Front))
                    .WillOnce(Return(data.objectPosition));
        }
    }

    const auto emptyLoggingCallback = [](const std::string&) {};
    WorldAnalyzer worldAnalyzer(&fakeWorld,
                                emptyLoggingCallback);
    const auto actualSpawnWillCauseCrashResult = worldAnalyzer.SpawnWillCauseCrash(route,
                                                                                  roadId,
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
