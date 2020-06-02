/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "osi3/osi_detectedobject.pb.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "fakeAgent.h"

#include "boundingBoxCalculation.h"


using ::testing::Return;
using ::testing::Lt;
using ::testing::NiceMock;

class BoundingBoxCalculation_Tests : public ::testing::Test
{
public:
    BoundingBoxCalculation_Tests()
    {
        ON_CALL(fakeEgoAgent, GetLength()).WillByDefault(Return(2.0));
        ON_CALL(fakeEgoAgent, GetWidth()).WillByDefault(Return(1.0));
        ON_CALL(fakeEgoAgent, GetDistanceReferencePointToLeadingEdge()).WillByDefault(Return(1.0));
    }

    void SetEgoValues (double velocity, double acceleration, double yawRate)
    {
        ON_CALL(fakeEgoAgent, GetVelocity()).WillByDefault(Return(velocity));
        ON_CALL(fakeEgoAgent, GetAcceleration()).WillByDefault(Return(acceleration));
        ON_CALL(fakeEgoAgent, GetYawRate()).WillByDefault(Return(yawRate));
    }

    NiceMock<FakeAgent> fakeEgoAgent;
};

TEST_F(BoundingBoxCalculation_Tests, MovingObjectAtTimestampZero)
{
    SetEgoValues(0.0, 0.0, 0.0);
    osi3::BaseMoving baseMoving;
    baseMoving.mutable_dimension()->set_length(3.0);
    baseMoving.mutable_dimension()->set_width(3.5);
    baseMoving.mutable_position()->set_x(-20.0);
    baseMoving.mutable_position()->set_y(35.0);
    baseMoving.mutable_orientation()->set_yaw(M_PI * 0.5);
    BoundingBoxCalculation bbCalculation(&fakeEgoAgent, 1.0, 1.5);

    double boxPoints[][2]
    {
        { -22.5, 33 },
        { -22.5, 37 },
        { -17.5, 37 },
        { -17.5, 33 },
        { -22.5, 33 }
    };

    polygon_t expectedBox;
    bg::append(expectedBox, boxPoints);
    bg::correct(expectedBox);

    polygon_t result = bbCalculation.CalculateBoundingBox(0.0, baseMoving);
    bg::correct(result);

    multi_polygon_t differenceMissing;
    bg::difference(expectedBox, result, differenceMissing);
    multi_polygon_t differenceOverhanging;
    bg::difference(result, expectedBox, differenceOverhanging);

    EXPECT_THAT(bg::area(differenceMissing), Lt(0.1));
    EXPECT_THAT(bg::area(differenceOverhanging), Lt(0.1));
}

TEST_F(BoundingBoxCalculation_Tests, MovingObjectMovingInStraightLine)
{
    SetEgoValues(0.0, 0.0, 0.0);
    osi3::BaseMoving baseMoving;
    baseMoving.mutable_dimension()->set_length(3.0);
    baseMoving.mutable_dimension()->set_width(3.5);
    baseMoving.mutable_position()->set_x(-20.0);
    baseMoving.mutable_position()->set_y(35.0);
    baseMoving.mutable_orientation()->set_yaw(M_PI * 0.5);
    baseMoving.mutable_velocity()->set_y(10.0);
    baseMoving.mutable_acceleration()->set_y(-2.0);
    BoundingBoxCalculation bbCalculation(&fakeEgoAgent, 1.0, 1.5);

    double boxPoints[][2]
    {
        { -22.5, 49 },
        { -22.5, 53 },
        { -17.5, 53 },
        { -17.5, 49 },
        { -22.5, 49 }
    };

    polygon_t expectedBox;
    bg::append(expectedBox, boxPoints);
    bg::correct(expectedBox);

    polygon_t result = bbCalculation.CalculateBoundingBox(2.0, baseMoving);
    bg::correct(result);

    multi_polygon_t differenceMissing;
    bg::difference(expectedBox, result, differenceMissing);
    multi_polygon_t differenceOverhanging;
    bg::difference(result, expectedBox, differenceOverhanging);

    EXPECT_THAT(bg::area(differenceMissing), Lt(0.1));
    EXPECT_THAT(bg::area(differenceOverhanging), Lt(0.1));
}

TEST_F(BoundingBoxCalculation_Tests, MovingObjectMovingInCircleLeft)
{
    SetEgoValues(0.0, 0.0, 0.0);
    osi3::BaseMoving baseMoving;
    baseMoving.mutable_dimension()->set_length(3.0);
    baseMoving.mutable_dimension()->set_width(3.5);
    baseMoving.mutable_position()->set_x(-20.0);
    baseMoving.mutable_position()->set_y(35.0);
    baseMoving.mutable_orientation()->set_yaw(M_PI * 0.5);
    baseMoving.mutable_orientation_rate()->set_yaw(M_PI * 0.25);
    baseMoving.mutable_velocity()->set_y(M_PI * 2.5);
    baseMoving.mutable_acceleration()->set_y(0.0);
    BoundingBoxCalculation bbCalculation(&fakeEgoAgent, 1.0, 1.5);

    double boxPoints[][2]
    {
        { -32, 42.5 },
        { -32, 47.5 },
        { -28, 47.5 },
        { -28, 42.5 },
        { -32, 42.5 }
    };

    polygon_t expectedBox;
    bg::append(expectedBox, boxPoints);
    bg::correct(expectedBox);

    polygon_t result = bbCalculation.CalculateBoundingBox(2.0, baseMoving);
    bg::correct(result);

    multi_polygon_t differenceMissing;
    bg::difference(expectedBox, result, differenceMissing);
    multi_polygon_t differenceOverhanging;
    bg::difference(result, expectedBox, differenceOverhanging);

    EXPECT_THAT(bg::area(differenceMissing), Lt(0.1));
    EXPECT_THAT(bg::area(differenceOverhanging), Lt(0.1));
}

TEST_F(BoundingBoxCalculation_Tests, MovingObjectMovingInCircleRight)
{
    SetEgoValues(0.0, 0.0, 0.0);
    osi3::BaseMoving baseMoving;
    baseMoving.mutable_dimension()->set_length(3.0);
    baseMoving.mutable_dimension()->set_width(3.5);
    baseMoving.mutable_position()->set_x(-20.0);
    baseMoving.mutable_position()->set_y(35.0);
    baseMoving.mutable_orientation()->set_yaw(M_PI * 0.5);
    baseMoving.mutable_orientation_rate()->set_yaw(-M_PI * 0.25);
    baseMoving.mutable_velocity()->set_y(M_PI * 2.5);
    baseMoving.mutable_acceleration()->set_y(0.0);
    BoundingBoxCalculation bbCalculation(&fakeEgoAgent, 1.0, 1.5);

    double boxPoints[][2]
    {
        { -12, 42.5 },
        { -12, 47.5 },
        { -8, 47.5 },
        { -8, 42.5 },
        { -12, 42.5 }
    };

    polygon_t expectedBox;
    bg::append(expectedBox, boxPoints);
    bg::correct(expectedBox);

    polygon_t result = bbCalculation.CalculateBoundingBox(2.0, baseMoving);
    bg::correct(result);

    multi_polygon_t differenceMissing;
    bg::difference(expectedBox, result, differenceMissing);
    multi_polygon_t differenceOverhanging;
    bg::difference(result, expectedBox, differenceOverhanging);

    EXPECT_THAT(bg::area(differenceMissing), Lt(0.1));
    EXPECT_THAT(bg::area(differenceOverhanging), Lt(0.1));
}

TEST_F(BoundingBoxCalculation_Tests, MovingObjectOwnAgentMovesInStraightLine)
{
    SetEgoValues(10.0, 2.0, 0.0);
    osi3::BaseMoving baseMoving;
    baseMoving.mutable_dimension()->set_length(3.0);
    baseMoving.mutable_dimension()->set_width(3.5);
    baseMoving.mutable_position()->set_x(-20.0);
    baseMoving.mutable_position()->set_y(35.0);
    baseMoving.mutable_orientation()->set_yaw(-M_PI * 0.5);
    baseMoving.mutable_velocity()->set_x(-10.0);
    baseMoving.mutable_velocity()->set_y(-10.0);
    baseMoving.mutable_acceleration()->set_y(1.0);
    baseMoving.mutable_acceleration()->set_x(-2.0);
    BoundingBoxCalculation bbCalculation(&fakeEgoAgent, 1.0, 1.5);

    double boxPoints[][2]
    {
        { -22.5, 15 },
        { -22.5, 19 },
        { -17.5, 19 },
        { -17.5, 15 },
        { -22.5, 15 }
    };

    polygon_t expectedBox;
    bg::append(expectedBox, boxPoints);
    bg::correct(expectedBox);

    polygon_t result = bbCalculation.CalculateBoundingBox(2.0, baseMoving);
    bg::correct(result);

    multi_polygon_t differenceMissing;
    bg::difference(expectedBox, result, differenceMissing);
    multi_polygon_t differenceOverhanging;
    bg::difference(result, expectedBox, differenceOverhanging);

    EXPECT_THAT(bg::area(differenceMissing), Lt(0.1));
    EXPECT_THAT(bg::area(differenceOverhanging), Lt(0.1));
}

TEST_F(BoundingBoxCalculation_Tests, MovingObjectBothAgentsMovingInCircle)
{
    SetEgoValues(M_PI * 2.5, 0.0, M_PI * 0.25);
    osi3::BaseMoving baseMoving;
    baseMoving.mutable_dimension()->set_length(3.0);
    baseMoving.mutable_dimension()->set_width(3.5);
    baseMoving.mutable_position()->set_x(-20.0);
    baseMoving.mutable_position()->set_y(35.0);
    baseMoving.mutable_orientation()->set_yaw(0.0);
    baseMoving.mutable_orientation_rate()->set_yaw(-M_PI * 0.5);
    BoundingBoxCalculation bbCalculation(&fakeEgoAgent, 1.0, 1.5);

    double boxPoints[][2]
    {
        { -12.5, 23 },
        { -12.5, 27 },
        { -7.5, 27 },
        { -7.5, 23 },
        { -12.5, 23 }
    };

    polygon_t expectedBox;
    bg::append(expectedBox, boxPoints);
    bg::correct(expectedBox);

    polygon_t result = bbCalculation.CalculateBoundingBox(2.0, baseMoving);
    bg::correct(result);

    multi_polygon_t differenceMissing;
    bg::difference(expectedBox, result, differenceMissing);
    multi_polygon_t differenceOverhanging;
    bg::difference(result, expectedBox, differenceOverhanging);

    EXPECT_THAT(bg::area(differenceMissing), Lt(0.1));
    EXPECT_THAT(bg::area(differenceOverhanging), Lt(0.1));
}

TEST_F(BoundingBoxCalculation_Tests, OwnAgentMovingInStraightLine)
{
    SetEgoValues(10.0, 2.0, 0.0);
    BoundingBoxCalculation bbCalculation(&fakeEgoAgent, 1.0, 1.0);

    double boxPoints[][2]
    {
        { 22.5, -1 },
        { 22.5, 1 },
        { 25.5, 1 },
        { 25.5, -1 },
        { 22.5, -1 }
    };

    polygon_t expectedBox;
    bg::append(expectedBox, boxPoints);
    bg::correct(expectedBox);

    polygon_t result = bbCalculation.CalculateOwnBoundingBox(2.0);
    bg::correct(result);

    multi_polygon_t differenceMissing;
    bg::difference(expectedBox, result, differenceMissing);
    multi_polygon_t differenceOverhanging;
    bg::difference(result, expectedBox, differenceOverhanging);

    EXPECT_THAT(bg::area(differenceMissing), Lt(0.1));
    EXPECT_THAT(bg::area(differenceOverhanging), Lt(0.1));
}

TEST_F(BoundingBoxCalculation_Tests, OwnAgentMovingInCircle)
{
    SetEgoValues(M_PI * 2.5, 0.0, M_PI * 0.25);
    BoundingBoxCalculation bbCalculation(&fakeEgoAgent, 1.0, 1.0);

    double boxPoints[][2]
    {
        { 9, 8.5 },
        { 9, 11.5 },
        { 11, 11.5 },
        { 11, 8.5 },
        { 9, 8.5 }
    };

    polygon_t expectedBox;
    bg::append(expectedBox, boxPoints);
    bg::correct(expectedBox);

    polygon_t result = bbCalculation.CalculateOwnBoundingBox(2.0);
    bg::correct(result);

    multi_polygon_t differenceMissing;
    bg::difference(expectedBox, result, differenceMissing);
    multi_polygon_t differenceOverhanging;
    bg::difference(result, expectedBox, differenceOverhanging);

    EXPECT_THAT(bg::area(differenceMissing), Lt(0.1));
    EXPECT_THAT(bg::area(differenceOverhanging), Lt(0.1));
}

TEST_F(BoundingBoxCalculation_Tests, StationaryObject)
{
    osi3::BaseStationary baseStationary;
    baseStationary.mutable_dimension()->set_length(3.0);
    baseStationary.mutable_dimension()->set_width(4.0);
    baseStationary.mutable_position()->set_x(20.0);
    baseStationary.mutable_position()->set_y(-10.0);
    baseStationary.mutable_orientation()->set_yaw(-M_PI * 0.5);

    BoundingBoxCalculation bbCalculation(&fakeEgoAgent, 1.0, 2.0);

    double boxPoints[][2]
    {
        { 17, -12 },
        { 17, -8 },
        { 23, -8 },
        { 23, -12 },
        { 17, -12 }
    };

    polygon_t expectedBox;
    bg::append(expectedBox, boxPoints);
    bg::correct(expectedBox);

    polygon_t result = bbCalculation.CalculateBoundingBox(baseStationary);
    bg::correct(result);

    multi_polygon_t differenceMissing;
    bg::difference(expectedBox, result, differenceMissing);
    multi_polygon_t differenceOverhanging;
    bg::difference(result, expectedBox, differenceOverhanging);

    EXPECT_THAT(bg::area(differenceMissing), Lt(0.1));
    EXPECT_THAT(bg::area(differenceOverhanging), Lt(0.1));
}
