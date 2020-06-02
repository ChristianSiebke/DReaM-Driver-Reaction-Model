/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*               2019 AMFD GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "sensor_driverImplementation.h"
#include "Signals/sensorDriverSignal.h"

#include "fakeAgent.h"
#include "fakeWorldObject.h"
#include "fakeWorld.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::Return;
using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::NiceMock;

TEST(SensorDriver_UnitTests, CorrectInformationInSignal)
{
    NiceMock<FakeAgent> egoAgent;
    NiceMock<FakeWorld> fakeWorld;

    ON_CALL(egoAgent, GetVelocity(VelocityScope::Absolute)).WillByDefault(Return(2.0));
    ON_CALL(egoAgent, GetAcceleration()).WillByDefault(Return(3.0));
    ON_CALL(egoAgent, GetPositionLateral()).WillByDefault(Return(4.0));
    ON_CALL(egoAgent, GetRelativeYaw()).WillByDefault(Return(5.0));
    ON_CALL(egoAgent, IsCrossingLanes()).WillByDefault(Return(true));
    ON_CALL(egoAgent, GetLaneRemainder(Side::Left)).WillByDefault(Return(6.0));
    ON_CALL(egoAgent, GetLaneRemainder(Side::Right)).WillByDefault(Return(7.0));
    std::vector<std::pair<ObjectTypeOSI, int>> collisionPartners = {{ObjectTypeOSI::Vehicle, 1}};
    ON_CALL(egoAgent, GetCollisionPartners()).WillByDefault(Return(collisionPartners));
    ON_CALL(egoAgent, GetMainLaneId(_)).WillByDefault(Return(-2));
    ON_CALL(egoAgent, GetLaneCurvature(0,_)).WillByDefault(Return(0.5));
    ON_CALL(egoAgent, GetLaneCurvature(1,_)).WillByDefault(Return(0.6));
    ON_CALL(egoAgent, GetLaneCurvature(-1,_)).WillByDefault(Return(0.7));
    ON_CALL(egoAgent, GetLaneWidth(0,_)).WillByDefault(Return(5.0));
    ON_CALL(egoAgent, GetLaneWidth(1,_)).WillByDefault(Return(6.0));
    ON_CALL(egoAgent, GetLaneWidth(-1,_)).WillByDefault(Return(7.0));
    ON_CALL(egoAgent, GetDistanceToEndOfLane(_,0)).WillByDefault(Return(100.0));
    ON_CALL(egoAgent, GetDistanceToEndOfLane(_,1)).WillByDefault(Return(200.0));
    ON_CALL(egoAgent, GetDistanceToEndOfLane(_,-1)).WillByDefault(Return(300.0));
    CommonTrafficSign::Entity trafficSign;
    std::vector<CommonTrafficSign::Entity> trafficSigns{{trafficSign}};
    ON_CALL(egoAgent, GetTrafficSignsInRange(_,0)).WillByDefault(Return(trafficSigns));
    ON_CALL(egoAgent, GetTrafficSignsInRange(_,1)).WillByDefault(Return(trafficSigns));
    ON_CALL(egoAgent, GetTrafficSignsInRange(_,-1)).WillByDefault(Return(trafficSigns));
    RelativeWorldView::Lanes relativeLanes {{0.0, 0.0,
            {{-1, true, LaneType::Driving, std::nullopt, std::nullopt},
            {0, true, LaneType::Driving, std::nullopt, std::nullopt},
            {1, true, LaneType::Driving, std::nullopt, std::nullopt}}}};
    ON_CALL(egoAgent, GetRelativeLanes(_)).WillByDefault(Return(relativeLanes));

    ON_CALL(fakeWorld, GetVisibilityDistance()).WillByDefault(Return(123.4));

    NiceMock<FakeAgent> otherAgent;
    std::vector<const WorldObjectInterface *> objectsInFront{{&otherAgent}};
    ON_CALL(egoAgent, GetObjectsInRange(0,0.0,_,_)).WillByDefault(Return(objectsInFront));
    ON_CALL(otherAgent, GetId()).WillByDefault(Return(2));
    ON_CALL(egoAgent, GetDistanceToObject(&otherAgent)).WillByDefault(Return(50.0));
    ON_CALL(otherAgent, GetRelativeYaw()).WillByDefault(Return(0.1));
    ON_CALL(otherAgent, GetLength()).WillByDefault(Return(1.0));
    ON_CALL(otherAgent, GetWidth()).WillByDefault(Return(1.1));
    ON_CALL(otherAgent, GetHeight()).WillByDefault(Return(1.2));
    ON_CALL(otherAgent, GetVelocity()).WillByDefault(Return(10.0));
    ON_CALL(otherAgent, GetAcceleration()).WillByDefault(Return(11.0));
    ON_CALL(otherAgent, GetMainLaneId(_)).WillByDefault(Return(-2));
    ON_CALL(otherAgent, GetPositionLateral()).WillByDefault(Return(2.0));

    NiceMock<FakeWorldObject> trafficObject;
    std::vector<const WorldObjectInterface *> objectsBehind{{&trafficObject}};
    ON_CALL(egoAgent, GetObjectsInRange(1,_,0.0,_)).WillByDefault(Return(objectsBehind));
    ON_CALL(trafficObject, GetId()).WillByDefault(Return(3));
    ON_CALL(egoAgent, GetDistanceToObject(&trafficObject)).WillByDefault(Return(60.0));
    ON_CALL(trafficObject, GetYaw()).WillByDefault(Return(0.2));
    ON_CALL(trafficObject, GetLength()).WillByDefault(Return(2.0));
    ON_CALL(trafficObject, GetWidth()).WillByDefault(Return(2.1));
    ON_CALL(trafficObject, GetHeight()).WillByDefault(Return(2.2));

    SensorDriverImplementation sensorDriver("SensorDriver",
                                            false,
                                            0,
                                            0,
                                            0,
                                            100,
                                            nullptr,
                                            &fakeWorld,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            &egoAgent);

    sensorDriver.Trigger(0);
    std::shared_ptr<const SignalInterface> data;
    sensorDriver.UpdateOutput(0, data, 0);

    std::shared_ptr<const SensorDriverSignal> sensorDriverSignal = std::dynamic_pointer_cast<const SensorDriverSignal>(data);

    auto ownVehicleInformation = sensorDriverSignal->GetOwnVehicleInformation();
    EXPECT_THAT(ownVehicleInformation.velocity, Eq(2.0));
    EXPECT_THAT(ownVehicleInformation.acceleration, Eq(3.0));
    EXPECT_THAT(ownVehicleInformation.lateralPosition, Eq(4.0));
    EXPECT_THAT(ownVehicleInformation.heading, Eq(5.0));
    EXPECT_THAT(ownVehicleInformation.isCrossingLanes, Eq(true));
    EXPECT_THAT(ownVehicleInformation.distanceToLaneBoundaryLeft, Eq(6.0));
    EXPECT_THAT(ownVehicleInformation.distanceToLaneBoundaryRight, Eq(7.0));
    EXPECT_THAT(ownVehicleInformation.collision, Eq(true));

    auto geometryInformation = sensorDriverSignal->GetGeometryInformation();
    EXPECT_THAT(geometryInformation.visibilityDistance, Eq(123.4));
    EXPECT_THAT(geometryInformation.laneEgo.exists, Eq(true));
    EXPECT_THAT(geometryInformation.laneEgo.curvature, Eq(0.5));
    EXPECT_THAT(geometryInformation.laneEgo.width, Eq(5.0));
    EXPECT_THAT(geometryInformation.laneEgo.distanceToEndOfLane, Eq(100.0));
    EXPECT_THAT(geometryInformation.laneLeft.exists, Eq(true));
    EXPECT_THAT(geometryInformation.laneLeft.curvature, Eq(0.6));
    EXPECT_THAT(geometryInformation.laneLeft.width, Eq(6.0));
    EXPECT_THAT(geometryInformation.laneLeft.distanceToEndOfLane, Eq(200.0));
    EXPECT_THAT(geometryInformation.laneRight.exists, Eq(true));
    EXPECT_THAT(geometryInformation.laneRight.curvature, Eq(0.7));
    EXPECT_THAT(geometryInformation.laneRight.width, Eq(7.0));
    EXPECT_THAT(geometryInformation.laneRight.distanceToEndOfLane, Eq(300.0));

    auto trafficRuleInformation = sensorDriverSignal->GetTrafficRuleInformation();
    EXPECT_THAT(trafficRuleInformation.laneEgo.trafficSigns.size(), Eq(1));
    EXPECT_THAT(trafficRuleInformation.laneLeft.trafficSigns.size(), Eq(1));
    EXPECT_THAT(trafficRuleInformation.laneRight.trafficSigns.size(), Eq(1));

    auto surroundingObjects = sensorDriverSignal->GetSurroundingObjects();
    EXPECT_THAT(surroundingObjects.objectFront.exist, Eq(true));
    EXPECT_THAT(surroundingObjects.objectFront.isStatic, Eq(false));
    EXPECT_THAT(surroundingObjects.objectFront.id, Eq(2));
    EXPECT_THAT(surroundingObjects.objectFront.velocity, Eq(10.0));
    EXPECT_THAT(surroundingObjects.objectFront.acceleration, Eq(11.0));
    EXPECT_THAT(surroundingObjects.objectFront.heading, Eq(0.1));
    EXPECT_THAT(surroundingObjects.objectFront.length, Eq(1.0));
    EXPECT_THAT(surroundingObjects.objectFront.width, Eq(1.1));
    EXPECT_THAT(surroundingObjects.objectFront.height, Eq(1.2));
    EXPECT_THAT(surroundingObjects.objectFront.relativeLongitudinalDistance, Eq(50.0));
    EXPECT_THAT(surroundingObjects.objectFront.relativeLateralDistance, DoubleEq(-2.0));
    EXPECT_THAT(surroundingObjects.objectRearLeft.exist, Eq(true));
    EXPECT_THAT(surroundingObjects.objectRearLeft.isStatic, Eq(true));
    EXPECT_THAT(surroundingObjects.objectRearLeft.id, Eq(3));
    EXPECT_THAT(surroundingObjects.objectRearLeft.heading, Eq(0.2));
    EXPECT_THAT(surroundingObjects.objectRearLeft.length, Eq(2.0));
    EXPECT_THAT(surroundingObjects.objectRearLeft.width, Eq(2.1));
    EXPECT_THAT(surroundingObjects.objectRearLeft.height, Eq(2.2));
    EXPECT_THAT(surroundingObjects.objectRearLeft.relativeLongitudinalDistance, Eq(60.0));
    EXPECT_THAT(surroundingObjects.objectRear.exist, Eq(false));
    EXPECT_THAT(surroundingObjects.objectFrontLeft.exist, Eq(false));
    EXPECT_THAT(surroundingObjects.objectFrontRight.exist, Eq(false));
    EXPECT_THAT(surroundingObjects.objectRearRight.exist, Eq(false));
}

TEST(SensorDriverCalculations_UnitTests, GetLateralDistanceToObjectSameLane)
{
    NiceMock<FakeAgent> egoAgent;
    ON_CALL(egoAgent, GetMainLaneId(_)).WillByDefault(Return(-3));
    ON_CALL(egoAgent, GetPositionLateral()).WillByDefault(Return(0.5));

    NiceMock<FakeAgent> otherAgent;
    ON_CALL(otherAgent, GetMainLaneId(_)).WillByDefault(Return(-3));
    ON_CALL(otherAgent, GetPositionLateral()).WillByDefault(Return(-1.0));

    SensorDriverCalculations sensorDriverCalculations(&egoAgent);
    double lateralDistance = sensorDriverCalculations.GetLateralDistanceToObject(&otherAgent);
    ASSERT_THAT(lateralDistance, Eq(-1.5));
}

TEST(SensorDriverCalculations_UnitTests, GetLateralDistanceToObjectLeftLane)
{
    NiceMock<FakeAgent> egoAgent;
    ON_CALL(egoAgent, GetMainLaneId(_)).WillByDefault(Return(-3));
    ON_CALL(egoAgent, GetLaneIdLeft()).WillByDefault(Return(-2));
    ON_CALL(egoAgent, GetPositionLateral()).WillByDefault(Return(0.5));
    ON_CALL(egoAgent, GetLaneWidth(0, _)).WillByDefault(Return(4.0));
    ON_CALL(egoAgent, GetLaneWidth(1, _)).WillByDefault(Return(5.0));

    NiceMock<FakeAgent> otherAgent;
    ON_CALL(otherAgent, GetMainLaneId(_)).WillByDefault(Return(-2));
    ON_CALL(otherAgent, GetPositionLateral()).WillByDefault(Return(-1.0));

    SensorDriverCalculations sensorDriverCalculations(&egoAgent);
    double lateralDistance = sensorDriverCalculations.GetLateralDistanceToObject(&otherAgent);
    ASSERT_THAT(lateralDistance, Eq(3.0));
}

TEST(SensorDriverCalculations_UnitTests, GetLateralDistanceToObjectRightLane)
{
    NiceMock<FakeAgent> egoAgent;
    ON_CALL(egoAgent, GetMainLaneId(_)).WillByDefault(Return(-3));
    ON_CALL(egoAgent, GetLaneIdRight()).WillByDefault(Return(-4));
    ON_CALL(egoAgent, GetPositionLateral()).WillByDefault(Return(0.5));
    ON_CALL(egoAgent, GetLaneWidth(0, _)).WillByDefault(Return(4.0));
    ON_CALL(egoAgent, GetLaneWidth(-1, _)).WillByDefault(Return(5.0));

    NiceMock<FakeAgent> otherAgent;
    ON_CALL(otherAgent, GetMainLaneId(_)).WillByDefault(Return(-4));
    ON_CALL(otherAgent, GetPositionLateral()).WillByDefault(Return(-1.0));

    SensorDriverCalculations sensorDriverCalculations(&egoAgent);
    double lateralDistance = sensorDriverCalculations.GetLateralDistanceToObject(&otherAgent);
    ASSERT_THAT(lateralDistance, Eq(-6.0));
}
