# /*********************************************************************
# * Copyright (c) 2020 in-tech GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#include "common/commonTools.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::TestWithParam;
using ::testing::Eq;
using ::testing::Values;
using ::testing::Return;
using ::testing::DoubleNear;

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
    const std::map<const std::string, GlobalRoadPosition> roadPositions;
    RouteElement expectedResult;
};

class GetRoadWithLowestHeading_Test : public ::TestWithParam<GetRoadWithLowestHeading_Data>
{

};

TEST_P(GetRoadWithLowestHeading_Test, GetRoadWithLowestHeading)
{
    auto data = GetParam();
    auto result = CommonHelper::GetRoadWithLowestHeading(data.roadPositions);

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
