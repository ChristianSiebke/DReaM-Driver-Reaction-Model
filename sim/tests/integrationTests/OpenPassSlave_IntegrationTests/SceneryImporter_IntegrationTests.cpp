/*********************************************************************
* Copyright (c) 2017, 2018, 2019, 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#define TESTING
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <algorithm>

#include <boost/filesystem.hpp>

#include "core/slave/modules/Stochastics/stochastics_implementation.h"
#include "importer/scenery.h"
#include "importer/sceneryImporter.h"
#include "modelElements/agentBlueprint.h"
#include "bindings/world.h"
#include "fakeDataBuffer.h"
#include "fakeSceneryDynamics.h"

#include "AgentAdapter.h"
#include "WorldData.h"

using ::testing::Return;
using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::ElementsAre;
using ::testing::Contains;
using ::testing::Eq;
using ::testing::IsEmpty;
using ::testing::IsTrue;
using ::testing::Ne;
using ::testing::UnorderedElementsAre;
using ::testing::SizeIs;
using ::testing::IsEmpty;
using ::testing::NiceMock;

using namespace boost::filesystem;
using namespace Configuration;
using namespace Importer;

struct TESTSCENERY_FACTORY
{
    const std::string libraryName = "World_OSI";

    NiceMock<FakeDataBuffer> fakeDataBuffer;
    SimulationCommon::Callbacks callbacks;
    StochasticsImplementation stochastics{&callbacks};
    SimulationSlave::WorldBinding worldBinding;
    SimulationSlave::World world;
    Scenery scenery;
    openScenario::EnvironmentAction environment;

    TESTSCENERY_FACTORY() :
        worldBinding(libraryName, &callbacks, &stochastics, &fakeDataBuffer),
        world(&worldBinding)
    {
    }

    bool instantiate(const std::string& sceneryFile, const std::vector<openScenario::TrafficSignalController>&& trafficSignalControllers = {})
    {
        path sceneryPath = initial_path() / "Resources" / "ImporterTest" / sceneryFile;

        if (!world.Instantiate())
        {
            return false;
        }

        if (!SceneryImporter::Import(sceneryPath.string(), &scenery))
        {
            return false;
        }

        FakeSceneryDynamics sceneryDynamics;
        ON_CALL(sceneryDynamics, GetEnvironment()).WillByDefault(Return(environment));
        ON_CALL(sceneryDynamics, GetTrafficSignalControllers()).WillByDefault(Return(trafficSignalControllers));

        if (!(world.CreateScenery(&scenery, sceneryDynamics)))
        {
            return false;
        }

        return true;
    }
};

namespace RelativeWorldView {
std::ostream& operator<<(std::ostream& os, const Lane& lane)
{
    os << "id: " << lane.relativeId << ", "
       << "direction: " << lane.inDrivingDirection << ","
       << "type: " << static_cast<int>(lane.type) << ","
       << "predecessor: " << lane.predecessor.value_or(-999) << ","
       << "successor: " <<lane.successor.value_or(-999);

    return os;
}
}

//! This enum is used to help checking lane connections as specified in the openDrive file.
//! Note: It's possible for two connected lanes to be each others predecessor/successor.
enum LaneConnectionType
{
    REGULAR = 0,    //lane a has next lane b, lane b has previous lane a
    NEXT = 1,       //lane a has next lane b, lane b has next lane a
    PREVIOUS = 2    //lane a has previous lane b, lane b has previous lane a
};


//! Helper function to sort all sections for a given road by their length
//! This is used to make checking the correct import of the sceneries easier
//! Note: in all sceneries section lengths in each road are increasing.
std::vector<const OWL::Interfaces::Section*> GetDistanceSortedSectionsForRoad(OWL::Interfaces::WorldData* worldData, std::string roadId)
{
    //Extract sections for given roadID
    auto sections = worldData->GetRoads().at(roadId)->GetSections();
    std::vector<const OWL::Interfaces::Section*> queriedSections{sections.cbegin(),sections.cend()};

    //Sort by distance
    std::sort(queriedSections.begin(), queriedSections.end(),
              [](auto s1, auto s2)
    {
        return s1->GetDistance(OWL::MeasurementPoint::RoadStart) < s2->GetDistance(OWL::MeasurementPoint::RoadStart);
    });

    return queriedSections;
}

//! Query lane by id for a given section
const OWL::Interfaces::Lane* GetLaneById(OWL::Interfaces::WorldData* worldData, std::list<const OWL::Interfaces::Lane*> sectionLanes, int laneId)
{
    auto queriedLane = std::find_if(sectionLanes.begin(), sectionLanes.end(), [worldData, laneId](const OWL::Interfaces::Lane* lane)
    {
        return worldData->GetLaneIdMapping().at(lane->GetId()) == laneId;
    });

    return *queriedLane;
}

//! Check if lanes are connected according to openDrive definition.
//! The connection (e.g. predecessor or succesor) can be specified for each lane.
void CheckLaneConnections(OWL::Interfaces::WorldData* worldData, const std::list<const OWL::Interfaces::Lane*>& firstSectionLanes, std::list<const OWL::Interfaces::Lane*> secondSectionLanes, int firstLaneId, int secondLaneId, LaneConnectionType howIsConnection = LaneConnectionType::REGULAR, bool strict = true)
{
    auto firstLane = GetLaneById(worldData, firstSectionLanes, firstLaneId);
    auto secondLane = GetLaneById(worldData, secondSectionLanes, secondLaneId);

    if (strict)
    {
        switch(howIsConnection)
        {
        case PREVIOUS:
            ASSERT_THAT(firstLane->GetPrevious(), ElementsAre(secondLane->GetId()));
            ASSERT_THAT(secondLane->GetPrevious(), ElementsAre(firstLane->GetId()));
            break;
        case NEXT:
            ASSERT_THAT(firstLane->GetNext(), ElementsAre(secondLane->GetId()));
            ASSERT_THAT(secondLane->GetNext(), ElementsAre(firstLane->GetId()));
            break;
        default:
            ASSERT_THAT(firstLane->GetNext(), ElementsAre(secondLane->GetId()));
            ASSERT_THAT(secondLane->GetPrevious(), ElementsAre(firstLane->GetId()));
            break;
        }
    }
    else
    {
        switch(howIsConnection)
        {
        case PREVIOUS:
            ASSERT_THAT(firstLane->GetPrevious(), Contains(secondLane->GetId()));
            ASSERT_THAT(secondLane->GetPrevious(), Contains(firstLane->GetId()));
            break;
        case NEXT:
            ASSERT_THAT(firstLane->GetNext(), Contains(secondLane->GetId()));
            ASSERT_THAT(secondLane->GetNext(), Contains(firstLane->GetId()));
            break;
        default:
            ASSERT_THAT(firstLane->GetNext(), Contains(secondLane->GetId()));
            ASSERT_THAT(secondLane->GetPrevious(), Contains(firstLane->GetId()));
            break;
        }
    }
}

//! Test correctly imported scenery
//! Scope is on World-level
TEST(SceneryImporter_IntegrationTests, SingleRoad_ImportWithCorrectLanes)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("IntegrationTestScenery.xodr"), IsTrue());

    auto& world = tsf.world;

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"1", true}, roadGraph);
    const auto relativeLanes = world.GetRelativeLanes(roadGraph, root, -1, 0.0, 150.0).at(root);
    ASSERT_EQ(relativeLanes.size(), 5);

    const auto firstSection = relativeLanes.at(0);
    ASSERT_EQ(firstSection.startS, 0.0);
    ASSERT_EQ(firstSection.endS, 10.0);
    ASSERT_THAT(firstSection.lanes, UnorderedElementsAre(RelativeWorldView::Lane{0, true, LaneType::Driving, std::nullopt, 0}));

    const auto secondSection = relativeLanes.at(1);
    ASSERT_EQ(secondSection.startS, 10.0);
    ASSERT_EQ(secondSection.endS, 30.0);
    ASSERT_THAT(secondSection.lanes, UnorderedElementsAre(RelativeWorldView::Lane{0, true, LaneType::Driving, 0, 0},
                                                          RelativeWorldView::Lane{-1, true, LaneType::Driving, std::nullopt, -1}));

    const auto thirdSection = relativeLanes.at(2);
    ASSERT_EQ(thirdSection.startS, 30.0);
    ASSERT_EQ(thirdSection.endS, 60.0);
    ASSERT_THAT(thirdSection.lanes, UnorderedElementsAre(RelativeWorldView::Lane{0, true, LaneType::Driving, 0, 0},
                                                         RelativeWorldView::Lane{-1, true, LaneType::Biking, -1, -1},
                                                         RelativeWorldView::Lane{-2, true, LaneType::Sidewalk, std::nullopt, -2}));

    const auto forthSection = relativeLanes.at(3);
    ASSERT_EQ(forthSection.startS, 60.0);
    ASSERT_EQ(forthSection.endS, 100.0);
    ASSERT_THAT(forthSection.lanes, UnorderedElementsAre(RelativeWorldView::Lane{0, true, LaneType::Stop, 0, std::nullopt},
                                                         RelativeWorldView::Lane{-1, true, LaneType::Driving, -1, -1},
                                                         RelativeWorldView::Lane{-2, true, LaneType::Driving, -2, -2}));

    const auto fifthSection = relativeLanes.at(4);
    ASSERT_EQ(fifthSection.startS, 100.0);
    ASSERT_EQ(fifthSection.endS, 150.0);
    ASSERT_THAT(fifthSection.lanes, UnorderedElementsAre(RelativeWorldView::Lane{-1, true, LaneType::Driving, -1, std::nullopt},
                                                         RelativeWorldView::Lane{-2, true, LaneType::Stop, -2, std::nullopt}));

    double maxSearchDistance = 1000.0;
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(roadGraph, root, -1, 0.0, maxSearchDistance, {LaneType::Driving, LaneType::Stop}).at(root), 100.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(roadGraph, root, -2, 15.0, maxSearchDistance, {LaneType::Driving, LaneType::Biking}).at(root), 135.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(roadGraph, root, -3, 35.0, maxSearchDistance, {LaneType::Driving, LaneType::Stop, LaneType::Sidewalk}).at(root), 115.0);

    ASSERT_DOUBLE_EQ(world.GetLaneWidth("1", -1, 60.0), 3.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth("1", -2, 60.0), 4.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth("1", -3, 60.0), 5.0);

}

TEST(SceneryImporter_IntegrationTests, MultipleRoads_ImportWithCorrectLanes)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("MultipleRoadsIntegrationScenery.xodr"), IsTrue());

    auto& world = tsf.world;

    RoadGraph roadGraph;
    RoadGraphVertex node1 = add_vertex(RouteElement{"1", true}, roadGraph);
    RoadGraphVertex node2 = add_vertex(RouteElement{"2", false}, roadGraph);
    RoadGraphVertex node3 = add_vertex(RouteElement{"3", true}, roadGraph);
    add_edge(node1, node2, roadGraph);
    add_edge(node2, node3, roadGraph);

    const auto relativeLanes = world.GetRelativeLanes(roadGraph, node1, -1, 0.0, 6000.0).at(node3);
    ASSERT_EQ(relativeLanes.size(), 6);

    const auto firstSection = relativeLanes.at(0);
    ASSERT_EQ(firstSection.startS, 0.0);
    ASSERT_EQ(firstSection.endS, 400.0);
    ASSERT_THAT(firstSection.lanes, UnorderedElementsAre(RelativeWorldView::Lane{0, true, LaneType::Driving, std::nullopt, 0},
                                                         RelativeWorldView::Lane{-1, true, LaneType::Driving, std::nullopt, -1},
                                                         RelativeWorldView::Lane{-2, true, LaneType::Driving, std::nullopt, -2}));

    const auto secondSection = relativeLanes.at(1);
    ASSERT_EQ(secondSection.startS, 400.0);
    ASSERT_EQ(secondSection.endS, 1000.0);
    ASSERT_THAT(secondSection.lanes, UnorderedElementsAre(RelativeWorldView::Lane{0, true, LaneType::Driving, 0, 0},
                                                          RelativeWorldView::Lane{-1, true, LaneType::Driving, -1, -1},
                                                          RelativeWorldView::Lane{-2, true, LaneType::Driving, -2, -2}));

    const auto thirdSection = relativeLanes.at(2);
    ASSERT_EQ(thirdSection.startS, 1000.0);
    ASSERT_EQ(thirdSection.endS, 2100.0);
    ASSERT_THAT(thirdSection.lanes, UnorderedElementsAre(RelativeWorldView::Lane{0, true, LaneType::Driving, 0, 0},
                                                         RelativeWorldView::Lane{-1, true, LaneType::Driving, -1, -1},
                                                         RelativeWorldView::Lane{-2, true, LaneType::Driving, -2, -2}));

    const auto forthSection = relativeLanes.at(3);
    ASSERT_EQ(forthSection.startS, 2100.0);
    ASSERT_EQ(forthSection.endS, 3000.0);
    ASSERT_THAT(forthSection.lanes, UnorderedElementsAre(RelativeWorldView::Lane{0, true, LaneType::Driving, 0, 0},
                                                         RelativeWorldView::Lane{-1, true, LaneType::Driving, -1, -1},
                                                         RelativeWorldView::Lane{-2, true, LaneType::Driving, -2, -2}));

    const auto fifthSection = relativeLanes.at(4);
    ASSERT_EQ(fifthSection.startS, 3000.0);
    ASSERT_EQ(fifthSection.endS, 4400.0);
    ASSERT_THAT(fifthSection.lanes, UnorderedElementsAre(RelativeWorldView::Lane{0, true, LaneType::Driving, 0, 0},
                                                         RelativeWorldView::Lane{-1, true, LaneType::Driving, -1, -1},
                                                         RelativeWorldView::Lane{-2, true, LaneType::Driving, -2, -2}));

    const auto sixthSection = relativeLanes.at(5);
    ASSERT_EQ(sixthSection.startS, 4400.0);
    ASSERT_EQ(sixthSection.endS, 6000.0);
    ASSERT_THAT(sixthSection.lanes, UnorderedElementsAre(RelativeWorldView::Lane{0, true, LaneType::Driving, 0, std::nullopt},
                                                         RelativeWorldView::Lane{-1, true, LaneType::Driving, -1, std::nullopt},
                                                         RelativeWorldView::Lane{-2, true, LaneType::Driving, -2, std::nullopt}));

    double maxSearchLength = 10000.0;
    //--------------------------------------------------------RoId, laneId, s, maxsearch
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(roadGraph, node1, -1, 0.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}).at(node3), 6000.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(roadGraph, node1, -2, 650.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}).at(node3), 5350.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(roadGraph, node2, 2, 0.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}).at(node3), 3000.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(roadGraph, node2, 2, 1500.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}).at(node3), 4500.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(roadGraph, node3, -3, 0.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}).at(node3), 3000.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(roadGraph, node3, -3, 1500.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}).at(node3), 1500.0);

    //-----------------------------RoId, laneId, s
    ASSERT_DOUBLE_EQ(world.GetLaneWidth("1", -1, 60.0), 3.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth("1", -3, 999.9), 5.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth("2", 1, 0.0), 3.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth("3", -2, 1500.0), 4.0);

}

TEST(SceneryImporter_IntegrationTests, MultipleRoadsWithJunctions_ImportWithCorrectLanes)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("MultipleRoadsWithJunctionIntegrationScenery.xodr"), IsTrue());

    auto& world = tsf.world;

    RoadGraph roadGraph;
    RoadGraphVertex node1 = add_vertex(RouteElement{"1", true}, roadGraph);
    RoadGraphVertex node2 = add_vertex(RouteElement{"2", true}, roadGraph);
    RoadGraphVertex node3 = add_vertex(RouteElement{"3", true}, roadGraph);
    RoadGraphVertex node4 = add_vertex(RouteElement{"4", true}, roadGraph);
    RoadGraphVertex node5 = add_vertex(RouteElement{"5", true}, roadGraph);
    add_edge(node1, node4, roadGraph);
    add_edge(node4, node2, roadGraph);
    add_edge(node1, node5, roadGraph);
    add_edge(node5, node3, roadGraph);

    const auto relativeLanes = world.GetRelativeLanes(roadGraph, node1, -1, 0.0, 320.0);
    const auto relativeLanesUp = relativeLanes.at(node2);
    ASSERT_EQ(relativeLanesUp.size(), 3);

    const auto firstSectionUp = relativeLanesUp.at(0);
    ASSERT_EQ(firstSectionUp.startS, 0.0);
    ASSERT_EQ(firstSectionUp.endS, 100.0);
    ASSERT_THAT(firstSectionUp.lanes, UnorderedElementsAre(RelativeWorldView::Lane{0, true, LaneType::Driving, std::nullopt, 0},
                                                           RelativeWorldView::Lane{-1, true, LaneType::Driving, std::nullopt, -1},
                                                           RelativeWorldView::Lane{-2, true, LaneType::Driving, std::nullopt, std::nullopt},
                                                           RelativeWorldView::Lane{-3, true, LaneType::Driving, std::nullopt, std::nullopt}));

    const auto secondSectionUp = relativeLanesUp.at(1);
    ASSERT_EQ(secondSectionUp.startS, 100.0);
    ASSERT_EQ(secondSectionUp.endS, 120.0);
    ASSERT_THAT(secondSectionUp.lanes, UnorderedElementsAre(RelativeWorldView::Lane{0, true, LaneType::Driving, 0, 0},
                                                            RelativeWorldView::Lane{-1, true, LaneType::Driving, -1, -1}));

    const auto thirdSectionUp = relativeLanesUp.at(2);
    ASSERT_EQ(thirdSectionUp.startS, 120.0);
    ASSERT_EQ(thirdSectionUp.endS, 320.0);
    ASSERT_THAT(thirdSectionUp.lanes, UnorderedElementsAre(RelativeWorldView::Lane{0, true, LaneType::Driving, 0, std::nullopt},
                                                           RelativeWorldView::Lane{-1, true, LaneType::Driving, -1, std::nullopt}));

    const auto relativeLanesDown = relativeLanes.at(node3);
    ASSERT_EQ(relativeLanesDown.size(), 3);

    const auto firstSectionDown = relativeLanesDown.at(0);
    ASSERT_EQ(firstSectionDown.startS, 0.0);
    ASSERT_EQ(firstSectionDown.endS, 100.0);
    ASSERT_THAT(firstSectionDown.lanes, UnorderedElementsAre(RelativeWorldView::Lane{0, true, LaneType::Driving, std::nullopt, std::nullopt},
                                                             RelativeWorldView::Lane{-1, true, LaneType::Driving, std::nullopt, std::nullopt},
                                                             RelativeWorldView::Lane{-2, true, LaneType::Driving, std::nullopt, -1},
                                                             RelativeWorldView::Lane{-3, true, LaneType::Driving, std::nullopt, -2}));

    const auto secondSectionDown = relativeLanesDown.at(1);
    ASSERT_EQ(secondSectionDown.startS, 100.0);
    ASSERT_EQ(secondSectionDown.endS, 120.0);
    ASSERT_THAT(secondSectionDown.lanes, UnorderedElementsAre(RelativeWorldView::Lane{-1, true, LaneType::Driving, -2, -1},
                                                              RelativeWorldView::Lane{-2, true, LaneType::Driving, -3, -2}));

    const auto thirdSectionDown = relativeLanesDown.at(2);
    ASSERT_EQ(thirdSectionDown.startS, 120.0);
    ASSERT_EQ(thirdSectionDown.endS, 420.0);
    ASSERT_THAT(thirdSectionDown.lanes, UnorderedElementsAre(RelativeWorldView::Lane{-1, true, LaneType::Driving, -1, std::nullopt},
                                                             RelativeWorldView::Lane{-2, true, LaneType::Driving, -2, std::nullopt}));

    double maxSearchLength = 1000.0;
    //--------------------------------------------------------RoId, laneId, s, maxsearch
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(roadGraph, node1, -1, 0.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}).at(node2), 320.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(roadGraph, node1, -2, 90.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}).at(node2), 230.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(roadGraph, node1, -3, 10.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}).at(node3), 410.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(roadGraph, node1, -4, 0.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}).at(node3), 420.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(roadGraph, node2, -1, 0.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}).at(node2), 200.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(roadGraph, node2, -2, 150.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}).at(node2), 50.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(roadGraph, node3, -1, 0.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}).at(node3), 300.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(roadGraph, node4, -1, 0.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}).at(node2), 220.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(roadGraph, node5, -2, 18.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}).at(node3), 302.0);

    //-----------------------------RoId, laneId, s
    ASSERT_DOUBLE_EQ(world.GetLaneWidth("1", -1, 60.0), 3.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth("1", -2, 95.0), 4.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth("1", -3, 99.0), 5.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth("1", -4, 0.0), 6.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth("2", -1, 1.0), 3.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth("2", -2, 20.0), 4.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth("3", -1, 123.0), 5.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth("3", -2, 200.0), 6.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth("4", -1, 15.0), 3.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth("4", -2, 15.0), 4.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth("5", -1, 0.0), 5.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth("5", -2, 15.0), 6.0);
}

TEST(SceneryImporter_IntegrationTests, TJunction_ImportWithCorrectLanes)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("TJunctionBig.xodr"), IsTrue());

    auto& world = tsf.world;

    RoadGraph roadGraph;
    RoadGraphVertex node1 = add_vertex(RouteElement{"R1", false}, roadGraph);
    RoadGraphVertex node2 = add_vertex(RouteElement{"R2", true}, roadGraph);
    RoadGraphVertex node3 = add_vertex(RouteElement{"R3", false}, roadGraph);
    RoadGraphVertex node2_1 = add_vertex(RouteElement{"R2-1", false}, roadGraph);
    RoadGraphVertex node2_3 = add_vertex(RouteElement{"R2-3", true}, roadGraph);
    add_edge(node2, node2_1, roadGraph);
    add_edge(node2_1, node1, roadGraph);
    add_edge(node2, node2_3, roadGraph);
    add_edge(node2_3, node3, roadGraph);

    const auto relativeLanes = world.GetRelativeLanes(roadGraph, node2, -1, 0.0, 320.0);
    const auto relativeLanesLeft = relativeLanes.at(node1);
    ASSERT_THAT(relativeLanesLeft, SizeIs(4));

    const auto firstSectionLeft = relativeLanesLeft.at(0);
    ASSERT_THAT(firstSectionLeft.startS, DoubleEq(0.0));
    ASSERT_THAT(firstSectionLeft.endS, DoubleEq(200.0));
    ASSERT_THAT(firstSectionLeft.lanes, UnorderedElementsAre(RelativeWorldView::Lane{2, false, LaneType::Driving, std::nullopt, std::nullopt},
                                                             RelativeWorldView::Lane{1, false, LaneType::Driving, std::nullopt, std::nullopt},
                                                             RelativeWorldView::Lane{0, true, LaneType::Driving, std::nullopt, 0},
                                                             RelativeWorldView::Lane{-1, true, LaneType::Driving, std::nullopt, -1}));

    const auto secondSectionLeft = relativeLanesLeft.at(1);
    ASSERT_THAT(secondSectionLeft.startS, DoubleEq(200.0));
    ASSERT_THAT(secondSectionLeft.endS, DoubleEq(205.708));
    ASSERT_THAT(secondSectionLeft.lanes, UnorderedElementsAre(RelativeWorldView::Lane{0, true, LaneType::Driving, 0, 0},
                                                              RelativeWorldView::Lane{-1, true, LaneType::Driving, -1, -1}));

    const auto thirdSectionLeft = relativeLanesLeft.at(2);
    ASSERT_THAT(thirdSectionLeft.startS, DoubleEq(205.708));
    ASSERT_THAT(thirdSectionLeft.endS, DoubleEq(215.708));
    ASSERT_THAT(thirdSectionLeft.lanes, UnorderedElementsAre(RelativeWorldView::Lane{0, true, LaneType::Driving, 0, 0},
                                                              RelativeWorldView::Lane{-1, true, LaneType::Driving, -1, -1}));

    const auto forthSectionLeft = relativeLanesLeft.at(3);
    ASSERT_THAT(forthSectionLeft.startS, DoubleEq(215.708));
    ASSERT_THAT(forthSectionLeft.endS, DoubleEq(415.708));
    ASSERT_THAT(forthSectionLeft.lanes, UnorderedElementsAre(RelativeWorldView::Lane{2, false, LaneType::Driving, std::nullopt, std::nullopt},
                                                             RelativeWorldView::Lane{1, false, LaneType::Driving, std::nullopt, std::nullopt},
                                                             RelativeWorldView::Lane{0, true, LaneType::Driving, 0, std::nullopt},
                                                             RelativeWorldView::Lane{-1, true, LaneType::Driving, -1, std::nullopt}));

    const auto relativeLanesRight = relativeLanes.at(node3);
    ASSERT_THAT(relativeLanesRight, SizeIs(4));

    const auto firstSectionRight = relativeLanesRight.at(0);
    ASSERT_THAT(firstSectionRight.startS, DoubleEq(0.0));
    ASSERT_THAT(firstSectionRight.endS, DoubleEq(200.0));
    ASSERT_THAT(firstSectionRight.lanes, UnorderedElementsAre(RelativeWorldView::Lane{2, false, LaneType::Driving, std::nullopt, std::nullopt},
                                                              RelativeWorldView::Lane{1, false, LaneType::Driving, std::nullopt, std::nullopt},
                                                              RelativeWorldView::Lane{0, true, LaneType::Driving, std::nullopt, 0},
                                                              RelativeWorldView::Lane{-1, true, LaneType::Driving, std::nullopt, -1}));

    const auto secondSectionRight = relativeLanesRight.at(1);
    ASSERT_THAT(secondSectionRight.startS, DoubleEq(200.0));
    ASSERT_THAT(secondSectionRight.endS, DoubleEq(210.0));
    ASSERT_THAT(secondSectionRight.lanes, UnorderedElementsAre(RelativeWorldView::Lane{0, true, LaneType::Driving, 0, 0},
                                                               RelativeWorldView::Lane{-1, true, LaneType::Driving, -1, -1}));

    const auto thirdSectionRight = relativeLanesRight.at(2);
    ASSERT_THAT(thirdSectionRight.startS, DoubleEq(210.0));
    ASSERT_THAT(thirdSectionRight.endS, DoubleEq(215.708));
    ASSERT_THAT(thirdSectionRight.lanes, UnorderedElementsAre(RelativeWorldView::Lane{0, true, LaneType::Driving, 0, 0},
                                                               RelativeWorldView::Lane{-1, true, LaneType::Driving, -1, -1}));

    const auto forthSectionRight = relativeLanesRight.at(3);
    ASSERT_THAT(forthSectionRight.startS, DoubleEq(215.708));
    ASSERT_THAT(forthSectionRight.endS, DoubleEq(415.708));
    ASSERT_THAT(forthSectionRight.lanes, UnorderedElementsAre(RelativeWorldView::Lane{2, false, LaneType::Driving, std::nullopt, std::nullopt},
                                                              RelativeWorldView::Lane{1, false, LaneType::Driving, std::nullopt, std::nullopt},
                                                              RelativeWorldView::Lane{0, true, LaneType::Driving, 0, std::nullopt},
                                                              RelativeWorldView::Lane{-1, true, LaneType::Driving, -1, std::nullopt}));
}


//! Test correct lane predeccessor and successors
//! Scope is on WorldData and OWL-Level
TEST(SceneryImporter_IntegrationTests, SingleRoad_CheckForCorrectLaneConnections)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("IntegrationTestScenery.xodr"), IsTrue());

    auto& world = tsf.world;

    OWL::Interfaces::WorldData* worldData = static_cast<OWL::Interfaces::WorldData*>(world.GetWorldData());

    ASSERT_EQ(worldData->GetRoads().at("1")->GetSections().size(), 5);

    auto sections = GetDistanceSortedSectionsForRoad(worldData, "1");

    const std::vector<int> numberOfLanesPerSection = {1, 2, 3, 3, 2};
    const std::vector<std::vector<int>> laneConnections = {{ -1}, {-1,-2}, {-1, -2, -3}, {0, -1, -2}};

    for(unsigned count = 0; count < 4; count++)
    {
        auto firstSection = sections.at(count);
        auto firstSectionLanes = firstSection->GetLanes();
        auto secondSection = sections.at(count + 1);
        auto secondSectionLanes = secondSection->GetLanes();

        ASSERT_EQ(firstSectionLanes.size(),  numberOfLanesPerSection[count]);

        for(int laneNumber = 0; laneNumber < numberOfLanesPerSection[count]; laneNumber++)
        {
            int secondLaneId = laneConnections.at(count).at(static_cast<unsigned>(laneNumber));
            if (secondLaneId != 0)
            {
                CheckLaneConnections(worldData, firstSectionLanes, secondSectionLanes, -laneNumber - 1, secondLaneId);
            }
        }
    }
}

TEST(SceneryImporter_IntegrationTests, MultipleRoads_CheckForCorrectLaneConnections)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("MultipleRoadsIntegrationScenery.xodr"), IsTrue());

    auto& world = tsf.world;

    OWL::Interfaces::WorldData* worldData = static_cast<OWL::Interfaces::WorldData*>(world.GetWorldData());

    ASSERT_EQ(worldData->GetRoads().at("1")->GetSections().size(), 2);
    ASSERT_EQ(worldData->GetRoads().at("2")->GetSections().size(), 2);
    ASSERT_EQ(worldData->GetRoads().at("3")->GetSections().size(), 2);

    auto sectionsRoad1 = GetDistanceSortedSectionsForRoad(worldData, "1");
    const auto& lanesRoad1Section1 = sectionsRoad1.front()->GetLanes();
    const auto& lanesRoad1Section2 = sectionsRoad1.back()->GetLanes();
    auto sectionsRoad2 = GetDistanceSortedSectionsForRoad(worldData, "2");
    const auto& lanesRoad2Section1 = sectionsRoad2.front()->GetLanes();
    const auto& lanesRoad2Section2 = sectionsRoad2.back()->GetLanes();
    auto sectionsRoad3 = GetDistanceSortedSectionsForRoad(worldData, "3");
    const auto& lanesRoad3Section1 = sectionsRoad3.front()->GetLanes();
    const auto& lanesRoad3Section2 = sectionsRoad3.back()->GetLanes();

    //check connections inside road
    CheckLaneConnections(worldData, lanesRoad1Section1, lanesRoad1Section2, -1, -1);
    CheckLaneConnections(worldData, lanesRoad1Section1, lanesRoad1Section2, -2, -2);
    CheckLaneConnections(worldData, lanesRoad1Section1, lanesRoad1Section2, -3, -3);

    CheckLaneConnections(worldData, lanesRoad2Section1, lanesRoad2Section2, 1, 1);
    CheckLaneConnections(worldData, lanesRoad2Section1, lanesRoad2Section2, 2, 2);
    CheckLaneConnections(worldData, lanesRoad2Section1, lanesRoad2Section2, 3, 3);

    CheckLaneConnections(worldData, lanesRoad3Section1, lanesRoad3Section2, -1, -1);
    CheckLaneConnections(worldData, lanesRoad3Section1, lanesRoad3Section2, -2, -2);
    CheckLaneConnections(worldData, lanesRoad3Section1, lanesRoad3Section2, -3, -3);

    //check connections between roads
    CheckLaneConnections(worldData, lanesRoad1Section2, lanesRoad2Section2, -1, 1, LaneConnectionType::NEXT);
    CheckLaneConnections(worldData, lanesRoad1Section2, lanesRoad2Section2, -2, 2, LaneConnectionType::NEXT);
    CheckLaneConnections(worldData, lanesRoad1Section2, lanesRoad2Section2, -3, 3, LaneConnectionType::NEXT);

    CheckLaneConnections(worldData, lanesRoad2Section1, lanesRoad3Section1, 1, -1, LaneConnectionType::PREVIOUS);
    CheckLaneConnections(worldData, lanesRoad2Section1, lanesRoad3Section1, 2, -2, LaneConnectionType::PREVIOUS);
    CheckLaneConnections(worldData, lanesRoad2Section1, lanesRoad3Section1, 3, -3, LaneConnectionType::PREVIOUS);
}

TEST(SceneryImporter_IntegrationTests, MultipleRoadsWithJunctions_CheckForCorrectLaneConnections)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("MultipleRoadsWithJunctionIntegrationScenery.xodr"), IsTrue());

    auto& world = tsf.world;

    OWL::Interfaces::WorldData* worldData = static_cast<OWL::Interfaces::WorldData*>(world.GetWorldData());

    ASSERT_EQ(worldData->GetRoads().at("1")->GetSections().size(), 1);
    ASSERT_EQ(worldData->GetRoads().at("2")->GetSections().size(), 1);
    ASSERT_EQ(worldData->GetRoads().at("3")->GetSections().size(), 1);
    ASSERT_EQ(worldData->GetRoads().at("4")->GetSections().size(), 1);
    ASSERT_EQ(worldData->GetRoads().at("5")->GetSections().size(), 1);

    const auto& lanesIncomingRoad = GetDistanceSortedSectionsForRoad(worldData, "1").back()->GetLanes();
    const auto& lanesUpperOutgoingRoad = GetDistanceSortedSectionsForRoad(worldData, "2").back()->GetLanes();
    const auto& lanesLowerOutgoingRoad = GetDistanceSortedSectionsForRoad(worldData, "3").back()->GetLanes();
    const auto& lanesUpperConnectingRoad = GetDistanceSortedSectionsForRoad(worldData, "4").back()->GetLanes();
    const auto& lanesLowerConnectingRoad = GetDistanceSortedSectionsForRoad(worldData, "5").back()->GetLanes();


    //check connections between incoming road and connecting roads
    CheckLaneConnections(worldData, lanesIncomingRoad, lanesUpperConnectingRoad, -1, -1);
    CheckLaneConnections(worldData, lanesIncomingRoad, lanesUpperConnectingRoad, -2, -2);

    CheckLaneConnections(worldData, lanesIncomingRoad, lanesLowerConnectingRoad, -3, -1);
    CheckLaneConnections(worldData, lanesIncomingRoad, lanesLowerConnectingRoad, -4, -2);

    //check connections between connecting roads and outgoing roads
    CheckLaneConnections(worldData, lanesUpperConnectingRoad, lanesUpperOutgoingRoad, -1, -1);
    CheckLaneConnections(worldData, lanesUpperConnectingRoad, lanesUpperOutgoingRoad, -2, -2);

    CheckLaneConnections(worldData, lanesLowerConnectingRoad, lanesLowerOutgoingRoad, -1, -1);
    CheckLaneConnections(worldData, lanesLowerConnectingRoad, lanesLowerOutgoingRoad, -2, -2);
}

TEST(SceneryImporter_IntegrationTests, TJunction_CheckForCorrectLaneConnections)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("TJunctionBig.xodr"), IsTrue());

    auto& world = tsf.world;

    OWL::Interfaces::WorldData* worldData = static_cast<OWL::Interfaces::WorldData*>(world.GetWorldData());

    ASSERT_EQ(worldData->GetRoads().at("R1")->GetSections().size(), 1);
    ASSERT_EQ(worldData->GetRoads().at("R2")->GetSections().size(), 1);
    ASSERT_EQ(worldData->GetRoads().at("R3")->GetSections().size(), 1);
    ASSERT_EQ(worldData->GetRoads().at("R2-1")->GetSections().size(), 2);
    ASSERT_EQ(worldData->GetRoads().at("R2-3")->GetSections().size(), 2);

    const auto& lanesRoad1 = GetDistanceSortedSectionsForRoad(worldData, "R1").back()->GetLanes();
    const auto& lanesRoad2 = GetDistanceSortedSectionsForRoad(worldData, "R2").back()->GetLanes();
    const auto& lanesRoad3 = GetDistanceSortedSectionsForRoad(worldData, "R3").back()->GetLanes();
    const auto& lanesRoad2_1first = GetDistanceSortedSectionsForRoad(worldData, "R2-1").front()->GetLanes();
    const auto& lanesRoad2_1second = GetDistanceSortedSectionsForRoad(worldData, "R2-1").back()->GetLanes();
    const auto& lanesRoad2_3first = GetDistanceSortedSectionsForRoad(worldData, "R2-3").front()->GetLanes();
    const auto& lanesRoad2_3second = GetDistanceSortedSectionsForRoad(worldData, "R2-3").back()->GetLanes();


    //check connections between incoming road and connecting roads
    CheckLaneConnections(worldData, lanesRoad2, lanesRoad2_1second, -1, 1, LaneConnectionType::NEXT, false);
    CheckLaneConnections(worldData, lanesRoad2, lanesRoad2_1second, -2, 2, LaneConnectionType::NEXT, false);

    CheckLaneConnections(worldData, lanesRoad2, lanesRoad2_3first, -1, -1, LaneConnectionType::REGULAR, false);
    CheckLaneConnections(worldData, lanesRoad2, lanesRoad2_3first, -2, -2, LaneConnectionType::REGULAR, false);

    //check connections between connecting roads and outgoing roads
    CheckLaneConnections(worldData, lanesRoad1, lanesRoad2_1first, 1, 1, LaneConnectionType::REGULAR, false);
    CheckLaneConnections(worldData, lanesRoad1, lanesRoad2_1first, 2, 2, LaneConnectionType::REGULAR, false);

    CheckLaneConnections(worldData, lanesRoad2_3second, lanesRoad3, -1, 1, LaneConnectionType::NEXT, false);
    CheckLaneConnections(worldData, lanesRoad2_3second, lanesRoad3, -2, 2, LaneConnectionType::NEXT, false);
}

void CheckLaneNeighbours(OWL::Interfaces::WorldData* worldData, const std::list<const OWL::Interfaces::Lane*>& lanes, int leftLaneId, int rightLaneId)
{
    auto leftLane = GetLaneById(worldData, lanes, leftLaneId);
    auto rightLane = GetLaneById(worldData, lanes, rightLaneId);

    EXPECT_THAT(&leftLane->GetRightLane(), Eq(rightLane));
    EXPECT_THAT(&rightLane->GetLeftLane(), Eq(leftLane));
}

TEST(SceneryImporter_IntegrationTests, TJunction_CheckForCorrectLaneNeighbours)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("TJunctionBig.xodr"), IsTrue());

    auto& world = tsf.world;

    OWL::Interfaces::WorldData* worldData = static_cast<OWL::Interfaces::WorldData*>(world.GetWorldData());

    const auto& lanesRoad2 = GetDistanceSortedSectionsForRoad(worldData, "R2").back()->GetLanes();
    const auto& lanesRoad2_1 = GetDistanceSortedSectionsForRoad(worldData, "R2-1").back()->GetLanes();
    const auto& lanesRoad2_3 = GetDistanceSortedSectionsForRoad(worldData, "R2-3").back()->GetLanes();

    CheckLaneNeighbours(worldData, lanesRoad2, 2, 1);
    CheckLaneNeighbours(worldData, lanesRoad2, 1, -1);
    CheckLaneNeighbours(worldData, lanesRoad2, -1, -2);

    CheckLaneNeighbours(worldData, lanesRoad2_1, 2, 1);

    CheckLaneNeighbours(worldData, lanesRoad2_3, -1, -2);
}
TEST(SceneryImporter_IntegrationTests, TJunction_ImportWithCorrectLaneMarkings)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("TJunctionBig.xodr"), IsTrue());

    auto& world = tsf.world;

    RoadGraph roadGraph;
    RoadGraphVertex node1 = add_vertex(RouteElement{"R1", true}, roadGraph);
    RoadGraphVertex node2 = add_vertex(RouteElement{"R2", false}, roadGraph);
    RoadGraphVertex node1_2 = add_vertex(RouteElement{"R1-2", true}, roadGraph);
    add_edge(node1, node1_2, roadGraph);
    add_edge(node1_2, node2, roadGraph);

    auto laneMarkings = world.GetLaneMarkings(roadGraph, node1, 2, 0.0, 400.0, Side::Left).at(node2);

    ASSERT_THAT(laneMarkings, SizeIs(1));
    ASSERT_THAT(laneMarkings.at(0).relativeStartDistance, DoubleEq(0.0));
    ASSERT_THAT(laneMarkings.at(0).type, Eq(LaneMarking::Type::Solid));
    ASSERT_THAT(laneMarkings.at(0).width, DoubleEq(0.15));
    ASSERT_THAT(laneMarkings.at(0).color, Eq(LaneMarking::Color::White));

    laneMarkings = world.GetLaneMarkings(roadGraph, node1, 2, 0.0, 400.0, Side::Right).at(node2);

    ASSERT_THAT(laneMarkings, SizeIs(1));
    ASSERT_THAT(laneMarkings.at(0).relativeStartDistance, DoubleEq(0.0));
    ASSERT_THAT(laneMarkings.at(0).type, Eq(LaneMarking::Type::Broken));
    ASSERT_THAT(laneMarkings.at(0).width, DoubleEq(0.15));
    ASSERT_THAT(laneMarkings.at(0).color, Eq(LaneMarking::Color::White));

    laneMarkings = world.GetLaneMarkings(roadGraph, node1, 1, 0.0, 400.0, Side::Left).at(node2);

    ASSERT_THAT(laneMarkings, SizeIs(1));
    ASSERT_THAT(laneMarkings.at(0).relativeStartDistance, DoubleEq(0.0));
    ASSERT_THAT(laneMarkings.at(0).type, Eq(LaneMarking::Type::Broken));
    ASSERT_THAT(laneMarkings.at(0).width, DoubleEq(0.15));
    ASSERT_THAT(laneMarkings.at(0).color, Eq(LaneMarking::Color::White));

    laneMarkings = world.GetLaneMarkings(roadGraph, node1, 1, 0.0, 400.0, Side::Right).at(node2);

    ASSERT_THAT(laneMarkings, SizeIs(1));
    ASSERT_THAT(laneMarkings.at(0).relativeStartDistance, DoubleEq(0.0));
    ASSERT_THAT(laneMarkings.at(0).type, Eq(LaneMarking::Type::Solid));
    ASSERT_THAT(laneMarkings.at(0).width, DoubleEq(0.15));
    ASSERT_THAT(laneMarkings.at(0).color, Eq(LaneMarking::Color::White));

    laneMarkings = world.GetLaneMarkings(roadGraph, node1, -1, 0.0, 400.0, Side::Left).at(node2);
    ASSERT_THAT(laneMarkings, SizeIs(3));
    ASSERT_THAT(laneMarkings.at(0).relativeStartDistance, DoubleEq(0.0));
    ASSERT_THAT(laneMarkings.at(0).type, Eq(LaneMarking::Type::Solid));
    ASSERT_THAT(laneMarkings.at(0).width, DoubleEq(0.15));
    ASSERT_THAT(laneMarkings.at(0).color, Eq(LaneMarking::Color::White));
    ASSERT_THAT(laneMarkings.at(1).relativeStartDistance, DoubleEq(200.0));
    ASSERT_THAT(laneMarkings.at(1).type, Eq(LaneMarking::Type::None));
    ASSERT_THAT(laneMarkings.at(2).relativeStartDistance, DoubleEq(215.708));
    ASSERT_THAT(laneMarkings.at(2).type, Eq(LaneMarking::Type::Solid));
    ASSERT_THAT(laneMarkings.at(2).width, DoubleEq(0.15));
    ASSERT_THAT(laneMarkings.at(2).color, Eq(LaneMarking::Color::White));
}

//!Workaround, because the OSI lane is a private member
osi3::Lane GetOsiLane(const OWL::Interfaces::Lane* lane)
{
    osi3::GroundTruth groundTruth;
    lane->CopyToGroundTruth(groundTruth);
    return groundTruth.lane(0);
}

void CheckLaneNeighbours(OWL::Interfaces::WorldData* worldData, std::list<const OWL::Interfaces::Lane*> lanes)
{
    auto nrOfLanes = static_cast<int>(lanes.size());
    for (int laneId = -1; -laneId < nrOfLanes; --laneId)
    {
        auto firstLane = GetLaneById(worldData, lanes, laneId);
        auto secondLane = GetLaneById(worldData, lanes, laneId-1);
        auto firstLaneId = firstLane->GetId();
        auto secondLaneId = secondLane->GetId();
        EXPECT_THAT(GetOsiLane(firstLane).classification().right_adjacent_lane_id(0).value(), secondLaneId);
        EXPECT_THAT(GetOsiLane(secondLane).classification().left_adjacent_lane_id(0).value(), firstLaneId);
    }
}

void CheckLaneType(OWL::Interfaces::WorldData* worldData, std::list<const OWL::Interfaces::Lane*> lanes, std::vector<osi3::Lane_Classification_Type> expectedTypes)
{
    // Only negative lanes are checked and lane "0" is only a placeholder without internal representation.
    // Calling GetLaneById with 0 would fail so start at 1.
    // Yet, we expect expectedTypes to carry information about the 0th lane
    for (size_t i = 1; i < lanes.size(); ++i)
    {
        int laneId = -i;
        auto lane = GetLaneById(worldData, lanes, laneId);
        EXPECT_THAT(GetOsiLane(lane).classification().type(), expectedTypes.at(i));
    }
}

void CheckLaneSubtype(OWL::Interfaces::WorldData* worldData, std::list<const OWL::Interfaces::Lane*> lanes, std::vector<osi3::Lane_Classification_Subtype> expectedTypes)
{
    // Only negative lanes are checked and lane "0" is only a placeholder without internal representation.
    // Calling GetLaneById with 0 would fail so start at 1.
    // Yet, we expect expectedTypes to carry information about the 0th lane
    for (size_t i = 1; i < lanes.size(); ++i)
    {
        int laneId = -i;
        auto lane = GetLaneById(worldData, lanes, laneId);
        EXPECT_THAT(GetOsiLane(lane).classification().subtype(), expectedTypes.at(i));
    }
}

TEST(SceneryImporter_IntegrationTests, SingleRoad_CheckForCorrectOsiLaneClassification)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("IntegrationTestScenery.xodr"), IsTrue());

    auto& world = tsf.world;

    OWL::Interfaces::WorldData* worldData = static_cast<OWL::Interfaces::WorldData*>(world.GetWorldData());

    ASSERT_EQ(worldData->GetRoads().at("1")->GetSections().size(), 5);

    auto sections = GetDistanceSortedSectionsForRoad(worldData, "1");

    for(const auto section: sections)
    {
        CheckLaneNeighbours(worldData, section->GetLanes());
    }

    CheckLaneType(worldData, sections[0]->GetLanes(), {osi3::Lane_Classification_Type_TYPE_NONDRIVING,
                                                       osi3::Lane_Classification_Type_TYPE_DRIVING});

    CheckLaneSubtype(worldData, sections[0]->GetLanes(), {osi3::Lane_Classification_Subtype_SUBTYPE_OTHER,
                                                          osi3::Lane_Classification_Subtype_SUBTYPE_NORMAL});

    CheckLaneType(worldData, sections[1]->GetLanes(), {osi3::Lane_Classification_Type_TYPE_NONDRIVING,
                                                       osi3::Lane_Classification_Type_TYPE_DRIVING,
                                                       osi3::Lane_Classification_Type_TYPE_DRIVING});

    CheckLaneSubtype(worldData, sections[1]->GetLanes(), {osi3::Lane_Classification_Subtype_SUBTYPE_OTHER,
                                                          osi3::Lane_Classification_Subtype_SUBTYPE_NORMAL,
                                                          osi3::Lane_Classification_Subtype_SUBTYPE_NORMAL});

    CheckLaneType(worldData, sections[2]->GetLanes(), {osi3::Lane_Classification_Type_TYPE_NONDRIVING,
                                                       osi3::Lane_Classification_Type_TYPE_DRIVING,
                                                       osi3::Lane_Classification_Type_TYPE_NONDRIVING,
                                                       osi3::Lane_Classification_Type_TYPE_NONDRIVING});

    CheckLaneSubtype(worldData, sections[2]->GetLanes(), {osi3::Lane_Classification_Subtype_SUBTYPE_OTHER,
                                                          osi3::Lane_Classification_Subtype_SUBTYPE_NORMAL,
                                                          osi3::Lane_Classification_Subtype_SUBTYPE_BIKING,
                                                          osi3::Lane_Classification_Subtype_SUBTYPE_SIDEWALK});
}

TEST(SceneryImporter_IntegrationTests, MultipleRoadsWithJunctions_CheckForCorrectOsiLaneClassification)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("MultipleRoadsWithJunctionIntegrationScenery.xodr"), IsTrue());

    auto& world = tsf.world;

    OWL::Interfaces::WorldData* worldData = static_cast<OWL::Interfaces::WorldData*>(world.GetWorldData());

    ASSERT_EQ(worldData->GetRoads().at("1")->GetSections().size(), 1);

    auto sections1 = GetDistanceSortedSectionsForRoad(worldData, "1");

    CheckLaneNeighbours(worldData, sections1[0]->GetLanes());

    CheckLaneType(worldData, sections1[0]->GetLanes(), {osi3::Lane_Classification_Type_TYPE_NONDRIVING,
                                                       osi3::Lane_Classification_Type_TYPE_DRIVING,
                                                       osi3::Lane_Classification_Type_TYPE_DRIVING,
                                                       osi3::Lane_Classification_Type_TYPE_DRIVING,
                                                       osi3::Lane_Classification_Type_TYPE_DRIVING});

    ASSERT_EQ(worldData->GetRoads().at("4")->GetSections().size(), 1);

    auto sections4 = GetDistanceSortedSectionsForRoad(worldData, "4");

    CheckLaneNeighbours(worldData, sections4[0]->GetLanes());

    CheckLaneType(worldData, sections4[0]->GetLanes(), {osi3::Lane_Classification_Type_TYPE_NONDRIVING,
                                                       osi3::Lane_Classification_Type_TYPE_INTERSECTION,
                                                       osi3::Lane_Classification_Type_TYPE_INTERSECTION});
}

TEST(SceneryImporter_IntegrationTests, MultipleRoadsWithNonIntersectingJunctions_JunctionsHaveNoIntersectionInformation)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("MultipleRoadsWithJunctionIntegrationScenery.xodr"), IsTrue());

    auto& world = tsf.world;

    OWL::Interfaces::WorldData* worldData = static_cast<OWL::Interfaces::WorldData*>(world.GetWorldData());
    const auto& junctionMap = worldData->GetJunctions();

    ASSERT_THAT(junctionMap.size(), 1);

    const auto& junction = junctionMap.begin()->second;

    ASSERT_THAT(junction->GetIntersections().size(), 0);
}

MATCHER_P(GeometryDoublePairEq, comparisonPair, "")
{
    constexpr static const double EPS = 1e-3;   // epsilon value for geometric comparisons
    return std::abs(arg.first - comparisonPair.first) < EPS && std::abs(arg.second - comparisonPair.second) < EPS;
}
TEST(SceneryImporter_IntegrationTests, MultipleRoadsWithIntersectingJunctions_JunctionsHaveIntersectionInformation)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("IntersectedJunctionScenery.xodr"), IsTrue());

    auto& world = tsf.world;
    OWL::Interfaces::WorldData* worldData = static_cast<OWL::Interfaces::WorldData*>(world.GetWorldData());
    WorldDataQuery worldDataQuery(*worldData);

    const std::string verticalRoadStringId = "vertical_connecting";
    const std::string horizontalRoadStringId = "horizontal_connecting";

    const auto verticalRoad = worldDataQuery.GetRoadByOdId(verticalRoadStringId);
    const auto horizontalRoad = worldDataQuery.GetRoadByOdId(horizontalRoadStringId);

    const auto& verticalLane1 = worldDataQuery.GetLaneByOdId(verticalRoadStringId, -1, 0.0);
    const auto& verticalLane2 = worldDataQuery.GetLaneByOdId(verticalRoadStringId, -2, 0.0);
    const auto& horizontalLane1 = worldDataQuery.GetLaneByOdId(horizontalRoadStringId, -1, 0.0);
    const auto& horizontalLane2 = worldDataQuery.GetLaneByOdId(horizontalRoadStringId, -2, 0.0);

    const std::pair<OWL::Id, OWL::Id> v1h1{verticalLane1.GetId(), horizontalLane1.GetId()};
    const std::pair<OWL::Id, OWL::Id> v1h2{verticalLane1.GetId(), horizontalLane2.GetId()};
    const std::pair<OWL::Id, OWL::Id> v2h1{verticalLane2.GetId(), horizontalLane1.GetId()};
    const std::pair<OWL::Id, OWL::Id> v2h2{verticalLane2.GetId(), horizontalLane2.GetId()};

    const std::pair<OWL::Id, OWL::Id> h1v1{horizontalLane1.GetId(), verticalLane1.GetId()};
    const std::pair<OWL::Id, OWL::Id> h1v2{horizontalLane1.GetId(), verticalLane2.GetId()};
    const std::pair<OWL::Id, OWL::Id> h2v1{horizontalLane2.GetId(), verticalLane1.GetId()};
    const std::pair<OWL::Id, OWL::Id> h2v2{horizontalLane2.GetId(), verticalLane2.GetId()};

    const std::pair<double, double> v1h1SOffset{10.0, 13.0};
    const std::pair<double, double> v1h2SOffset{13.0, 16.0};
    const std::pair<double, double> v2h1SOffset{10.0, 13.0};
    const std::pair<double, double> v2h2SOffset{13.0, 16.0};

    const std::pair<double, double> h1v1SOffset{7, 10};
    const std::pair<double, double> h1v2SOffset{4, 7};
    const std::pair<double, double> h2v1SOffset{7, 10};
    const std::pair<double, double> h2v2SOffset{4, 7};

    const auto& junctionMap = worldData->GetJunctions();
    ASSERT_THAT(junctionMap.size(), 1);

    const auto& junction = junctionMap.begin()->second;
    ASSERT_THAT(junction->GetIntersections().size(), 2);

    std::vector<OWL::IntersectionInfo> verticalIntersectionInfos;
    std::vector<OWL::IntersectionInfo> horizontalIntersectionInfos;

    ASSERT_THAT(junction->GetIntersections().count(verticalRoadStringId), Eq(1));
    ASSERT_THAT(junction->GetIntersections().count(horizontalRoadStringId), Eq(1));

    verticalIntersectionInfos = junction->GetIntersections().at(verticalRoadStringId);
    horizontalIntersectionInfos = junction->GetIntersections().at(horizontalRoadStringId);

    ASSERT_THAT(verticalIntersectionInfos.size(), 1);
    ASSERT_THAT(horizontalIntersectionInfos.size(), 1);

    const auto& verticalConnectionInfo = verticalIntersectionInfos.front();
    const auto& horizontalConnectionInfo = horizontalIntersectionInfos.front();

    ASSERT_THAT(verticalConnectionInfo.intersectingRoad, Eq(horizontalRoad->GetId()));// horizontalRoadIdPair->first);
    ASSERT_THAT(horizontalConnectionInfo.intersectingRoad, Eq(verticalRoad->GetId()));

    ASSERT_THAT(verticalConnectionInfo.relativeRank, IntersectingConnectionRank::Higher);
    ASSERT_THAT(horizontalConnectionInfo.relativeRank, IntersectingConnectionRank::Lower);

    ASSERT_THAT(verticalConnectionInfo.sOffsets.at(v1h1), GeometryDoublePairEq(v1h1SOffset));
    ASSERT_THAT(verticalConnectionInfo.sOffsets.at(v1h2), GeometryDoublePairEq(v1h2SOffset));
    ASSERT_THAT(verticalConnectionInfo.sOffsets.at(v2h1), GeometryDoublePairEq(v2h1SOffset));
    ASSERT_THAT(verticalConnectionInfo.sOffsets.at(v2h2), GeometryDoublePairEq(v2h2SOffset));

    ASSERT_THAT(horizontalConnectionInfo.sOffsets.at(h1v1), GeometryDoublePairEq(h1v1SOffset));
    ASSERT_THAT(horizontalConnectionInfo.sOffsets.at(h1v2), GeometryDoublePairEq(h1v2SOffset));
    ASSERT_THAT(horizontalConnectionInfo.sOffsets.at(h2v1), GeometryDoublePairEq(h2v1SOffset));
    ASSERT_THAT(horizontalConnectionInfo.sOffsets.at(h2v2), GeometryDoublePairEq(h2v2SOffset));
}

[[maybe_unused]] WorldObjectInterface* UNWRAP(const std::unique_ptr<AgentInterface>& agent)
{
    return dynamic_cast<WorldObjectInterface*>(agent.get());
}

[[nodiscard]] std::unique_ptr<AgentInterface> ADD_AGENT (SimulationSlave::World& world,
                               double x, double y, double width = 1.0, double length = 1.0)
{
    VehicleModelParameters vehicleParameter;
    vehicleParameter.vehicleType = AgentVehicleType::Car;
    vehicleParameter.boundingBoxDimensions.width = width;
    vehicleParameter.boundingBoxDimensions.length = length;
    vehicleParameter.boundingBoxCenter.x = 0.0;

    SpawnParameter spawnParameter;
    spawnParameter.positionX = x;
    spawnParameter.positionY = y;
    spawnParameter.velocity = 1.0;
    spawnParameter.yawAngle = 0.0;
    auto position = world.WorldCoord2LaneCoord(x,y,0);
    RoadGraph roadGraph;
    auto vertex = add_vertex(RouteElement{position.cbegin()->second.roadId, true}, roadGraph);
    spawnParameter.route = {roadGraph, vertex, vertex};

    AgentBlueprint agentBlueprint;
    agentBlueprint.SetVehicleModelParameters(vehicleParameter);
    agentBlueprint.SetSpawnParameter(spawnParameter);

    auto agent = world.CreateAgentAdapter({});
    world.RegisterAgent(agent.get());
    agent->InitParameter(agentBlueprint);
    return std::move(agent);
}

[[deprecated("Use ADD_AGENT instead as soon as AgentNetwork::Clear is not destroying the agents anymore")]]
AgentInterface* AddAgentToWorld (SimulationSlave::World& world,
                               double x, double y, double width = 1.0, double length = 1.0)
{
    auto agent = ADD_AGENT(world, x, y, width, length);
    auto agent_ptr = agent.get();
    agent.release(); // release unique_ptr without delete to mimic old ptr memory management
    return agent_ptr;
}

TEST(GetObjectsInRange_IntegrationTests, OneObjectOnQueriedLane)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("SceneryLeftLaneEnds.xodr"), IsTrue());

    auto& world = tsf.world;
    auto agent0 = AddAgentToWorld(world, 10.0, 2.0);
    auto agent1 = AddAgentToWorld(world, 10.0, 5.0);
    auto agent2 = AddAgentToWorld(world, 10.0, 9.0);
    world.SyncGlobalData(0);

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"1", true}, roadGraph);
    const auto objectsInRange = world.GetObjectsInRange(roadGraph, root, -3, 0, 0, 500).at(root);
    ASSERT_THAT(objectsInRange, SizeIs(1));
    ASSERT_THAT(objectsInRange.at(0), Eq(world.GetAgent(1)));
}

TEST(GetObjectsInRange_IntegrationTests, NoObjectOnQueriedLane)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("SceneryLeftLaneEnds.xodr"), IsTrue());

    auto& world = tsf.world;
    auto agent0 = AddAgentToWorld(world, 10.0, 2.0);
    auto agent1 = AddAgentToWorld(world, 10.0, 9.0);
    world.SyncGlobalData(0);

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"1", true}, roadGraph);
    const auto objectsInRange = world.GetObjectsInRange(roadGraph, root, -3, 0, 0, 500).at(root);
    ASSERT_THAT(objectsInRange, IsEmpty());
}

TEST(GetObjectsInRange_IntegrationTests, TwoObjectsInDifferentSections)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("SceneryLeftLaneEnds.xodr"), IsTrue());

    auto& world = tsf.world;
    auto agent0 = AddAgentToWorld(world, 10.0, 2.0);
    auto agent1 = AddAgentToWorld(world, 310.0, 5.0);
    auto agent2 = AddAgentToWorld(world, 10.0, 5.0);
    auto agent3 = AddAgentToWorld(world, 10.0, 9.0);
    world.SyncGlobalData(0);

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"1", true}, roadGraph);
    const auto objectsInRange = world.GetObjectsInRange(roadGraph, root, -3, 0, 0, 500).at(root);
    ASSERT_THAT(objectsInRange, SizeIs(2));
    ASSERT_THAT(objectsInRange.at(0), Eq(world.GetAgent(2)));
    ASSERT_THAT(objectsInRange.at(1), Eq(world.GetAgent(1)));
}

TEST(GetObjectsInRange_IntegrationTests, OneObjectOnSectionBorder)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("SceneryLeftLaneEnds.xodr"), IsTrue());

    auto& world = tsf.world;
    auto agent0 = AddAgentToWorld(world, 300.0, 2.0);
    auto agent1 = AddAgentToWorld(world, 300.0, 5.0);
    auto agent2 = AddAgentToWorld(world, 300.0, 9.0);
    world.SyncGlobalData(0);

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"1", true}, roadGraph);
    const auto objectsInRange = world.GetObjectsInRange(roadGraph, root, -3, 0, 0, 500).at(root);
    ASSERT_THAT(objectsInRange, SizeIs(1));
    ASSERT_THAT(objectsInRange.at(0), Eq(world.GetAgent(1)));
}

TEST(GetObjectsInRange_IntegrationTests, MultipleRoads)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("MultipleRoadsIntegrationScenery.xodr"), IsTrue());

    auto& world = tsf.world;
    auto agent0 = AddAgentToWorld(world, 510.0, 6.0);
    auto agent1 = AddAgentToWorld(world, 1300.0, 2.0);
    auto agent2 = AddAgentToWorld(world, 510.0, 2.0);
    auto agent3 = AddAgentToWorld(world, 510.0, -1.0);
    world.SyncGlobalData(0);

    RoadGraph roadGraph;
    RoadGraphVertex node1 = add_vertex(RouteElement{"1", true}, roadGraph);
    RoadGraphVertex node2 = add_vertex(RouteElement{"2", false}, roadGraph);
    RoadGraphVertex node3 = add_vertex(RouteElement{"3", true}, roadGraph);
    add_edge(node1, node2, roadGraph);
    add_edge(node2, node3, roadGraph);
    const auto objectsInRange = world.GetObjectsInRange(roadGraph, node1, -2, 500, 0, 1500).at(node3);
    ASSERT_THAT(objectsInRange, SizeIs(2));
    ASSERT_THAT(objectsInRange.at(0), Eq(world.GetAgent(2)));
    ASSERT_THAT(objectsInRange.at(1), Eq(world.GetAgent(1)));
}

TEST(Locator_IntegrationTests, AgentOnStraightRoad_CalculatesCorrectLocateResult)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("MultipleRoadsIntegrationScenery.xodr"), IsTrue());

    auto& world = tsf.world;
    const auto agent1 = AddAgentToWorld(world, 399.0, 1.0, 2.0, 5.0);
    const auto agent2 = AddAgentToWorld(world, 2500.0, 2.0, 2.0, 5.0);
    world.SyncGlobalData(0);

    ASSERT_THAT(agent1->GetRoads(MeasurementPoint::Front), ElementsAre("1"));
    ASSERT_THAT(agent1->GetObjectPosition().mainLocatePoint.at("1").laneId, Eq(-2));
    ASSERT_THAT(agent1->GetDistanceToStartOfRoad(MeasurementPoint::Front, "1"), DoubleNear(401.5, 0.01));
    ASSERT_THAT(agent1->GetDistanceToStartOfRoad(MeasurementPoint::Rear, "1"), DoubleNear(396.5, 0.01));
    ASSERT_THAT(agent1->GetLaneRemainder("1", Side::Left), DoubleNear(2.5, 0.01));
    ASSERT_THAT(agent1->GetLaneRemainder("1", Side::Right), DoubleNear(4.5, 0.01));
    ASSERT_THAT(agent2->GetRoads(MeasurementPoint::Front), ElementsAre("2"));
    ASSERT_THAT(agent2->GetObjectPosition().mainLocatePoint.at("2").laneId, Eq(2));
    ASSERT_THAT(agent2->GetDistanceToStartOfRoad(MeasurementPoint::Front, "2"), DoubleNear(502.5, 0.01));
    ASSERT_THAT(agent2->GetDistanceToStartOfRoad(MeasurementPoint::Rear, "2"), DoubleNear(497.5, 0.01));
    ASSERT_THAT(agent2->GetLaneRemainder("2", Side::Left), DoubleNear(0.5, 0.01));
    ASSERT_THAT(agent2->GetLaneRemainder("2", Side::Right), DoubleNear(1.5, 0.01));
}

TEST(Locator_IntegrationTests, AgentOnJunction_CalculatesCorrectLocateResult)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("TJunction.xodr"), IsTrue());

    auto& world = tsf.world;
    auto agent = AddAgentToWorld(world, 208.0, -2.0, 2.0, 4.0);
    world.SyncGlobalData(0);

    EXPECT_THAT(agent->GetDistanceToStartOfRoad(MeasurementPoint::Front, "R1-3"), DoubleNear(10.0, 0.01));
    EXPECT_THAT(agent->GetDistanceToStartOfRoad(MeasurementPoint::Rear, "R1-3"), DoubleNear(6.0, 0.01));
    EXPECT_THAT(agent->GetDistanceToStartOfRoad(MeasurementPoint::Front, "R2-3"), DoubleNear(std::atan(2.5) * 6, 0.15)); //front left corner
    EXPECT_THAT(agent->GetDistanceToStartOfRoad(MeasurementPoint::Rear, "R2-3"), DoubleNear(M_PI, 0.15));       //intersection point on right boundary
    EXPECT_THAT(agent->GetDistanceToStartOfRoad(MeasurementPoint::Front, "R3-2"), DoubleNear(std::atan(2.0) * 6, 0.15)); //rear right corner
    EXPECT_THAT(agent->GetDistanceToStartOfRoad(MeasurementPoint::Rear, "R3-2"), DoubleNear(std::acos(5.0 / 6.0) * 6, 0.15)); //intersection point on left boundary
    EXPECT_THAT(agent->GetDistanceToStartOfRoad(MeasurementPoint::Front, "R2-1"), DoubleNear(std::atan(5.0 / 6.0) * 6, 0.15)); //intersection point on right boundary
    EXPECT_THAT(agent->GetDistanceToStartOfRoad(MeasurementPoint::Rear, "R2-1"), DoubleNear(std::asin(0.3) * 6, 0.15)); //rear left corner
}

TEST(SceneryImporter_IntegrationTests, SingleRoad_ImportWithCorrectLaneMarkings)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("IntegrationTestScenery.xodr"), IsTrue());

    auto& world = tsf.world;

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"1", true}, roadGraph);
    auto laneMarkings = world.GetLaneMarkings(roadGraph, root, -1, 0.0, 99.0, Side::Left).at(root);
    ASSERT_THAT(laneMarkings, SizeIs(5));
    ASSERT_THAT(laneMarkings.at(0).relativeStartDistance, DoubleEq(0.0));
    ASSERT_THAT(laneMarkings.at(0).type, Eq(LaneMarking::Type::Solid));
    ASSERT_THAT(laneMarkings.at(0).width, DoubleEq(0.15));
    ASSERT_THAT(laneMarkings.at(0).color, Eq(LaneMarking::Color::White));
    ASSERT_THAT(laneMarkings.at(1).relativeStartDistance, DoubleEq(10.0));
    ASSERT_THAT(laneMarkings.at(1).type, Eq(LaneMarking::Type::None));
    ASSERT_THAT(laneMarkings.at(1).width, DoubleEq(0.15));
    ASSERT_THAT(laneMarkings.at(1).color, Eq(LaneMarking::Color::White));
    ASSERT_THAT(laneMarkings.at(2).relativeStartDistance, DoubleEq(18.0));
    ASSERT_THAT(laneMarkings.at(2).type, Eq(LaneMarking::Type::Solid));
    ASSERT_THAT(laneMarkings.at(2).width, DoubleEq(0.3));
    ASSERT_THAT(laneMarkings.at(2).color, Eq(LaneMarking::Color::White));
    ASSERT_THAT(laneMarkings.at(3).relativeStartDistance, DoubleEq(30.0));

    laneMarkings = world.GetLaneMarkings(roadGraph, root, -1, 0.0, 99.0, Side::Right).at(root);
    ASSERT_THAT(laneMarkings, SizeIs(5));
    ASSERT_THAT(laneMarkings.at(0).relativeStartDistance, DoubleEq(0.0));
    ASSERT_THAT(laneMarkings.at(0).type, Eq(LaneMarking::Type::Broken));
    ASSERT_THAT(laneMarkings.at(0).width, DoubleEq(0.15));
    ASSERT_THAT(laneMarkings.at(0).color, Eq(LaneMarking::Color::Yellow));
    ASSERT_THAT(laneMarkings.at(1).relativeStartDistance, DoubleEq(10.0));
    ASSERT_THAT(laneMarkings.at(1).type, Eq(LaneMarking::Type::Broken_Solid));
    ASSERT_THAT(laneMarkings.at(1).width, DoubleEq(0.3));
    ASSERT_THAT(laneMarkings.at(1).color, Eq(LaneMarking::Color::Red));
    ASSERT_THAT(laneMarkings.at(2).relativeStartDistance, DoubleEq(21.0));
    ASSERT_THAT(laneMarkings.at(2).type, Eq(LaneMarking::Type::Solid_Broken));
    ASSERT_THAT(laneMarkings.at(2).width, DoubleEq(0.3));
    ASSERT_THAT(laneMarkings.at(2).color, Eq(LaneMarking::Color::Blue));
    ASSERT_THAT(laneMarkings.at(3).relativeStartDistance, DoubleEq(30.0));
    ASSERT_THAT(laneMarkings.at(3).type, Eq(LaneMarking::Type::None));

    laneMarkings = world.GetLaneMarkings(roadGraph, root, -2, 11.0, 88.0, Side::Left).at(root);
    ASSERT_THAT(laneMarkings, SizeIs(4));
    ASSERT_THAT(laneMarkings.at(0).relativeStartDistance, DoubleEq(-1.0));
    ASSERT_THAT(laneMarkings.at(0).type, Eq(LaneMarking::Type::Broken_Solid));
    ASSERT_THAT(laneMarkings.at(0).width, DoubleEq(0.3));
    ASSERT_THAT(laneMarkings.at(0).color, Eq(LaneMarking::Color::Red));
    ASSERT_THAT(laneMarkings.at(1).relativeStartDistance, DoubleEq(10.0));
    ASSERT_THAT(laneMarkings.at(1).type, Eq(LaneMarking::Type::Solid_Broken));
    ASSERT_THAT(laneMarkings.at(1).width, DoubleEq(0.3));
    ASSERT_THAT(laneMarkings.at(1).color, Eq(LaneMarking::Color::Blue));
    ASSERT_THAT(laneMarkings.at(2).relativeStartDistance, DoubleEq(19.0));

    laneMarkings = world.GetLaneMarkings(roadGraph, root, -2, 11.0, 88.0, Side::Right).at(root);
    ASSERT_THAT(laneMarkings, SizeIs(4));
    ASSERT_THAT(laneMarkings.at(0).relativeStartDistance, DoubleEq(-1.0));
    ASSERT_THAT(laneMarkings.at(0).type, Eq(LaneMarking::Type::Broken_Broken));
    ASSERT_THAT(laneMarkings.at(0).width, DoubleEq(0.15));
    ASSERT_THAT(laneMarkings.at(0).color, Eq(LaneMarking::Color::White));
    ASSERT_THAT(laneMarkings.at(1).relativeStartDistance, DoubleEq(4.0));
    ASSERT_THAT(laneMarkings.at(1).type, Eq(LaneMarking::Type::Solid_Solid));
    ASSERT_THAT(laneMarkings.at(1).width, DoubleEq(0.3));
    ASSERT_THAT(laneMarkings.at(1).color, Eq(LaneMarking::Color::White));
    ASSERT_THAT(laneMarkings.at(2).relativeStartDistance, DoubleEq(19.0));
}

TEST(SceneryImporter_IntegrationTests, SingleRoad_ImportWithCorrectTrafficSigns)
{
    openScenario::TrafficSignalController trafficSignalController;
    trafficSignalController.delay = 0.0;
    trafficSignalController.phases.push_back(openScenario::TrafficSignalControllerPhase{"", 1, {{"8", "red yellow"}, {"9", "green"}}});
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("IntegrationTestScenery.xodr", {trafficSignalController}), IsTrue());

    auto& world = tsf.world;

    RoadGraph roadGraph;
    RoadGraphVertex root = add_vertex(RouteElement{"1", true}, roadGraph);
    auto trafficSigns = world.GetTrafficSignsInRange(roadGraph, root, -1, 5, 90).at(root);
    std::sort(trafficSigns.begin(), trafficSigns.end(),
              [](CommonTrafficSign::Entity first, CommonTrafficSign::Entity second){return first.relativeDistance < second.relativeDistance;});
    ASSERT_THAT(trafficSigns, SizeIs(3));
    ASSERT_THAT(trafficSigns.at(0).relativeDistance, DoubleEq(10.0));
    ASSERT_THAT(trafficSigns.at(0).type, Eq(CommonTrafficSign::Type::MaximumSpeedLimit));
    ASSERT_THAT(trafficSigns.at(0).value, DoubleNear(50 / 3.6, 1e-3));
    ASSERT_THAT(trafficSigns.at(0).unit, Eq(CommonTrafficSign::Unit::MeterPerSecond));
    ASSERT_THAT(trafficSigns.at(0).supplementarySigns, IsEmpty());
    ASSERT_THAT(trafficSigns.at(1).relativeDistance, DoubleEq(30.0));
    ASSERT_THAT(trafficSigns.at(1).type, Eq(CommonTrafficSign::Type::SpeedLimitZoneBegin));
    ASSERT_THAT(trafficSigns.at(1).value, DoubleNear(30 / 3.6, 1e-3));
    ASSERT_THAT(trafficSigns.at(1).unit, Eq(CommonTrafficSign::Unit::MeterPerSecond));
    ASSERT_THAT(trafficSigns.at(1).supplementarySigns, IsEmpty());
    ASSERT_THAT(trafficSigns.at(2).relativeDistance, DoubleEq(31.0));
    ASSERT_THAT(trafficSigns.at(2).type, Eq(CommonTrafficSign::Type::AnnounceLeftLaneEnd));
    ASSERT_THAT(trafficSigns.at(2).value, Eq(2));
    ASSERT_THAT(trafficSigns.at(2).supplementarySigns, IsEmpty());

    trafficSigns = world.GetTrafficSignsInRange(roadGraph, root, -2, 11, 90).at(root);
    std::sort(trafficSigns.begin(), trafficSigns.end(),
              [](CommonTrafficSign::Entity first, CommonTrafficSign::Entity second){return first.relativeDistance < second.relativeDistance;});
    ASSERT_THAT(trafficSigns, SizeIs(5));
    ASSERT_THAT(trafficSigns.at(0).relativeDistance, DoubleEq(4.0));
    ASSERT_THAT(trafficSigns.at(0).type, Eq(CommonTrafficSign::Type::MaximumSpeedLimit));
    ASSERT_THAT(trafficSigns.at(0).value, DoubleNear(50 / 3.6, 1e-3));
    ASSERT_THAT(trafficSigns.at(0).unit, Eq(CommonTrafficSign::Unit::MeterPerSecond));
    ASSERT_THAT(trafficSigns.at(0).supplementarySigns, IsEmpty());
    ASSERT_THAT(trafficSigns.at(1).relativeDistance, DoubleEq(14.0));
    ASSERT_THAT(trafficSigns.at(1).type, Eq(CommonTrafficSign::Type::OvertakingBanBegin));
    ASSERT_THAT(trafficSigns.at(1).supplementarySigns, IsEmpty());
    ASSERT_THAT(trafficSigns.at(2).relativeDistance, DoubleEq(24.0));
    ASSERT_THAT(trafficSigns.at(2).type, Eq(CommonTrafficSign::Type::SpeedLimitZoneBegin));
    ASSERT_THAT(trafficSigns.at(2).value, DoubleNear(30 / 3.6, 1e-3));
    ASSERT_THAT(trafficSigns.at(2).unit, Eq(CommonTrafficSign::Unit::MeterPerSecond));
    ASSERT_THAT(trafficSigns.at(2).supplementarySigns, IsEmpty());
    ASSERT_THAT(trafficSigns.at(3).relativeDistance, DoubleEq(25.0));
    ASSERT_THAT(trafficSigns.at(3).type, Eq(CommonTrafficSign::Type::AnnounceLeftLaneEnd));
    ASSERT_THAT(trafficSigns.at(3).value, Eq(2));
    ASSERT_THAT(trafficSigns.at(3).unit, Eq(CommonTrafficSign::Unit::None));
    ASSERT_THAT(trafficSigns.at(3).supplementarySigns, IsEmpty());
    ASSERT_THAT(trafficSigns.at(4).relativeDistance, DoubleEq(29.0));
    ASSERT_THAT(trafficSigns.at(4).type, Eq(CommonTrafficSign::Type::Stop));
    ASSERT_THAT(trafficSigns.at(4).supplementarySigns, SizeIs(1));
    ASSERT_THAT(trafficSigns.at(4).supplementarySigns.front().type, Eq(CommonTrafficSign::Type::DistanceIndication));
    ASSERT_THAT(trafficSigns.at(4).supplementarySigns.front().value, DoubleEq(200.0));
    ASSERT_THAT(trafficSigns.at(4).supplementarySigns.front().unit, Eq(CommonTrafficSign::Unit::Meter));

    auto roadMarkings = world.GetRoadMarkingsInRange(roadGraph, root, -2, 11, 90).at(root);
    ASSERT_THAT(roadMarkings, SizeIs(1));
    ASSERT_THAT(roadMarkings.at(0).relativeDistance, DoubleEq(30.0));
    ASSERT_THAT(roadMarkings.at(0).type, Eq(CommonTrafficSign::Type::Stop));

    auto trafficLights = world.GetTrafficLightsInRange(roadGraph, root, -2, 11, 90).at(root);
    ASSERT_THAT(trafficLights.size(), Eq(1));
    ASSERT_THAT(trafficLights.at(0).relativeDistance, DoubleEq(49.0));
    ASSERT_THAT(trafficLights.at(0).type, Eq(CommonTrafficLight::Type::ThreeLightsLeft));
    ASSERT_THAT(trafficLights.at(0).state, Eq(CommonTrafficLight::State::RedYellow));

    trafficLights = world.GetTrafficLightsInRange(roadGraph, root, -3, 31, 90).at(root);
    ASSERT_THAT(trafficLights.size(), Eq(2));
    ASSERT_THAT(trafficLights.at(0).relativeDistance, DoubleEq(29.0));
    ASSERT_THAT(trafficLights.at(0).type, Eq(CommonTrafficLight::Type::ThreeLightsLeft));
    ASSERT_THAT(trafficLights.at(0).state, Eq(CommonTrafficLight::State::RedYellow));
    ASSERT_THAT(trafficLights.at(1).relativeDistance, DoubleEq(34.0));
    ASSERT_THAT(trafficLights.at(1).type, Eq(CommonTrafficLight::Type::ThreeLights));
    ASSERT_THAT(trafficLights.at(1).state, Eq(CommonTrafficLight::State::Green));
}

TEST(SceneryImporter_IntegrationTests, SingleRoad_ImportWithCorrectTrafficSignGeometriess)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("IntegrationTestScenery.xodr"), IsTrue());

    auto& world = tsf.world;

    OWL::Interfaces::WorldData* worldData = static_cast<OWL::Interfaces::WorldData*>(world.GetWorldData());
    auto& groundTruth = worldData->GetOsiGroundTruth();

    ASSERT_THAT(groundTruth.traffic_sign_size(), Eq(5));
    auto& trafficSign0 = groundTruth.traffic_sign(0);
    ASSERT_THAT(trafficSign0.main_sign().base().position().x(), DoubleEq(15));
    ASSERT_THAT(trafficSign0.main_sign().base().position().y(), DoubleEq(-0.5));
    ASSERT_THAT(trafficSign0.main_sign().base().position().z(), DoubleEq(1.7));
    ASSERT_THAT(trafficSign0.main_sign().base().dimension().width(), DoubleEq(0.4));
    ASSERT_THAT(trafficSign0.main_sign().base().dimension().height(), DoubleEq(0.4));
    ASSERT_THAT(trafficSign0.main_sign().base().orientation().yaw(), DoubleEq(0.0));

    auto& trafficSign1 = groundTruth.traffic_sign(1);
    ASSERT_THAT(trafficSign1.main_sign().base().position().x(), DoubleEq(25));
    ASSERT_THAT(trafficSign1.main_sign().base().position().y(), DoubleEq(-0.5));
    ASSERT_THAT(trafficSign1.main_sign().base().position().z(), DoubleEq(1.7));
    ASSERT_THAT(trafficSign1.main_sign().base().dimension().width(), DoubleEq(0.4));
    ASSERT_THAT(trafficSign1.main_sign().base().dimension().height(), DoubleEq(0.4));
    ASSERT_THAT(trafficSign1.main_sign().base().orientation().yaw(), DoubleNear(0.1, 1e-3));

    auto& trafficSign2 = groundTruth.traffic_sign(2);
    ASSERT_THAT(trafficSign2.main_sign().base().position().x(), DoubleEq(35));
    ASSERT_THAT(trafficSign2.main_sign().base().position().y(), DoubleEq(-0.5));
    ASSERT_THAT(trafficSign2.main_sign().base().position().z(), DoubleEq(1.7));
    ASSERT_THAT(trafficSign2.main_sign().base().dimension().width(), DoubleEq(0.4));
    ASSERT_THAT(trafficSign2.main_sign().base().dimension().height(), DoubleEq(0.4));
    ASSERT_THAT(trafficSign2.main_sign().base().orientation().yaw(), DoubleEq(-M_PI + 0.1));

    auto& trafficSign3 = groundTruth.traffic_sign(3);
    ASSERT_THAT(trafficSign3.main_sign().base().position().x(), DoubleEq(36));
    ASSERT_THAT(trafficSign3.main_sign().base().position().y(), DoubleEq(-0.5));
    ASSERT_THAT(trafficSign3.main_sign().base().position().z(), DoubleEq(2.0));
    ASSERT_THAT(trafficSign3.main_sign().base().dimension().width(), DoubleEq(0.5));
    ASSERT_THAT(trafficSign3.main_sign().base().dimension().height(), DoubleEq(1.0));
    ASSERT_THAT(trafficSign3.main_sign().base().orientation().yaw(), DoubleEq(0.0));

    auto& trafficSign4 = groundTruth.traffic_sign(4);
    ASSERT_THAT(trafficSign4.main_sign().base().position().x(), DoubleEq(40));
    ASSERT_THAT(trafficSign4.main_sign().base().position().y(), DoubleEq(-0.5));
    ASSERT_THAT(trafficSign4.main_sign().base().position().z(), DoubleEq(1.7));
    ASSERT_THAT(trafficSign4.main_sign().base().dimension().width(), DoubleEq(0.4));
    ASSERT_THAT(trafficSign4.main_sign().base().dimension().height(), DoubleEq(0.4));
    ASSERT_THAT(trafficSign4.main_sign().base().orientation().yaw(), DoubleEq(0.0));
    ASSERT_THAT(trafficSign4.supplementary_sign(0).base().position().x(), DoubleEq(40));
    ASSERT_THAT(trafficSign4.supplementary_sign(0).base().position().y(), DoubleEq(-0.5));
    ASSERT_THAT(trafficSign4.supplementary_sign(0).base().position().z(), DoubleEq(1.3));
    ASSERT_THAT(trafficSign4.supplementary_sign(0).base().dimension().width(), DoubleEq(0.4));
    ASSERT_THAT(trafficSign4.supplementary_sign(0).base().dimension().height(), DoubleEq(0.2));
    ASSERT_THAT(trafficSign4.supplementary_sign(0).base().orientation().yaw(), DoubleEq(0.0));

    ASSERT_THAT(groundTruth.road_marking_size(), Eq(2));

    auto& roadMarking1 = groundTruth.road_marking(0);
    ASSERT_THAT(roadMarking1.base().position().x(), DoubleEq(10));
    ASSERT_THAT(roadMarking1.base().position().y(), DoubleEq(7.5));
    ASSERT_THAT(roadMarking1.base().position().z(), DoubleEq(0.0));
    ASSERT_THAT(roadMarking1.base().dimension().width(), DoubleEq(8.0));
    ASSERT_THAT(roadMarking1.base().dimension().length(), DoubleEq(4.0));
    ASSERT_THAT(roadMarking1.base().orientation().yaw(), DoubleEq(0.0));
    ASSERT_THAT(roadMarking1.classification().traffic_main_sign_type(), Eq(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ZEBRA_CROSSING));

    auto& roadMarking2 = groundTruth.road_marking(1);
    ASSERT_THAT(roadMarking2.base().position().x(), DoubleEq(41));
    ASSERT_THAT(roadMarking2.base().position().y(), DoubleEq(0.5));
    ASSERT_THAT(roadMarking2.base().position().z(), DoubleEq(0.0));
    ASSERT_THAT(roadMarking2.base().dimension().width(), DoubleEq(4.0));
    ASSERT_THAT(roadMarking2.base().dimension().height(), DoubleEq(0.0));
    ASSERT_THAT(roadMarking2.base().orientation().yaw(), DoubleEq(0.0));
    ASSERT_THAT(roadMarking2.classification().traffic_main_sign_type(), Eq(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_STOP));
}

TEST(SceneryImporter_IntegrationTests, TJunction_ImportWithCorrectConnectionsAndPriorities)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("TJunction.xodr"), IsTrue());

    auto& world = tsf.world;

    auto connections = world.GetConnectionsOnJunction("J0", "R1");
    ASSERT_THAT(connections, SizeIs(2));
    ASSERT_THAT(connections.at(0).connectingRoadId, Eq("R1-2"));
    ASSERT_THAT(connections.at(0).outgoingRoadId, Eq("R2"));
    ASSERT_THAT(connections.at(0).outgoingStreamDirection, Eq(false));
    ASSERT_THAT(connections.at(1).connectingRoadId, Eq("R1-3"));
    ASSERT_THAT(connections.at(1).outgoingRoadId, Eq("R3"));
    ASSERT_THAT(connections.at(1).outgoingStreamDirection, Eq(false));

    connections = world.GetConnectionsOnJunction("J0", "R2");
    ASSERT_THAT(connections, SizeIs(2));
    ASSERT_THAT(connections.at(0).connectingRoadId, Eq("R2-1"));
    ASSERT_THAT(connections.at(0).outgoingRoadId, Eq("R1"));
    ASSERT_THAT(connections.at(0).outgoingStreamDirection, Eq(false));
    ASSERT_THAT(connections.at(1).connectingRoadId, Eq("R2-3"));
    ASSERT_THAT(connections.at(1).outgoingRoadId, Eq("R3"));
    ASSERT_THAT(connections.at(1).outgoingStreamDirection, Eq(false));

    connections = world.GetConnectionsOnJunction("J0", "R3");
    ASSERT_THAT(connections, SizeIs(2));
    ASSERT_THAT(connections.at(0).connectingRoadId, Eq("R3-1"));
    ASSERT_THAT(connections.at(0).outgoingRoadId, Eq("R1"));
    ASSERT_THAT(connections.at(0).outgoingStreamDirection, Eq(false));
    ASSERT_THAT(connections.at(1).connectingRoadId, Eq("R3-2"));
    ASSERT_THAT(connections.at(1).outgoingRoadId, Eq("R2"));
    ASSERT_THAT(connections.at(1).outgoingStreamDirection, Eq(false));

    auto priorities = world.GetPrioritiesOnJunction("J0");
    std::sort(priorities.begin(), priorities.end(),
              [](const JunctionConnectorPriority& first, const JunctionConnectorPriority& second)
    {return first.high < second.high || (first.high == second.high && first.low < second.low);});
    ASSERT_THAT(priorities.at(0).high, Eq("R1-2"));
    ASSERT_THAT(priorities.at(0).low, Eq("R3-2"));
    ASSERT_THAT(priorities.at(1).high, Eq("R1-3"));
    ASSERT_THAT(priorities.at(1).low, Eq("R2-1"));
    ASSERT_THAT(priorities.at(2).high, Eq("R1-3"));
    ASSERT_THAT(priorities.at(2).low, Eq("R2-3"));
    ASSERT_THAT(priorities.at(3).high, Eq("R1-3"));
    ASSERT_THAT(priorities.at(3).low, Eq("R3-2"));
    ASSERT_THAT(priorities.at(4).high, Eq("R3-1"));
    ASSERT_THAT(priorities.at(4).low, Eq("R2-1"));
    ASSERT_THAT(priorities.at(5).high, Eq("R3-2"));
    ASSERT_THAT(priorities.at(5).low, Eq("R2-1"));
}

TEST(GetObstruction_IntegrationTests, AgentsOnStraightRoad)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("SceneryLeftLaneEnds.xodr"), IsTrue());

    auto& world = tsf.world;
    auto agent0 = AddAgentToWorld(world, 10.0, 2.0);
    auto agent1 = AddAgentToWorld(world, 100.0, 2.5, 2.0);
    world.SyncGlobalData(0);

    auto& egoAgent = agent0->GetEgoAgent();
    RoadGraph roadGraph;
    auto root = add_vertex(RouteElement{"1", true}, roadGraph);
    egoAgent.SetRoadGraph(std::move(roadGraph), root, root);
    const auto obstruction = egoAgent.GetObstruction(agent1); // use UNWRAP when agent gets unique_ptr
    EXPECT_THAT(obstruction.left, DoubleEq(1.5));
    EXPECT_THAT(obstruction.right, DoubleEq(-0.5));
}

TEST(GetObstruction_IntegrationTests, AgentBehindJunction)
{
    TESTSCENERY_FACTORY tsf;
    ASSERT_THAT(tsf.instantiate("TJunction.xodr"), IsTrue());

    auto& world = tsf.world;
    auto agent0 = AddAgentToWorld(world, 10.0, -3.0);
    auto agent1 = AddAgentToWorld(world, 203.5, -10.0, 1.0, 3.0);
    world.SyncGlobalData(0);

    auto& egoAgent = agent0->GetEgoAgent();
    RoadGraph roadGraph;
    auto node1 = add_vertex(RouteElement{"R1", true}, roadGraph);
    auto node2 = add_vertex(RouteElement{"R1-2", true}, roadGraph);
    auto node3 = add_vertex(RouteElement{"R2", false}, roadGraph);
    add_edge(node1, node2, roadGraph);
    add_edge(node2, node3, roadGraph);
    egoAgent.SetRoadGraph(std::move(roadGraph), node1, node3);
    const auto obstruction = egoAgent.GetObstruction(agent1); // use UNWRAP when agent gets unique_ptr
    EXPECT_THAT(obstruction.left, DoubleNear(2.0, 1e-3));
    EXPECT_THAT(obstruction.right, DoubleNear(-1.0, 1e-3));
}
