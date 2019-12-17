/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "Common/globalDefinitions.h"
#include "gmock/gmock.h"
#include "fakeWorldObject.h"
#include "Interfaces/agentInterface.h"

class FakeAgent : public FakeWorldObject, public AgentInterface
{
  public:
    MOCK_CONST_METHOD0(GetAgentId, int());
    MOCK_CONST_METHOD0(GetSpawnTime, int());
    MOCK_CONST_METHOD0(GetVehicleType, AgentVehicleType());
    MOCK_CONST_METHOD0(GetVehicleModelType, std::string());
    MOCK_CONST_METHOD0(GetVehicleModelParameters, VehicleModelParameters());
    MOCK_CONST_METHOD0(GetDriverProfileName, std::string());
    MOCK_CONST_METHOD0(GetScenarioName, std::string());
    MOCK_CONST_METHOD0(GetAgentCategory, AgentCategory());
    MOCK_CONST_METHOD0(GetAgentTypeName, std::string());
    MOCK_CONST_METHOD0(IsEgoAgent, bool());
    MOCK_CONST_METHOD0(GetVelocityX, double());
    MOCK_CONST_METHOD0(GetVelocityY, double());
    MOCK_CONST_METHOD0(GetDistanceCOGtoFrontAxle, double());
    MOCK_CONST_METHOD0(GetWeight, double());
    MOCK_CONST_METHOD0(GetHeightCOG, double());
    MOCK_CONST_METHOD0(GetWheelbase, double());
    MOCK_CONST_METHOD0(GetMomentInertiaRoll, double());
    MOCK_CONST_METHOD0(GetMomentInertiaPitch, double());
    MOCK_CONST_METHOD0(GetMomentInertiaYaw, double());
    MOCK_CONST_METHOD0(GetFrictionCoeff, double());
    MOCK_CONST_METHOD0(GetTrackWidth, double());
    MOCK_CONST_METHOD0(GetGear, int());
    MOCK_CONST_METHOD0(GetDistanceCOGtoLeadingEdge, double());
    MOCK_CONST_METHOD0(GetAccelerationX, double());
    MOCK_CONST_METHOD0(GetAccelerationY, double());
    MOCK_CONST_METHOD0(GetRelativeYaw, double());
    MOCK_CONST_METHOD2(GetCollisionData, std::vector<void *>(int collisionPartnerId, int collisionDataId));
    MOCK_CONST_METHOD0(GetCollisionPartners, std::vector<CollisionPartner>());
    MOCK_METHOD1(SetPositionX, void(double positionX));
    MOCK_METHOD1(SetPositionY, void(double positionY));
    MOCK_METHOD1(SetWidth, void(double width));
    MOCK_METHOD1(SetLength, void(double length));
    MOCK_METHOD1(SetHeight, void(double height));
    MOCK_METHOD1(SetVelocityX, void(double velocityX));
    MOCK_METHOD1(SetVelocityY, void(double velocityY));
    MOCK_METHOD1(SetVelocity, void(double value));
    MOCK_METHOD1(SetAcceleration, void(double value));
    MOCK_METHOD1(SetYaw, void(double value));
    MOCK_METHOD1(SetDistanceTraveled, void(double distanceTraveled));
    MOCK_CONST_METHOD0(GetDistanceTraveled, double());
    MOCK_METHOD1(SetDistanceCOGtoFrontAxle, void(double distanceCOGtoFrontAxle));
    MOCK_METHOD1(SetGear, void(int gear));
    MOCK_METHOD1(SetEngineSpeed, void(double engineSpeed));
    MOCK_METHOD1(SetEffAccelPedal, void(double percent));
    MOCK_METHOD1(SetEffBrakePedal, void(double percent));
    MOCK_METHOD1(SetSteeringWheelAngle, void(double steeringWheelAngle));
    MOCK_METHOD1(SetWeight, void(double weight));
    MOCK_METHOD1(SetHeightCOG, void(double heightCOG));
    MOCK_METHOD1(SetDistanceReferencePointToFrontAxle, void(double distanceReferencePointToFrontAxle));
    MOCK_METHOD1(SetDistanceReferencePointToLeadingEdge, void(double distanceReferencePointToLeadingEdge));
    MOCK_METHOD1(SetWheelbase, void(double wheelbase));
    MOCK_METHOD1(SetSteeringRatio, void(double steeringRatio));
    MOCK_METHOD1(SetMomentInertiaRoll, void(double momentInertiaRoll));
    MOCK_METHOD1(SetMomentInertiaPitch, void(double momentInertiaPitch));
    MOCK_METHOD1(SetMomentInertiaYaw, void(double momentInertiaYaw));
    MOCK_METHOD1(SetMaxAcceleration, void(double maxAcceleration));
    MOCK_METHOD1(SetMaxDeceleration, void(double maxDeceleration));
    MOCK_METHOD1(SetFrictionCoeff, void(double frictionCoeff));
    MOCK_METHOD1(SetTrackWidth, void(double trackWidth));
    MOCK_METHOD1(SetDistanceCOGtoLeadingEdge, void(double distanceCOGtoLeadingEdge));
    MOCK_METHOD1(SetAccelerationX, void(double accelerationX));
    MOCK_METHOD1(SetAccelerationY, void(double accelerationY));
    MOCK_METHOD0(RemoveAgent, void());
    MOCK_METHOD1(UpdateCollision, void(CollisionPartner collisionPartner));
    MOCK_METHOD0(Unlocate, void());
    MOCK_METHOD0(Locate, bool());
    MOCK_METHOD0(Update, bool());
    MOCK_METHOD1(SetBrakeLight, void(bool brakeLightStatus));
    MOCK_CONST_METHOD0(GetBrakeLight, bool());
    MOCK_METHOD1(SetIndicatorState, void(IndicatorState indicatorState));
    MOCK_CONST_METHOD0(GetIndicatorState, IndicatorState());
    MOCK_METHOD1(SetHorn, void(bool hornSwitch));
    MOCK_CONST_METHOD0(GetHorn, bool());
    MOCK_METHOD1(SetHeadLight, void(bool headLightSwitch));
    MOCK_CONST_METHOD0(GetHeadLight, bool());
    MOCK_METHOD1(SetHighBeamLight, void(bool headLightSwitch));
    MOCK_CONST_METHOD0(GetHighBeamLight, bool());
    MOCK_CONST_METHOD0(GetLightState, LightState());
    MOCK_METHOD1(SetFlasher, void(bool flasherSwitch));
    MOCK_CONST_METHOD0(GetFlasher, bool());
    MOCK_METHOD5(InitAgentParameter, bool(int id, int agentTypeId, int spawnTime, const AgentSpawnItem *agentSpawnItem,
                                          const SpawnItemParameterInterface &spawnItemParameter));
    MOCK_METHOD3(InitAgentParameter, bool(int id, int spawnTime, AgentBlueprintInterface *agentBlueprint));
    MOCK_CONST_METHOD0(IsValid, bool());
    MOCK_CONST_METHOD0(GetAgentTypeId, int());
    MOCK_CONST_METHOD1(GetRoadId, std::string(MeasurementPoint mp));
    MOCK_CONST_METHOD1(GetMainLaneId, int(MeasurementPoint mp));
    MOCK_METHOD0(GetSecondaryCoveredLanes, std::set<int>());
    MOCK_CONST_METHOD0(GetLaneIdLeft, int());
    MOCK_CONST_METHOD0(GetLaneIdRight, int());
    MOCK_CONST_METHOD0(IsAgentInWorld, bool());
    MOCK_METHOD0(IsAgentAtEndOfRoad, bool());
    MOCK_METHOD1(SetPosition, void(Position pos));
    MOCK_CONST_METHOD0(GetDistanceToStartOfRoad, double());
    MOCK_CONST_METHOD2(GetLaneWidth, double(int relativeLane, double distance));
    MOCK_CONST_METHOD0(GetLaneWidthRightDrivingAndStopLane, double());
    MOCK_CONST_METHOD2(GetLaneCurvature, double(int relativeLane, double distance));
    MOCK_CONST_METHOD2(GetLaneDirection, double(int relativeLane, double distance));
    MOCK_METHOD1(GetDistanceToFrontAgent, double(int laneId));
    MOCK_METHOD1(GetDistanceToRearAgent, double(int laneId));
    MOCK_CONST_METHOD1(GetAgentInFront, const AgentInterface*(int laneId));
    MOCK_CONST_METHOD1(GetAgentBehind, const AgentInterface*(int laneId));
    MOCK_CONST_METHOD1(GetDistanceToObject, double(const WorldObjectInterface *otherObject));
    MOCK_METHOD0(RemoveSpecialAgentMarker, void());
    MOCK_METHOD0(SetSpecialAgentMarker, void());
    MOCK_CONST_METHOD0(ExistsLaneLeft, bool());
    MOCK_CONST_METHOD0(ExistsLaneRight, bool());
    MOCK_CONST_METHOD2(IsLaneDrivingLane, bool(int laneId, double distance));
    MOCK_CONST_METHOD2(IsLaneStopLane, bool(int laneId, double distance));
    MOCK_CONST_METHOD2(IsLaneExitLane, bool(int laneId, double distance));
    MOCK_CONST_METHOD2(IsLaneRamp, bool(int laneId, double distance));
    MOCK_METHOD0(SetObstacleFlag, void());
    MOCK_METHOD0(GetDistanceToSpecialAgent, double());
    MOCK_METHOD0(IsObstacle, bool());
    MOCK_CONST_METHOD2(GetDistanceToEndOfLane, double(double sightDistance, int relativeLane));
    MOCK_CONST_METHOD2(GetDistanceToEndOfExit, double(int laneID, double sightDistance));
    MOCK_CONST_METHOD2(GetDistanceToEndOfRamp, double(int laneID, double sightDistance));
    MOCK_CONST_METHOD0(GetPositionLateral, double());
    MOCK_CONST_METHOD0(IsLeavingWorld, bool());
    MOCK_CONST_METHOD0(IsCrossingLanes, bool());
    MOCK_METHOD0(GetNumberOfLanes, int());
    MOCK_METHOD0(GetDistanceFrontAgentToEgo, double());
    MOCK_METHOD0(HasTwoLeftLanes, bool());
    MOCK_METHOD0(HasTwoRightLanes, bool());
    MOCK_METHOD1(EstimateLaneChangeState, LaneChangeState(double thresholdLooming));
    MOCK_METHOD4(GetAllAgentsInLane,
                 std::list<AgentInterface *>(int laneID, double minDistance, double maxDistance, double AccSensDist));
    MOCK_CONST_METHOD0(IsBicycle, bool());
    MOCK_CONST_METHOD0(Unregister, void());
    MOCK_CONST_METHOD0(IsFirstCarInLane, bool());
    MOCK_CONST_METHOD2(GetObjectInFront, WorldObjectInterface *(double previewDistance, int relativeLaneId));
    MOCK_CONST_METHOD2(GetObjectBehind, WorldObjectInterface *(double previewDistance, int relativeLaneId));
    MOCK_CONST_METHOD0(GetAllAgentsInFront, std::vector<AgentInterface *>());
    MOCK_CONST_METHOD0(GetAllWorldObjectsInFront, std::vector<const WorldObjectInterface *>());
    MOCK_CONST_METHOD4(GetObjectsInRange,
                       std::vector<const WorldObjectInterface *>(int relativeLane, double backwardsRange,
                                                                 double forwardRange, MeasurementPoint mp));
    MOCK_CONST_METHOD4(GetAgentsInRange, std::vector<const AgentInterface *>(int relativeLane, double backwardsRange,
                                                                             double forwardRange, MeasurementPoint mp));
    MOCK_CONST_METHOD3(GetAgentsInRangeAbsolute,
                       std::vector<const AgentInterface *>(int laneId, double minDistance, double maxDistance));
    MOCK_CONST_METHOD0(GetTypeOfNearestMark, MarkType());
    MOCK_CONST_METHOD0(GetTypeOfNearestMarkString, std::string());
    MOCK_CONST_METHOD1(GetDistanceToNearestMark, double(MarkType markType));
    MOCK_CONST_METHOD1(GetOrientationOfNearestMark, double(MarkType markType));
    MOCK_CONST_METHOD1(GetViewDirectionToNearestMark, double(MarkType markType));
    MOCK_CONST_METHOD1(GetAgentViewDirectionToNearestMark, AgentViewDirection(MarkType markType));
    MOCK_CONST_METHOD2(GetDistanceToNearestMarkInViewDirection,
                       double(MarkType markType, AgentViewDirection agentViewDirection));
    MOCK_CONST_METHOD2(GetDistanceToNearestMarkInViewDirection, double(MarkType markType, double mainViewDirection));
    MOCK_CONST_METHOD2(GetOrientationOfNearestMarkInViewDirection,
                       double(MarkType markType, AgentViewDirection agentViewDirection));
    MOCK_CONST_METHOD2(GetOrientationOfNearestMarkInViewDirection, double(MarkType markType, double mainViewDirection));
    MOCK_CONST_METHOD3(GetDistanceToNearestMarkInViewRange,
                       double(MarkType markType, AgentViewDirection agentViewDirection, double range));
    MOCK_CONST_METHOD3(GetDistanceToNearestMarkInViewRange,
                       double(MarkType markType, double mainViewDirection, double range));
    MOCK_CONST_METHOD3(GetOrientationOfNearestMarkInViewRange,
                       double(MarkType markType, AgentViewDirection agentViewDirection, double range));
    MOCK_CONST_METHOD3(GetOrientationOfNearestMarkInViewRange,
                       double(MarkType markType, double mainViewDirection, double range));
    MOCK_CONST_METHOD3(GetViewDirectionToNearestMarkInViewRange,
                       double(MarkType markType, AgentViewDirection agentViewDirection, double range));
    MOCK_CONST_METHOD3(GetViewDirectionToNearestMarkInViewRange,
                       double(MarkType markType, double mainViewDirection, double range));
    MOCK_CONST_METHOD2(GetTypeOfNearestObject, std::string(AgentViewDirection agentViewDirection, double range));
    MOCK_CONST_METHOD2(GetTypeOfNearestObject, std::string(double mainViewDirection, double range));
    MOCK_CONST_METHOD3(GetDistanceToNearestObjectInViewRange,
                       double(ObjectType objectType, AgentViewDirection agentViewDirection, double range));
    MOCK_CONST_METHOD3(GetDistanceToNearestObjectInViewRange,
                       double(ObjectType objectType, double mainViewDirection, double range));
    MOCK_CONST_METHOD3(GetViewDirectionToNearestObjectInViewRange,
                       double(ObjectType objectType, AgentViewDirection agentViewDirection, double range));
    MOCK_CONST_METHOD3(GetViewDirectionToNearestObjectInViewRange,
                       double(ObjectType objectType, double mainViewDirection, double range));
    MOCK_CONST_METHOD2(GetIdOfNearestAgent, int(AgentViewDirection agentViewDirection, double range));
    MOCK_CONST_METHOD2(GetIdOfNearestAgent, int(double mainViewDirection, double range));
    MOCK_CONST_METHOD2(GetDistanceToNearestAgentInViewRange,
                       double(AgentViewDirection agentViewDirection, double range));
    MOCK_CONST_METHOD2(GetDistanceToNearestAgentInViewRange, double(double mainViewDirection, double range));
    MOCK_CONST_METHOD2(GetViewDirectionToNearestAgentInViewRange,
                       double(AgentViewDirection agentViewDirection, double range));
    MOCK_CONST_METHOD2(GetViewDirectionToNearestAgentInViewRange, double(double mainViewDirection, double range));
    MOCK_CONST_METHOD2(GetVisibilityToNearestAgentInViewRange, double(double mainViewDirection, double range));
    MOCK_CONST_METHOD0(GetYawRate, double());
    MOCK_METHOD1(SetYawRate, void(double yawRate));
    MOCK_CONST_METHOD0(GetCentripetalAcceleration, double());
    MOCK_METHOD1(SetCentripetalAcceleration, void(double centripetalAcceleration));
    MOCK_METHOD0(GetYawAcceleration, double());
    MOCK_METHOD1(SetYawAcceleration, void(double yawAcceleration));
    MOCK_CONST_METHOD0(GetTrajectoryTime, const std::vector<int>*());
    MOCK_CONST_METHOD0(GetTrajectoryXPos, const std::vector<double>*());
    MOCK_CONST_METHOD0(GetTrajectoryYPos, const std::vector<double>*());
    MOCK_CONST_METHOD0(GetTrajectoryVelocity, const std::vector<double>*());
    MOCK_CONST_METHOD0(GetTrajectoryAngle, const std::vector<double>*());
    MOCK_METHOD1(SetAccelerationIntention, void(double accelerationIntention));
    MOCK_CONST_METHOD0(GetAccelerationIntention, double());
    MOCK_METHOD1(SetDecelerationIntention, void(double decelerationIntention));
    MOCK_CONST_METHOD0(GetDecelerationIntention, double());
    MOCK_METHOD1(SetAngleIntention, void(double angleIntention));
    MOCK_CONST_METHOD0(GetAngleIntention, double());
    MOCK_METHOD1(SetCollisionState, void(bool collisionState));
    MOCK_CONST_METHOD0(GetCollisionState, bool());
    MOCK_CONST_METHOD0(GetAccelerationAbsolute, double());
    MOCK_CONST_METHOD0(GetRoadPosition, RoadPosition());
    MOCK_CONST_METHOD1(GetDistanceToStartOfRoad, double(MeasurementPoint mp));
    MOCK_CONST_METHOD2(GetDistanceToStartOfRoad, double(MeasurementPoint mp, std::string roadId));
    MOCK_CONST_METHOD1(GetObstruction, const Obstruction(const WorldObjectInterface &worldObject));
    MOCK_CONST_METHOD0(GetDistanceReferencePointToLeadingEdge, double());
    MOCK_CONST_METHOD0(GetEngineSpeed, double());
    MOCK_CONST_METHOD0(GetEffAccelPedal, double());
    MOCK_CONST_METHOD0(GetEffBrakePedal, double());
    MOCK_CONST_METHOD0(GetSteeringWheelAngle, double());
    MOCK_CONST_METHOD0(GetMaxAcceleration, double());
    MOCK_CONST_METHOD0(GetMaxDeceleration, double());
    MOCK_CONST_METHOD1(GetLaneRemainder, double(Side));
    MOCK_CONST_METHOD2(GetTrafficSignsInRange,
                       std::vector<CommonTrafficSign::Entity>(double searchDistance, int relativeLane));
    MOCK_CONST_METHOD3(GetLaneMarkingsInRange, std::vector<LaneMarking::Entity> (double searchDistance, int relativeLane, Side side));
    MOCK_CONST_METHOD0(GetSpeedGoalMin, double());
    MOCK_CONST_METHOD0(GetDistanceReferencePointToFrontAxle, double());
    MOCK_CONST_METHOD0(GetSensorParameters, const openpass::sensors::Parameters&());
    MOCK_METHOD1(SetSensorParameters, void(openpass::sensors::Parameters sensorParameters));
    MOCK_CONST_METHOD3(GetDistanceToConnectorEntrance, double (std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId));
    MOCK_CONST_METHOD3(GetDistanceToConnectorDeparture, double (std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId));
    MOCK_CONST_METHOD0(GetDistanceToNextJunction, double ());
    MOCK_CONST_METHOD0(GetRoute, const Route&());
    MOCK_CONST_METHOD1(GetRelativeJunctions, RelativeWorldView::Junctions (double range));
    MOCK_CONST_METHOD1(GetRelativeLanes, RelativeWorldView::Lanes (double range));
};
