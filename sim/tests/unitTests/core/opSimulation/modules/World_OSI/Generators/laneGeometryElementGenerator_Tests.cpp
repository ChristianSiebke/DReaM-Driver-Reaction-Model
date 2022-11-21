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

#include "OWL/DataTypes.h"
#include "laneGeometryElementGenerator.h"

using ::testing::DoubleNear;

struct LaneGeometryElementGenerator_DataSet
{
    // do not change order of items
    // unless you also change it in INSTANTIATE_TEST_CASE_P
    // (see bottom of file)

    // data for generator
    Common::Vector2d origin;
    double width;
    double length;
    double hdg;

    // expected values
    Common::Vector2d currentLeft;
    Common::Vector2d currentReference;
    Common::Vector2d currentRight;
    Common::Vector2d nextLeft;
    Common::Vector2d nextReference;
    Common::Vector2d nextRight;

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const LaneGeometryElementGenerator_DataSet& obj)
    {
        return os
                << "origin " << obj.origin << ", "
                << "width "<< obj.width << ", "
                << "length "<< obj.length << ", "
                << "hdg "<< obj.hdg << ","
                << "currentLeft " << obj.currentLeft << ", "
                << "currentReference " << obj.currentReference << ", "
                << "currentRight " << obj.currentRight << ", "
                << "nextLeft " << obj.nextLeft << ", "
                << "nextReference " << obj.nextReference << ", "
                << "nextRight " << obj.nextRight;
    }
};


/// \see https://github.com/google/googletest/blob/master/googletest/docs/AdvancedGuide.md
class LaneGeometryElementGenerator:
        public ::testing::Test,
        public ::testing::WithParamInterface<LaneGeometryElementGenerator_DataSet>
{
};


TEST_P(LaneGeometryElementGenerator, RectangularLaneGeometryElementVerification)
{
    auto data = GetParam();

    auto elementUnderTest =
            OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement(
                data.origin,
                data.width,
                data.length,
                data.hdg);

    auto current = elementUnderTest.joints.current;
    auto next = elementUnderTest.joints.next;

    EXPECT_THAT(current.points.left.x,      DoubleNear(data.currentLeft.x,      1e-2));
    EXPECT_THAT(current.points.left.y,      DoubleNear(data.currentLeft.y,      1e-2));
    EXPECT_THAT(current.points.reference.x, DoubleNear(data.currentReference.x, 1e-2));
    EXPECT_THAT(current.points.reference.y, DoubleNear(data.currentReference.y, 1e-2));
    EXPECT_THAT(current.points.right.x,     DoubleNear(data.currentRight.x,     1e-2));
    EXPECT_THAT(current.points.right.y,     DoubleNear(data.currentRight.y,     1e-2));

    EXPECT_THAT(next.points.left.x,         DoubleNear(data.nextLeft.x,         1e-2));
    EXPECT_THAT(next.points.left.y,         DoubleNear(data.nextLeft.y,         1e-2));
    EXPECT_THAT(next.points.reference.x,    DoubleNear(data.nextReference.x,    1e-2));
    EXPECT_THAT(next.points.reference.y,    DoubleNear(data.nextReference.y,    1e-2));
    EXPECT_THAT(next.points.right.x,        DoubleNear(data.nextRight.x,        1e-2));
    EXPECT_THAT(next.points.right.y,        DoubleNear(data.nextRight.y,        1e-2));
}


TEST_P(LaneGeometryElementGenerator, TriangularLaneGeometryElementVerification)
{
    auto data = GetParam();

    auto elementUnderTest =
            OWL::Testing::LaneGeometryElementGenerator::TriangularLaneGeometryElement(
                data.origin,
                data.width,
                data.length,
                data.hdg);

    auto current = elementUnderTest.joints.current;
    auto next = elementUnderTest.joints.next;

    EXPECT_THAT(current.points.left.x,      DoubleNear(data.currentReference.x, 1e-2));
    EXPECT_THAT(current.points.left.y,      DoubleNear(data.currentReference.y, 1e-2));
    EXPECT_THAT(current.points.reference.x, DoubleNear(data.currentReference.x, 1e-2));
    EXPECT_THAT(current.points.reference.y, DoubleNear(data.currentReference.y, 1e-2));
    EXPECT_THAT(current.points.right.x,     DoubleNear(data.currentReference.x, 1e-2));
    EXPECT_THAT(current.points.right.y,     DoubleNear(data.currentReference.y, 1e-2));

    EXPECT_THAT(next.points.left.x,         DoubleNear(data.nextLeft.x,         1e-2));
    EXPECT_THAT(next.points.left.y,         DoubleNear(data.nextLeft.y,         1e-2));
    EXPECT_THAT(next.points.reference.x,    DoubleNear(data.nextReference.x,    1e-2));
    EXPECT_THAT(next.points.reference.y,    DoubleNear(data.nextReference.y,    1e-2));
    EXPECT_THAT(next.points.right.x,        DoubleNear(data.nextRight.x,        1e-2));
    EXPECT_THAT(next.points.right.y,        DoubleNear(data.nextRight.y,        1e-2));
}



INSTANTIATE_TEST_CASE_P(RandomSetWithRotatingHeading, LaneGeometryElementGenerator,
  testing::Values(
    /*                                     origin  width length hdg    curr_left         curr_ref          curr_right        next_left         next_ref          next_right  */
    /*                                        v       v   v     v          v                 v                 v                 v                 v                 v       */
    LaneGeometryElementGenerator_DataSet{{-16,   1},  8, 12, -6.28, {-16.01,   5.00}, {-16.00,   1.00}, {-15.99,  -3.00}, { -4.01,   5.04}, { -4.00,   1.04}, { -3.99,  -2.96}},
    LaneGeometryElementGenerator_DataSet{{  7,   0},  4, 12, -5.50, {  5.59,   1.42}, {  7.00,   0.00}, {  8.41,  -1.42}, { 14.09,   9.88}, { 15.50,   8.47}, { 16.92,   7.05}},
    LaneGeometryElementGenerator_DataSet{{ -7,  10},  3, 12, -4.71, { -8.50,  10.00}, { -7.00,  10.00}, { -5.50,  10.00}, { -8.53,  22.00}, { -7.03,  22.00}, { -5.53,  22.00}},
    LaneGeometryElementGenerator_DataSet{{-17,  13},  7, 12, -3.93, {-19.48,  10.53}, {-17.00,  13.00}, {-14.52,  15.47}, {-27.94,  19.04}, {-25.46,  21.51}, {-22.98,  23.98}},
    LaneGeometryElementGenerator_DataSet{{-17,   7},  9, 11, -3.14, {-16.99,   2.50}, {-17.00,   7.00}, {-17.01,  11.50}, {-27.99,   2.48}, {-28.00,   6.98}, {-28.01,  11.48}},
    LaneGeometryElementGenerator_DataSet{{ -1,  14},  3, 11, -2.36, {  0.06,  12.94}, { -1.00,  14.00}, { -2.06,  15.06}, { -7.75,   5.19}, { -8.81,   6.25}, { -9.86,   7.32}},
    LaneGeometryElementGenerator_DataSet{{-15,   8},  3, 11, -1.57, {-13.50,   8.00}, {-15.00,   8.00}, {-16.50,   8.00}, {-13.49,  -3.00}, {-14.99,  -3.00}, {-16.49,  -3.00}},
    LaneGeometryElementGenerator_DataSet{{-15,   6},  3, 11, -0.79, {-13.93,   7.06}, {-15.00,   6.00}, {-16.07,   4.94}, { -6.19,  -0.76}, { -7.26,  -1.81}, { -8.32,  -2.87}},
    LaneGeometryElementGenerator_DataSet{{ 19,   3}, 10, 12,  0.00, { 19.00,   8.00}, { 19.00,   3.00}, { 19.00,  -2.00}, { 31.00,   8.00}, { 31.00,   3.00}, { 31.00,  -2.00}},
    LaneGeometryElementGenerator_DataSet{{-12,  -9},  6, 12,  0.79, {-14.13,  -6.89}, {-12.00,  -9.00}, { -9.87, -11.11}, { -5.68,   1.64}, { -3.55,  -0.48}, { -1.42,  -2.59}},
    LaneGeometryElementGenerator_DataSet{{  5, -17},  3, 10,  1.57, {  3.50, -17.00}, {  5.00, -17.00}, {  6.50, -17.00}, {  3.51,  -7.00}, {  5.01,  -7.00}, {  6.51,  -7.00}},
    LaneGeometryElementGenerator_DataSet{{ 14,  -8},  9, 11,  2.36, { 10.83, -11.19}, { 14.00,  -8.00}, { 17.17,  -4.81}, {  3.02,  -3.45}, {  6.19,  -0.25}, {  9.36,   2.94}},
    LaneGeometryElementGenerator_DataSet{{  1,  -2},  6, 12,  3.14, {  1.00,  -5.00}, {  1.00,  -2.00}, {  1.00,   1.00}, {-11.00,  -4.98}, {-11.00,  -1.98}, {-11.00,   1.02}},
    LaneGeometryElementGenerator_DataSet{{ -3,  16},  5, 10,  3.93, { -1.23,  14.24}, { -3.00,  16.00}, { -4.77,  17.76}, { -8.28,   7.15}, {-10.05,   8.91}, {-11.82,  10.67}},
    LaneGeometryElementGenerator_DataSet{{ 14, -20},  6, 11,  4.71, { 17.00, -20.01}, { 14.00, -20.00}, { 11.00, -19.99}, { 16.97, -31.01}, { 13.97, -31.00}, { 10.97, -30.99}},
    LaneGeometryElementGenerator_DataSet{{ 14,   1},  9, 11,  5.5,  { 17.17,   4.19}, { 14.00,   1.00}, { 10.83,  -2.19}, { 24.97,  -3.57}, { 21.80,  -6.76}, { 18.62,  -9.95}},
    LaneGeometryElementGenerator_DataSet{{  5,   6},  8, 11,  6.28, {  5.01,  10.00}, {  5.00,   6.00}, {  4.99,   2.00}, { 16.01,   9.96}, { 16.00,   5.96}, { 15.99,   1.96}},
    LaneGeometryElementGenerator_DataSet{{  5,   6},  6,  0,   0.0, {  5.00,   9.00}, {  5.00,   6.00}, {  5.00,   3.00}, {  5.00,   9.00}, {  5.00,   6.00}, {  5.00,   3.00}}
));
