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

INSTANTIATE_TEST_CASE_P(CartesianNetDistanceTestCase, CartesianNetDistanceTest, ::testing::Values(
//                             ownBoundingBox                          otherBoundingBox                                 x y
    CartesianNetDistance_Data{polygon_t{{{0,0},{1,0},{1,1},{0,1}}},  polygon_t{{{1,0},{2,0},{2,1},{1,1}}},              0,0},
    CartesianNetDistance_Data{polygon_t{{{-1,0},{0,-1},{1,0},{0,1}}},polygon_t{{{1,1},{2,1},{2,2},{1,2}}},              0,0},
    CartesianNetDistance_Data{polygon_t{{{-1,0},{0,-1},{1,0},{0,1}}},polygon_t{{{3,4},{4,4},{4,6},{3,6}}},              2,3},
    CartesianNetDistance_Data{polygon_t{{{-1,0},{0,-1},{1,0},{0,1}}},polygon_t{{{-10,-10},{-8,-10},{-8,-9},{-10,-9}}},-7,-8}
));
