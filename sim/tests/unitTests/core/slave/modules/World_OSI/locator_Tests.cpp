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

#include "Localization.h"
#include "fakeWorldData.h"
#include "fakeLane.h"
#include "fakeRoad.h"
#include "Generators/laneGeometryElementGenerator.h"

using ::testing::_;
using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::SizeIs;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::IsEmpty;
using ::testing::UnorderedElementsAreArray;

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
