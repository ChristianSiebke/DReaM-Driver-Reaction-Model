/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "Interfaces/agentInterface.h"
#include "Interfaces/worldInterface.h"
#include "Interfaces/egoAgentInterface.h"

class EgoAgent: public EgoAgentInterface
{
public:
    EgoAgent(const AgentInterface* agent, const WorldInterface* world) :
        agent(agent),
        world(world)
    {}

    const AgentInterface* GetAgent () const override;

    void SetRoadGraph(const RoadGraph&& roadGraph, RoadGraphVertex current, RoadGraphVertex target) override;

    void UpdatePositionInGraph() override;

    bool HasValidRoute() const override;

    void SetNewTarget (size_t alternativeIndex) override;

    const std::string& GetRoadId() const override;

    double GetDistanceToEndOfLane (double range, int relativeLane = 0) const override;

    double GetDistanceToEndOfLane(double range, int relativeLane, const LaneTypes &acceptableLaneTypes) const override;

    RelativeWorldView::Lanes GetRelativeLanes(double range, int relativeLane = 0) const override;

    RelativeWorldView::Junctions GetRelativeJunctions(double range) const override;

    std::vector<const WorldObjectInterface*> GetObjectsInRange (double backwardRange, double forwardRange, int relativeLane = 0) const override;

    std::vector<const AgentInterface*> GetAgentsInRange (double backwardRange, double forwardRange, int relativeLane = 0) const override;

    std::vector<CommonTrafficSign::Entity> GetTrafficSignsInRange(double range, int relativeLane = 0) const override;

    std::vector<CommonTrafficSign::Entity> GetRoadMarkingsInRange(double range, int relativeLane = 0) const override;

    std::vector<LaneMarking::Entity> GetLaneMarkingsInRange(double range, Side side, int relativeLane = 0) const override;

    LongitudinalDistance GetDistanceToObject(const WorldObjectInterface* otherObject) const override;

    Obstruction GetObstruction(const WorldObjectInterface* otherObject) const override;

    double GetRelativeYaw () const override;

    double GetPositionLateral() const override;

    double GetLaneRemainder(Side side) const override;

    double GetLaneWidth(int relativeLane = 0) const override;

    std::optional<double> GetLaneWidth(double distance, int relativeLane = 0) const override;

    double GetLaneCurvature(int relativeLane = 0) const override;

    std::optional<double> GetLaneCurvature(double distance, int relativeLane = 0) const override;

    double GetLaneDirection(int relativeLane = 0) const override;

    std::optional<double> GetLaneDirection(double distance, int relativeLane = 0) const override;

    const GlobalRoadPosition& GetMainLocatePosition() const override;

    std::optional<GlobalRoadPosition> GetReferencePointPosition() const override;

    int GetLaneIdFromRelative (int relativeLaneId) const override;

    Position GetWorldPosition (double sDistance, double tDistance, double yaw = 0) const override;

private:

    std::optional<RouteElement> GetPreviousRoad(size_t steps = 1) const;

    std::optional<RoadGraphVertex> GetReferencePointVertex() const;

    ExecuteReturn<DistanceToEndOfLane> executeQueryDistanceToEndOfLane(DistanceToEndOfLaneParameter parameter) const override;

    ExecuteReturn<ObjectsInRange> executeQueryObjectsInRange(ObjectsInRangeParameter parameter) const override;

    void SetWayToTarget(RoadGraphVertex targetVertex);

    const AgentInterface* agent;
    const WorldInterface* world;
    bool graphValid{false};
    RoadGraph roadGraph{};
    RoadGraphVertex current{0};
    RoadGraph wayToTarget{};
    RoadGraphVertex rootOfWayToTargetGraph{0};
    std::vector<RoadGraphVertex> alternatives{};
};
