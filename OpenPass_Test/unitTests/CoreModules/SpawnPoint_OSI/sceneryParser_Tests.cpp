/*********************************************************************
* Copyright (c) 2018 2019 in-tech
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "SpawnPointSceneryParser.h"
#include "fakeWorld.h"

using ::testing::_;
using ::testing::Eq;
using ::testing::IsEmpty;
using ::testing::Lt;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::SizeIs;

TEST(EntryLaneEvaluation, WithTwoDrivingLanesOutOfThreeLanes_GeneratesTwoParameterSets)
{
    NiceMock<FakeWorld> fakeWorld;
    //QueryResult: streamId, startDistance, endDistance, laneCategory, isDrivingLane
    ON_CALL(fakeWorld, QueryLane(_, -1, 0.0))
    .WillByDefault(Return(LaneQueryResult{0, 0.0, 100.0, LaneCategory::RegularLane, true}));
    ON_CALL(fakeWorld, QueryLane(_, -2, 0.0))
    .WillByDefault(Return(LaneQueryResult{1, 0.0, 100.0, LaneCategory::RightMostLane, true}));
    ON_CALL(fakeWorld, QueryLane(_, -3, 0.0))
    .WillByDefault(Return(LaneQueryResult{2, 0.0, 100.0, LaneCategory::RegularLane, false}));
    ON_CALL(fakeWorld, QueryLane(_, -4, 0.0))
    .WillByDefault(Return(LaneQueryResult::InvalidResult()));

    SpawnPointHelper::SceneryParser sceneryParser(fakeWorld, "");

    auto lanes = sceneryParser.GetEntryLanes();
    ASSERT_THAT(lanes, SizeIs(2));
    EXPECT_THAT(lanes[0].laneId, -1);
    EXPECT_THAT(lanes[1].laneId, -2);
}

TEST(EntryLaneEvaluation, WithoutAnyDrivingLanes_GeneratesNoParameterSets)
{
    NiceMock<FakeWorld> fakeWorld;

    ON_CALL(fakeWorld, QueryLane(_, -1, 0.0)).WillByDefault(Return(LaneQueryResult::InvalidResult()));

    SpawnPointHelper::SceneryParser sceneryParser(fakeWorld, "");

    ASSERT_THAT(sceneryParser.GetEntryLanes(), IsEmpty());
}

TEST(EgoAndScenery, AddEgoLaneParameter_AddsTheParameterRemovesItFromCommons)
{
    NiceMock<FakeWorld> fakeWorld;
    auto laneQuery = std::list<LaneQueryResult>
    {
        LaneQueryResult{0,  0.0, 100.0, LaneCategory::RegularLane, true},
        LaneQueryResult{1,  0.0, 100.0, LaneCategory::RegularLane, true},
        LaneQueryResult{2, 50.0, 100.0, LaneCategory::RegularLane, true}
    };
    ON_CALL(fakeWorld, QueryLanes(_, _, _)).WillByDefault(Return(laneQuery));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));

    EgoSpawnParameters egoSpawnParameter;
    egoSpawnParameter.spawnRadius = 1000;
    SpawnPointHelper::SceneryParser sceneryParser(fakeWorld, "");
    sceneryParser.Parse(egoSpawnParameter);

    LaneParameters egoLaneParameters;
    egoLaneParameters.streamId = 1;

    EXPECT_THAT(sceneryParser.GetEgoAndSceneryParameters(), IsEmpty());
    EXPECT_THAT(sceneryParser.GetCommonLaneStreamParameters(), SizeIs(3));

    sceneryParser.PushEgoParameters(egoLaneParameters);

    EXPECT_THAT(sceneryParser.GetCommonLaneStreamParameters(), SizeIs(2));
    EXPECT_THAT(sceneryParser.GetEgoAndSceneryParameters(), SizeIs(1));
}

TEST(EgoAndScenery, AddEgoLaneParameterTwice_ThrowsLogicError)
{
    NiceMock<FakeWorld> fakeWorld;

    ON_CALL(fakeWorld, QueryLane(_, -1, 0.0)).WillByDefault(Return(LaneQueryResult::InvalidResult()));
    SpawnPointHelper::SceneryParser sceneryParser(fakeWorld, "");

    LaneParameters egoLaneParameters;

    sceneryParser.PushEgoParameters(egoLaneParameters);
    try
    {
        sceneryParser.PushEgoParameters(egoLaneParameters);
        FAIL() << "Expected std::logic_error";
    }
    catch (std::logic_error const& err)
    {
        EXPECT_THAT(err.what(), ::testing::HasSubstr("ego"));
    }
    catch (...)
    {
        FAIL() << "Expected std::logic_error";
    }
}

TEST(EgoAndScenery, AddSceneryLaneParameter_AddsTheParameterRemovesItFromCommons)
{
    NiceMock<FakeWorld> fakeWorld;
    auto laneQuery = std::list<LaneQueryResult>
    {
        LaneQueryResult{0,  0.0, 100.0, LaneCategory::RegularLane, true},
        LaneQueryResult{1,  0.0, 100.0, LaneCategory::RegularLane, true},
        LaneQueryResult{2, 50.0, 100.0, LaneCategory::RegularLane, true}
    };
    ON_CALL(fakeWorld, QueryLanes(_, _, _)).WillByDefault(Return(laneQuery));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));

    LaneParameters sceneryLaneParameters(1, -1, LaneCategory::RegularLane, 50, 0);

    EgoSpawnParameters egoSpawnParameter;
    egoSpawnParameter.spawnRadius = 1000;
    SpawnPointHelper::SceneryParser sceneryParser(fakeWorld, "");
    sceneryParser.Parse(egoSpawnParameter);

    EXPECT_THAT(sceneryParser.GetCommonLaneStreamParameters(), SizeIs(3));
    EXPECT_THAT(sceneryParser.GetEgoAndSceneryParameters(), IsEmpty());

    sceneryParser.PushSceneryParameters(sceneryLaneParameters);
    EXPECT_THAT(sceneryParser.GetCommonLaneStreamParameters(), SizeIs(2));
    EXPECT_THAT(sceneryParser.GetEgoAndSceneryParameters(), SizeIs(1));

}

TEST(Common, RemoveLastCommonLaneParameters_ReducesList)
{
    NiceMock<FakeWorld> fakeWorld;
    auto laneQuery = std::list<LaneQueryResult>
    {
            LaneQueryResult{0,  0.0, 100.0, LaneCategory::RegularLane, true},
            LaneQueryResult{1,  0.0, 100.0, LaneCategory::RegularLane, true}
    };
    ON_CALL(fakeWorld, QueryLanes(_, _, _)).WillByDefault(Return(laneQuery));
    ON_CALL(fakeWorld, QueryLane(_, _, _)).WillByDefault(Return(LaneQueryResult::InvalidResult()));

    EgoSpawnParameters egoSpawnParameter;
    egoSpawnParameter.spawnRadius = 1000;
    SpawnPointHelper::SceneryParser sceneryParser(fakeWorld, "");
    sceneryParser.Parse(egoSpawnParameter);

    LaneParameters laneParameters;
    EXPECT_THAT(sceneryParser.RemoveLastCommonLaneParameter(), Eq(true));
    EXPECT_THAT(sceneryParser.RemoveLastCommonLaneParameter(), Eq(true));
    EXPECT_THAT(sceneryParser.RemoveLastCommonLaneParameter(), Eq(false));
}
