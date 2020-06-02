/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#pragma once

#include "gmock/gmock.h"

#include "Interfaces/parameterInterface.h"
#include "Interfaces/worldInterface.h"
#include "Common/globalDefinitions.h"

class FakeWorld : public WorldInterface
{
  public:
    MOCK_METHOD0(CreateAgentAdapterForAgent, AgentInterface*());
    MOCK_CONST_METHOD1(GetAgent, AgentInterface*(int id));
    MOCK_METHOD1(GetAgentByName, AgentInterface*(const std::string &scenarioName));
    MOCK_METHOD0(GetEgoAgent, AgentInterface*());
    MOCK_METHOD2(AddAgent, bool(int id, AgentInterface* agent));
    MOCK_METHOD0(CreateGlobalDrivingView, bool());
    MOCK_METHOD1(CreateScenery, bool(SceneryInterface *scenery));
    MOCK_METHOD1(CreateWorldScenario, bool(const std::string &scenarioFilename));
    MOCK_METHOD1(CreateWorldScenery, bool(const std::string &sceneryFilename));
    MOCK_METHOD0(Instantiate, bool());
    MOCK_METHOD6(IntersectsWithAgent, bool(double x, double y, double rotation, double length, double width, double center));
    MOCK_METHOD0(isInstantiated, bool());
    MOCK_METHOD3(IsSValidOnLane, bool(std::string roadId, int laneId, double distance));
    MOCK_CONST_METHOD0(GetBicycle, const AgentInterface*());
    MOCK_METHOD1(GetLastCarInlane, const AgentInterface*(int laneNumber));
    MOCK_METHOD0(GetSpecialAgent, const AgentInterface*());
    MOCK_CONST_METHOD0(GetRemovedAgents, const std::list<const AgentInterface*>&());
    MOCK_CONST_METHOD0(GetAgents, const std::map<int, AgentInterface*>&());
    MOCK_CONST_METHOD0(GetTrafficObjects, const std::vector<const TrafficObjectInterface*>&());
    MOCK_CONST_METHOD0(GetWorldObjects, const std::vector<const WorldObjectInterface*>&());
    MOCK_METHOD5(GetDistanceToEndOfLane, double(Route route, std::string roadId, int laneNumber, double initialSearchDistance, double maxSearchLength));
    MOCK_METHOD6(GetDistanceToEndOfLane, double(Route route, std::string roadId, int laneNumber, double initialSearchDistance, double maxSearchLength, const LaneTypes& laneTypes));
    MOCK_CONST_METHOD0(GetFriction, double());
    MOCK_CONST_METHOD5(GetLaneCurvature, double(Route route, std::string roadId, int laneId, double position, double distance));
    MOCK_CONST_METHOD5(GetLaneDirection, double(Route route, std::string roadId, int laneId, double position, double distance));
    MOCK_CONST_METHOD5(GetLaneWidth, double(Route route, std::string roadId, int laneId, double position, double distance));
    MOCK_CONST_METHOD0(GetVisibilityDistance, double());
    MOCK_CONST_METHOD2(GetLaneId, int(uint64_t streamId, double endDistance));
    MOCK_CONST_METHOD4(LaneCoord2WorldCoord, Position(double distanceOnLane, double offset, std::string roadId, int laneId));
    MOCK_CONST_METHOD2(RoadCoord2WorldCoord, Position(RoadPosition roadCoord, std::string roadID));
    MOCK_CONST_METHOD4(GetObstruction, Obstruction (const Route& route, const GlobalRoadPosition& ownPosition, const ObjectPosition& otherPosition, const std::vector<Common::Vector2d>& objectCorners));
    MOCK_CONST_METHOD0(GetTimeOfDay, std::string());
    MOCK_CONST_METHOD5(GetTrafficSignsInRange, std::vector<CommonTrafficSign::Entity>(const Route& route, std::string roadId, int laneId, double startDistance, double searchRange));
    MOCK_CONST_METHOD6(GetLaneMarkings, std::vector<LaneMarking::Entity> (const Route& route, std::string roadId, int laneId, double startDistance, double range, Side side));
    MOCK_CONST_METHOD6(GetAgentsInRange, std::vector<const AgentInterface*>(Route route, std::string roadId, int laneId, double startDistance, double backwardRange, double forwardRange));
    MOCK_CONST_METHOD6(GetObjectsInRange, std::vector<const WorldObjectInterface*>(Route route, std::string roadId, int laneId, double startDistance, double backwardRange, double forwardRange));
    MOCK_CONST_METHOD2(GetAgentsInRangeOfJunctionConnection, std::vector<const AgentInterface*> (std::string connectingRoadId, double range));
    MOCK_CONST_METHOD4(GetDistanceToConnectorEntrance, double (const ObjectPosition position, std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId));
    MOCK_CONST_METHOD4(GetDistanceToConnectorDeparture, double (const ObjectPosition position, std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId));
    MOCK_METHOD0(GetGlobalDrivingView, void*());
    MOCK_METHOD0(GetGlobalObjects, void*());
    MOCK_METHOD0(Clear, void());
    MOCK_METHOD1(ExtractParameter, void(ParameterInterface *parameters));
    MOCK_METHOD1(QueueAgentRemove, void(const AgentInterface*agent));
    MOCK_METHOD1(QueueAgentUpdate, void(std::function<void()> func));
    MOCK_METHOD2(QueueAgentUpdate, void(std::function<void(double)> func, double val));
    MOCK_METHOD0(Reset, void());
    MOCK_METHOD1(SetParameter, void(WorldParameter *worldParameter));
    MOCK_METHOD1(SetTimeOfDay, void(int timeOfDay));
    MOCK_METHOD1(SetWeekday, void(Weekday weekday));
    MOCK_METHOD0(SyncGlobalData, void());
    MOCK_METHOD0(GetOsiGroundTruth, void*());
    MOCK_METHOD0(GetWorldData, void*());
    MOCK_CONST_METHOD0(GetWeekday, Weekday());
    MOCK_CONST_METHOD2(GetConnectionsOnJunction, std::vector<JunctionConnection> (std::string junctionId, std::string incomingRoadId));
    MOCK_CONST_METHOD1(GetIntersectingConnections, std::vector<IntersectingConnection> (std::string connectingRoadId));
    MOCK_CONST_METHOD1(GetPrioritiesOnJunction, std::vector<JunctionConnectorPriority> (std::string junctionId));
    MOCK_CONST_METHOD1(GetRoadSuccessor, RoadNetworkElement (std::string roadId));
    MOCK_CONST_METHOD1(GetRoadPredecessor, RoadNetworkElement (std::string roadId));
    MOCK_CONST_METHOD1(GetRoute, Route (GlobalRoadPosition start));
    MOCK_CONST_METHOD3(GetDistanceBetweenObjects, double (const Route&, const ObjectPosition&, const ObjectPosition&));
    MOCK_CONST_METHOD2(GetNextJunctionIdOnRoute, std::string (const Route&, const ObjectPosition&));
    MOCK_CONST_METHOD3(GetDistanceToJunction, double (const Route&, const ObjectPosition&, const std::string&));
    MOCK_CONST_METHOD4(GetRelativeJunctions, RelativeWorldView::Junctions (const Route& route, std::string roadId, double startDistance, double range));
    MOCK_CONST_METHOD5(GetRelativeLanes, RelativeWorldView::Lanes (const Route& route, std::string roadId, int laneId, double distance, double range));
};
