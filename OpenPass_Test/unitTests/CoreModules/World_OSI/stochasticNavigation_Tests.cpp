/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "RoutePlanning/StochasticNavigation.h"
#include "fakeWorldData.h"
#include "fakeStochastics.h"
#include "fakeRoad.h"
#include "fakeOWLJunction.h"
#include "fakeLaneManager.h"
#include <iostream>

using namespace OWL;

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;
using ::testing::Eq;
using ::testing::ElementsAre;

std::ostream& operator<<(std::ostream& os, const RouteElement& obj)
{
    return os
            << "road " << obj.roadId << " "
            << (obj.inRoadDirection ? "in" : "against") << " OpenDrive direction";
}

TEST(RouteCalculation_Tests, TwoJunctions)
{
    Fakes::WorldData worldData;
    FakeStochastics stochastics;

    Fakes::Road startRoad, roadA, roadB, roadC, roadD;
    OWL::Id idStartRoad = 1, idRoadA = 2, idRoadB = 3, idRoadC = 4, idRoadD = 5;
    Fakes::Road connectorA, connectorB, connectorC, connectorD;
    OWL::Id idConnectorA = 6, idConnectorB = 7, idConnectorC = 8, idConnectorD = 9;
    std::unordered_map<OWL::Id, OWL::Interfaces::Road*> roads { {idStartRoad, &startRoad}, {idRoadA, &roadA}, {idRoadB, &roadB}, {idRoadC, &roadC}, {idRoadD, &roadD},
                                                               {idConnectorA, &connectorA}, {idConnectorB, &connectorB}, {idConnectorC, &connectorC}, {idConnectorD, &connectorD} };;
    std::unordered_map<OWL::Id, std::string> roadIdMapping { {idStartRoad, "StartRoad"}, {idRoadA, "RoadA"}, {idRoadB, "RoadB"}, {idRoadC, "RoadC"}, {idRoadD, "RoadD"},
                                                             {idConnectorA, "ConnectorA"}, {idConnectorB, "ConnectorB"}, {idConnectorC, "ConnectorC"}, {idConnectorD, "ConnectorD"} };
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    ON_CALL(startRoad, GetId()).WillByDefault(Return(idStartRoad));
    ON_CALL(roadA, GetId()).WillByDefault(Return(idRoadA));
    ON_CALL(roadB, GetId()).WillByDefault(Return(idRoadB));
    ON_CALL(roadC, GetId()).WillByDefault(Return(idRoadC));
    ON_CALL(roadD, GetId()).WillByDefault(Return(idRoadD));
    ON_CALL(connectorA, GetId()).WillByDefault(Return(idConnectorA));
    ON_CALL(connectorB, GetId()).WillByDefault(Return(idConnectorB));
    ON_CALL(connectorC, GetId()).WillByDefault(Return(idConnectorC));
    ON_CALL(connectorD, GetId()).WillByDefault(Return(idConnectorD));

    Fakes::Junction junction1, junction2;
    OWL::Id idJunction1 = 10, idJunction2 = 11;
    std::unordered_map<OWL::Id, OWL::Interfaces::Junction*> junctions { {idJunction1, &junction1}, {idJunction2, &junction2} };
    std::unordered_map<OWL::Id, std::string> junctionIdMapping { {idJunction1, "Junction1"}, {idJunction2, "Junction2"} };
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    ON_CALL(worldData, GetJunctionIdMapping()).WillByDefault(ReturnRef(junctionIdMapping));
    ON_CALL(junction1, GetId()).WillByDefault(Return(idJunction1));
    ON_CALL(junction2, GetId()).WillByDefault(Return(idJunction2));

    ON_CALL(startRoad, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(startRoad, GetSuccessor()).WillByDefault(Return(idJunction1));
    OWL::Interfaces::Roads connectionsJunction1 { {&connectorA, &connectorB} };
    ON_CALL(junction1, GetConnectingRoads()).WillByDefault(ReturnRef(connectionsJunction1));
    ON_CALL(connectorA, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(connectorA, GetPredecessor()).WillByDefault(Return(idStartRoad));
    ON_CALL(connectorA, GetSuccessor()).WillByDefault(Return(idRoadA));
    ON_CALL(connectorB, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(connectorB, GetPredecessor()).WillByDefault(Return(idStartRoad));
    ON_CALL(connectorB, GetSuccessor()).WillByDefault(Return(idRoadB));
    ON_CALL(roadA, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(roadA, GetPredecessor()).WillByDefault(Return(idJunction1));
    ON_CALL(roadB, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(roadB, GetPredecessor()).WillByDefault(Return(idJunction2));
    ON_CALL(roadB, GetSuccessor()).WillByDefault(Return(idJunction1));
    OWL::Interfaces::Roads connectionsJunction2 { {&connectorC, &connectorD} };
    ON_CALL(junction2, GetConnectingRoads()).WillByDefault(ReturnRef(connectionsJunction2));
    ON_CALL(connectorC, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(connectorC, GetPredecessor()).WillByDefault(Return(idRoadB));
    ON_CALL(connectorC, GetSuccessor()).WillByDefault(Return(idRoadC));
    ON_CALL(connectorD, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(connectorD, GetPredecessor()).WillByDefault(Return(idRoadB));
    ON_CALL(connectorD, GetSuccessor()).WillByDefault(Return(idRoadD));;
    ON_CALL(roadC, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(roadC, GetPredecessor()).WillByDefault(Return(idJunction2));
    ON_CALL(roadC, GetSuccessor()).WillByDefault(Return(OWL::InvalidId));
    ON_CALL(roadD, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(roadD, GetPredecessor()).WillByDefault(Return(idJunction2));

    EXPECT_CALL(stochastics, GetUniformDistributed(_,_)).WillOnce(Return(1.0)).WillOnce(Return(0.0));

    StochasticNavigation navigation(stochastics, worldData);
    GlobalRoadPosition startPostion;
    startPostion.roadId = "StartRoad";
    const auto result = navigation.GetRoute(startPostion).roads;

    ASSERT_THAT(result.size(), Eq(5));
    ASSERT_THAT(result.at(0), Eq(RouteElement{"StartRoad", true}));
    ASSERT_THAT(result.at(1), Eq(RouteElement{"ConnectorB", true}));
    ASSERT_THAT(result.at(2), Eq(RouteElement{"RoadB", false}));
    ASSERT_THAT(result.at(3), Eq(RouteElement{"ConnectorC", true}));
    ASSERT_THAT(result.at(4), Eq(RouteElement{"RoadC", true}));
}

TEST(RouteCalculation_Tests, ThreeRoadsWithMiddleRoadAgainstStreamDirection)
{
    Fakes::WorldData worldData;
    FakeStochastics stochastics;

    Fakes::Road startRoad, roadA, roadB;
    OWL::Id idStartRoad = 1, idRoadA = 2, idRoadB = 3;
    std::unordered_map<OWL::Id, OWL::Interfaces::Road*> roads { {idStartRoad, &startRoad}, {idRoadA, &roadA}, {idRoadB, &roadB} };;
    std::unordered_map<OWL::Id, std::string> roadIdMapping { {idStartRoad, "StartRoad"}, {idRoadA, "RoadA"}, {idRoadB, "RoadB"} };
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    ON_CALL(startRoad, GetId()).WillByDefault(Return(idStartRoad));
    ON_CALL(roadA, GetId()).WillByDefault(Return(idRoadA));
    ON_CALL(roadB, GetId()).WillByDefault(Return(idRoadB));

    std::unordered_map<OWL::Id, OWL::Interfaces::Junction*> junctions {};
    std::unordered_map<OWL::Id, std::string> junctionIdMapping {};
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    ON_CALL(worldData, GetJunctionIdMapping()).WillByDefault(ReturnRef(junctionIdMapping));

    ON_CALL(startRoad, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(startRoad, GetSuccessor()).WillByDefault(Return(idRoadA));
    ON_CALL(roadA, IsInStreamDirection()).WillByDefault(Return(false));
    ON_CALL(roadA, GetPredecessor()).WillByDefault(Return(idRoadB));
    ON_CALL(roadA, GetSuccessor()).WillByDefault(Return(idStartRoad));
    ON_CALL(roadB, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(roadB, GetPredecessor()).WillByDefault(Return(idRoadA));;

    StochasticNavigation navigation(stochastics, worldData);
    GlobalRoadPosition startPostion;
    startPostion.roadId = "StartRoad";
    const auto result = navigation.GetRoute(startPostion).roads;

    ASSERT_THAT(result, ElementsAre(RouteElement{"StartRoad", true}, RouteElement{"RoadA", false}, RouteElement{"RoadB", true}));
}

TEST(GetLaneStream_Tests, SameLaneAndRoute_LaneStreamIsReused)
{
    FakeLaneManager fakelm{2, 2, 3.0, {10.0, 11.0}, ""};
    auto road = fakelm.GetRoads().cbegin()->second;
    auto roadId = fakelm.GetRoads().cbegin()->first;
    std::vector<OWL::Id> successorsLanes0_0 {fakelm.GetLane(1, 0).GetId()};
    ON_CALL(fakelm.GetLane(0, 0), GetNext()).WillByDefault(Return(successorsLanes0_0));
    std::vector<OWL::Id> precessorsLanes1_0 {fakelm.GetLane(0, 0).GetId()};
    ON_CALL(fakelm.GetLane(1, 1), GetPrevious()).WillByDefault(Return(precessorsLanes1_0));

    std::unordered_map<OWL::Id, OWL::Road*> roads {{roadId, road}};
    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, std::string> roadIdMapping { {roadId, "Road1"}};
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(fakelm.GetLaneIdMapping()));

    FakeStochastics stochastics;
    StochasticNavigation navigation{stochastics, worldData};
    Route route{"Road1"};

    const auto& resultFirstQuery = navigation.GetLaneStream(route, "Road1", -1, 5.0);
    const auto& resultSecondQuery = navigation.GetLaneStream(route, "Road1", -1, 7.0);

    EXPECT_EQ(&resultFirstQuery, &resultSecondQuery);
}

TEST(GetLaneStream_Tests, SuccessorLaneAndSameRoute_LaneStreamIsReused)
{
    FakeLaneManager fakelm{2, 2, 3.0, {10.0, 11.0}, ""};
    auto road = fakelm.GetRoads().cbegin()->second;
    auto roadId = fakelm.GetRoads().cbegin()->first;
    std::vector<OWL::Id> successorsLanes0_0 {fakelm.GetLane(1, 1).GetId()};
    ON_CALL(fakelm.GetLane(0, 0), GetNext()).WillByDefault(Return(successorsLanes0_0));
    std::vector<OWL::Id> precessorsLanes1_1 {fakelm.GetLane(0, 0).GetId()};
    ON_CALL(fakelm.GetLane(1, 1), GetPrevious()).WillByDefault(Return(precessorsLanes1_1));

    std::unordered_map<OWL::Id, OWL::Road*> roads {{roadId, road}};
    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, std::string> roadIdMapping { {roadId, "Road1"}};
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(fakelm.GetLaneIdMapping()));

    FakeStochastics stochastics;
    StochasticNavigation navigation{stochastics, worldData};
    Route route{"Road1"};

    const auto& resultFirstQuery = navigation.GetLaneStream(route, "Road1", -1, 5.0);
    const auto& resultSecondQuery = navigation.GetLaneStream(route, "Road1", -2, 15.0);

    EXPECT_EQ(&resultFirstQuery, &resultSecondQuery);
}

TEST(GetLaneStream_Tests, DifferentLaneIds_LaneStreamIsNotReused)
{
    FakeLaneManager fakelm{2, 2, 3.0, {10.0, 11.0}, ""};
    auto road = fakelm.GetRoads().cbegin()->second;
    auto roadId = fakelm.GetRoads().cbegin()->first;
    std::vector<OWL::Id> successorsLanes0_0 {fakelm.GetLane(1, 1).GetId()};
    ON_CALL(fakelm.GetLane(0, 0), GetNext()).WillByDefault(Return(successorsLanes0_0));
    std::vector<OWL::Id> precessorsLanes1_1 {fakelm.GetLane(0, 0).GetId()};
    ON_CALL(fakelm.GetLane(1, 1), GetPrevious()).WillByDefault(Return(precessorsLanes1_1));

    std::unordered_map<OWL::Id, OWL::Road*> roads {{roadId, road}};
    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, std::string> roadIdMapping { {roadId, "Road1"}};
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(fakelm.GetLaneIdMapping()));

    FakeStochastics stochastics;
    StochasticNavigation navigation{stochastics, worldData};
    Route route{"Road1"};

    const auto& resultFirstQuery = navigation.GetLaneStream(route, "Road1", -1, 5.0);
    const auto& resultSecondQuery = navigation.GetLaneStream(route, "Road1", -2, 5.0);

    EXPECT_NE(&resultFirstQuery, &resultSecondQuery);
    EXPECT_THAT(resultSecondQuery.GetElements().front().element->GetId(), Eq(fakelm.GetLane(0,1).GetId()));
}

TEST(GetLaneStream_Tests, DifferentSCoordinatesOnDifferentStreams_LaneStreamIsNotReused)
{
    FakeLaneManager fakelm{2, 2, 3.0, {10.0, 11.0}, ""};
    auto road = fakelm.GetRoads().cbegin()->second;
    auto roadId = fakelm.GetRoads().cbegin()->first;
    std::vector<OWL::Id> successorsLanes0_0 {fakelm.GetLane(1, 1).GetId()};
    ON_CALL(fakelm.GetLane(0, 0), GetNext()).WillByDefault(Return(successorsLanes0_0));
    std::vector<OWL::Id> precessorsLanes1_0 {};
    ON_CALL(fakelm.GetLane(1, 0), GetPrevious()).WillByDefault(Return(precessorsLanes1_0));
    std::vector<OWL::Id> precessorsLanes1_1 {fakelm.GetLane(0, 0).GetId()};
    ON_CALL(fakelm.GetLane(1, 1), GetPrevious()).WillByDefault(Return(precessorsLanes1_1));

    std::unordered_map<OWL::Id, OWL::Road*> roads {{roadId, road}};
    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, std::string> roadIdMapping { {roadId, "Road1"}};
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(fakelm.GetLaneIdMapping()));

    FakeStochastics stochastics;
    StochasticNavigation navigation{stochastics, worldData};
    Route route{"Road1"};

    const auto& resultFirstQuery = navigation.GetLaneStream(route, "Road1", -1, 5.0);
    const auto& resultSecondQuery = navigation.GetLaneStream(route, "Road1", -1, 15.0);

    EXPECT_NE(&resultFirstQuery, &resultSecondQuery);
    EXPECT_THAT(resultSecondQuery.GetElements().front().element->GetId(), Eq(fakelm.GetLane(1,0).GetId()));
}

TEST(GetLaneStream_Tests, DifferentRoutes_LaneStreamIsNotReused)
{
    FakeLaneManager fakelm{2, 2, 3.0, {10.0, 11.0}, ""};
    auto road1 = fakelm.GetRoads().cbegin()->second;
    auto road1Id = fakelm.GetRoads().cbegin()->first;
    std::vector<OWL::Id> successorsLanes0_0 {fakelm.GetLane(1, 1).GetId()};
    ON_CALL(fakelm.GetLane(0, 0), GetNext()).WillByDefault(Return(successorsLanes0_0));
    std::vector<OWL::Id> precessorsLanes1_1 {fakelm.GetLane(0, 0).GetId()};
    ON_CALL(fakelm.GetLane(1, 1), GetPrevious()).WillByDefault(Return(precessorsLanes1_1));
    OWL::Fakes::Road road2;
    OWL::Id road2Id = 100;
    ON_CALL(road2, GetId()).WillByDefault(Return(road2Id));
    OWL::Fakes::Road road3;
    OWL::Id road3Id = 101;
    ON_CALL(road3, GetId()).WillByDefault(Return(road3Id));
    std::list<const OWL::Interfaces::Section*> sections{};
    ON_CALL(road2, GetSections()).WillByDefault(ReturnRef(sections));
    ON_CALL(road3, GetSections()).WillByDefault(ReturnRef(sections));

    std::unordered_map<OWL::Id, OWL::Road*> roads {{road1Id, road1}, {road2Id, &road2}, {road3Id, &road3}};
    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, std::string> roadIdMapping { {road1Id, "Road1"}, {road2Id, "Road2"}, {road3Id, "Road3"} };
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(roadIdMapping));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(fakelm.GetLaneIdMapping()));

    FakeStochastics stochastics;
    StochasticNavigation navigation{stochastics, worldData};
    Route route1{{{"Road1", true}, {"Road2", true}}, {}, 1};
    Route route2{{{"Road1", true}, {"Road3", true}}, {}, 2};

    const auto& resultFirstQuery = navigation.GetLaneStream(route1, "Road1", -1, 5.0);
    const auto& resultSecondQuery = navigation.GetLaneStream(route2, "Road1", -1, 5.0);

    EXPECT_NE(&resultFirstQuery, &resultSecondQuery);
}
