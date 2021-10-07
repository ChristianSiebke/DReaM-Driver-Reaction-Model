/*******************************************************************************
* Copyright (c) 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "include/egoAgentInterface.h"

class FakeEgoAgent : public EgoAgentInterface
{
  public:
    MOCK_CONST_METHOD0(GetAgent, const AgentInterface* ());
    MOCK_METHOD3(SetRoadGraph, void (const RoadGraph&& roadGraph, RoadGraphVertex current, RoadGraphVertex target));
    MOCK_METHOD0(Update, void ());
    MOCK_CONST_METHOD0(HasValidRoute, bool ());
    MOCK_METHOD1(SetNewTarget, void (size_t alternativeIndex));
    MOCK_CONST_METHOD0(GetRoadId, const std::string& ());
    MOCK_CONST_METHOD1(GetVelocity, double (VelocityScope velocityScope));
    MOCK_CONST_METHOD2(GetVelocity, double (VelocityScope velocityScope, const WorldObjectInterface* object));
    MOCK_CONST_METHOD2(GetDistanceToEndOfLane, double (double range, int relativeLane));
    MOCK_CONST_METHOD3(GetDistanceToEndOfLane, double (double range, int relativeLane, const LaneTypes& acceptableLaneTypes));
    MOCK_CONST_METHOD3(GetRelativeLanes, RelativeWorldView::Lanes (double range, int relativeLane, bool includeOncoming));
    MOCK_CONST_METHOD2(GetRelativeLaneId, std::optional<int> (const WorldObjectInterface* object, MeasurementPoint mp));
    MOCK_CONST_METHOD1(GetRelativeJunctions, RelativeWorldView::Junctions (double range));
    MOCK_CONST_METHOD3(GetObjectsInRange, std::vector<const WorldObjectInterface*> (double backwardRange, double forwardRange, int relativeLane));
    MOCK_CONST_METHOD3(GetAgentsInRange, AgentInterfaces (double backwardRange, double forwardRange, int relativeLane));
    MOCK_CONST_METHOD2(GetTrafficSignsInRange, std::vector<CommonTrafficSign::Entity> (double range, int relativeLane));
    MOCK_CONST_METHOD2(GetRoadMarkingsInRange, std::vector<CommonTrafficSign::Entity> (double range, int relativeLane));
    MOCK_CONST_METHOD2(GetTrafficLightsInRange, std::vector<CommonTrafficLight::Entity> (double range, int relativeLane));
    MOCK_CONST_METHOD3(GetLaneMarkingsInRange, std::vector<LaneMarking::Entity> (double range, Side side, int relativeLane));
    MOCK_CONST_METHOD1(GetDistanceToObject, LongitudinalDistance (const WorldObjectInterface* otherObject));
    MOCK_CONST_METHOD1(GetObstruction, Obstruction (const WorldObjectInterface* otherObject));
    MOCK_CONST_METHOD0(GetRelativeYaw, double ());
    MOCK_CONST_METHOD0(GetPositionLateral, double ());
    MOCK_CONST_METHOD1(GetLaneRemainder, double (Side side));
    MOCK_CONST_METHOD1(GetLaneWidth, double (int relativeLane));
    MOCK_CONST_METHOD2(GetLaneWidth, std::optional<double> (double distance, int relativeLane));
    MOCK_CONST_METHOD1(GetLaneCurvature, double (int relativeLane));
    MOCK_CONST_METHOD2(GetLaneCurvature, std::optional<double> (double distance, int relativeLane));
    MOCK_CONST_METHOD1(GetLaneDirection, double (int relativeLane));
    MOCK_CONST_METHOD2(GetLaneDirection, std::optional<double> (double distance, int relativeLane));
    MOCK_CONST_METHOD0(GetMainLocatePosition, const GlobalRoadPosition&());
    MOCK_CONST_METHOD0(GetReferencePointPosition, std::optional<GlobalRoadPosition> ());
    MOCK_CONST_METHOD1(GetLaneIdFromRelative, int (int relativeLaneId));
    MOCK_CONST_METHOD3(GetWorldPosition, Position (double sDistance, double tDistance, double yaw));
    MOCK_CONST_METHOD1(executeQueryDistanceToEndOfLane, ExecuteReturn<DistanceToEndOfLane> (DistanceToEndOfLaneParameter parameter));
    MOCK_CONST_METHOD1(executeQueryObjectsInRange, ExecuteReturn<ObjectsInRange> (ObjectsInRangeParameter parameter));
};
