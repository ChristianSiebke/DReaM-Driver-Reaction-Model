/********************************************************************************
 * Copyright (c) 2019 AMFD GmbH
 *               2019-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "sensor_driverImpl.h"
#include "Signals/sensorDriverSignal.h"

#include "fakeAgent.h"
#include "fakeEgoAgent.h"
#include "fakePublisher.h"
#include "fakeWorldObject.h"
#include "fakeWorld.h"
#include "fakeStochastics.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::NiceMock;

bool operator== (const ObjectPosition& rhs, const  ObjectPosition& lhs)
{
    return rhs.mainLocatePoint.begin()->second.roadPosition.s == lhs.mainLocatePoint.begin()->second.roadPosition.s;
}

TEST(SensorDriver_UnitTests, CorrectInformationInSignal)
{
    NiceMock<FakeAgent> fakeAgent;
    NiceMock<FakeEgoAgent> fakeEgoAgent;
    NiceMock<FakeWorld> fakeWorld;
    NiceMock<FakeStochastics> fakeStochastics;
    NiceMock<FakePublisher> fakePublisher;

    ON_CALL(fakeAgent, GetEgoAgent()).WillByDefault(ReturnRef(fakeEgoAgent));
    ON_CALL(fakeEgoAgent, GetAgent()).WillByDefault(Return(&fakeAgent));
    const std::string roadId = "SomeRoad";
    RoadGraph roadGraph;
    RoadGraphVertex start = add_vertex(RouteElement{roadId, true}, roadGraph);
    RoadGraphVertex target = add_vertex(roadGraph);
    RoadGraphEdge edge = add_edge(start, target, roadGraph).first;

    ObjectPosition egoAgentPosition{{}, {{roadId, GlobalRoadPosition{roadId, -2, 50, 4.0, 0.5}}},{{roadId, RoadInterval{{-1}, 0, 0, {6.0, 0.7}}}}};
    ON_CALL(fakeAgent, GetObjectPosition()).WillByDefault(ReturnRef(egoAgentPosition));
    ON_CALL(fakeAgent, GetVelocity(VelocityScope::Absolute)).WillByDefault(Return(2.0));
    ON_CALL(fakeAgent, GetAcceleration()).WillByDefault(Return(3.0));
    ON_CALL(fakeAgent, IsCrossingLanes()).WillByDefault(Return(true));
    ON_CALL(fakeEgoAgent, GetPositionLateral()).WillByDefault(Return(4.0));
    ON_CALL(fakeEgoAgent, GetRelativeYaw()).WillByDefault(Return(5.0));
    ON_CALL(fakeEgoAgent, GetLaneRemainder(Side::Left)).WillByDefault(Return(6.0));
    ON_CALL(fakeEgoAgent, GetLaneRemainder(Side::Right)).WillByDefault(Return(7.0));
    std::vector<std::pair<ObjectTypeOSI, int>> collisionPartners = {{ObjectTypeOSI::Vehicle, 1}};
    ON_CALL(fakeAgent, GetCollisionPartners()).WillByDefault(Return(collisionPartners));
    ON_CALL(fakeAgent, GetRoads(_)).WillByDefault(Return(std::vector<std::string>{roadId}));
    ON_CALL(fakeEgoAgent, GetLaneCurvature(1)).WillByDefault(Return(0.5));
    ON_CALL(fakeEgoAgent, GetLaneCurvature(0)).WillByDefault(Return(0.6));
    ON_CALL(fakeEgoAgent, GetLaneCurvature(-1)).WillByDefault(Return(0.7));
    ON_CALL(fakeEgoAgent, GetLaneWidth(1)).WillByDefault(Return(5.0));
    ON_CALL(fakeEgoAgent, GetLaneWidth(0)).WillByDefault(Return(6.0));
    ON_CALL(fakeEgoAgent, GetLaneWidth(-1)).WillByDefault(Return(7.0));
    ON_CALL(fakeEgoAgent, GetDistanceToEndOfLane(_, 1)).WillByDefault(Return(100.0));
    ON_CALL(fakeEgoAgent, GetDistanceToEndOfLane(_, 0)).WillByDefault(Return(200.0));
    ON_CALL(fakeEgoAgent, GetDistanceToEndOfLane(_, -1)).WillByDefault(Return(300.0));
    CommonTrafficSign::Entity trafficSign;
    std::vector<CommonTrafficSign::Entity> trafficSigns{{trafficSign}};
    ON_CALL(fakeEgoAgent, GetTrafficSignsInRange(_, 1)).WillByDefault(Return(trafficSigns));
    ON_CALL(fakeEgoAgent, GetTrafficSignsInRange(_, 0)).WillByDefault(Return(trafficSigns));
    ON_CALL(fakeEgoAgent, GetTrafficSignsInRange(_, -1)).WillByDefault(Return(trafficSigns));
    LaneMarking::Entity laneMarking;
    std::vector<LaneMarking::Entity> laneMarkings{{laneMarking}};
    ON_CALL(fakeEgoAgent, GetLaneMarkingsInRange(_, _, _)).WillByDefault(Return(laneMarkings));
    RelativeWorldView::Lanes relativeLanes {{0.0, 0.0,
            {{-1, true, LaneType::Driving, std::nullopt, std::nullopt},
            {0, true, LaneType::Driving, std::nullopt, std::nullopt},
            {1, true, LaneType::Driving, std::nullopt, std::nullopt}}}};
    ON_CALL(fakeEgoAgent, GetRelativeLanes(_, _, _)).WillByDefault(Return(relativeLanes));

    ON_CALL(fakeWorld, GetRoadGraph(RouteElement{roadId, true}, _)).WillByDefault(Return(std::make_pair(roadGraph, start)));
    ON_CALL(fakeWorld, IsDirectionalRoadExisting(roadId, true)).WillByDefault(Return(true));
    ON_CALL(fakeWorld, GetVisibilityDistance()).WillByDefault(Return(123.4));
    std::map<RoadGraphEdge, double> edgeWeights{{edge, 1.0}};
    ON_CALL(fakeWorld, GetEdgeWeights(_)).WillByDefault([&edgeWeights](const RoadGraph& graph){auto [firstEdge, edgeEnd] = edges(graph); return std::map<RoadGraphEdge, double>{{*firstEdge, 1.0}};} );

    std::vector<const WorldObjectInterface *> noObjects{};
    EXPECT_CALL(fakeEgoAgent, GetObjectsInRange(_, _, _)).WillRepeatedly(Return(noObjects));

    NiceMock<FakeAgent> otherAgent;
    std::vector<const WorldObjectInterface *> objectsInFront{{&otherAgent}};
    EXPECT_CALL(fakeEgoAgent, GetObjectsInRange(0.0,_,0)).WillRepeatedly(Return(objectsInFront));
    ON_CALL(otherAgent, GetId()).WillByDefault(Return(2));
    ON_CALL(fakeEgoAgent, GetDistanceToObject(&otherAgent)).WillByDefault(Return(LongitudinalDistance{50.0, 52.0}));
    ON_CALL(otherAgent, GetYaw()).WillByDefault(Return(0.1));
    ON_CALL(otherAgent, GetLength()).WillByDefault(Return(1.0));
    ON_CALL(otherAgent, GetWidth()).WillByDefault(Return(1.1));
    ON_CALL(otherAgent, GetHeight()).WillByDefault(Return(1.2));
    ON_CALL(otherAgent, GetVelocity()).WillByDefault(Return(10.0));
    ON_CALL(otherAgent, GetAcceleration()).WillByDefault(Return(11.0));
    ObjectPosition otherAgentPosition{{}, {{roadId, GlobalRoadPosition{roadId, -2, 50, 2.0, 0}}},{}};
    ON_CALL(otherAgent, GetObjectPosition()).WillByDefault(ReturnRef(otherAgentPosition));

    NiceMock<FakeWorldObject> trafficObject;
    std::vector<const WorldObjectInterface *> objectsBehind{{&trafficObject}};
    EXPECT_CALL(fakeEgoAgent, GetObjectsInRange(_,0.0,1)).WillRepeatedly(Return(objectsBehind));
    ON_CALL(trafficObject, GetId()).WillByDefault(Return(3));
    ON_CALL(fakeEgoAgent, GetDistanceToObject(&trafficObject)).WillByDefault(Return(LongitudinalDistance{60.0, 62.0}));
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
                                            &fakeStochastics,
                                            &fakeWorld,
                                            nullptr,
                                            &fakePublisher,
                                            nullptr,
                                            &fakeAgent);

    sensorDriver.Trigger(0);
    std::shared_ptr<const SignalInterface> data;
    sensorDriver.UpdateOutput(0, data, 0);

    std::shared_ptr<const SensorDriverSignal> sensorDriverSignal = std::dynamic_pointer_cast<const SensorDriverSignal>(data);

    auto ownVehicleInformation = sensorDriverSignal->GetOwnVehicleInformation();
    EXPECT_THAT(ownVehicleInformation.absoluteVelocity, Eq(2.0));
    EXPECT_THAT(ownVehicleInformation.acceleration, Eq(3.0));
    EXPECT_THAT(ownVehicleInformation.lateralPosition, Eq(4.0));
    EXPECT_THAT(ownVehicleInformation.heading, Eq(5.0));
    EXPECT_THAT(ownVehicleInformation.distanceToLaneBoundaryLeft, Eq(6.0));
    EXPECT_THAT(ownVehicleInformation.distanceToLaneBoundaryRight, Eq(7.0));
    EXPECT_THAT(ownVehicleInformation.collision, Eq(true));

    auto geometryInformation = sensorDriverSignal->GetGeometryInformation();
    EXPECT_THAT(geometryInformation.visibilityDistance, Eq(123.4));
    EXPECT_THAT(geometryInformation.laneEgo.exists, Eq(true));
    EXPECT_THAT(geometryInformation.laneEgo.curvature, Eq(0.6));
    EXPECT_THAT(geometryInformation.laneEgo.width, Eq(6.0));
    EXPECT_THAT(geometryInformation.laneEgo.distanceToEndOfLane, Eq(200.0));
    EXPECT_THAT(geometryInformation.laneLeft.exists, Eq(true));
    EXPECT_THAT(geometryInformation.laneLeft.curvature, Eq(0.5));
    EXPECT_THAT(geometryInformation.laneLeft.width, Eq(5.0));
    EXPECT_THAT(geometryInformation.laneLeft.distanceToEndOfLane, Eq(100.0));
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
    EXPECT_THAT(surroundingObjects.objectFront.absoluteVelocity, Eq(10.0));
    EXPECT_THAT(surroundingObjects.objectFront.acceleration, Eq(11.0));
    EXPECT_THAT(surroundingObjects.objectFront.heading, Eq(0.1));
    EXPECT_THAT(surroundingObjects.objectFront.length, Eq(1.0));
    EXPECT_THAT(surroundingObjects.objectFront.width, Eq(1.1));
    EXPECT_THAT(surroundingObjects.objectFront.height, Eq(1.2));
    EXPECT_THAT(surroundingObjects.objectFront.relativeLongitudinalDistance, Eq(50.0));
//    EXPECT_THAT(surroundingObjects.objectFront.relativeLateralDistance, DoubleEq(-2.0));
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
    NiceMock<FakeAgent> agent;
    FakeEgoAgent egoAgent;
    ON_CALL(egoAgent, GetLaneIdFromRelative(-1)).WillByDefault(Return(-4));
    ON_CALL(egoAgent, GetLaneIdFromRelative(0)).WillByDefault(Return(-3));
    ON_CALL(egoAgent, GetLaneIdFromRelative(1)).WillByDefault(Return(-2));
    ON_CALL(egoAgent, GetPositionLateral()).WillByDefault(Return(0.5));

    NiceMock<FakeAgent> otherAgent;
    ObjectPosition otherAgentPosition{{{"MyRoad", GlobalRoadPosition{"MyRoad", -3, 50, -1.0, 0}}}, {},{}};
    ON_CALL(otherAgent, GetObjectPosition()).WillByDefault(ReturnRef(otherAgentPosition));

    SensorDriverCalculations sensorDriverCalculations(egoAgent);
    double lateralDistance = sensorDriverCalculations.GetLateralDistanceToObject("MyRoad", &otherAgent);
    ASSERT_THAT(lateralDistance, Eq(-1.5));
}

TEST(SensorDriverCalculations_UnitTests, GetLateralDistanceToObjectLeftLane)
{
    NiceMock<FakeAgent> agent;
    FakeEgoAgent egoAgent;
    ON_CALL(egoAgent, GetLaneIdFromRelative(-1)).WillByDefault(Return(-4));
    ON_CALL(egoAgent, GetLaneIdFromRelative(0)).WillByDefault(Return(-3));
    ON_CALL(egoAgent, GetLaneIdFromRelative(1)).WillByDefault(Return(-2));
    ObjectPosition position{{},{{"MyRoad",GlobalRoadPosition{"",-3,0,0.5,0}},{}}};
    ON_CALL(agent, GetObjectPosition()).WillByDefault(ReturnRef(position));
    ON_CALL(egoAgent, GetPositionLateral()).WillByDefault(Return(0.5));
    ON_CALL(egoAgent, GetLaneWidth(0)).WillByDefault(Return(4.0));
    ON_CALL(egoAgent, GetLaneWidth(1)).WillByDefault(Return(5.0));

    NiceMock<FakeAgent> otherAgent;
    ObjectPosition otherAgentPosition{{{"MyRoad", GlobalRoadPosition{"MyRoad", -2, 50, -1.0, 0}}}, {},{}};
    ON_CALL(otherAgent, GetObjectPosition()).WillByDefault(ReturnRef(otherAgentPosition));

    SensorDriverCalculations sensorDriverCalculations(egoAgent);
    double lateralDistance = sensorDriverCalculations.GetLateralDistanceToObject("MyRoad", &otherAgent);
    ASSERT_THAT(lateralDistance, Eq(3.0));
}

TEST(SensorDriverCalculations_UnitTests, GetLateralDistanceToObjectRightLane)
{
    NiceMock<FakeAgent> agent;
    FakeEgoAgent egoAgent;
    ON_CALL(egoAgent, GetLaneIdFromRelative(-1)).WillByDefault(Return(-4));
    ON_CALL(egoAgent, GetLaneIdFromRelative(0)).WillByDefault(Return(-3));
    ON_CALL(egoAgent, GetLaneIdFromRelative(1)).WillByDefault(Return(-2));
    ObjectPosition position{{},{{"MyRoad",GlobalRoadPosition{"",-3,0,0.5,0}},{}}};
    ON_CALL(agent, GetObjectPosition()).WillByDefault(ReturnRef(position));
    ON_CALL(egoAgent, GetPositionLateral()).WillByDefault(Return(0.5));
    ON_CALL(egoAgent, GetLaneWidth(0)).WillByDefault(Return(4.0));
    ON_CALL(egoAgent, GetLaneWidth(-1)).WillByDefault(Return(5.0));

    NiceMock<FakeAgent> otherAgent;
    ObjectPosition otherAgentPosition{{{"MyRoad", GlobalRoadPosition{"MyRoad", -4, 50, -1.0, 0}}}, {},{}};
    ON_CALL(otherAgent, GetObjectPosition()).WillByDefault(ReturnRef(otherAgentPosition));

    SensorDriverCalculations sensorDriverCalculations(egoAgent);
    double lateralDistance = sensorDriverCalculations.GetLateralDistanceToObject("MyRoad", &otherAgent);
    ASSERT_THAT(lateralDistance, Eq(-6.0));
}
