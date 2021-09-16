/*******************************************************************************
 * Copyright (c) 2021, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "SpawnerPreRunCommon.h"
#include "SpawnerPreRunCommonParameterExtractor.h"
#include "common/WorldAnalyzer.h"
#include "fakeCallback.h"
#include "fakeParameter.h"
#include "fakeStream.h"
#include "fakeWorld.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::_;
using ::testing::ByMove;
using ::testing::ElementsAre;
using ::testing::Eq;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::SizeIs;

TEST(SpawnerPreRunCommonParameterExtractor, GetWayToNextRoad_LinearGraph)
{
    RoadGraph roadGraph;
    auto start = add_vertex(RouteElement{"RoadStart", true}, roadGraph);
    auto node1 = add_vertex(RouteElement{"Road1", false}, roadGraph);
    auto node2 = add_vertex(RouteElement{"Road2", true}, roadGraph);
    auto node3 = add_vertex(RouteElement{"Road3", true}, roadGraph);
    add_edge(start, node1, roadGraph);
    add_edge(node1, node2, roadGraph);
    add_edge(node2, node3, roadGraph);

    auto result = GetWayToNextRoad("Road2", roadGraph, start);

    EXPECT_THAT(result.first, ElementsAre(RouteElement{"Road1", false}, RouteElement{"Road2", true}));
    EXPECT_THAT(result.second, Eq(node2));
}

TEST(SpawnerPreRunCommonParameterExtractor, GetWayToNextRoad_BranchingGraph)
{
    RoadGraph roadGraph;
    auto start = add_vertex(RouteElement{"RoadStart", true}, roadGraph);
    auto node1a = add_vertex(RouteElement{"Road1a", false}, roadGraph);
    auto node1b = add_vertex(RouteElement{"Road1b", true}, roadGraph);
    auto node2a = add_vertex(RouteElement{"Road2a", false}, roadGraph);
    auto node2b = add_vertex(RouteElement{"Road2b", true}, roadGraph);
    add_edge(start, node1a, roadGraph);
    add_edge(start, node1b, roadGraph);
    add_edge(node1b, node2a, roadGraph);
    add_edge(node1b, node2b, roadGraph);

    auto result = GetWayToNextRoad("Road2a", roadGraph, start);

    EXPECT_THAT(result.first, ElementsAre(RouteElement{"Road1b", true}, RouteElement{"Road2a", false}));
    EXPECT_THAT(result.second, Eq(node2a));
}

TEST(SpawnerPreRunCommonParameterExtractor, GetWayToNextRoad_NoPossibleWay)
{
    RoadGraph roadGraph;
    auto start = add_vertex(RouteElement{"RoadStart", true}, roadGraph);
    auto node1a = add_vertex(RouteElement{"Road1a", false}, roadGraph);
    auto node1b = add_vertex(RouteElement{"Road1b", true}, roadGraph);
    auto node2a = add_vertex(RouteElement{"Road2a", false}, roadGraph);
    auto node2b = add_vertex(RouteElement{"Road2b", true}, roadGraph);
    add_edge(start, node1a, roadGraph);
    add_edge(start, node1b, roadGraph);
    add_edge(node1b, node2a, roadGraph);
    add_edge(node1b, node2b, roadGraph);

    auto result = GetWayToNextRoad("OtherRoad", roadGraph, start);

    EXPECT_THAT(result.first, ElementsAre());
}

TEST(SpawnerPreRunCommonParameterExtractor, ExtractSpawnAreas_AllOptionalParameters)
{
    FakeCallback callbacks;

    FakeParameter parameter;
    auto spawnPoint1 = std::make_shared<FakeParameter>();
    auto spawnPoint2 = std::make_shared<FakeParameter>();
    ParameterInterface::ParameterLists spawnPoints {{spawnPoint1, spawnPoint2}};

    std::map<std::string, const std::vector<std::string>> strings1{{"Roads", {"RoadA", "RoadB"}}};
    ON_CALL(*spawnPoint1, GetParametersStringVector()).WillByDefault(ReturnRef(strings1));
    std::map<std::string, const std::vector<int>> intVectors1{{"Lanes", {-1,-2}}};
    ON_CALL(*spawnPoint1, GetParametersIntVector()).WillByDefault(ReturnRef(intVectors1));
    std::map<std::string, double> doubles1{{"SStart", 10.0}, {"SEnd", 50.0}};
    ON_CALL(*spawnPoint1, GetParametersDouble()).WillByDefault(ReturnRef(doubles1));

    std::map<std::string, const std::vector<std::string>> strings2{{"Roads", {"RoadA"}}};
    ON_CALL(*spawnPoint2, GetParametersStringVector()).WillByDefault(ReturnRef(strings2));
    std::map<std::string, const std::vector<int>> intVectors2{{"Lanes", {-3,-4}}};
    ON_CALL(*spawnPoint2, GetParametersIntVector()).WillByDefault(ReturnRef(intVectors2));
    std::map<std::string, double> doubles2{{"SStart", 15.0}, {"SLength", 40.0}};
    ON_CALL(*spawnPoint2, GetParametersDouble()).WillByDefault(ReturnRef(doubles2));

    std::map<std::string, ParameterInterface::ParameterLists> parameterLists{{"SpawnZones", spawnPoints}};
    ON_CALL(parameter, GetParameterLists()).WillByDefault(ReturnRef(parameterLists));

    FakeWorld fakeWorld;
    ON_CALL(fakeWorld, IsDirectionalRoadExisting(_,true)).WillByDefault(Return(true));
    ON_CALL(fakeWorld, IsDirectionalRoadExisting(_,false)).WillByDefault(Return(false));

    RoadGraph roadGraphIn;
    auto nodeA = add_vertex(RouteElement{"RoadA", true}, roadGraphIn);
    auto nodeB = add_vertex(RouteElement{"RoadB", true}, roadGraphIn);
    add_edge(nodeA, nodeB, roadGraphIn);
    RoadGraph roadGraphAgainst;
    auto nodeAAgainst = add_vertex(RouteElement{"RoadA", false}, roadGraphAgainst);
    ON_CALL(fakeWorld, GetRoadGraph(RouteElement{"RoadA", true}, _)).WillByDefault(Return(std::make_pair(roadGraphIn, nodeA)));
    ON_CALL(fakeWorld, GetRoadGraph(RouteElement{"RoadA", false}, _)).WillByDefault(Return(std::make_pair(roadGraphAgainst, nodeAAgainst)));

    std::unique_ptr<FakeRoadStream> roadStreamLong = std::make_unique<FakeRoadStream>();
    LaneStreamInterface* laneStream1 = new FakeLaneStream;
    ON_CALL(*roadStreamLong, GetLaneStream(_,-1)).WillByDefault(Return(ByMove(std::unique_ptr<LaneStreamInterface>(laneStream1))));
    LaneStreamInterface* laneStream2 = new FakeLaneStream;
    ON_CALL(*roadStreamLong, GetLaneStream(_,-2)).WillByDefault(Return(ByMove(std::unique_ptr<LaneStreamInterface>(laneStream2))));
    ON_CALL(*roadStreamLong, GetStreamPosition(_)).WillByDefault([](GlobalRoadPosition position)
    {if(position.roadId == "RoadA")
        {return StreamPosition{position.roadPosition.s, 0};}
        if(position.roadId == "RoadB")
        {return StreamPosition{position.roadPosition.s + 1000.0, 0};}
        return StreamPosition{-1, 0};});
    ON_CALL(fakeWorld, GetRoadStream(std::vector<RouteElement>{{"RoadA", true}, {"RoadB", true}})).
            WillByDefault(Return(ByMove(std::move(roadStreamLong))));

    std::unique_ptr<FakeRoadStream> roadStreamShort = std::make_unique<FakeRoadStream>();
    LaneStreamInterface* laneStream3 = new FakeLaneStream;
    ON_CALL(*roadStreamShort, GetLaneStream(_,-3)).WillByDefault(Return(ByMove(std::unique_ptr<LaneStreamInterface>(laneStream3))));
    LaneStreamInterface* laneStream4 = new FakeLaneStream;
    ON_CALL(*roadStreamShort, GetLaneStream(_,-4)).WillByDefault(Return(ByMove(std::unique_ptr<LaneStreamInterface>(laneStream4))));
    ON_CALL(*roadStreamShort, GetStreamPosition(_)).WillByDefault([](GlobalRoadPosition position)
    {if(position.roadId == "RoadA")
        {return StreamPosition{position.roadPosition.s, 0};}
        return StreamPosition{-1, 0};});
    ON_CALL(fakeWorld, GetRoadStream(std::vector<RouteElement>{{"RoadA", true}})).
            WillByDefault(Return(ByMove(std::move(roadStreamShort))));

    auto result = ExtractSpawnAreas(parameter, fakeWorld, &callbacks);

    ASSERT_THAT(result, SizeIs(4));
    EXPECT_THAT(result.at(0).laneStream.get(), Eq(laneStream1));
    EXPECT_THAT(result.at(0).sStart, Eq(10.0));
    EXPECT_THAT(result.at(0).sEnd, Eq(1050.0));
    EXPECT_THAT(result.at(1).laneStream.get(), Eq(laneStream2));
    EXPECT_THAT(result.at(1).sStart, Eq(10.0));
    EXPECT_THAT(result.at(1).sEnd, Eq(1050.0));
    EXPECT_THAT(result.at(2).laneStream.get(), Eq(laneStream3));
    EXPECT_THAT(result.at(2).sStart, Eq(15.0));
    EXPECT_THAT(result.at(2).sEnd, Eq(55.0));
    EXPECT_THAT(result.at(3).laneStream.get(), Eq(laneStream4));
    EXPECT_THAT(result.at(3).sStart, Eq(15.0));
    EXPECT_THAT(result.at(3).sEnd, Eq(55.0));
}

TEST(SpawnerPreRunCommonParameterExtractor, ExtractSpawnAreas_NoOptionalParameters)
{
    FakeCallback callbacks;

    FakeParameter parameter;
    auto spawnPoint1 = std::make_shared<FakeParameter>();
    auto spawnPoint2 = std::make_shared<FakeParameter>();
    ParameterInterface::ParameterLists spawnPoints {{spawnPoint1, spawnPoint2}};

    std::map<std::string, const std::vector<std::string>> strings1{{"Roads", {"RoadA", "RoadB"}}};
    ON_CALL(*spawnPoint1, GetParametersStringVector()).WillByDefault(ReturnRef(strings1));
    std::map<std::string, const std::vector<int>> intVectors1{};
    ON_CALL(*spawnPoint1, GetParametersIntVector()).WillByDefault(ReturnRef(intVectors1));
    std::map<std::string, double> doubles1{};
    ON_CALL(*spawnPoint1, GetParametersDouble()).WillByDefault(ReturnRef(doubles1));

    std::map<std::string, const std::vector<std::string>> strings2{{"Roads", {"RoadA"}}};
    ON_CALL(*spawnPoint2, GetParametersStringVector()).WillByDefault(ReturnRef(strings2));
    std::map<std::string, const std::vector<int>> intVectors2{};
    ON_CALL(*spawnPoint2, GetParametersIntVector()).WillByDefault(ReturnRef(intVectors2));
    std::map<std::string, double> doubles2{};
    ON_CALL(*spawnPoint2, GetParametersDouble()).WillByDefault(ReturnRef(doubles2));

    std::map<std::string, ParameterInterface::ParameterLists> parameterLists{{"SpawnZones", spawnPoints}};
    ON_CALL(parameter, GetParameterLists()).WillByDefault(ReturnRef(parameterLists));

    FakeWorld fakeWorld;
    ON_CALL(fakeWorld, IsDirectionalRoadExisting(_,true)).WillByDefault(Return(true));
    ON_CALL(fakeWorld, IsDirectionalRoadExisting(_,false)).WillByDefault(Return(false));

    RoadGraph roadGraphIn;
    auto nodeA = add_vertex(RouteElement{"RoadA", true}, roadGraphIn);
    auto nodeB = add_vertex(RouteElement{"RoadB", true}, roadGraphIn);
    add_edge(nodeA, nodeB, roadGraphIn);
    RoadGraph roadGraphAgainst;
    auto nodeAAgainst = add_vertex(RouteElement{"RoadA", false}, roadGraphAgainst);
    ON_CALL(fakeWorld, GetRoadGraph(RouteElement{"RoadA", true}, _)).WillByDefault(Return(std::make_pair(roadGraphIn, nodeA)));
    ON_CALL(fakeWorld, GetRoadGraph(RouteElement{"RoadA", false}, _)).WillByDefault(Return(std::make_pair(roadGraphAgainst, nodeAAgainst)));

    std::unique_ptr<FakeRoadStream> roadStreamLong = std::make_unique<FakeRoadStream>();
    LaneStreamInterface* laneStream1 = new FakeLaneStream;
    LaneStreamInterface* laneStream2 = new FakeLaneStream;
    std::vector<std::unique_ptr<LaneStreamInterface>> laneStreamsA{};
    laneStreamsA.emplace_back(laneStream1);
    laneStreamsA.emplace_back(laneStream2);
    ON_CALL(*roadStreamLong, GetAllLaneStreams()).WillByDefault(Return(ByMove(std::move(laneStreamsA))));
    ON_CALL(*roadStreamLong, GetLength()).WillByDefault(Return(1500.0));
    ON_CALL(fakeWorld, GetRoadStream(std::vector<RouteElement>{{"RoadA", true}, {"RoadB", true}})).
            WillByDefault(Return(ByMove(std::move(roadStreamLong))));

    std::unique_ptr<FakeRoadStream> roadStreamShort = std::make_unique<FakeRoadStream>();
    LaneStreamInterface* laneStream3 = new FakeLaneStream;
    LaneStreamInterface* laneStream4 = new FakeLaneStream;
    std::vector<std::unique_ptr<LaneStreamInterface>> laneStreamsB{};
    laneStreamsB.emplace_back(laneStream3);
    laneStreamsB.emplace_back(laneStream4);
    ON_CALL(*roadStreamShort, GetAllLaneStreams()).WillByDefault(Return(ByMove(std::move(laneStreamsB))));
    ON_CALL(*roadStreamShort, GetLength()).WillByDefault(Return(1000.0));
    ON_CALL(fakeWorld, GetRoadStream(std::vector<RouteElement>{{"RoadA", true}})).
            WillByDefault(Return(ByMove(std::move(roadStreamShort))));

    auto result = ExtractSpawnAreas(parameter, fakeWorld, &callbacks);

    ASSERT_THAT(result, SizeIs(4));
    EXPECT_THAT(result.at(0).laneStream.get(), Eq(laneStream1));
    EXPECT_THAT(result.at(0).sStart, Eq(0.0));
    EXPECT_THAT(result.at(0).sEnd, Eq(1500.0));
    EXPECT_THAT(result.at(1).laneStream.get(), Eq(laneStream2));
    EXPECT_THAT(result.at(1).sStart, Eq(0.0));
    EXPECT_THAT(result.at(1).sEnd, Eq(1500.0));
    EXPECT_THAT(result.at(2).laneStream.get(), Eq(laneStream3));
    EXPECT_THAT(result.at(2).sStart, Eq(0.0));
    EXPECT_THAT(result.at(2).sEnd, Eq(1000.0));
    EXPECT_THAT(result.at(3).laneStream.get(), Eq(laneStream4));
    EXPECT_THAT(result.at(3).sStart, Eq(0.0));
    EXPECT_THAT(result.at(3).sEnd, Eq(1000.0));
}

TEST(SpawnerPreRunCommonParameterExtractor, ExtractSpawnAreas_AgaintOdDirection)
{
    FakeCallback callbacks;

    FakeParameter parameter;
    auto spawnPoint = std::make_shared<FakeParameter>();
    ParameterInterface::ParameterLists spawnPoints {{spawnPoint}};

    std::map<std::string, const std::vector<std::string>> strings1{{"Roads", {"RoadA", "RoadB"}}};
    ON_CALL(*spawnPoint, GetParametersStringVector()).WillByDefault(ReturnRef(strings1));
    std::map<std::string, const std::vector<int>> intVectors1{{"Lanes", {1,2}}};
    ON_CALL(*spawnPoint, GetParametersIntVector()).WillByDefault(ReturnRef(intVectors1));
    std::map<std::string, double> doubles1{{"SStart", 10.0}, {"SEnd", 50.0}};
    ON_CALL(*spawnPoint, GetParametersDouble()).WillByDefault(ReturnRef(doubles1));

    std::map<std::string, ParameterInterface::ParameterLists> parameterLists{{"SpawnZones", spawnPoints}};
    ON_CALL(parameter, GetParameterLists()).WillByDefault(ReturnRef(parameterLists));

    FakeWorld fakeWorld;
    ON_CALL(fakeWorld, IsDirectionalRoadExisting(_,true)).WillByDefault(Return(false));
    ON_CALL(fakeWorld, IsDirectionalRoadExisting(_,false)).WillByDefault(Return(true));

    RoadGraph roadGraph;
    auto nodeA = add_vertex(RouteElement{"RoadA", false}, roadGraph);
    auto nodeB = add_vertex(RouteElement{"RoadB", false}, roadGraph);
    add_edge(nodeA, nodeB, roadGraph);
    ON_CALL(fakeWorld, GetRoadGraph(RouteElement{"RoadA", false}, _)).WillByDefault(Return(std::make_pair(roadGraph, nodeA)));

    std::unique_ptr<FakeRoadStream> roadStream = std::make_unique<FakeRoadStream>();
    LaneStreamInterface* laneStream1 = new FakeLaneStream;
    ON_CALL(*roadStream, GetLaneStream(_,1)).WillByDefault(Return(ByMove(std::unique_ptr<LaneStreamInterface>(laneStream1))));
    LaneStreamInterface* laneStream2 = new FakeLaneStream;
    ON_CALL(*roadStream, GetLaneStream(_,2)).WillByDefault(Return(ByMove(std::unique_ptr<LaneStreamInterface>(laneStream2))));
    ON_CALL(*roadStream, GetStreamPosition(_)).WillByDefault([](GlobalRoadPosition position)
    {if(position.roadId == "RoadA")
        {return StreamPosition{1000. - position.roadPosition.s, 0};}
        if(position.roadId == "RoadB")
        {return StreamPosition{1500. - position.roadPosition.s, 0};}
        return StreamPosition{-1, 0};});
    ON_CALL(fakeWorld, GetRoadStream(std::vector<RouteElement>{{"RoadA", false}, {"RoadB", false}})).
            WillByDefault(Return(ByMove(std::move(roadStream))));

    auto result = ExtractSpawnAreas(parameter, fakeWorld, &callbacks);

    ASSERT_THAT(result, SizeIs(2));
    EXPECT_THAT(result.at(0).laneStream.get(), Eq(laneStream1));
    EXPECT_THAT(result.at(0).sStart, Eq(990.0));
    EXPECT_THAT(result.at(0).sEnd, Eq(1450.0));
    EXPECT_THAT(result.at(1).laneStream.get(), Eq(laneStream2));
    EXPECT_THAT(result.at(1).sStart, Eq(990.0));
    EXPECT_THAT(result.at(1).sEnd, Eq(1450.0));
}


TEST(SpawnerPreRunCommonParameterExtractor, ExtractSpawnAreas_AllLanesBothDirection)
{
    FakeCallback callbacks;

    FakeParameter parameter;
    auto spawnPoint = std::make_shared<FakeParameter>();
    ParameterInterface::ParameterLists spawnPoints {{spawnPoint}};

    std::map<std::string, const std::vector<std::string>> strings1{{"Roads", {"RoadA", "RoadB"}}};
    ON_CALL(*spawnPoint, GetParametersStringVector()).WillByDefault(ReturnRef(strings1));
    std::map<std::string, const std::vector<int>> intVectors1{};
    ON_CALL(*spawnPoint, GetParametersIntVector()).WillByDefault(ReturnRef(intVectors1));
    std::map<std::string, double> doubles1{{"SStart", 10.0}, {"SEnd", 50.0}};
    ON_CALL(*spawnPoint, GetParametersDouble()).WillByDefault(ReturnRef(doubles1));

    std::map<std::string, ParameterInterface::ParameterLists> parameterLists{{"SpawnZones", spawnPoints}};
    ON_CALL(parameter, GetParameterLists()).WillByDefault(ReturnRef(parameterLists));

    FakeWorld fakeWorld;
    ON_CALL(fakeWorld, IsDirectionalRoadExisting(_,_)).WillByDefault(Return(true));

    RoadGraph roadGraphAIn;
    auto node0 = add_vertex(RouteElement{"RoadA", true}, roadGraphAIn);
    auto node1 = add_vertex(RouteElement{"RoadB", true}, roadGraphAIn);
    add_edge(node0, node1, roadGraphAIn);
    ON_CALL(fakeWorld, GetRoadGraph(RouteElement{"RoadA", true}, _)).WillByDefault(Return(std::make_pair(roadGraphAIn, node0)));
    RoadGraph roadGraphAAgainst;
    auto node2 = add_vertex(RouteElement{"RoadA", false}, roadGraphAAgainst);
    ON_CALL(fakeWorld, GetRoadGraph(RouteElement{"RoadA", false}, _)).WillByDefault(Return(std::make_pair(roadGraphAAgainst, node2)));
    RoadGraph roadGraphBAgainst;
    auto node3 = add_vertex(RouteElement{"RoadB", false}, roadGraphBAgainst);
    auto node4 = add_vertex(RouteElement{"RoadA", false}, roadGraphBAgainst);
    add_edge(node3, node4, roadGraphBAgainst);
    ON_CALL(fakeWorld, GetRoadGraph(RouteElement{"RoadB", false}, _)).WillByDefault(Return(std::make_pair(roadGraphBAgainst, node3)));

    std::unique_ptr<FakeRoadStream> roadStreamIn = std::make_unique<FakeRoadStream>();
    LaneStreamInterface* laneStreamIn = new FakeLaneStream;
    std::vector<std::unique_ptr<LaneStreamInterface>> laneStreamsIn{};
    laneStreamsIn.emplace_back(laneStreamIn);
    ON_CALL(*roadStreamIn, GetAllLaneStreams()).WillByDefault(Return(ByMove(std::move(laneStreamsIn))));
    ON_CALL(*roadStreamIn, GetStreamPosition(_)).WillByDefault([](GlobalRoadPosition position)
    {if(position.roadId == "RoadA")
        {return StreamPosition{position.roadPosition.s, 0};}
        if(position.roadId == "RoadB")
        {return StreamPosition{1000. + position.roadPosition.s, 0};}
        return StreamPosition{-1, 0};});
    ON_CALL(fakeWorld, GetRoadStream(std::vector<RouteElement>{{"RoadA", true}, {"RoadB", true}})).
            WillByDefault(Return(ByMove(std::move(roadStreamIn))));

    std::unique_ptr<FakeRoadStream> roadStreamAgainst = std::make_unique<FakeRoadStream>();
    LaneStreamInterface* laneStreamAgainst = new FakeLaneStream;
    std::vector<std::unique_ptr<LaneStreamInterface>> laneStreamsAgainst{};
    laneStreamsAgainst.emplace_back(laneStreamAgainst);
    ON_CALL(*roadStreamAgainst, GetAllLaneStreams()).WillByDefault(Return(ByMove(std::move(laneStreamsAgainst))));
    ON_CALL(*roadStreamAgainst, GetStreamPosition(_)).WillByDefault([](GlobalRoadPosition position)
    {if(position.roadId == "RoadA")
        {return StreamPosition{1500. - position.roadPosition.s, 0};}
        if(position.roadId == "RoadB")
        {return StreamPosition{500. - position.roadPosition.s, 0};}
        return StreamPosition{-1, 0};});
    ON_CALL(fakeWorld, GetRoadStream(std::vector<RouteElement>{{"RoadB", false}, {"RoadA", false}})).
            WillByDefault(Return(ByMove(std::move(roadStreamAgainst))));

    auto result = ExtractSpawnAreas(parameter, fakeWorld, &callbacks);

    ASSERT_THAT(result, SizeIs(2));
    EXPECT_THAT(result.at(0).laneStream.get(), Eq(laneStreamIn));
    EXPECT_THAT(result.at(0).sStart, Eq(10.0));
    EXPECT_THAT(result.at(0).sEnd, Eq(1050.0));
    EXPECT_THAT(result.at(1).laneStream.get(), Eq(laneStreamAgainst));
    EXPECT_THAT(result.at(1).sStart, Eq(450.0));
    EXPECT_THAT(result.at(1).sEnd, Eq(1490.0));
}
