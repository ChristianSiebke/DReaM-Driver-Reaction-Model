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

#include "fakeWorld.h"
#include "fakeSection.h"
#include "fakeLane.h"
#include "fakeWorldData.h"
#include "fakeLaneManager.h"
#include "fakeMovingObject.h"
#include "fakeAgent.h"

#include "Generators/laneGeometryElementGenerator.h"

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::IsNull;
using ::testing::_;
using ::testing::Eq;
using ::testing::Ge;
using ::testing::Le;
using ::testing::Lt;
using ::testing::AllOf;

/////////////////////////////////////////////////////////////////////////////
TEST(GetNeighbouringJoints, DistanceBetweenTwoJoints_ReturnsJoints)
{
    const double length = 10.0;

    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr);

    auto laneGeometryElement =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement(
    { 0.0, 0.0 },  // origin
    0.0,           // width
    length,
    0.0);          // heading

    const OWL::Primitive::LaneGeometryJoint& firstJoint = laneGeometryElement.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = laneGeometryElement.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, 0.0, 0.0);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, 0.0, 0.0);

    const OWL::Primitive::LaneGeometryJoint* prevJoint;
    const OWL::Primitive::LaneGeometryJoint* nextJoint;

    std::tie(prevJoint, nextJoint) = lane.GetNeighbouringJoints(length / 2.0);

    ASSERT_NE(prevJoint, nullptr);
    ASSERT_NE(nextJoint, nullptr);

    ASSERT_EQ(prevJoint->points.reference.x, 0.0);
    ASSERT_EQ(nextJoint->points.reference.x, length);
}

TEST(GetNeighbouringJoints, DistanceAtPrevJoint_ReturnsJoints)
{
    const double length = 10.0;

    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr);

    auto laneGeometryElement =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement(
    { 0.0, 0.0 },  // origin
    0.0,           // width
    length,
    0.0);          // heading

    const OWL::Primitive::LaneGeometryJoint& firstJoint = laneGeometryElement.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = laneGeometryElement.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, 0.0, 0.0);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, 0.0, 0.0);

    const OWL::Primitive::LaneGeometryJoint* prevJoint;
    const OWL::Primitive::LaneGeometryJoint* nextJoint;

    std::tie(prevJoint, nextJoint) = lane.GetNeighbouringJoints(0.0);

    ASSERT_NE(prevJoint, nullptr);
    ASSERT_NE(nextJoint, nullptr);

    ASSERT_EQ(prevJoint->points.reference.x, 0.0);
    ASSERT_EQ(nextJoint->points.reference.x, length);
}

TEST(GetNeighbouringJoints, DistanceAtNextJoint_ReturnsPrevJointOnly)
{
    const double length = 10.0;

    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr);

    auto laneGeometryElement =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement(
    { 0.0, 0.0 },  // origin
    0.0,           // width
    length,
    0.0);          // heading

    const OWL::Primitive::LaneGeometryJoint& firstJoint = laneGeometryElement.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = laneGeometryElement.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, 0.0, 0.0);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, 0.0, 0.0);

    const OWL::Primitive::LaneGeometryJoint* prevJoint;
    const OWL::Primitive::LaneGeometryJoint* nextJoint;

    std::tie(prevJoint, nextJoint) = lane.GetNeighbouringJoints(10.0);

    ASSERT_NE(prevJoint, nullptr);
    ASSERT_EQ(nextJoint, nullptr);

    ASSERT_EQ(prevJoint->points.reference.x, 10.0);
}

TEST(GetNeighbouringJoints, DistanceAtMiddleJoint_ReturnsJoints)
{
    const double length = 10.0;

    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr);

    auto laneGeometryElement1 =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement(
    { 0.0, 0.0 },  // origin
    0.0,           // width
    length,
    0.0);          // heading

    auto laneGeometryElement2 =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement(
    { 10.0, 0.0 }, // origin
    0.0,           // width
    length,
    0.0);          // heading

    const OWL::Primitive::LaneGeometryJoint& firstJoint = laneGeometryElement1.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = laneGeometryElement2.joints.current;
    const OWL::Primitive::LaneGeometryJoint& thirdJoint = laneGeometryElement2.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;
    Common::Vector2d leftPoint3 = thirdJoint.points.left;
    Common::Vector2d referencePoint3 = thirdJoint.points.reference;
    Common::Vector2d rightPoint3 = thirdJoint.points.right;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, 0.0, 0.0);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, 0.0, 0.0);
    lane.AddLaneGeometryJoint(leftPoint3, referencePoint3, rightPoint3, 2.0 * length, 0.0, 0.0);

    const OWL::Primitive::LaneGeometryJoint* prevJoint;
    const OWL::Primitive::LaneGeometryJoint* nextJoint;

    std::tie(prevJoint, nextJoint) = lane.GetNeighbouringJoints(10.0);

    ASSERT_NE(prevJoint, nullptr);
    ASSERT_NE(nextJoint, nullptr);

    ASSERT_EQ(prevJoint->points.reference.x, 10.0);
    ASSERT_EQ(nextJoint->points.reference.x, 20.0);
}

/////////////////////////////////////////////////////////////////////////////
//ToDo: Add more tests including CurvedRoad, heading and multilane

TEST(GetInterpolatedPointsAtDistance, DistanceAtPrevJoint_ReturnsPrevJoint)
{
    const double length = 10.0;
    const double width = 3.75;

    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr);

    auto laneGeometryElement =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement(
    { 0.0, 0.0 },  // origin
    width,           // width
    length,
    0.0);          // heading

    const OWL::Primitive::LaneGeometryJoint& firstJoint = laneGeometryElement.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = laneGeometryElement.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, 0.0, 0.0);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, 0.0, 0.0);

    const OWL::Primitive::LaneGeometryJoint::Points ipPoints = lane.GetInterpolatedPointsAtDistance(0.0);

    ASSERT_NE(&firstJoint, &secondJoint);
    ASSERT_NE(&firstJoint, nullptr);
    ASSERT_EQ(ipPoints.left, firstJoint.points.left);
    ASSERT_EQ(ipPoints.right, firstJoint.points.right);
    ASSERT_EQ(ipPoints.reference, firstJoint.points.reference);
}

TEST(GetInterpolatedPointsAtDistance, DistanceAtNextJoint_ReturnsNextJoint)
{
    const double length = 10.0;
    const double width = 3.75;

    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr);

    auto laneGeometryElement =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement(
    { 0.0, 0.0 },  // origin
    width,           // width
    length,
    0.0);          // heading

    const OWL::Primitive::LaneGeometryJoint& firstJoint = laneGeometryElement.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = laneGeometryElement.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, 0.0, 0.0);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, 0.0, 0.0);

    const OWL::Primitive::LaneGeometryJoint::Points ipPoints = lane.GetInterpolatedPointsAtDistance(10.0);

    ASSERT_NE(&firstJoint, &secondJoint);
    ASSERT_NE(&firstJoint, nullptr);
    ASSERT_EQ(ipPoints.left, secondJoint.points.left);
    ASSERT_EQ(ipPoints.right, secondJoint.points.right);
    ASSERT_EQ(ipPoints.reference, secondJoint.points.reference);
}

TEST(GetInterpolatedPointsAtDistance, DistanceBetweenJoints_ReturnsInterpolatedPoint)
{
    const double length = 10.0;
    const double width = 4.0;
    const double distance = 5.0;

    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr);

    auto laneGeometryElement =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement(
    { 0.0, 0.0 },  // origin
    width,           // width
    length,
    0.0);          // heading

    const OWL::Primitive::LaneGeometryJoint& firstJoint = laneGeometryElement.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = laneGeometryElement.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, 0.0, 0.0);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, 0.0, 0.0);

    const OWL::Primitive::LaneGeometryJoint::Points ipPoints = lane.GetInterpolatedPointsAtDistance(distance);
    const Common::Vector2d defaultValues;

    ASSERT_NE(&firstJoint, &secondJoint);
    ASSERT_NE(&firstJoint, nullptr);
    ASSERT_EQ(ipPoints.left.x, distance);
    ASSERT_EQ(ipPoints.left.y, width/2);

    ASSERT_EQ(ipPoints.reference.x, distance);
    ASSERT_EQ(ipPoints.reference.y, 0.0);

    ASSERT_EQ(ipPoints.right.x, distance);
    ASSERT_EQ(ipPoints.right.y, -width/2);
}

TEST(GetInterpolatedPointsAtDistance, DistanceBetweenJointsWithHeading_ReturnsInterpolatedPoint)
{
    const double length = 10.0;
    const double width = 4.0;
    const double distance = 5.0;

    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr);

    auto laneGeometryElement =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement(
    { 0.0, 0.0 },  // origin
    width,           // width
    length,
    PI/4);          // heading

    const OWL::Primitive::LaneGeometryJoint& firstJoint = laneGeometryElement.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = laneGeometryElement.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, 0.0, 0.0);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, 0.0, 0.0);

    const OWL::Primitive::LaneGeometryJoint::Points ipPoints = lane.GetInterpolatedPointsAtDistance(distance);
    const Common::Vector2d defaultValues;

    ASSERT_NE(&firstJoint, &secondJoint);
    ASSERT_NE(&firstJoint, nullptr);
//    ASSERT_EQ(ipPoints.left.x, );
//    ASSERT_EQ(ipPoints.left.y, );

//    ASSERT_EQ(ipPoints.reference.x, );
//    ASSERT_EQ(ipPoints.reference.y, );

//    ASSERT_EQ(ipPoints.right.x, );
//    ASSERT_EQ(ipPoints.right.y, );
}

/////////////////////////////////////////////////////////////////////////////

TEST(GetLaneWidth, LaneWidthQueryOnTriangularLane_ReturnsZeroWidthAtStartAndFullWidthAtEnd)
{
    const double width = 3.75;
    const double length = 10;
    const double heading = 0.0;

    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr);

    auto elementUnderTest =
        OWL::Testing::LaneGeometryElementGenerator::TriangularLaneGeometryElement(
    { 0.0, 0.0},  // origin
    width,
    length,
    heading);

    const OWL::Primitive::LaneGeometryJoint& firstJoint = elementUnderTest.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = elementUnderTest.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, 0.0, heading);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, 0.0, heading);

    ASSERT_DOUBLE_EQ(lane.GetWidth(0.0), 0.0);
    ASSERT_DOUBLE_EQ(lane.GetWidth(length), width);
}

TEST(GetLaneWidth, LaneWidthQueryOnTriangularLane_ReturnsCorrectlyInterpolatedWidth)
{
    const double width = 3.75;
    const double length = 10;
    const double heading = 0.0;
    const double queryPosition = 3.0;

    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr);

    auto elementUnderTest =
        OWL::Testing::LaneGeometryElementGenerator::TriangularLaneGeometryElement(
    { 0.0, 0.0},  // origin
    width,
    length,
    heading);

    const OWL::Primitive::LaneGeometryJoint& firstJoint = elementUnderTest.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = elementUnderTest.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, 0.0, heading);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, 0.0, heading);

    ASSERT_DOUBLE_EQ(lane.GetWidth(queryPosition), width / length * queryPosition);
}

TEST(GetLaneWidth, LaneWidthQueryOnRotatedTriangularLane_ReturnsCorrectlyInterpolatedWidth)
{
    const double width = 3.75;
    const double length = 10;
    const double heading = M_PI / 6.0;
    const double queryPosition = 3.0;

    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr);

    auto elementUnderTest =
        OWL::Testing::LaneGeometryElementGenerator::TriangularLaneGeometryElement(
    { 0.0, 0.0},  // origin
    width,
    length,
    heading);

    const OWL::Primitive::LaneGeometryJoint& firstJoint = elementUnderTest.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = elementUnderTest.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, 0.0, heading);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, 0.0, heading);

    ASSERT_DOUBLE_EQ(lane.GetWidth(queryPosition), width / length * queryPosition);
}

TEST(DISABLED_GetLaneWidth, LaneWidthQueryOnRectangularLane_ReturnsFullWidthAtStartAndEnd)
{
    const double width = 3.75;
    const double length = 10;
    const double heading = 0.0;

    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr);

    auto elementUnderTest =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement(
    { 0.0, 0.0},  // origin
    width,
    length,
    heading);

    const OWL::Primitive::LaneGeometryJoint& firstJoint = elementUnderTest.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = elementUnderTest.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, 0.0, heading);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, 0.0, heading);

    ASSERT_DOUBLE_EQ(lane.GetWidth(0.0), width);
    ASSERT_DOUBLE_EQ(lane.GetWidth(length), width);
}

/////////////////////////////////////////////////////////////////////////////

TEST(GetLaneCurvature, LaneCurvatureQueryOnCurvedLane_ReturnsZeroCurvatureAtStartAndFullCurvatureAtEnd)
{
    const double width = 3.75;
    const double length = 10;
    const double heading = 0.0;
    const double curvatureStart = 0.2;
    const double curvatureEnd = 0.5;

    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr);

    auto elementUnderTest =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElementWithCurvature(
    { 0.0, 0.0},  // origin
    width,
    length,
    curvatureStart,
    curvatureEnd,
    heading);

    const OWL::Primitive::LaneGeometryJoint& firstJoint = elementUnderTest.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = elementUnderTest.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    double curvature1 = firstJoint.curvature;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;
    double curvature2 = secondJoint.curvature;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, curvature1, heading);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, curvature2, heading);

    ASSERT_DOUBLE_EQ(lane.GetCurvature(0.0), curvatureStart);
    ASSERT_DOUBLE_EQ(lane.GetCurvature(length), curvatureEnd);
}

TEST(GetLaneCurvature, LaneCurvatureQueryOnCurvedLane_ReturnsCorrectlyInterpolatedCurvature)
{
    const double width = 3.75;
    const double length = 10;
    const double heading = 0.0;
    const double queryPosition = 3.0;

    const double curvatureStart = 0.2;
    const double curvatureEnd = 0.5;

    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr);

    auto elementUnderTest =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElementWithCurvature(
    { 0.0, 0.0},  // origin
    width,
    length,
    curvatureStart,
    curvatureEnd,
    heading);

    const OWL::Primitive::LaneGeometryJoint& firstJoint = elementUnderTest.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = elementUnderTest.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    double curvature1 = firstJoint.curvature;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;
    double curvature2 = secondJoint.curvature;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, curvature1, heading);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, curvature2, heading);

    ASSERT_DOUBLE_EQ(lane.GetCurvature(queryPosition),
                     curvatureStart + (curvatureEnd - curvatureStart) / length * queryPosition);
}

TEST(GetLaneCurvature, LaneCurvatureQueryOnRotatedCurvedLane_ReturnsCorrectlyInterpolatedCurvature)
{
    const double width = 3.75;
    const double length = 10;
    const double heading = M_PI / 6.0;
    const double queryPosition = 3.0;

    const double curvatureStart = 0.2;
    const double curvatureEnd = 0.5;

    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr);

    auto elementUnderTest =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElementWithCurvature(
    { 0.0, 0.0},  // origin
    width,
    length,
    curvatureStart,
    curvatureEnd,
    heading);

    const OWL::Primitive::LaneGeometryJoint& firstJoint = elementUnderTest.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = elementUnderTest.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    double curvature1 = firstJoint.curvature;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;
    double curvature2 = secondJoint.curvature;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, curvature1, heading);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, curvature2, heading);

    ASSERT_DOUBLE_EQ(lane.GetCurvature(queryPosition),
                     curvatureStart + (curvatureEnd - curvatureStart) / length * queryPosition);
}

TEST(GetLaneCurvature, LaneCurvatureQueryOnStraightLane_ReturnsZeroCurvature)
{
    const double width = 3.75;
    const double length = 10;
    const double heading = 0.0;

    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr);

    auto elementUnderTest =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement(
    { 0.0, 0.0},  // origin
    width,
    length,
    heading);

    const OWL::Primitive::LaneGeometryJoint& firstJoint = elementUnderTest.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = elementUnderTest.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    double curvature1 = firstJoint.curvature;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;
    double curvature2 = secondJoint.curvature;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, curvature1, heading);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, curvature2, heading);

    ASSERT_DOUBLE_EQ(lane.GetCurvature(0.0), 0.0);
    ASSERT_DOUBLE_EQ(lane.GetCurvature(length / 2.0), 0.0);
    ASSERT_DOUBLE_EQ(lane.GetCurvature(length), 0.0);
}

/////////////////////////////////////////////////////////////////////////////

TEST(GetLaneDirection, LaneWidthQueryOnRotatedStraightLane_ReturnsCorrectDirection)
{
    const double width = 3.75;
    const double length = 10;
    const double heading = M_PI / 6.0;

    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr);

    auto elementUnderTest =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement(
    { 0.0, 0.0},  // origin
    width,
    length,
    heading);

    const OWL::Primitive::LaneGeometryJoint& firstJoint = elementUnderTest.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = elementUnderTest.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, 0.0, heading);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, 0.0, heading);

    ASSERT_DOUBLE_EQ(lane.GetDirection(0.0), heading);
    ASSERT_DOUBLE_EQ(lane.GetDirection(length / 2.0), heading);
    ASSERT_DOUBLE_EQ(lane.GetDirection(length), heading);
}

TEST(GetLaneDirection, LaneWidthQueryOnRotatedConstantlyCurvedLane_ReturnsCorrectDirection)
{
    const double width = 3.75;
    const double length = 10;
    const double heading = M_PI / 6.0;
    const double curvatureStart = 0.2;
    const double curvatureEnd = 0.2;

    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr);

    auto elementUnderTest =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElementWithCurvature(
    { 0.0, 0.0 },  // origin
    width,
    length,
    curvatureStart,
    curvatureEnd,
    heading);

    const OWL::Primitive::LaneGeometryJoint& firstJoint = elementUnderTest.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = elementUnderTest.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    double curvature1 = firstJoint.curvature;
    double direction1 = firstJoint.sHdg;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;
    double curvature2 = secondJoint.curvature;
    double direction2 = firstJoint.sHdg;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, curvature1, heading);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, curvature2, heading);

    ASSERT_DOUBLE_EQ(lane.GetDirection(0.0), direction1);
    ASSERT_DOUBLE_EQ(lane.GetDirection(length / 2.0), direction1 + (direction2 - direction1) * length / 2.0);
    ASSERT_DOUBLE_EQ(lane.GetDirection(length), direction2);
}

TEST(GetLaneDirection, LaneWidthQueryOnRotatedCurvedLane_ReturnsCorrectDirection)
{
    const double width = 3.75;
    const double length = 10;
    const double heading = M_PI / 6.0;
    const double curvatureStart = 0.2;
    const double curvatureEnd = 0.8;

    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr);

    auto elementUnderTest =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElementWithCurvature(
    { 0.0, 0.0 },  // origin
    width,
    length,
    curvatureStart,
    curvatureEnd,
    heading);

    const OWL::Primitive::LaneGeometryJoint& firstJoint = elementUnderTest.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = elementUnderTest.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    double curvature1 = firstJoint.curvature;
    double direction1 = firstJoint.sHdg;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;
    double curvature2 = secondJoint.curvature;
    double direction2 = firstJoint.sHdg;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, curvature1, heading);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, curvature2, heading);

    ASSERT_DOUBLE_EQ(lane.GetDirection(0.0), direction1);
    ASSERT_DOUBLE_EQ(lane.GetDirection(length / 2.0), direction1 + (direction2 - direction1) * length / 2.0);
    ASSERT_DOUBLE_EQ(lane.GetDirection(length), direction2);
}
