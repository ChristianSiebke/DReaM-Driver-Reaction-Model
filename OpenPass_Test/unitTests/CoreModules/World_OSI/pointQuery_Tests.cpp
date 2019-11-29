/*******************************************************************************
* Copyright (c) 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "OWL/LaneGeometryElement.h"
#include "OWL/LaneGeometryJoint.h"
#include "PointQuery.h"

using ::testing::Eq;

using namespace World::Localization;

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

    OWL::Primitive::LaneGeometryJoint current;
    OWL::Primitive::LaneGeometryJoint next;

    current.points.left  = { data.Ax, data.Ay };
    next.points.left     = { data.Bx, data.By };
    current.points.right = { data.Cx, data.Cy};
    next.points.right    = { data.Dx, data.Dy };

    OWL::Primitive::LaneGeometryElement laneGeometry(current, next, nullptr);

    Common::Vector2d point = {data.Px, data.Py};
    ASSERT_THAT(PointQuery::IsWithin(laneGeometry, point), Eq(data.withinPolygon));
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
