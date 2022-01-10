/********************************************************************************
 * Copyright (c) 2020-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "common/commonTools.h"
#include "fakeWorld.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::TestWithParam;
using ::testing::Eq;
using ::testing::Values;
using ::testing::Return;
using ::testing::DoubleNear;
using ::testing::_;
using ::testing::UnorderedElementsAreArray;

struct CartesianNetDistance_Data
{
    polygon_t ownBoundingBox;
    polygon_t otherBoundingBox;
    double expectedNetX;
    double expectedNetY;
};

class CartesianNetDistanceTest: public ::TestWithParam<CartesianNetDistance_Data>
{
};

TEST_P(CartesianNetDistanceTest, GetCartesianNetDistance_ReturnsCorrectDistances)
{
    auto data = GetParam();
    auto [netX, netY] = CommonHelper::GetCartesianNetDistance(data.ownBoundingBox, data.otherBoundingBox);

    ASSERT_THAT(netX, DoubleNear(data.expectedNetX, 1e-3));
    ASSERT_THAT(netY, DoubleNear(data.expectedNetY, 1e-3));
}

INSTANTIATE_TEST_SUITE_P(CartesianNetDistanceTestCase, CartesianNetDistanceTest, ::testing::Values(
//                             ownBoundingBox                          otherBoundingBox                                 x y
    CartesianNetDistance_Data{polygon_t{{{0,0},{1,0},{1,1},{0,1}}},  polygon_t{{{1,0},{2,0},{2,1},{1,1}}},              0,0},
    CartesianNetDistance_Data{polygon_t{{{-1,0},{0,-1},{1,0},{0,1}}},polygon_t{{{1,1},{2,1},{2,2},{1,2}}},              0,0},
    CartesianNetDistance_Data{polygon_t{{{-1,0},{0,-1},{1,0},{0,1}}},polygon_t{{{3,4},{4,4},{4,6},{3,6}}},              2,3},
    CartesianNetDistance_Data{polygon_t{{{-1,0},{0,-1},{1,0},{0,1}}},polygon_t{{{-10,-10},{-8,-10},{-8,-9},{-10,-9}}},-7,-8}
));

struct GetRoadWithLowestHeading_Data
{
    const GlobalRoadPositions roadPositions;
    RouteElement expectedResult;
};

class GetRoadWithLowestHeading_Test : public ::TestWithParam<GetRoadWithLowestHeading_Data>
{

};

TEST_P(GetRoadWithLowestHeading_Test, GetRoadWithLowestHeading)
{
    auto data = GetParam();
    FakeWorld world;
    ON_CALL(world, IsDirectionalRoadExisting(_,_)).WillByDefault(Return(true));
    auto result = CommonHelper::GetRoadWithLowestHeading(data.roadPositions, world);

    ASSERT_THAT(result, Eq(data.expectedResult));
}

INSTANTIATE_TEST_SUITE_P(GetRoadWithLowestHeadingTestCase, GetRoadWithLowestHeading_Test, ::testing::Values(
//                                  roadPositions                                                        expectedResult
 GetRoadWithLowestHeading_Data{{{"RoadA",{"RoadA",-1,0,0,0.1}}},                                        {"RoadA", true}},
 GetRoadWithLowestHeading_Data{{{"RoadA",{"RoadA",-1,0,0,0.1+M_PI}}},                                   {"RoadA", false}},
 GetRoadWithLowestHeading_Data{{{"RoadA",{"RoadA",-1,0,0,-0.1}},{"RoadB",{"RoadB",-1,0,0,0.2}}},        {"RoadA", true}},
 GetRoadWithLowestHeading_Data{{{"RoadA",{"RoadA",-1,0,0,-0.2}},{"RoadB",{"RoadB",-1,0,0,0.1}}},        {"RoadB", true}},
 GetRoadWithLowestHeading_Data{{{"RoadA",{"RoadA",-1,0,0,-0.1+M_PI}},{"RoadB",{"RoadB",-1,0,0,0.2}}},   {"RoadA", false}},
 GetRoadWithLowestHeading_Data{{{"RoadA",{"RoadA",-1,0,0,-0.2}},{"RoadB",{"RoadB",-1,0,0,M_PI+0.1}}},   {"RoadB", false}}
));

class SetAngleToValidRange_Data
{
public:
    // do not change order of items
    // unless you also change it in INSTANTIATE_TEST_CASE_P
    double angle;
    double normalizedAngle;

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const SetAngleToValidRange_Data& data)
    {
        return os << "angle: " << data.angle << ", normalizedAngle: " << data.normalizedAngle;
    }
};

class SetAngleToValidRange : public ::testing::TestWithParam<SetAngleToValidRange_Data>
{
};

TEST_P(SetAngleToValidRange, ReturnsAngleWithinPlusMinusPi)
{
    auto data = GetParam();

    double normalizedAngle = CommonHelper::SetAngleToValidRange(data.angle);

    ASSERT_THAT(normalizedAngle, DoubleNear(data.normalizedAngle, 1e-3));
}

INSTANTIATE_TEST_SUITE_P(AngleList, SetAngleToValidRange,
  /*                             angle   expectedNormalizedAngle  */
  testing::Values(
    SetAngleToValidRange_Data{                  0.0,               0.0 },
    SetAngleToValidRange_Data{               M_PI_4,            M_PI_4 },
    SetAngleToValidRange_Data{              -M_PI_4,           -M_PI_4 },
    SetAngleToValidRange_Data{               M_PI_2,            M_PI_2 },
    SetAngleToValidRange_Data{              -M_PI_2,           -M_PI_2 },
    SetAngleToValidRange_Data{         3.0 * M_PI_4,      3.0 * M_PI_4 },
    SetAngleToValidRange_Data{        -3.0 * M_PI_4,     -3.0 * M_PI_4 },
    SetAngleToValidRange_Data{  2.0 * M_PI + M_PI_4,            M_PI_4 },
    SetAngleToValidRange_Data{ -2.0 * M_PI - M_PI_4,           -M_PI_4 },
    SetAngleToValidRange_Data{  2.0 * M_PI + M_PI_2,            M_PI_2 },
    SetAngleToValidRange_Data{ -2.0 * M_PI - M_PI_2,           -M_PI_2 }
  )
);

struct GetIntersectionPoints_Data
{
    std::vector<Common::Vector2d> firstQuadrangle;
    std::vector<Common::Vector2d> secondQuadrangle;
    bool firstIsRectangular;
    std::vector<Common::Vector2d> expectedIntersection;
};

class GetIntersectionPoints_Test : public testing::Test,
        public ::testing::WithParamInterface<GetIntersectionPoints_Data>
{
};

TEST_P(GetIntersectionPoints_Test, CorrectIntersectionPoints)
{
    auto data = GetParam();
    const auto result = CommonHelper::IntersectionCalculation::GetIntersectionPoints(data.firstQuadrangle, data.secondQuadrangle, data.firstIsRectangular);
    EXPECT_THAT(result, UnorderedElementsAreArray(data.expectedIntersection));
}

INSTANTIATE_TEST_CASE_P(BothRectangular, GetIntersectionPoints_Test,
                        testing::Values(
                         //Element corners                  Object corners                             Intersection points
GetIntersectionPoints_Data{{{1,1},{1,3},{3,3},{3,1}},       {{2,4},{2,6},{4,6},{4,4}},           true, {}},
GetIntersectionPoints_Data{{{1,1},{1,3},{3,3},{3,1}},       {{2,2},{2,6},{4,6},{4,2}},           true, {{2,2},{2,3},{3,2},{3,3}}},
GetIntersectionPoints_Data{{{1,1},{1,3},{3,3},{3,1}},       {{0,0},{0,4},{4,4},{4,0}},           true, {{1,1},{1,3},{3,3},{3,1}}},
GetIntersectionPoints_Data{{{1,1},{1,3},{3,3},{3,1}},       {{2,2},{2,2.5},{2.5,2.5},{2.5,2}},   true, {{2,2},{2,2.5},{2.5,2.5},{2.5,2}}},
GetIntersectionPoints_Data{{{1,1},{1,3},{3,3},{3,1}},       {{1,4},{3,6},{6,3},{4,1}},           true, {{3,2},{2,3},{3,3}}},
GetIntersectionPoints_Data{{{-1,0},{0,1},{1,0},{0,-1}},     {{0,0},{1,1},{2,0},{1,-1}},          true, {{0,0},{0.5,0.5},{1,0},{0.5,-0.5}}}));

INSTANTIATE_TEST_CASE_P(SecondNotRectangular, GetIntersectionPoints_Test,
                        testing::Values(
                         //Element corners                  Object corners                              Intersection points
GetIntersectionPoints_Data{{{1,1},{1,2},{3,4},{3,1}},       {{2,5},{2,6},{4,6},{4,5}},           false, {}},
GetIntersectionPoints_Data{{{1,1},{1,2},{3,4},{3,1}},       {{2,2},{2,6},{4,6},{4,2}},           false, {{2,2},{2,3},{3,2},{3,4}}},
GetIntersectionPoints_Data{{{1,1},{1,2},{3,4},{3,1}},       {{0,0},{0,5},{4,5},{4,0}},           false, {{1,1},{1,2},{3,4},{3,1}}},
GetIntersectionPoints_Data{{{1,1},{1,2},{3,4},{3,1}},       {{2,2},{2,2.5},{2.5,2.5},{2.5,2}},   false, {{2,2},{2,2.5},{2.5,2.5},{2.5,2}}},
GetIntersectionPoints_Data{{{1,1},{1,2},{3,4},{3,1}},       {{-2,0},{-2,2},{0,2},{0,0}},         false, {}}));

/// Data table with the basic Informations for situations
/// \see PointQuery
struct WithinPolygon_Data
{
    // do not change order of items
    // unless you also change it in INSTANTIATE_TEST_CASE_P
    // (see bottom of file)
    double Ax;
    double Ay;
    double Bx;
    double By;
    double Dx;
    double Dy;
    double Cx;
    double Cy;
    double Px;
    double Py;
    bool withinPolygon;

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const WithinPolygon_Data& obj)
    {
        return os
               << "A (" << obj.Ax << ", " << obj.Ay << "), "
               << "B (" << obj.Bx << ", " << obj.By << "), "
               << "D (" << obj.Dx << ", " << obj.Dy << "), "
               << "C (" << obj.Cx << ", " << obj.Cy << "), "
               << "P (" << obj.Px << ", " << obj.Py << "), "
               << "withinPolygon: " << obj.withinPolygon;
    }
};

/// \see https://github.com/google/googletest/blob/master/googletest/docs/AdvancedGuide.md
class WithinPolygon: public ::testing::Test,
    public ::testing::WithParamInterface<WithinPolygon_Data>
{
};

/// Tests if point is within a given geometry element
TEST_P(WithinPolygon, ParameterTest)
{
    auto data = GetParam();
    Common::Vector2d point = {data.Px, data.Py};
    ASSERT_THAT(CommonHelper::IntersectionCalculation::IsWithin({data.Ax, data.Ay}, {data.Bx, data.By}, {data.Cx, data.Cy}, {data.Dx, data.Dy}, point),
                Eq(data.withinPolygon));
}

INSTANTIATE_TEST_CASE_P(OutsideBoundarySet, WithinPolygon,
                        testing::Values(
                            //    /*                    Ax     Ay     Bx     By     Dx     Dy     Cx     Cy     Px     Py   withinPolygon */
                            WithinPolygon_Data{ -10.0,  10.0,  10.0,  10.0,  10.0, -10.0, -10.0, -10.0, -10.1,   0.0,   false },
                            WithinPolygon_Data{ -10.0,  10.0,  10.0,  10.0,  10.0, -10.0, -10.0, -10.0,  10.1,   0.0,   false },
                            WithinPolygon_Data{ -10.0,  10.0,  10.0,  10.0,  10.0, -10.0, -10.0, -10.0,   0.0, -10.1,   false },
                            WithinPolygon_Data{ -10.0,  10.0,  10.0,  10.0,  10.0, -10.0, -10.0, -10.0,   0.0,  10.1,   false },
                            // 180° rotated
                            WithinPolygon_Data{  10.0, -10.0, -10.0, -10.0, -10.0,  10.0,  10.0,  10.0, -10.1,   0.0,   false },
                            WithinPolygon_Data{  10.0, -10.0, -10.0, -10.0, -10.0,  10.0,  10.0,  10.0,  10.1,   0.0,   false },
                            WithinPolygon_Data{  10.0, -10.0, -10.0, -10.0, -10.0,  10.0,  10.0,  10.0,   0.0, -10.1,   false },
                            WithinPolygon_Data{  10.0, -10.0, -10.0, -10.0, -10.0,  10.0,  10.0,  10.0,   0.0,  10.1,   false },
                            //  45° rotated
                            WithinPolygon_Data{   0.0,  10.0,  10.0,   0.0,   0.0, -10.0, -10.0,   0.0,  -7.0,  -7.0,   false },
                            WithinPolygon_Data{   0.0,  10.0,  10.0,   0.0,   0.0, -10.0, -10.0,   0.0,   7.0,  -7.0,   false },
                            WithinPolygon_Data{   0.0,  10.0,  10.0,   0.0,   0.0, -10.0, -10.0,   0.0,   7.0,   7.0,   false },
                            WithinPolygon_Data{   0.0,  10.0,  10.0,   0.0,   0.0, -10.0, -10.0,   0.0,  -7.0,   7.0,   false },
                            // other direction
                            WithinPolygon_Data{  10.0, -10.0,  10.0,  10.0, -10.0,  10.0, -10.0, -10.0, -10.1,   0.0,   false },
                            WithinPolygon_Data{  10.0, -10.0,  10.0,  10.0, -10.0,  10.0, -10.0, -10.0,  10.1,   0.0,   false },
                            WithinPolygon_Data{  10.0, -10.0,  10.0,  10.0, -10.0,  10.0, -10.0, -10.0,   0.0, -10.1,   false },
                            WithinPolygon_Data{  10.0, -10.0,  10.0,  10.0, -10.0,  10.0, -10.0, -10.0,   0.0,  10.1,   false }
                        )
                       );

INSTANTIATE_TEST_CASE_P(InsideBoundarySet, WithinPolygon,
                        testing::Values(
                            /*                    Ax     Ay     Bx     By     Dx     Dy     Cx     Cy     Px     Py   withinPolygon */
                            WithinPolygon_Data{ -12.3,  13.4,  15.6,  17.8,  19.2, -10.1, -12.3, -14.5,   0.0,   0.0,   true },
                            WithinPolygon_Data{ -12.3,  13.4,  15.6,  17.8,  19.2, -10.1, -12.3, -14.5, -10.0, -10.0,   true },
                            WithinPolygon_Data{ -12.3,  13.4,  15.6,  17.8,  19.2, -10.1, -12.3, -14.5,  10.0, -10.0,   true },
                            WithinPolygon_Data{ -12.3,  13.4,  15.6,  17.8,  19.2, -10.1, -12.3, -14.5, -10.0,  10.0,   true },
                            WithinPolygon_Data{ -12.3,  13.4,  15.6,  17.8,  19.2, -10.1, -12.3, -14.5,  10.0,  10.0,   true },
                            //  45° rotated
                            WithinPolygon_Data{   0.0,  10.0,  10.0,   0.0,   0.0, -10.0, -10.0,   0.0,  -3.0,  -3.0,   true },
                            WithinPolygon_Data{   0.0,  10.0,  10.0,   0.0,   0.0, -10.0, -10.0,   0.0,  -3.0,   3.0,   true },
                            WithinPolygon_Data{   0.0,  10.0,  10.0,   0.0,   0.0, -10.0, -10.0,   0.0,   3.0,  -3.0,   true },
                            WithinPolygon_Data{   0.0,  10.0,  10.0,   0.0,   0.0, -10.0, -10.0,   0.0,  -3.0,   3.0,   true },
                            // something in between
                            WithinPolygon_Data{ -12.3,  13.4,  15.6,  17.8,  19.0, -12.0, -12.1, -12.2,   0.0,   0.0,   true },
                            WithinPolygon_Data{ -12.3,  13.4,  15.6,  17.8,  19.0, -12.0, -12.1, -12.2, -10.0, -10.0,   true }
                        )
                       );

INSTANTIATE_TEST_CASE_P(OnEdgeSet, WithinPolygon,
                        testing::Values(
                            /*                    Ax     Ay     Bx     By     Dx     Dy     Cx     Cy     Px     Py   withinPolygon */
                            WithinPolygon_Data{ -10.0,  10.0,  10.0,  10.0,  10.0, -10.0, -10.0, -10.0,  10.0,   0.0,   true },
                            WithinPolygon_Data{ -10.0,  10.0,  10.0,  10.0,  10.0, -10.0, -10.0, -10.0,   0.0,  10.0,   true },
                            WithinPolygon_Data{ -10.0,  10.0,  10.0,  10.0,  10.0, -10.0, -10.0, -10.0,   0.0, -10.0,   true },
                            WithinPolygon_Data{ -10.0,  10.0,  10.0,  10.0,  10.0, -10.0, -10.0, -10.0, -10.0,   0.0,   true },
                            // diagonal in between
                            WithinPolygon_Data{ -10.0,  10.0,  10.0,  10.0,  10.0, -10.0, -10.0, -10.0,   2.0,   2.0,   true },
                            WithinPolygon_Data{ -10.0,  10.0,  10.0,  10.0,  10.0, -10.0, -10.0, -10.0,   2.0,  -2.0,   true },
                            WithinPolygon_Data{ -10.0,  10.0,  10.0,  10.0,  10.0, -10.0, -10.0, -10.0,  -2.0,  -2.0,   true },
                            WithinPolygon_Data{ -10.0,  10.0,  10.0,  10.0,  10.0, -10.0, -10.0, -10.0,  -2.0,   2.0,   true },
                            //  45° rotated deges
                            WithinPolygon_Data{   0.0,  10.0,  10.0,   0.0,   0.0, -10.0, -10.0,   0.0,   5.0,   5.0,   true },
                            WithinPolygon_Data{   0.0,  10.0,  10.0,   0.0,   0.0, -10.0, -10.0,   0.0,  -5.0,   5.0,   true },
                            WithinPolygon_Data{   0.0,  10.0,  10.0,   0.0,   0.0, -10.0, -10.0,   0.0,   5.0,   5.0,   true },
                            WithinPolygon_Data{   0.0,  10.0,  10.0,   0.0,   0.0, -10.0, -10.0,   0.0,   5.0,  -5.0,   true },
                            //  45° rotated diagonal in between
                            WithinPolygon_Data{   0.0,  10.0,  10.0,   0.0,   0.0, -10.0, -10.0,   0.0,   0.0,  -7.0,   true },
                            WithinPolygon_Data{   0.0,  10.0,  10.0,   0.0,   0.0, -10.0, -10.0,   0.0,   0.0,  -7.0,   true },
                            WithinPolygon_Data{   0.0,  10.0,  10.0,   0.0,   0.0, -10.0, -10.0,   0.0,   7.0,   0.0,   true },
                            WithinPolygon_Data{   0.0,  10.0,  10.0,   0.0,   0.0, -10.0, -10.0,   0.0,  -7.0,   0.0,   true }
                        )
                       );
