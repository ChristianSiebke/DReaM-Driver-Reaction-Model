/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "Localization.h"
#include "fakeWorldData.h"
#include "fakeLane.h"
#include "fakeRoad.h"
#include "OWL/fakes/fakeMovingObject.h"
#include "Generators/laneGeometryElementGenerator.h"

using ::testing::_;
using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::SizeIs;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::IsEmpty;
using ::testing::UnorderedElementsAreArray;
using ::testing::ElementsAre;

std::vector<Common::Vector2d> GenerateRectangularPolygon(double x_min, double x_max, double y_min, double y_max)
{
    std::vector<Common::Vector2d> points;
    points.emplace_back(x_min, y_max);
    points.emplace_back(x_max, y_max);
    points.emplace_back(x_max, y_min);
    points.emplace_back(x_min, y_min);
    return points;
}

class LocateOnGeometryElement : public ::testing::Test
{
public:
    LocateOnGeometryElement()
    {
        ON_CALL(lane, GetId()).WillByDefault(Return(idLane));
        ON_CALL(lane, GetWidth(_)).WillByDefault(Return(4));
        ON_CALL(road, GetId()).WillByDefault(ReturnRef(idRoad));
        ON_CALL(lane, GetRoad()).WillByDefault(ReturnRef(road));
        ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneIdMapping));
    }

    OWL::Fakes::WorldData worldData;
    OWL::Fakes::Lane lane;
    OWL::Id idLane{1};
    std::string idRoad{"Road"};
    OWL::Fakes::Road road;
    const std::unordered_map<OWL::Id, OWL::OdId> laneIdMapping {{idLane, -1}};
    OWL::Primitive::LaneGeometryElement laneGeometryElement{OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement({0.0,0.0}, 4.0, 4.0, 0.0, &lane)};
    World::Localization::LocalizationElement localizationElement{laneGeometryElement};
    double yaw{0.0};
};

TEST_F(LocateOnGeometryElement, ObjectOutsideElement_DoesNotLocateObject)
{
    World::Localization::LocatedObject locatedObject;
    auto agentBoundary = GenerateRectangularPolygon(-1, 0, -1, 1);
    Common::Vector2d referencePoint{-0.5,0};
    Common::Vector2d mainLaneLocator{-0.5,1};

    const auto locateOnGeometryElement = World::Localization::LocateOnGeometryElement(worldData, agentBoundary, referencePoint, mainLaneLocator, yaw, locatedObject);
    locateOnGeometryElement(std::make_pair(CoarseBoundingBox{}, &localizationElement));

    EXPECT_THAT(locatedObject.referencePoint, IsEmpty());
    EXPECT_THAT(locatedObject.mainLaneLocator, IsEmpty());
    EXPECT_THAT(locatedObject.laneOverlaps, IsEmpty());
}

TEST_F(LocateOnGeometryElement, ObjectPartiallyInsideGeometryElement_CorrectlyLocatesObjectOnElement)
{
    World::Localization::LocatedObject locatedObject;
    auto agentBoundary = GenerateRectangularPolygon(-1.0, 2.1, 1.0, 3.0);
    Common::Vector2d referencePoint{-0.5,2.0};
    Common::Vector2d mainLaneLocator{2.1,2.0};

    const auto locateOnGeometryElement = World::Localization::LocateOnGeometryElement(worldData, agentBoundary, referencePoint, mainLaneLocator, yaw, locatedObject);
    locateOnGeometryElement(std::make_pair(CoarseBoundingBox{}, &localizationElement));

    EXPECT_THAT(locatedObject.referencePoint, IsEmpty());
    ASSERT_THAT(locatedObject.mainLaneLocator, SizeIs(1));
    EXPECT_THAT(locatedObject.mainLaneLocator.at("Road").roadId, Eq("Road"));
    EXPECT_THAT(locatedObject.mainLaneLocator.at("Road").laneId, Eq(-1));
    EXPECT_THAT(locatedObject.mainLaneLocator.at("Road").roadPosition.s, DoubleEq(2.1));
    EXPECT_THAT(locatedObject.mainLaneLocator.at("Road").roadPosition.t, DoubleEq(2));
    ASSERT_THAT(locatedObject.laneOverlaps, SizeIs(1));
    EXPECT_THAT(locatedObject.laneOverlaps.at(&lane).s_min, DoubleEq(0.0));
    EXPECT_THAT(locatedObject.laneOverlaps.at(&lane).s_max, DoubleEq(2.1));
    EXPECT_THAT(locatedObject.laneOverlaps.at(&lane).min_delta_left, DoubleEq(0.0));
    EXPECT_THAT(locatedObject.laneOverlaps.at(&lane).min_delta_right, DoubleEq(3.0));
}

TEST_F(LocateOnGeometryElement, ObjectInsideGeometryElement_CorrectlyLocatesObjectOnElement)
{
    World::Localization::LocatedObject locatedObject;
    auto agentBoundary = GenerateRectangularPolygon(1.0, 2.1, -1.0, 1.0);
    Common::Vector2d referencePoint{1.5,0.0};
    Common::Vector2d mainLaneLocator{2.1,0.0};

    const auto locateOnGeometryElement = World::Localization::LocateOnGeometryElement(worldData, agentBoundary, referencePoint, mainLaneLocator, yaw, locatedObject);
    locateOnGeometryElement(std::make_pair(CoarseBoundingBox{}, &localizationElement));

    ASSERT_THAT(locatedObject.referencePoint, SizeIs(1));
    EXPECT_THAT(locatedObject.referencePoint.at("Road").roadId, Eq("Road"));
    EXPECT_THAT(locatedObject.referencePoint.at("Road").laneId, Eq(-1));
    EXPECT_THAT(locatedObject.referencePoint.at("Road").roadPosition.s, DoubleEq(1.5));
    EXPECT_THAT(locatedObject.referencePoint.at("Road").roadPosition.t, DoubleEq(0.0));
    ASSERT_THAT(locatedObject.mainLaneLocator, SizeIs(1));
    EXPECT_THAT(locatedObject.mainLaneLocator.at("Road").roadId, Eq("Road"));
    EXPECT_THAT(locatedObject.mainLaneLocator.at("Road").laneId, Eq(-1));
    EXPECT_THAT(locatedObject.mainLaneLocator.at("Road").roadPosition.s, DoubleEq(2.1));
    EXPECT_THAT(locatedObject.mainLaneLocator.at("Road").roadPosition.t, DoubleEq(0.0));
    ASSERT_THAT(locatedObject.laneOverlaps, SizeIs(1));
    EXPECT_THAT(locatedObject.laneOverlaps.at(&lane).s_min, DoubleEq(1.0));
    EXPECT_THAT(locatedObject.laneOverlaps.at(&lane).s_max, DoubleEq(2.1));
    EXPECT_THAT(locatedObject.laneOverlaps.at(&lane).min_delta_left, DoubleEq(1.0));
    EXPECT_THAT(locatedObject.laneOverlaps.at(&lane).min_delta_right, DoubleEq(1.0));
}

struct GetIntersectionPoints_Data
{
    std::vector<Common::Vector2d> element;
    std::vector<Common::Vector2d> object;
    std::vector<Common::Vector2d> expectedIntersection;
};

class GetIntersectionPoints_Test : public testing::Test,
        public ::testing::WithParamInterface<GetIntersectionPoints_Data>
{
};

TEST_P(GetIntersectionPoints_Test, CorrectIntersectionPoints)
{
    auto data = GetParam();
    const auto result = World::Localization::GetIntersectionPoints(data.element, data.object);
    EXPECT_THAT(result, UnorderedElementsAreArray(data.expectedIntersection));
}

INSTANTIATE_TEST_CASE_P(ElementAndObjectRectangular, GetIntersectionPoints_Test,
                        testing::Values(
                         //Element corners                  Object corners                       Intersection points
GetIntersectionPoints_Data{{{1,1},{1,3},{3,3},{3,1}},       {{2,4},{2,6},{4,6},{4,4}},           {}},
GetIntersectionPoints_Data{{{1,1},{1,3},{3,3},{3,1}},       {{2,2},{2,6},{4,6},{4,2}},           {{2,2},{2,3},{3,2},{3,3}}},
GetIntersectionPoints_Data{{{1,1},{1,3},{3,3},{3,1}},       {{0,0},{0,4},{4,4},{4,0}},           {{1,1},{1,3},{3,3},{3,1}}},
GetIntersectionPoints_Data{{{1,1},{1,3},{3,3},{3,1}},       {{2,2},{2,2.5},{2.5,2.5},{2.5,2}},   {{2,2},{2,2.5},{2.5,2.5},{2.5,2}}},
GetIntersectionPoints_Data{{{1,1},{1,3},{3,3},{3,1}},       {{1,4},{3,6},{6,3},{4,1}},           {{3,2},{2,3},{3,3}}},
GetIntersectionPoints_Data{{{-1,0},{0,1},{1,0},{0,-1}},     {{0,0},{1,1},{2,0},{1,-1}},          {{0,0},{0.5,0.5},{1,0},{0.5,-0.5}}}));

INSTANTIATE_TEST_CASE_P(ElementNotRectangular, GetIntersectionPoints_Test,
                        testing::Values(
                         //Element corners                  Object corners                       Intersection points
GetIntersectionPoints_Data{{{1,1},{1,2},{3,4},{3,1}},       {{2,5},{2,6},{4,6},{4,5}},           {}},
GetIntersectionPoints_Data{{{1,1},{1,2},{3,4},{3,1}},       {{2,2},{2,6},{4,6},{4,2}},           {{2,2},{2,3},{3,2},{3,4}}},
GetIntersectionPoints_Data{{{1,1},{1,2},{3,4},{3,1}},       {{0,0},{0,5},{4,5},{4,0}},           {{1,1},{1,2},{3,4},{3,1}}},
GetIntersectionPoints_Data{{{1,1},{1,2},{3,4},{3,1}},       {{2,2},{2,2.5},{2.5,2.5},{2.5,2}},   {{2,2},{2,2.5},{2.5,2.5},{2.5,2}}},
GetIntersectionPoints_Data{{{1,1},{1,2},{3,4},{3,1}},       {{-2,0},{-2,2},{0,2},{0,0}},         {}}));


class LocateTest : public ::testing::Test
{
public:
    LocateTest()
    {
        ON_CALL(lane1, GetId()).WillByDefault(Return(idLane1));
        ON_CALL(lane1, GetWidth(_)).WillByDefault(Return(4));
        ON_CALL(lane1, GetRoad()).WillByDefault(ReturnRef(road1));
        ON_CALL(lane1, GetLaneGeometryElements()).WillByDefault(ReturnRef(elements1));
        ON_CALL(lane2, GetId()).WillByDefault(Return(idLane2));
        ON_CALL(lane2, GetWidth(_)).WillByDefault(Return(4));
        ON_CALL(lane2, GetRoad()).WillByDefault(ReturnRef(road1));
        ON_CALL(lane2, GetLaneGeometryElements()).WillByDefault(ReturnRef(elements2));
        ON_CALL(lane3, GetId()).WillByDefault(Return(idLane3));
        ON_CALL(lane3, GetWidth(_)).WillByDefault(Return(4));
        ON_CALL(lane3, GetRoad()).WillByDefault(ReturnRef(road2));
        ON_CALL(lane3, GetLaneGeometryElements()).WillByDefault(ReturnRef(elements3));
        ON_CALL(road1, GetId()).WillByDefault(ReturnRef(idRoad1));
        ON_CALL(road2, GetId()).WillByDefault(ReturnRef(idRoad2));
        ON_CALL(worldData, GetLanes()).WillByDefault(ReturnRef(lanes));
        ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneIdMapping));
        localizer.Init();
    }

    OWL::Fakes::WorldData worldData;
    OWL::Fakes::Lane lane1, lane2, lane3;
    OWL::Id idLane1{1}, idLane2{2}, idLane3{3};
    std::unordered_map<OWL::Id, OWL::Interfaces::Lane*> lanes{{idLane1, &lane1}, {idLane2, &lane2}, {idLane3, &lane3}};
    OWL::Fakes::Road road1, road2;
    std::string idRoad1{"Road1"}, idRoad2{"Road2"};
    const std::unordered_map<OWL::Id, OWL::OdId> laneIdMapping {{idLane1, -1},{idLane2, -2},{idLane3, -3}};
    OWL::Primitive::LaneGeometryElement laneGeometryElement1{OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement({0.0,0.0}, 4.0, 4.0, 0.0, &lane1)};
    OWL::Interfaces::LaneGeometryElements elements1{&laneGeometryElement1};
    OWL::Primitive::LaneGeometryElement laneGeometryElement2{OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement({0.0,-4.0}, 4.0, 4.0, 0.0, &lane2)};
    OWL::Interfaces::LaneGeometryElements elements2{&laneGeometryElement2};
    OWL::Primitive::LaneGeometryElement laneGeometryElement3{OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement({2.0,-2.0}, 4.0, 4.0, M_PI_2, &lane3)};
    OWL::Interfaces::LaneGeometryElements elements3{&laneGeometryElement3};
    World::Localization::Localizer localizer{worldData};
};

TEST_F(LocateTest, WorldObject_OnRoute)
{
    OWL::Fakes::MovingObject object;
    OWL::Primitive::AbsPosition referencePoint;
    referencePoint.x = 2;
    referencePoint.y = 2;
    OWL::Primitive::AbsOrientation orientation;
    orientation.yaw = 0;
    ON_CALL(object, GetReferencePointPosition()).WillByDefault(Return(referencePoint));
    ON_CALL(object, GetAbsOrientation()).WillByDefault(Return(orientation));
    ON_CALL(object, GetDistanceReferencePointToLeadingEdge()).WillByDefault(Return(1));
    polygon_t boundingBox{{{1,1},{1,3},{3,3},{3,1}}};
    const auto result = localizer.Locate(boundingBox, object);

    ASSERT_THAT(result.isOnRoute, Eq(true));
    ASSERT_THAT(result.position.touchedRoads.size(), Eq(2));
    ASSERT_THAT(result.position.referencePoint.size(), Eq(2));
    ASSERT_THAT(result.position.mainLocatePoint.size(), Eq(2));
}

TEST_F(LocateTest, WorldObject_OffRoute)
{
    OWL::Fakes::MovingObject object;
    OWL::Primitive::AbsPosition referencePoint;
    referencePoint.x = 3.5;
    referencePoint.y = 2;
    OWL::Primitive::AbsOrientation orientation;
    orientation.yaw = 0;
    ON_CALL(object, GetReferencePointPosition()).WillByDefault(Return(referencePoint));
    ON_CALL(object, GetAbsOrientation()).WillByDefault(Return(orientation));
    ON_CALL(object, GetDistanceReferencePointToLeadingEdge()).WillByDefault(Return(1));
    polygon_t boundingBox{{{2.5,1},{2.5,3},{4.5,3},{4.5,1}}};
    const auto result = localizer.Locate(boundingBox, object);

    ASSERT_THAT(result.isOnRoute, Eq(false));
    ASSERT_THAT(result.position.touchedRoads.size(), Eq(2));
    ASSERT_THAT(result.position.referencePoint.size(), Eq(2));
    ASSERT_THAT(result.position.mainLocatePoint.size(), Eq(0));
}

bool operator== (const GlobalRoadPosition& lhs, const GlobalRoadPosition& rhs)
{
    return lhs.roadId == rhs.roadId
            && lhs.laneId == rhs.laneId
            && lhs.roadPosition == rhs.roadPosition;
}

TEST_F(LocateTest, Point_TwoRoads)
{
    const auto result = localizer.Locate({2,0},0);

    ASSERT_THAT(result, ElementsAre(std::make_pair(idRoad1, GlobalRoadPosition{idRoad1, -1, 2, 0, 0}),
                                    std::make_pair(idRoad2, GlobalRoadPosition{idRoad2, -3, 2, 0, -M_PI_2})));
}

TEST_F(LocateTest, Point_OneRoad)
{
    const auto result = localizer.Locate({2,-4},0);

    ASSERT_THAT(result, ElementsAre(std::make_pair(idRoad1, GlobalRoadPosition{idRoad1, -2, 2, 0, 0})));
}
