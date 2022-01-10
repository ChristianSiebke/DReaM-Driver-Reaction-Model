/********************************************************************************
 * Copyright (c) 2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "common/opMath.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "road.h"

using ::testing::DoubleNear;

constexpr double MAX_GEOMETRY_ERROR = 0.001;

struct GeometrySpiral_Data
{
    double x;              //!< geometry origin x
    double y;              //!< geometry origin y
    double hdg;            //!< geometry origin heading
    double length;         //!< lenth of spiral between curvature start and end
    double curvStart;      //!< curvature at s = 0
    double curvEnd;        //!< curvature at s = length

    double s;              //!< query s coordinate
    double t;              //!< query t coordinate

    double expected_x;     //!< expected x coordinate for query
    double expected_y;     //!< expected y coordinate for query
    double expected_hdg;   //!< expected heading for query

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const GeometrySpiral_Data& obj)
    {
       return os
          << "x: "              << obj.x
          << ", y: "            << obj.y
          << ", hdg: "          << obj.hdg
          << ", length: "       << obj.length
          << ", curvStart: "    << obj.curvStart
          << ", curvend: "      << obj.curvEnd
          << ", s: "            << obj.s
          << ", t: "            << obj.t
          << ", expected_x: "   << obj.expected_x
          << ", expected_y: "   << obj.expected_y
          << ", expected_hdg: " << obj.expected_hdg;
    }
};

class RoadGeometries_Spiral : public ::testing::TestWithParam<GeometrySpiral_Data>
{
public:
    virtual ~RoadGeometries_Spiral()
    {
    }
};

TEST_P(RoadGeometries_Spiral, GetCoordAndGetDir_ReturnCorrectValues)
{
    GeometrySpiral_Data data = GetParam();

    const RoadGeometrySpiral rgs{0, data.x, data.y, data.hdg, data.length, data.curvStart, data.curvEnd};
    const auto res = rgs.GetCoord(data.s, data.t);
    const auto hdg = rgs.GetDir(data.s);

    EXPECT_THAT(res.x, DoubleNear(data.expected_x, MAX_GEOMETRY_ERROR));
    EXPECT_THAT(res.y, DoubleNear(data.expected_y, MAX_GEOMETRY_ERROR));
    EXPECT_THAT(hdg, DoubleNear(data.expected_hdg, MAX_GEOMETRY_ERROR));
}

INSTANTIATE_TEST_SUITE_P(AtEndOfGeometryAndZeroTAndZeroOrigin, RoadGeometries_Spiral, ::testing::Values(
    //                   |       origin          | spirial definition | query pos |   expected result        |
    //                   | x     y           hdg | len c_start  c_end |   s     t |    x        y      hdg   |
    GeometrySpiral_Data{  0.0,  0.0,         0.0, 100.0,  0.00,  0.00, 100.0,  0.0, 100.000,   0.000,  0.0000 },
    GeometrySpiral_Data{  0.0,  0.0,         0.0, 100.0,  0.00,  0.01, 100.0,  0.0,  97.529,  16.371,  0.5000 },
    GeometrySpiral_Data{  0.0,  0.0,         0.0, 100.0,  0.00, -0.01, 100.0,  0.0,  97.529, -16.371, -0.5000 },
    GeometrySpiral_Data{  0.0,  0.0,         0.0, 100.0,  0.01,  0.00, 100.0,  0.0,  93.438,  32.391,  0.5000 },
    GeometrySpiral_Data{  0.0,  0.0,         0.0, 100.0, -0.01,  0.00, 100.0,  0.0,  93.438, -32.391, -0.5000 }
));

INSTANTIATE_TEST_SUITE_P(AtEndOfGeometryAndZeroTAndNonzeroOrigin, RoadGeometries_Spiral, ::testing::Values(
    //                   |       origin          | spirial definition | query pos |   expected result        |
    //                   | x     y           hdg | len c_start  c_end |   s     t |    x        y      hdg   |
    GeometrySpiral_Data{  1.0,  1.0,         0.0, 100.0,  0.00,  0.00, 100.0,  0.0, 101.000,   1.000,  0.0000 },
    GeometrySpiral_Data{ -1.0, -1.0,         0.0, 100.0,  0.00,  0.00, 100.0,  0.0,  99.000,  -1.000,  0.0000 },
    GeometrySpiral_Data{ -1.0,  1.0,         0.0, 100.0,  0.00,  0.00, 100.0,  0.0,  99.000,   1.000,  0.0000 },
    GeometrySpiral_Data{  1.0, -1.0,         0.0, 100.0,  0.00,  0.00, 100.0,  0.0, 101.000,  -1.000,  0.0000},

    GeometrySpiral_Data{  1.0,  1.0,         0.0, 100.0,  0.00,  0.01, 100.0,  0.0,  98.529,  17.371,  0.5000 },
    GeometrySpiral_Data{ -1.0, -1.0,         0.0, 100.0,  0.00,  0.01, 100.0,  0.0,  96.529,  15.371,  0.5000 },
    GeometrySpiral_Data{ -1.0,  1.0,         0.0, 100.0,  0.00,  0.01, 100.0,  0.0,  96.529,  17.371,  0.5000 },
    GeometrySpiral_Data{  1.0, -1.0,         0.0, 100.0,  0.00,  0.01, 100.0,  0.0,  98.529,  15.371,  0.5000 }
));

INSTANTIATE_TEST_SUITE_P(AtEndOfGeometryAndNonzeroTAndNonzeroOrigin, RoadGeometries_Spiral, ::testing::Values(
    //                   |       origin          | spirial definition | query pos |   expected result        |
    //                   | x     y           hdg | len c_start  c_end |   s     t |    x        y      hdg   |
    GeometrySpiral_Data{  1.0,  1.0,         0.0, 100.0,  0.00,  0.01, 100.0,  1.0,  98.049,  18.249,  0.5000 },
    GeometrySpiral_Data{ -1.0, -1.0,         0.0, 100.0,  0.00,  0.01, 100.0,  1.0,  96.049,  16.249,  0.5000 },
    GeometrySpiral_Data{ -1.0,  1.0,         0.0, 100.0,  0.00,  0.01, 100.0,  1.0,  96.049,  18.249,  0.5000 },
    GeometrySpiral_Data{  1.0, -1.0,         0.0, 100.0,  0.00,  0.01, 100.0,  1.0,  98.049,  16.249,  0.5000 },
    GeometrySpiral_Data{  1.0,  1.0,         0.0, 100.0,  0.00,  0.01, 100.0, -1.0,  99.008,  16.494,  0.5000 },
    GeometrySpiral_Data{ -1.0, -1.0,         0.0, 100.0,  0.00,  0.01, 100.0, -1.0,  97.008,  14.494,  0.5000 },
    GeometrySpiral_Data{ -1.0,  1.0,         0.0, 100.0,  0.00,  0.01, 100.0, -1.0,  97.008,  16.494,  0.5000 },
    GeometrySpiral_Data{  1.0, -1.0,         0.0, 100.0,  0.00,  0.01, 100.0, -1.0,  99.008,  14.494,  0.5000 }
));

INSTANTIATE_TEST_SUITE_P(AtEndOfGeometryAndZeroTAndNonzeroOriginHeading, RoadGeometries_Spiral, ::testing::Values(
    //                   |       origin          | spirial definition | query pos |   expected result        |
    //                   | x     y           hdg | len c_start  c_end |   s     t |    x        y      hdg   |
    GeometrySpiral_Data{  0.0,  0.0,      M_PI_4, 100.0,  0.00,  0.00, 100.0,  0.0,  70.711,  70.711,  0.7854 },
    GeometrySpiral_Data{  0.0,  0.0,  3 * M_PI_4, 100.0,  0.00,  0.00, 100.0,  0.0, -70.711,  70.711,  2.3570 },
    GeometrySpiral_Data{  0.0,  0.0, -3 * M_PI_4, 100.0,  0.00,  0.00, 100.0,  0.0, -70.711, -70.711, -2.3570 },
    GeometrySpiral_Data{  0.0,  0.0,  5 * M_PI_4, 100.0,  0.00,  0.00, 100.0,  0.0, -70.711, -70.711,  3.9270 },
    GeometrySpiral_Data{  0.0,  0.0,     -M_PI_4, 100.0,  0.00,  0.00, 100.0,  0.0,  70.711, -70.711, -0.7854 },
    GeometrySpiral_Data{  0.0,  0.0,  7 * M_PI_4, 100.0,  0.00,  0.00, 100.0,  0.0,  70.711, -70.711,  5.4978 },

    GeometrySpiral_Data{  0.0,  0.0,      M_PI_4, 100.0,  0.00,  0.01, 100.0,  0.0,  57.387,  80.540,  1.2854 },
    GeometrySpiral_Data{  0.0,  0.0,  3 * M_PI_4, 100.0,  0.00,  0.01, 100.0,  0.0, -80.540,  57.387,  2.8562 },
    GeometrySpiral_Data{  0.0,  0.0, -3 * M_PI_4, 100.0,  0.00,  0.01, 100.0,  0.0, -57.387, -80.540, -1.8562 },
    GeometrySpiral_Data{  0.0,  0.0,  5 * M_PI_4, 100.0,  0.00,  0.01, 100.0,  0.0, -57.387, -80.540,  4.4270 },
    GeometrySpiral_Data{  0.0,  0.0,     -M_PI_4, 100.0,  0.00,  0.01, 100.0,  0.0,  80.540, -57.387, -0.2854 },
    GeometrySpiral_Data{  0.0,  0.0,  7 * M_PI_4, 100.0,  0.00,  0.01, 100.0,  0.0,  80.540, -57.387,  5.9978 }
));

INSTANTIATE_TEST_SUITE_P(AtEndOfGeometryAndZeroTAndNonzeroOriginAndHeading, RoadGeometries_Spiral, ::testing::Values(
    //                   |       origin          | spirial definition | query pos |   expected result        |
    //                   | x     y           hdg | len c_start  c_end |   s     t |    x        y      hdg   |
    GeometrySpiral_Data{  1.0,  1.0,      M_PI_4, 100.0,  0.00,  0.01, 100.0,  0.0,  58.387,  81.540,  1.2854 },
    GeometrySpiral_Data{  1.0,  1.0,     -M_PI_4, 100.0,  0.00,  0.01, 100.0,  0.0,  81.540, -56.387, -0.2854 },
    GeometrySpiral_Data{ -1.0, -1.0,  3 * M_PI_4, 100.0,  0.00, -0.01, 100.0,  0.0, -58.387,  79.540,  1.8562 },
    GeometrySpiral_Data{ -1.0,  1.0, -3 * M_PI_4, 100.0,  0.00, -0.01, 100.0,  0.0, -81.540, -56.387, -2.8562 },
    GeometrySpiral_Data{  1.0, -1.0,  5 * M_PI_4, 100.0,  0.00, -0.01, 100.0,  0.0, -79.540, -58.387,  3.4270 }
));

INSTANTIATE_TEST_SUITE_P(AtEndOfGeometryAndZeroTWithNonzeroStartAndEndCurvature, RoadGeometries_Spiral, ::testing::Values(
    //                   |       origin          | spirial definition | query pos |   expected result        |
    //                   | x     y           hdg | len c_start  c_end |   s     t |    x        y      hdg   |
    GeometrySpiral_Data{  0.0,  0.0,         0.0, 100.0,  0.01,  0.02, 100.0,  0.0,  71.564,  55.928,  1.5    },
    GeometrySpiral_Data{  0.0,  0.0,      M_PI_4, 100.0,  0.01,  0.02, 100.0,  0.0,  11.057,  90.151,  2.2854 },
    GeometrySpiral_Data{  0.0,  0.0,     -M_PI_4, 100.0,  0.01,  0.02, 100.0,  0.0,  90.151, -11.057,  0.7146 },
    GeometrySpiral_Data{  0.0,  0.0,  3 * M_PI_4, 100.0,  0.01,  0.02, 100.0,  0.0, -90.151,  11.057,  3.8562 },
    GeometrySpiral_Data{  0.0,  0.0, -3 * M_PI_4, 100.0,  0.01,  0.02, 100.0,  0.0, -11.057, -90.151, -0.8562 },

    GeometrySpiral_Data{  0.0,  0.0,         0.0, 100.0, -0.01, -0.02, 100.0,  0.0,  71.564, -55.928, -1.5    },
    GeometrySpiral_Data{  0.0,  0.0,      M_PI_4, 100.0, -0.01, -0.02, 100.0,  0.0,  90.151,  11.057, -0.7146 },
    GeometrySpiral_Data{  0.0,  0.0,     -M_PI_4, 100.0, -0.01, -0.02, 100.0,  0.0,  11.057, -90.151, -2.2854 },
    GeometrySpiral_Data{  0.0,  0.0,  3 * M_PI_4, 100.0, -0.01, -0.02, 100.0,  0.0, -11.057,  90.151,  0.8562 },
    GeometrySpiral_Data{  0.0,  0.0, -3 * M_PI_4, 100.0, -0.01, -0.02, 100.0,  0.0, -90.151, -11.057, -3.8562 },

    GeometrySpiral_Data{  0.0,  0.0,         0.0, 100.0,  0.02,  0.01, 100.0,  0.0,  60.850,  67.429,  1.5    },
    GeometrySpiral_Data{  0.0,  0.0,      M_PI_4, 100.0,  0.02,  0.01, 100.0,  0.0,  -4.652,  90.707,  2.2854 },
    GeometrySpiral_Data{  0.0,  0.0,     -M_PI_4, 100.0,  0.02,  0.01, 100.0,  0.0,  90.707,   4.652,  0.7146 },
    GeometrySpiral_Data{  0.0,  0.0,  3 * M_PI_4, 100.0,  0.02,  0.01, 100.0,  0.0, -90.707,  -4.652,  3.8562 },
    GeometrySpiral_Data{  0.0,  0.0, -3 * M_PI_4, 100.0,  0.02,  0.01, 100.0,  0.0,   4.652, -90.707, -0.8562 },

    GeometrySpiral_Data{  0.0,  0.0,         0.0, 100.0, -0.02, -0.01, 100.0,  0.0,  60.850, -67.429, -1.5    },
    GeometrySpiral_Data{  0.0,  0.0,      M_PI_4, 100.0, -0.02, -0.01, 100.0,  0.0,  90.707,  -4.652, -0.7146 },
    GeometrySpiral_Data{  0.0,  0.0,     -M_PI_4, 100.0, -0.02, -0.01, 100.0,  0.0,  -4.652, -90.707, -2.2854 },
    GeometrySpiral_Data{  0.0,  0.0,  3 * M_PI_4, 100.0, -0.02, -0.01, 100.0,  0.0,   4.652,  90.707,  0.8562 },
    GeometrySpiral_Data{  0.0,  0.0, -3 * M_PI_4, 100.0, -0.02, -0.01, 100.0,  0.0, -90.707,   4.652, -3.8562 }
));

INSTANTIATE_TEST_SUITE_P(AtEndOfGeometryAndZeroTWithNonzeroStartAndEndCurvatureAndNonzeroOrigin, RoadGeometries_Spiral, ::testing::Values(
    //                   |       origin          | spirial definition | query pos |   expected result        |
    //                   | x     y           hdg | len c_start  c_end |   s     t |    x        y      hdg   |
    GeometrySpiral_Data{  0.0,  1.0,         0.0, 100.0,  0.01,  0.02, 100.0,  0.0,  71.564,  56.928,  1.5    },
    GeometrySpiral_Data{  1.0,  0.0,      M_PI_4, 100.0,  0.01,  0.02, 100.0,  0.0,  12.057,  90.151,  2.2854 },
    GeometrySpiral_Data{  1.0,  0.0,     -M_PI_4, 100.0,  0.01,  0.02, 100.0,  0.0,  91.151, -11.057,  0.7146 },
    GeometrySpiral_Data{  1.0,  0.0,  3 * M_PI_4, 100.0,  0.01,  0.02, 100.0,  0.0, -89.151,  11.057,  3.8562 },
    GeometrySpiral_Data{  0.0,  1.0, -3 * M_PI_4, 100.0,  0.01,  0.02, 100.0,  0.0, -11.057, -89.151, -0.8562 },

    GeometrySpiral_Data{  0.0,  1.0,         0.0, 100.0, -0.01, -0.02, 100.0,  0.0,  71.564, -54.928, -1.5    },
    GeometrySpiral_Data{  1.0,  0.0,      M_PI_4, 100.0, -0.01, -0.02, 100.0,  0.0,  91.151,  11.057, -0.7146 },
    GeometrySpiral_Data{  1.0,  0.0,     -M_PI_4, 100.0, -0.01, -0.02, 100.0,  0.0,  12.057, -90.151, -2.2854 },
    GeometrySpiral_Data{  1.0,  0.0,  3 * M_PI_4, 100.0, -0.01, -0.02, 100.0,  0.0, -10.057,  90.151,  0.8562 },
    GeometrySpiral_Data{  0.0,  1.0, -3 * M_PI_4, 100.0, -0.01, -0.02, 100.0,  0.0, -90.151, -10.057, -3.8562 },

    GeometrySpiral_Data{  0.0,  1.0,         0.0, 100.0,  0.02,  0.01, 100.0,  0.0,  60.850,  68.429,  1.5    },
    GeometrySpiral_Data{  1.0,  0.0,      M_PI_4, 100.0,  0.02,  0.01, 100.0,  0.0,  -3.652,  90.707,  2.2854 },
    GeometrySpiral_Data{  1.0,  0.0,     -M_PI_4, 100.0,  0.02,  0.01, 100.0,  0.0,  91.707,   4.652,  0.7146 },
    GeometrySpiral_Data{  1.0,  0.0,  3 * M_PI_4, 100.0,  0.02,  0.01, 100.0,  0.0, -89.707,  -4.652,  3.8562 },
    GeometrySpiral_Data{  0.0,  1.0, -3 * M_PI_4, 100.0,  0.02,  0.01, 100.0,  0.0,   4.652, -89.707, -0.8562 },

    GeometrySpiral_Data{  0.0,  1.0,         0.0, 100.0, -0.02, -0.01, 100.0,  0.0,  60.850, -66.429, -1.5    },
    GeometrySpiral_Data{  1.0,  0.0,      M_PI_4, 100.0, -0.02, -0.01, 100.0,  0.0,  91.707,  -4.652, -0.7146 },
    GeometrySpiral_Data{  1.0,  0.0,     -M_PI_4, 100.0, -0.02, -0.01, 100.0,  0.0,  -3.652, -90.707, -2.2854 },
    GeometrySpiral_Data{  1.0,  0.0,  3 * M_PI_4, 100.0, -0.02, -0.01, 100.0,  0.0,   5.652,  90.707,  0.8562 },
    GeometrySpiral_Data{  0.0,  1.0, -3 * M_PI_4, 100.0, -0.02, -0.01, 100.0,  0.0, -90.707,   5.652, -3.8562 }
));

INSTANTIATE_TEST_SUITE_P(AtEndOfGeometryAndNonzeroTWithNonzeroStartAndEndCurvatureAndNonzeroOrigin, RoadGeometries_Spiral, ::testing::Values(
    //                   |       origin          | spirial definition | query pos |   expected result        |
    //                   | x     y           hdg | len c_start  c_end |   s     t |    x        y      hdg   |
    GeometrySpiral_Data{  0.0,  1.0,         0.0, 100.0,  0.01,  0.02, 100.0,  2.0,  69.569,  57.069,  1.5    },
    GeometrySpiral_Data{  1.0,  0.0,      M_PI_4, 100.0,  0.01,  0.02, 100.0, -2.0,  13.568,  91.461,  2.2854 },
    GeometrySpiral_Data{  1.0,  1.0,     -M_PI_4, 100.0,  0.01,  0.02, 100.0,  2.0,  89.840,  -8.546,  0.7146 },
    GeometrySpiral_Data{  1.0,  0.0,  3 * M_PI_4, 100.0,  0.01,  0.02, 100.0, -2.0, -90.461,  12.568,  3.8562 },
    GeometrySpiral_Data{  0.0,  1.0, -3 * M_PI_4, 100.0,  0.01,  0.02, 100.0,  2.0,  -9.546, -87.840, -0.8562 },

    GeometrySpiral_Data{  0.0,  1.0,         0.0, 100.0, -0.01, -0.02, 100.0, -2.0,  69.569, -55.069, -1.5    },
    GeometrySpiral_Data{  1.0,  0.0,      M_PI_4, 100.0, -0.01, -0.02, 100.0,  2.0,  92.461,  12.568, -0.7146 },
    GeometrySpiral_Data{  1.0,  1.0,     -M_PI_4, 100.0, -0.01, -0.02, 100.0, -2.0,  10.546, -87.840, -2.2854 },
    GeometrySpiral_Data{  1.0,  0.0,  3 * M_PI_4, 100.0, -0.01, -0.02, 100.0,  2.0, -11.568,  91.461,  0.8562 },
    GeometrySpiral_Data{  0.0,  1.0, -3 * M_PI_4, 100.0, -0.01, -0.02, 100.0, -2.0, -88.840,  -8.546, -3.8562 },

    GeometrySpiral_Data{  0.0,  1.0,         0.0, 100.0,  0.02,  0.01, 100.0,  2.0,  58.855,  68.570,  1.5    },
    GeometrySpiral_Data{  1.0,  0.0,      M_PI_4, 100.0,  0.02,  0.01, 100.0,  2.0,  -5.163,  89.396,  2.2854 },
    GeometrySpiral_Data{  1.0,  1.0,     -M_PI_4, 100.0,  0.02,  0.01, 100.0, -2.0,  93.017,   4.141,  0.7146 },
    GeometrySpiral_Data{  1.0,  0.0,  3 * M_PI_4, 100.0,  0.02,  0.01, 100.0, -2.0, -91.017,  -3.141,  3.8562 },
    GeometrySpiral_Data{  0.0,  1.0, -3 * M_PI_4, 100.0,  0.02,  0.01, 100.0, -2.0,   3.141, -91.017, -0.8562 },

    GeometrySpiral_Data{  0.0,  1.0,         0.0, 100.0, -0.02, -0.01, 100.0, -2.0,  58.855, -66.570, -1.5    },
    GeometrySpiral_Data{  1.0,  0.0,      M_PI_4, 100.0, -0.02, -0.01, 100.0, -2.0,  90.396,  -6.163, -0.7146 },
    GeometrySpiral_Data{  1.0,  1.0,     -M_PI_4, 100.0, -0.02, -0.01, 100.0, -2.0,  -5.163, -88.396, -2.2854 },
    GeometrySpiral_Data{  1.0,  0.0,  3 * M_PI_4, 100.0, -0.02, -0.01, 100.0,  2.0,   4.141,  92.017,  0.8562 },
    GeometrySpiral_Data{  0.0,  1.0, -3 * M_PI_4, 100.0, -0.02, -0.01, 100.0,  2.0, -92.017,   4.141, -3.8562 }
));

INSTANTIATE_TEST_SUITE_P(AtCenterOfGeometryAndNonzeroTWithNonzeroStartAndEndCurvatureAndNonzeroOrigin, RoadGeometries_Spiral, ::testing::Values(
    //                   |       origin          | spirial definition | query pos |   expected result        |
    //                   | x     y           hdg | len c_start  c_end |   s     t |    x        y      hdg   |
    GeometrySpiral_Data{  0.0,  1.0,         0.0, 100.0,  0.01,  0.02,  50.0,  2.0,  45.942,  16.759,  0.6250 },
    GeometrySpiral_Data{  1.0,  0.0,      M_PI_4, 100.0,  0.01,  0.02,  50.0, -2.0,  26.291,  42.991,  1.4104 },
    GeometrySpiral_Data{  1.0,  1.0,     -M_PI_4, 100.0,  0.01,  0.02,  50.0,  2.0,  44.630, -20.343, -0.1604 },
    GeometrySpiral_Data{  1.0,  0.0,  3 * M_PI_4, 100.0,  0.01,  0.02,  50.0, -2.0, -41.991,  25.291,  2.9812 },
    GeometrySpiral_Data{  0.0,  1.0, -3 * M_PI_4, 100.0,  0.01,  0.02,  50.0,  2.0, -21.343, -42.630, -1.7312 },

    GeometrySpiral_Data{  0.0,  1.0,         0.0, 100.0, -0.01, -0.02,  50.0,  2.0,  48.283, -11.516, -0.6250 },
    GeometrySpiral_Data{  1.0,  0.0,      M_PI_4, 100.0, -0.01, -0.02,  50.0, -2.0,  44.630,  21.343,  0.1604 },
    GeometrySpiral_Data{  1.0,  1.0,     -M_PI_4, 100.0, -0.01, -0.02,  50.0,  2.0,  26.291, -41.991, -1.4104 },
    GeometrySpiral_Data{  1.0,  0.0,  3 * M_PI_4, 100.0, -0.01, -0.02,  50.0, -2.0, -20.343,  43.630,  1.7312 },
    GeometrySpiral_Data{  0.0,  1.0, -3 * M_PI_4, 100.0, -0.01, -0.02,  50.0,  2.0, -42.991, -24.291, -2.9812 },

    GeometrySpiral_Data{  0.0,  1.0,         0.0, 100.0,  0.02,  0.01,  50.0,  2.0,  41.880,  23.716,  0.8750 },
    GeometrySpiral_Data{  1.0,  0.0,      M_PI_4, 100.0,  0.02,  0.01,  50.0,  2.0,  14.551,  45.677,  1.6604 },
    GeometrySpiral_Data{  1.0,  1.0,     -M_PI_4, 100.0,  0.02,  0.01,  50.0, -2.0,  47.035, -16.535,  0.0896 },
    GeometrySpiral_Data{  1.0,  0.0,  3 * M_PI_4, 100.0,  0.02,  0.01,  50.0, -2.0, -45.035,  17.535,  3.2312 },
    GeometrySpiral_Data{  0.0,  1.0, -3 * M_PI_4, 100.0,  0.02,  0.01,  50.0, -2.0, -17.535, -45.035, -1.4812 },

    GeometrySpiral_Data{  0.0,  1.0,         0.0, 100.0, -0.02, -0.01,  50.0, -2.0,  41.880, -21.716, -0.8750 },
    GeometrySpiral_Data{  1.0,  0.0,      M_PI_4, 100.0, -0.02, -0.01,  50.0, -2.0,  46.677,  13.551, -0.0896 },
    GeometrySpiral_Data{  1.0,  1.0,     -M_PI_4, 100.0, -0.02, -0.01,  50.0, -2.0,  14.551, -44.677, -1.6604 },
    GeometrySpiral_Data{  1.0,  0.0,  3 * M_PI_4, 100.0, -0.02, -0.01,  50.0,  2.0, -16.535,  46.035,  1.4812 },
    GeometrySpiral_Data{  0.0,  1.0, -3 * M_PI_4, 100.0, -0.02, -0.01,  50.0,  2.0, -46.035, -16.535, -3.2312 }
));

INSTANTIATE_TEST_SUITE_P(AtEndOfGeometryAndZeroTWithAlternatingCurvatureSign, RoadGeometries_Spiral, ::testing::Values(
    //                   |       origin          | spirial definition | query pos |   expected result        |
    //                   | x     y           hdg | len c_start  c_end |   s     t |    x        y      hdg   |
    GeometrySpiral_Data{ -49.688, -4.148,   0.25, 100.0, -0.01,  0.01,  50.0,  0.0,   0.000,   0.000,  0.0000 },
    GeometrySpiral_Data{ -49.688,  4.148,  -0.25, 100.0,  0.01, -0.01,  50.0,  0.0,   0.000,   0.000,  0.0000 },
    GeometrySpiral_Data{  0.0,  0.0,         0.0, 100.0, -0.01,  0.01,  50.0,  0.0,  49.170,  -8.274, -0.2500 },
    GeometrySpiral_Data{  0.0,  0.0,         0.0, 100.0, -0.01,  0.01, 100.0,  0.0,  98.340, -16.548,  0.0000 },
    GeometrySpiral_Data{  0.0,  0.0,         0.0, 100.0,  0.01, -0.01,  50.0,  0.0,  49.170,   8.274,  0.2500 },
    GeometrySpiral_Data{  0.0,  0.0,         0.0, 100.0,  0.01, -0.01, 100.0,  0.0,  98.340,  16.548,  0.0000 },
    GeometrySpiral_Data{  0.0,  0.0,         0.0, 100.0, -0.02,  0.01, 100.0,  0.0,  86.252, -47.254, -0.5000 },
    GeometrySpiral_Data{  0.0,  0.0,         0.0, 100.0, -0.02,  0.01,  50.0,  0.0,  45.747, -18.029, -0.6250 },
    GeometrySpiral_Data{  0.0,  0.0,         0.0, 100.0,  0.02, -0.01, 100.0,  0.0,  86.252,  47.254,  0.5000 },
    GeometrySpiral_Data{  0.0,  0.0,         0.0, 100.0,  0.02, -0.01,  50.0,  0.0,  45.747,  18.029,  0.6250 }
));
