/*********************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
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

#include "scenery.h"
#include "sceneryImporter.h"
#include "world.h"
#include "WorldData.h"
#include "AgentAdapter.h"
#include <algorithm>
#include "CoreModules/Stochastics/stochastics_implementation.h"

#include <boost/filesystem.hpp>

using ::testing::ElementsAre;
using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::Ne;
using ::testing::UnorderedElementsAre;
using ::testing::IsEmpty;

using namespace boost::filesystem;
using namespace Configuration;
using namespace Importer;

struct TESTSCENERY_FACTORY
{
    const std::string libraryPath = "./lib/World_OSI";

    SimulationCommon::Callbacks callbacks;
    StochasticsImplementation stochastics{&callbacks};
    SimulationSlave::WorldBinding worldBinding;
    SimulationSlave::World world;
    Scenery scenery;

    TESTSCENERY_FACTORY(std::string sceneryFile) :
        worldBinding(libraryPath, &callbacks, &stochastics),
        world(&worldBinding)
    {
        path sceneryPath = initial_path() / "Resources" / "ImporterTest" / sceneryFile;
        world.Instantiate();
        SceneryImporter::Import(sceneryPath.string(), &scenery);
        world.CreateScenery(&scenery);
    }
};

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
std::vector<OWL::Interfaces::Section*> GetDistanceSortedSectionsForRoad(OWL::Interfaces::WorldData* worldData, std::string roadId)
{
    //Extract sections for given roadID
    std::vector<OWL::Interfaces::Section*> queriedSections{};
    for(auto& section : worldData->GetSections())
    {
        auto osiRoadId = section.second->GetRoad().GetId();
        if(worldData->GetRoadIdMapping().at(osiRoadId) == roadId)
        {
            queriedSections.push_back(section.second);
        }
    }

    //Sort by distance
    std::sort(queriedSections.begin(), queriedSections.end(),
              [](OWL::Interfaces::Section* s1, OWL::Interfaces::Section* s2)
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
void CheckLaneConnections(OWL::Interfaces::WorldData* worldData, std::list<const OWL::Interfaces::Lane*> firstSectionLanes, std::list<const OWL::Interfaces::Lane*> secondSectionLanes, int firstLaneId, int secondLaneId, LaneConnectionType howIsConnection = LaneConnectionType::REGULAR)
{
    auto firstLane = GetLaneById(worldData, firstSectionLanes, firstLaneId);
    auto secondLane = GetLaneById(worldData, secondSectionLanes, secondLaneId);

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

//! Test correctly imported scenery
//! Scope is on World-level
TEST(SceneryImporter_IntegrationTests, SingleRoad_ImportWithCorrectLanes)
{
    TESTSCENERY_FACTORY tsf("IntegrationTestScenery.xodr");
    auto& world = tsf.world;

    Route route{"1"};
    const auto relativeLanes = world.GetRelativeLanes(route, "1", -1, 0.0, 150.0);
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
                                                         RelativeWorldView::Lane{-1, true, LaneType::Stop, -1, -1},
                                                         RelativeWorldView::Lane{-2, true, LaneType::Driving, std::nullopt, -2}));

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
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(route, "1", -1, 0.0, maxSearchDistance, {LaneType::Driving, LaneType::Stop}), 100.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(route, "1", -2, 15.0, maxSearchDistance, {LaneType::Driving, LaneType::Stop}), 135.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(route, "1", -3, 35.0, maxSearchDistance, {LaneType::Driving, LaneType::Stop}), 115.0);

    ASSERT_DOUBLE_EQ(world.GetLaneWidth(route, "1", -1, 60.0), 3.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth(route, "1", -2, 60.0), 4.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth(route, "1", -3, 60.0), 5.0);

}

TEST(SceneryImporter_IntegrationTests, MultipleRoads_ImportWithCorrectLanes)
{
    TESTSCENERY_FACTORY tsf("MultipleRoadsIntegrationScenery.xodr");
    auto& world = tsf.world;

    Route route{{{"1", true}, {"2", false}, {"3", true}}, {}, 0};

    const auto relativeLanes = world.GetRelativeLanes(route, "1", -1, 0.0, 6000.0);
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
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(route, "1", -1, 0.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}), 6000.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(route, "1", -2, 650.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}), 5350.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(route, "2", 2, 0.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}), 3000.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(route, "2", 2, 1500.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}), 4500.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(route, "3", -3, 0.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}), 3000.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(route, "3", -3, 1500.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}), 1500.0);

    //-----------------------------RoId, laneId, s
    ASSERT_DOUBLE_EQ(world.GetLaneWidth(route, "1", -1, 60.0), 3.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth(route, "1", -3, 999.9), 5.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth(route, "2", 1, 0.0), 3.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth(route, "3", -2, 1500.0), 4.0);

}

TEST(SceneryImporter_IntegrationTests, MultipleRoadsWithJunctions_ImportWithCorrectLanes)
{
    TESTSCENERY_FACTORY tsf("MultipleRoadsWithJunctionIntegrationScenery.xodr");
    auto& world = tsf.world;

    Route routeUp{{{"1", true}, {"4", true}, {"2", true}}, {}, 1};
    Route routeDown{{{"1", true}, {"5", true}, {"3", true}}, {}, 2};

    const auto relativeLanesUp = world.GetRelativeLanes(routeUp, "1", -1, 0.0, 320.0);
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

    const auto relativeLanesDown = world.GetRelativeLanes(routeDown, "1", -1, 0.0, 320.0);
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
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(routeUp, "1", -1, 0.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}), 320.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(routeUp, "1", -2, 90.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}), 230.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(routeDown, "1", -3, 10.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}), 410.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(routeDown, "1", -4, 0.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}), 420.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(routeUp, "2", -1, 0.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}), 200.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(routeUp, "2", -2, 150.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}), 50.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(routeDown, "3", -1, 0.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}), 300.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(routeUp, "4", -1, 0.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}), 220.0);
    ASSERT_DOUBLE_EQ(world.GetDistanceToEndOfLane(routeDown, "5", -2, 18.0, maxSearchLength, {LaneType::Driving, LaneType::Stop}), 302.0);

    //-----------------------------RoId, laneId, s
    ASSERT_DOUBLE_EQ(world.GetLaneWidth(routeUp, "1", -1, 60.0), 3.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth(routeUp, "1", -2, 95.0), 4.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth(routeDown, "1", -3, 99.0), 5.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth(routeDown, "1", -4, 0.0), 6.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth(routeUp, "2", -1, 1.0), 3.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth(routeUp, "2", -2, 20.0), 4.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth(routeDown, "3", -1, 123.0), 5.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth(routeDown, "3", -2, 200.0), 6.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth(routeUp, "4", -1, 15.0), 3.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth(routeUp, "4", -2, 15.0), 4.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth(routeDown, "5", -1, 0.0), 5.0);
    ASSERT_DOUBLE_EQ(world.GetLaneWidth(routeDown, "5", -2, 15.0), 6.0);
}


//! Test correct lane predeccessor and successors
//! Scope is on WorldData and OWL-Level
TEST(SceneryImporter_IntegrationTests, SingleRoad_CheckForCorrectLaneConnections)
{
    TESTSCENERY_FACTORY tsf("IntegrationTestScenery.xodr");
    auto& world = tsf.world;

    OWL::Interfaces::WorldData* worldData = static_cast<OWL::Interfaces::WorldData*>(world.GetWorldData());

    ASSERT_EQ(worldData->GetSections().size(), 5);

    std::vector<OWL::Interfaces::Section*> sections = GetDistanceSortedSectionsForRoad(worldData, "1");

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
    TESTSCENERY_FACTORY tsf("MultipleRoadsIntegrationScenery.xodr");
    auto& world = tsf.world;

    OWL::Interfaces::WorldData* worldData = static_cast<OWL::Interfaces::WorldData*>(world.GetWorldData());

    ASSERT_EQ(worldData->GetSections().size(), 6);

    std::vector<OWL::Interfaces::Section*> sectionsRoad1 = GetDistanceSortedSectionsForRoad(worldData, "1");
    const auto& lanesRoad1Section1 = sectionsRoad1.front()->GetLanes();
    const auto& lanesRoad1Section2 = sectionsRoad1.back()->GetLanes();
    std::vector<OWL::Interfaces::Section*> sectionsRoad2 = GetDistanceSortedSectionsForRoad(worldData, "2");
    const auto& lanesRoad2Section1 = sectionsRoad2.front()->GetLanes();
    const auto& lanesRoad2Section2 = sectionsRoad2.back()->GetLanes();
    std::vector<OWL::Interfaces::Section*> sectionsRoad3 = GetDistanceSortedSectionsForRoad(worldData, "3");
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
    TESTSCENERY_FACTORY tsf("MultipleRoadsWithJunctionIntegrationScenery.xodr");
    auto& world = tsf.world;

    OWL::Interfaces::WorldData* worldData = static_cast<OWL::Interfaces::WorldData*>(world.GetWorldData());

    ASSERT_EQ(worldData->GetSections().size(), 5);

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

TEST(SceneryImporter_IntegrationTests, MultipleRoadsWithNonIntersectingJunctions_JunctionsHaveNoIntersectionInformation)
{
    TESTSCENERY_FACTORY tsf("MultipleRoadsWithJunctionIntegrationScenery.xodr");
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
    TESTSCENERY_FACTORY tsf("IntersectedJunctionScenery.xodr");
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

AgentAdapter* AddAgentToWorld (SimulationSlave::World& world,
                               int id, double x, double y, double width = 1.0, double length = 1.0)
{
    auto agent = static_cast<AgentAdapter*>(world.CreateAgentAdapterForAgent());
    world.AddAgent(id, agent);
    agent->UpdatePositionX(x);
    agent->UpdatePositionY(y);
    agent->UpdateWidth(width);
    agent->UpdateLength(length);
    agent->UpdateVelocity(1.0);
    return agent;
}

TEST(GetObjectsInRange_IntegrationTests, OneObjectOnQueriedLane)
{
    TESTSCENERY_FACTORY tsf("SceneryLeftLaneEnds.xodr");
    auto& world = tsf.world;
    AddAgentToWorld(world, 0, 10.0, 2.0);
    AddAgentToWorld(world, 1, 10.0, 5.0);
    AddAgentToWorld(world, 2, 10.0, 9.0);
    world.SyncGlobalData();

    Route route{"1"};
    const auto objectsInRange = world.GetObjectsInRange(route, "1", -3, 0, 0, 500);
    const auto agent1 = world.GetAgent(1);
    ASSERT_THAT(objectsInRange.size(), Eq(1));
    ASSERT_THAT(objectsInRange.at(0), Eq(agent1));
}

TEST(GetObjectsInRange_IntegrationTests, NoObjectOnQueriedLane)
{
    TESTSCENERY_FACTORY tsf("SceneryLeftLaneEnds.xodr");
    auto& world = tsf.world;
    AddAgentToWorld(world, 0, 10.0, 2.0);
    AddAgentToWorld(world, 1, 10.0, 9.0);
    world.SyncGlobalData();

    Route route{"1"};
    const auto objectsInRange = world.GetObjectsInRange(route, "1", -3, 0, 0, 500);
    ASSERT_THAT(objectsInRange.size(), Eq(0));
}

TEST(GetObjectsInRange_IntegrationTests, TwoObjectsInDifferentSections)
{
    TESTSCENERY_FACTORY tsf("SceneryLeftLaneEnds.xodr");
    auto& world = tsf.world;
    AddAgentToWorld(world, 0, 10.0, 2.0);
    AddAgentToWorld(world, 1, 310.0, 5.0);
    AddAgentToWorld(world, 2, 10.0, 5.0);
    AddAgentToWorld(world, 3, 10.0, 9.0);
    world.SyncGlobalData();

    Route route{"1"};
    const auto objectsInRange = world.GetObjectsInRange(route, "1", -3, 0, 0, 500);
    const auto agent1 = world.GetAgent(1);
    const auto agent2 = world.GetAgent(2);
    ASSERT_THAT(objectsInRange.size(), Eq(2));
    ASSERT_THAT(objectsInRange.at(0), Eq(agent2));
    ASSERT_THAT(objectsInRange.at(1), Eq(agent1));
}

TEST(GetObjectsInRange_IntegrationTests, OneObjectOnSectionBorder)
{
    TESTSCENERY_FACTORY tsf("SceneryLeftLaneEnds.xodr");
    auto& world = tsf.world;
    AddAgentToWorld(world, 0, 300.0, 2.0);
    AddAgentToWorld(world, 1, 300.0, 5.0);
    AddAgentToWorld(world, 2, 300.0, 9.0);
    world.SyncGlobalData();

    Route route{"1"};
    const auto objectsInRange = world.GetObjectsInRange(route, "1", -3, 0, 0, 500);
    const auto agent1 = world.GetAgent(1);
    ASSERT_THAT(objectsInRange.size(), Eq(1));
    ASSERT_THAT(objectsInRange.at(0), Eq(agent1));
}

TEST(GetObjectsInRange_IntegrationTests, MultipleRoads)
{
    TESTSCENERY_FACTORY tsf("MultipleRoadsIntegrationScenery.xodr");
    auto& world = tsf.world;
    AddAgentToWorld(world, 0, 510.0, 6.0);
    AddAgentToWorld(world, 1, 1300.0, 2.0);
    AddAgentToWorld(world, 2, 510.0, 2.0);
    AddAgentToWorld(world, 3, 510.0, -1.0);
    world.SyncGlobalData();

    Route route{{{"1", true}, {"2", false}, {"3", true}}, {}, 1};
    const auto objectsInRange = world.GetObjectsInRange(route, "1", -2, 500, 0, 1500);
    const auto agent1 = world.GetAgent(1);
    const auto agent2 = world.GetAgent(2);
    ASSERT_THAT(objectsInRange.size(), Eq(2));
    ASSERT_THAT(objectsInRange.at(0), Eq(agent2));
    ASSERT_THAT(objectsInRange.at(1), Eq(agent1));
}

TEST(Locator_IntegrationTests, AgentOnStraightRoad_CalculatesCorrectLocateResult)
{
    TESTSCENERY_FACTORY tsf("MultipleRoadsIntegrationScenery.xodr");
    auto& world = tsf.world;
    const auto agent1 = AddAgentToWorld(world, 1, 399.0, 1.0, 2.0, 5.0);
    agent1->UpdateRoute(Route{ {{"1", true}, {"2", false}, {"3", true}}, {}, 1});
    const auto agent2 = AddAgentToWorld(world, 2, 2500.0, 2.0, 2.0, 5.0);
    agent2->UpdateRoute(Route{ {{"1", true}, {"2", false}, {"3", true}}, {}, 2});
    world.SyncGlobalData();

    ASSERT_THAT(agent1->GetRoadId(MeasurementPoint::Front), Eq("1"));
    ASSERT_THAT(agent1->GetMainLaneId(MeasurementPoint::Front), Eq(-2));
    ASSERT_THAT(agent1->GetDistanceToStartOfRoad(MeasurementPoint::Front, "1"), DoubleNear(401.5, 0.01));
    ASSERT_THAT(agent1->GetDistanceToStartOfRoad(MeasurementPoint::Rear, "1"), DoubleNear(396.5, 0.01));
    ASSERT_THAT(agent1->IsCrossingLanes(), Eq(true));
    ASSERT_THAT(agent1->GetLaneRemainder(Side::Left), DoubleNear(2.5, 0.01));
    ASSERT_THAT(agent1->GetLaneRemainder(Side::Right), DoubleNear(4.5, 0.01));
    ASSERT_THAT(agent2->GetRoadId(MeasurementPoint::Front), Eq("2"));
    ASSERT_THAT(agent2->GetMainLaneId(MeasurementPoint::Front), Eq(2));
    ASSERT_THAT(agent2->GetDistanceToStartOfRoad(MeasurementPoint::Front, "2"), DoubleNear(502.5, 0.01));
    ASSERT_THAT(agent2->GetDistanceToStartOfRoad(MeasurementPoint::Rear, "2"), DoubleNear(497.5, 0.01));
    ASSERT_THAT(agent2->IsCrossingLanes(), Eq(false));
    ASSERT_THAT(agent2->GetLaneRemainder(Side::Left), DoubleNear(0.5, 0.01));
    ASSERT_THAT(agent2->GetLaneRemainder(Side::Right), DoubleNear(1.5, 0.01));
}

TEST(Locator_IntegrationTests, AgentOnJunction_CalculatesCorrectLocateResult)
{
    TESTSCENERY_FACTORY tsf("TJunction.xodr");
    auto& world = tsf.world;
    AddAgentToWorld(world, 1, 208.0, -2.0, 2.0, 4.0);
    world.SyncGlobalData();

    const auto agent = world.GetAgent(1);
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
    TESTSCENERY_FACTORY tsf("IntegrationTestScenery.xodr");
    auto& world = tsf.world;

    Route route{"1"};
    auto laneMarkings = world.GetLaneMarkings(route, "1", -1, 0.0, 99.0, Side::Left);
    ASSERT_THAT(laneMarkings.size(), Eq(5));
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

    laneMarkings = world.GetLaneMarkings(route, "1", -1, 0.0, 99.0, Side::Right);
    ASSERT_THAT(laneMarkings.size(), Eq(5));
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

    laneMarkings = world.GetLaneMarkings(route, "1", -2, 11.0, 88.0, Side::Left);
    ASSERT_THAT(laneMarkings.size(), Eq(4));
    ASSERT_THAT(laneMarkings.at(0).relativeStartDistance, DoubleEq(-1.0));
    ASSERT_THAT(laneMarkings.at(0).type, Eq(LaneMarking::Type::Broken_Solid));
    ASSERT_THAT(laneMarkings.at(0).width, DoubleEq(0.3));
    ASSERT_THAT(laneMarkings.at(0).color, Eq(LaneMarking::Color::Red));
    ASSERT_THAT(laneMarkings.at(1).relativeStartDistance, DoubleEq(10.0));
    ASSERT_THAT(laneMarkings.at(1).type, Eq(LaneMarking::Type::Solid_Broken));
    ASSERT_THAT(laneMarkings.at(1).width, DoubleEq(0.3));
    ASSERT_THAT(laneMarkings.at(1).color, Eq(LaneMarking::Color::Blue));
    ASSERT_THAT(laneMarkings.at(2).relativeStartDistance, DoubleEq(19.0));

    laneMarkings = world.GetLaneMarkings(route, "1", -2, 11.0, 88.0, Side::Right);
    ASSERT_THAT(laneMarkings.size(), Eq(4));
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
    TESTSCENERY_FACTORY tsf("IntegrationTestScenery.xodr");
    auto& world = tsf.world;

    Route route{"1"};
    auto trafficSigns = world.GetTrafficSignsInRange(route, "1", -1, 5, 90);
    std::sort(trafficSigns.begin(), trafficSigns.end(),
              [](CommonTrafficSign::Entity first, CommonTrafficSign::Entity second){return first.relativeDistance < second.relativeDistance;});
    ASSERT_THAT(trafficSigns.size(), Eq(3));
    ASSERT_THAT(trafficSigns.at(0).relativeDistance, DoubleEq(10.0));
    ASSERT_THAT(trafficSigns.at(0).type, Eq(CommonTrafficSign::Type::MaximumSpeedLimit));
    ASSERT_THAT(trafficSigns.at(0).value, DoubleNear(50 / 3.6, 1e-3));
    ASSERT_THAT(trafficSigns.at(0).unit, Eq(CommonTrafficSign::Unit::MeterPerSecond));
    ASSERT_THAT(trafficSigns.at(0).supplementarySigns.size(), Eq(0));
    ASSERT_THAT(trafficSigns.at(1).relativeDistance, DoubleEq(30.0));
    ASSERT_THAT(trafficSigns.at(1).type, Eq(CommonTrafficSign::Type::SpeedLimitZoneBegin));
    ASSERT_THAT(trafficSigns.at(1).value, DoubleNear(30 / 3.6, 1e-3));
    ASSERT_THAT(trafficSigns.at(1).unit, Eq(CommonTrafficSign::Unit::MeterPerSecond));
    ASSERT_THAT(trafficSigns.at(1).supplementarySigns.size(), Eq(0));
    ASSERT_THAT(trafficSigns.at(2).relativeDistance, DoubleEq(31.0));
    ASSERT_THAT(trafficSigns.at(2).type, Eq(CommonTrafficSign::Type::AnnounceLeftLaneEnd));
    ASSERT_THAT(trafficSigns.at(2).value, Eq(2));
    ASSERT_THAT(trafficSigns.at(2).supplementarySigns.size(), Eq(0));

    trafficSigns = world.GetTrafficSignsInRange(route, "1", -2, 11, 90);
    std::sort(trafficSigns.begin(), trafficSigns.end(),
              [](CommonTrafficSign::Entity first, CommonTrafficSign::Entity second){return first.relativeDistance < second.relativeDistance;});
    ASSERT_THAT(trafficSigns.size(), Eq(5));
    ASSERT_THAT(trafficSigns.at(0).relativeDistance, DoubleEq(4.0));
    ASSERT_THAT(trafficSigns.at(0).type, Eq(CommonTrafficSign::Type::MaximumSpeedLimit));
    ASSERT_THAT(trafficSigns.at(0).value, DoubleNear(50 / 3.6, 1e-3));
    ASSERT_THAT(trafficSigns.at(0).unit, Eq(CommonTrafficSign::Unit::MeterPerSecond));
    ASSERT_THAT(trafficSigns.at(0).supplementarySigns.size(), Eq(0));
    ASSERT_THAT(trafficSigns.at(1).relativeDistance, DoubleEq(14.0));
    ASSERT_THAT(trafficSigns.at(1).type, Eq(CommonTrafficSign::Type::OvertakingBanBegin));
    ASSERT_THAT(trafficSigns.at(1).supplementarySigns.size(), Eq(0));
    ASSERT_THAT(trafficSigns.at(2).relativeDistance, DoubleEq(24.0));
    ASSERT_THAT(trafficSigns.at(2).type, Eq(CommonTrafficSign::Type::SpeedLimitZoneBegin));
    ASSERT_THAT(trafficSigns.at(2).value, DoubleNear(30 / 3.6, 1e-3));
    ASSERT_THAT(trafficSigns.at(2).unit, Eq(CommonTrafficSign::Unit::MeterPerSecond));
    ASSERT_THAT(trafficSigns.at(2).supplementarySigns.size(), Eq(0));
    ASSERT_THAT(trafficSigns.at(3).relativeDistance, DoubleEq(25.0));
    ASSERT_THAT(trafficSigns.at(3).type, Eq(CommonTrafficSign::Type::AnnounceLeftLaneEnd));
    ASSERT_THAT(trafficSigns.at(3).value, Eq(2));
    ASSERT_THAT(trafficSigns.at(3).unit, Eq(CommonTrafficSign::Unit::None));
    ASSERT_THAT(trafficSigns.at(3).supplementarySigns.size(), Eq(0));
    ASSERT_THAT(trafficSigns.at(4).relativeDistance, DoubleEq(29.0));
    ASSERT_THAT(trafficSigns.at(4).type, Eq(CommonTrafficSign::Type::Stop));
    ASSERT_THAT(trafficSigns.at(4).supplementarySigns.size(), Eq(1));
    ASSERT_THAT(trafficSigns.at(4).supplementarySigns.front().type, Eq(CommonTrafficSign::Type::DistanceIndication));
    ASSERT_THAT(trafficSigns.at(4).supplementarySigns.front().value, DoubleEq(200.0));
    ASSERT_THAT(trafficSigns.at(4).supplementarySigns.front().unit, Eq(CommonTrafficSign::Unit::Meter));
}

TEST(SceneryImporter_IntegrationTests, TJunction_ImportWithCorrectConnectionsAndPriorities)
{
    TESTSCENERY_FACTORY tsf("TJunction.xodr");
    auto& world = tsf.world;

    auto connections = world.GetConnectionsOnJunction("J0", "R1");
    ASSERT_THAT(connections.size(), Eq(2));
    ASSERT_THAT(connections.at(0).connectingRoadId, Eq("R1-2"));
    ASSERT_THAT(connections.at(0).outgoingRoadId, Eq("R2"));
    ASSERT_THAT(connections.at(0).outgoingStreamDirection, Eq(false));
    ASSERT_THAT(connections.at(1).connectingRoadId, Eq("R1-3"));
    ASSERT_THAT(connections.at(1).outgoingRoadId, Eq("R3"));
    ASSERT_THAT(connections.at(1).outgoingStreamDirection, Eq(false));

    connections = world.GetConnectionsOnJunction("J0", "R2");
    ASSERT_THAT(connections.size(), Eq(2));
    ASSERT_THAT(connections.at(0).connectingRoadId, Eq("R2-1"));
    ASSERT_THAT(connections.at(0).outgoingRoadId, Eq("R1"));
    ASSERT_THAT(connections.at(0).outgoingStreamDirection, Eq(false));
    ASSERT_THAT(connections.at(1).connectingRoadId, Eq("R2-3"));
    ASSERT_THAT(connections.at(1).outgoingRoadId, Eq("R3"));
    ASSERT_THAT(connections.at(1).outgoingStreamDirection, Eq(false));

    connections = world.GetConnectionsOnJunction("J0", "R3");
    ASSERT_THAT(connections.size(), Eq(2));
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
    TESTSCENERY_FACTORY tsf("SceneryLeftLaneEnds.xodr");
    auto& world = tsf.world;
    auto agent0 = AddAgentToWorld(world, 0, 10.0, 2.0);
    auto agent1 = AddAgentToWorld(world, 1, 100.0, 2.5, 2.0);
    world.SyncGlobalData();

    Route route{"1"};
    agent0->SetRoute(route);
    const auto obstruction = agent0->GetObstruction(*agent1);
    EXPECT_THAT(obstruction.left, DoubleEq(1.5));
    EXPECT_THAT(obstruction.right, DoubleEq(-0.5));
}

TEST(GetObstruction_IntegrationTests, AgentBehindJunction)
{
    TESTSCENERY_FACTORY tsf("TJunction.xodr");
    auto& world = tsf.world;
    auto agent0 = AddAgentToWorld(world, 0, 10.0, -3.0);
    auto agent1 = AddAgentToWorld(world, 1, 203.5, -10.0, 1.0, 3.0);
    world.SyncGlobalData();

    Route route{{{"R1", true}, {"R1-2", true}, {"R2", false}}, {}, 0};
    agent0->SetRoute(route);
    const auto obstruction = agent0->GetObstruction(*agent1);
    EXPECT_THAT(obstruction.left, DoubleNear(2.0, 1e-3));
    EXPECT_THAT(obstruction.right, DoubleNear(-1.0, 1e-3));
}
