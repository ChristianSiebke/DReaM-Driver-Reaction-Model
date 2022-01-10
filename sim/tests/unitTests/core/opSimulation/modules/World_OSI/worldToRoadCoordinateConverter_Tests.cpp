/********************************************************************************
 * Copyright (c) 2018-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <cmath>
#include "OWL/DataTypes.h"
#include "WorldToRoadCoordinateConverter.h"
#include "Generators/laneGeometryElementGenerator.h"

using ::testing::Eq;
using ::testing::DoubleNear;

using namespace World::Localization;

struct WorldToRoadCoordinateConverterRectangular_Data
{
    // do not change order of items
    // unless you also change it in INSTANTIATE_TEST_CASE_P
    // (see bottom of file)

    // data for generator
    Common::Vector2d origin;
    double width;
    double length;
    double hdg;

    // test point
    Common::Vector2d point;
    double pointHdg;

    // expected values
    OWL::Primitive::RoadCoordinate expected;

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const WorldToRoadCoordinateConverterRectangular_Data& obj)
    {
        return os
               << "origin " << obj.origin << ", "
               << "width " << obj.width << ", "
               << "length " << obj.length << ", "
               << "hdg " << obj.hdg << ", "
               << "point (" << obj.point.x << ", " << obj.point.y << "), "
               << "pointHdg " << obj.pointHdg << ", "
               << "expected (s " << obj.expected.s << ", t " << obj.expected.t << ", hdg " << obj.expected.yaw << ")";
    }
};

/// \see https://github.com/google/googletest/blob/master/googletest/docs/AdvancedGuide.md
class WorldToRoadCoordinateConverterRectangular_Test:
    public ::testing::Test,
    public ::testing::WithParamInterface<WorldToRoadCoordinateConverterRectangular_Data>
{
};

struct WorldToRoadCoordinateConverterCurved_Data
{
    // do not change order of items
    // unless you also change it in INSTANTIATE_TEST_CASE_P
    // (see bottom of file)

    // data for generator
    Common::Vector2d origin;
    double width;
    double length;
    double sDistance;
    double radius;

    // test point
    Common::Vector2d point;
    double pointHdg;

    // expected values
    OWL::Primitive::RoadCoordinate expected;

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const WorldToRoadCoordinateConverterCurved_Data& obj)
    {
        return os
               << "origin " << obj.origin << ", "
               << "width " << obj.width << ", "
               << "length " << obj.length << ", "
               << "sDistance " << obj.sDistance << ", "
               << "radius " << obj.radius << ", "
               << "point (" << obj.point.x << ", " << obj.point.y << "), "
               << "pointHdg " << obj.pointHdg << ", "
               << "expected (s " << obj.expected.s << ", t " << obj.expected.t << ", hdg " << obj.expected.yaw << ")";
    }
};

class WorldToRoadCoordinateConverterCurved_Test:
    public ::testing::Test,
    public ::testing::WithParamInterface<WorldToRoadCoordinateConverterCurved_Data>
{
};

/// \test Checks if a point is matched if it is inside the geometric element
/// \sa PointQuery
TEST(WorldToRoadCoordinateConverter_Test, Point_IsOnlyMatchedIfInsideElement)
{
    Common::Vector2d origin{-5, 0};
    double width = 5;
    double length = 10;
    double hdg = 0;

    auto laneGeometryElement = OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement(
                                   origin, width, length, hdg);
    LocalizationElement localizationElement{laneGeometryElement};
    WorldToRoadCoordinateConverter converter(localizationElement);

    Common::Vector2d pointOutsideElement{1000, 1000};
    ASSERT_THAT(converter.IsConvertible(pointOutsideElement), Eq(false));

    Common::Vector2d pointWithinElement{0.1, -0.1};
    ASSERT_THAT(converter.IsConvertible(pointWithinElement), Eq(true));
}

/// \test Checks if a positive t coordinate is calculated, even if the first points of the
///       geometric element fall together, and so the projection vector for t collapses
TEST(WorldToRoadCoordinateConverter_Test, CurrentPointTrippleIsSingular_CalculatesPositiveTCoordinate)
{
    Common::Vector2d origin{-5, 0};
    double width = 5;
    double length = 10;
    double hdg = 0;

    auto laneGeometryElement = OWL::Testing::LaneGeometryElementGenerator::TriangularLaneGeometryElement(
                                   origin, width, length, hdg);
    LocalizationElement localizationElement{laneGeometryElement};
    WorldToRoadCoordinateConverter converter(localizationElement);

    Common::Vector2d pointWithinElement{0.0, 2.0};
    auto roadCoordinate = converter.GetRoadCoordinate(pointWithinElement, 0.0);

    EXPECT_THAT(roadCoordinate.s,   DoubleNear(5, 1e-2));
    EXPECT_THAT(roadCoordinate.t,   DoubleNear(2, 1e-2));
    EXPECT_THAT(roadCoordinate.hdg, DoubleNear(0, 1e-2));
}

/// \test Checks if a negative t coordinate is calculated, even if the first points of the
///       geometric element fall together, and so the projection vector for t collapses
TEST(WorldToRoadCoordinateConverter_Test, CurrentPointTrippleIsSingular_CalculatesNegativeTCoordinate)
{
    Common::Vector2d origin{-5, 0};
    double width = 5;
    double length = 10;
    double hdg = 0;

    auto laneGeometryElement = OWL::Testing::LaneGeometryElementGenerator::TriangularLaneGeometryElement(
                                   origin, width, length, hdg);
    LocalizationElement localizationElement{laneGeometryElement};
    WorldToRoadCoordinateConverter converter(localizationElement);

    Common::Vector2d pointWithinElement{0.0, -2.0};
    auto roadCoordinate = converter.GetRoadCoordinate(pointWithinElement, 0.0);

    EXPECT_THAT(roadCoordinate.s,   DoubleNear(5, 1e-2));
    EXPECT_THAT(roadCoordinate.t,   DoubleNear(-2, 1e-2));
    EXPECT_THAT(roadCoordinate.hdg, DoubleNear(0, 1e-2));
}

/// \test Checks if the road coordinates are calculated right
///
TEST_P(WorldToRoadCoordinateConverterRectangular_Test, PointWithinElement_ReturnsExpectedRoadCoordinates)
{
    auto data = GetParam();
    auto laneGeometryElement =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement(
            data.origin, data.width, data.length, data.hdg);
    LocalizationElement localizationElement{laneGeometryElement};
    WorldToRoadCoordinateConverter converter(localizationElement);
    auto roadCoordinate = converter.GetRoadCoordinate(data.point, data.pointHdg);

    EXPECT_THAT(roadCoordinate.s,   DoubleNear(data.expected.s,   1e-2));
    EXPECT_THAT(roadCoordinate.t,   DoubleNear(data.expected.t,   1e-2));
    EXPECT_THAT(roadCoordinate.hdg, DoubleNear(data.expected.yaw, 1e-2));
}


INSTANTIATE_TEST_CASE_P(sCoordinateTestSet, WorldToRoadCoordinateConverterRectangular_Test,
                        testing::Values(
                                     /* origin width length hdg          point     pointHdg          expected      */
                                      /*x y    /      /     /|         x       y      |         s       t      yaw */
WorldToRoadCoordinateConverterRectangular_Data{{ -5,   0},   5,   10,   0.00, {  1.00,   0.00},  0.00, {   6.00,   0.00,   0.00}},
WorldToRoadCoordinateConverterRectangular_Data{{ -4,   0},   5,   10,   0.00, {  1.00,   0.00},  0.00, {   5.00,   0.00,   0.00}},
WorldToRoadCoordinateConverterRectangular_Data{{ -5,   0},   5,   10,   0.00, { -1.00,   0.00},  0.00, {   4.00,   0.00,   0.00}},
// 180°
WorldToRoadCoordinateConverterRectangular_Data{{  5,   0},   5,   10,   M_PI, { -1.00,   0.00},  M_PI, {   6.00,   0.00,   0.00}},
WorldToRoadCoordinateConverterRectangular_Data{{  4,   0},   5,   10,   M_PI, { -1.00,   0.00},  M_PI, {   5.00,   0.00,   0.00}},
WorldToRoadCoordinateConverterRectangular_Data{{  5,   0},   5,   10,   M_PI, {  1.00,   0.00},  M_PI, {   4.00,   0.00,   0.00}},
// +/-45°
WorldToRoadCoordinateConverterRectangular_Data{{  0,   0},   5,   10,  M_PI_4, { 1.41,   1.41},  M_PI_4, { 2.00,   0.00,   0.00}},
WorldToRoadCoordinateConverterRectangular_Data{{  0,   0},   5,   10, -M_PI_4, { 1.41,  -1.41}, -M_PI_4, { 2.00,   0.00,   0.00}}
                        ));

INSTANTIATE_TEST_CASE_P(tCoordinateTestSet, WorldToRoadCoordinateConverterRectangular_Test,
                        testing::Values(
                            /*         origin    width length hdg          point     pointHdg          expected      */
                            /*        x      y     |     |     |         x       y      |         s       t      yaw */
WorldToRoadCoordinateConverterRectangular_Data{{   -5,     0},   5,   10,   0.00, {  0.00,   1.00},  0.00, {   5.00,   1.00,   0.00}},
WorldToRoadCoordinateConverterRectangular_Data{{   -5,     0},   5,   10,   0.00, {  0.00,   0.00},  0.00, {   5.00,   0.00,   0.00}},
WorldToRoadCoordinateConverterRectangular_Data{{   -5,     0},   5,   10,   0.00, {  0.00,  -1.00},  0.00, {   5.00,  -1.00,   0.00}},
// 180°
WorldToRoadCoordinateConverterRectangular_Data{{    5,     0},   5,   10,   M_PI, {  0.00,  -1.00},  M_PI, {   5.00,   1.00,   0.00}},
WorldToRoadCoordinateConverterRectangular_Data{{    5,     0},   5,   10,   M_PI, {  0.00,   0.00},  M_PI, {   5.00,   0.00,   0.00}},
WorldToRoadCoordinateConverterRectangular_Data{{    5,     0},   5,   10,   M_PI, {  0.00,   1.00},  M_PI, {   5.00,  -1.00,   0.00}},
// +/-45°
WorldToRoadCoordinateConverterRectangular_Data{{-1.41, -1.41},   5,   10,  M_PI_4, { -1.41,  1.41}, M_PI_4, {  2.00,   2.00,   0.00}},
WorldToRoadCoordinateConverterRectangular_Data{{-1.41, -1.41},   5,   10,  M_PI_4, {  1.41, -1.41}, M_PI_4, {  2.00,  -2.00,   0.00}},
WorldToRoadCoordinateConverterRectangular_Data{{-1.41,  1.41},   5,   10, -M_PI_4, {  1.41,  1.41}, -M_PI_4, {  2.00,   2.00,   0.00}},
WorldToRoadCoordinateConverterRectangular_Data{{-1.41,  1.41},   5,   10, -M_PI_4, { -1.41, -1.41}, -M_PI_4, {  2.00,  -2.00,   0.00}}
                        ));


INSTANTIATE_TEST_CASE_P(yawTestSet, WorldToRoadCoordinateConverterRectangular_Test,
                        testing::Values(
/*                                   origin   width length hdg          point     pointHdg          expected      */
/*                                   x    y     |     |     |         x       y      |         s       t      yaw */
WorldToRoadCoordinateConverterRectangular_Data{{ -5,   0},   5,   10,   0.00, {  0.00,   0.00},  0.10, {   5.00,   0.00,   0.10}},
WorldToRoadCoordinateConverterRectangular_Data{{ -5,   0},   5,   10,   0.00, {  0.00,   0.00}, -0.10, {   5.00,   0.00,  -0.10}},
// 180°
WorldToRoadCoordinateConverterRectangular_Data{{  5,   0},   5,   10,   M_PI, {  0.00,   0.00}, M_PI + .1, { 5.00,   0.00,   0.10}},
WorldToRoadCoordinateConverterRectangular_Data{{  5,   0},   5,   10,   M_PI, {  0.00,   0.00}, M_PI - .1, { 5.00,   0.00,  -0.10}}
                        ));

INSTANTIATE_TEST_CASE_P(yawWithinPiTestSet, WorldToRoadCoordinateConverterRectangular_Test,
                        testing::Values(
/*                                   origin   width length hdg         point     pointHdg              expected      */
/*                                   x    y     |     |     |        x       y      |             s       t      yaw */
WorldToRoadCoordinateConverterRectangular_Data{{ -5,   0},   5,   10,  0.00, {  0.00,   0.00},  M_PI + 1.0, {   5.00,   0.00,  -M_PI + 1.00}},
WorldToRoadCoordinateConverterRectangular_Data{{ -5,   0},   5,   10,  0.00, {  0.00,   0.00}, -M_PI - 1.0, {   5.00,   0.00,  M_PI - 1.00}}
                        ));

TEST_P(WorldToRoadCoordinateConverterCurved_Test, PointWithinElement_ReturnsExpectedRoadCoordinates)
{
    auto data = GetParam();
    auto laneGeometryElement =
        OWL::Testing::LaneGeometryElementGenerator::CurvedLaneGeometryElement(
            data.origin, data.width, data.length, data.sDistance, data.radius);
    LocalizationElement localizationElement{laneGeometryElement};
    WorldToRoadCoordinateConverter converter(localizationElement);
    auto roadCoordinate = converter.GetRoadCoordinate(data.point, data.pointHdg);

    EXPECT_THAT(roadCoordinate.s,   DoubleNear(data.expected.s,   1e-2));
    EXPECT_THAT(roadCoordinate.t,   DoubleNear(data.expected.t,   1e-2));
    EXPECT_THAT(roadCoordinate.hdg, DoubleNear(data.expected.yaw, 1e-2));
}

INSTANTIATE_TEST_CASE_P(curvedTestSet, WorldToRoadCoordinateConverterCurved_Test,
                        testing::Values(
/*                           origin  width length sDistance radius    point     pointHdg   expected    */
/*                             x   y    |    |      |       |      x       y       |     s     t   yaw */
WorldToRoadCoordinateConverterCurved_Data{{ -5, 5},  4,   3,     3,  20.0, {  -5.0,       5.0}, 0.0, { 0.0, 0.0, 0.0}},
WorldToRoadCoordinateConverterCurved_Data{{ -5, 5},  4,   3,     3,  20.0, {  -2.0112,  5.225}, 0.0, { 3.0, 0.0, 0.0}},
WorldToRoadCoordinateConverterCurved_Data{{ -5, 5},  4,   3,     5,  20.0, {  -2.0112,  5.225}, 0.0, { 5.0, 0.0, 0.0}},
WorldToRoadCoordinateConverterCurved_Data{{ -5, 5},  4,   3,     3,  20.0, {  -5,         3.0}, 0.0, { 0.0, -2.0, 0.0}},
WorldToRoadCoordinateConverterCurved_Data{{ -5, 5},  4,   3,     5,  20.0, {  -2.3101, 7.2021}, 0.0, { 5.0, 2.0, 0.0}},
WorldToRoadCoordinateConverterCurved_Data{{ -5, 5},  4,   3,     5,  20.0, {  -3.6551, 7.1011}, 0.0, { 2.5, 2.0, 0.0}},
WorldToRoadCoordinateConverterCurved_Data{{ -5, 5},  4,   3,     5,   8.0, {  -2.0698, 5.5559}, 0.0, { 5.0, 0.0, 0.0}}
                        ));
