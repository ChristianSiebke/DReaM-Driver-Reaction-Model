//-----------------------------------------------------------------------------
//
// Copyright (c) TU Dresden.
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v1.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v10.html
//-----------------------------------------------------------------------------/
#pragma once

#include "Common/vector2d.h"
#include "MentalInfrastructure/Intersection.h"
#include "MentalInfrastructure/Lane.h"
#include "MentalInfrastructure/Road.h"
#include "MentalInfrastructure/RoadmapGraph/roadmap_graph.h"
#include "MentalInfrastructure/Section.h"
#include <unordered_map>

struct InfrastructurePerception;
struct DynamicInfrastructurePerception;
struct ObjectPerception {
    virtual ~ObjectPerception() = default;

    int id{-999};
    double yawAngle{-999};
    double width{-999};
    double length{-999};
    Common::Vector2d refPosition{-999, -999};
};

struct PositionAlongRoad {
    const MentalInfrastructure::Lane* newLane = nullptr;
    double newSCoordinate = -999;
};

struct IntersectionDistance {
    double distanceOnIntersection = -1;
    double distanceToNextIntersection = -1;
};

struct NextDirectionLanes {
    std::vector<const MentalInfrastructure::Lane*> rightLanes;
    std::vector<const MentalInfrastructure::Lane*> leftLanes;
    std::vector<const MentalInfrastructure::Lane*> straightLanes;
};

struct AgentPerception : ObjectPerception {
    virtual ~AgentPerception() override = default;

    /*!
     * \brief Return Position in distance on lane
     *
     * @param[in]     distance              distance from the current S-coordniate to the searched position
     *
     * @return        Position if lane is present in distance
     */
    virtual std::optional<PositionAlongRoad> FindNewPositionInDistance(double distance) const;

    /*!
     * \brief  check if agent is moving in lane direction
     * @param[in]     agentLane
     *
     * @return        true if agent is moving in lane direction
     */
    static bool IsMovingInLaneDirection(const MentalInfrastructure::Lane* agentLane, double yawAngle, double sCoordinate, double velocitz);

    /*!
     * \brief  calculates distance to next intersection
     * @param[in]     agentRoad
     * @param[in]     agentLane
     *
     * @return        distance on/to intersection
     */
    IntersectionDistance CalculateIntersectionDistance(const MentalInfrastructure::Road* agentRoad,
                                                       const MentalInfrastructure::Lane* agentLane) const;

    AgentVehicleType vehicleType{AgentVehicleType::NONE};

    double acceleration{-999};
    double velocity{-999};

    double distanceReferencePointToLeadingEdge{-999};

    bool brakeLight{true};
    IndicatorState indicatorState{IndicatorState::IndicatorState_Warn};

    double sCoordinate{-999};
    LaneType laneType{LaneType::Undefined};
    bool movingInLaneDirection{false};

    const MentalInfrastructure::Road* road;
    const MentalInfrastructure::Lane* lane;
    const MentalInfrastructure::Lane* nextLane;

    double distanceToNextIntersection{-999};
    double distanceOnIntersection{-999};
};

struct EgoPerception : AgentPerception {
    ~EgoPerception() override = default;

    //! position of the driver in the vehicle (global coordinate)
    Common::Vector2d driverPosition{-999, -999};

    //! additional vehicle information
    double steeringWheelAngle{-999};

    //! additional road information
    double laneWidth{-999};
    double lateralDisplacement{-999}; // lateral deviation
    double curvature{-999};
    double heading{-999};
};

struct LookupTableRoadNetwork {
    std::unordered_map<Id, const MentalInfrastructure::Road*> roads;
    std::unordered_map<Id, const MentalInfrastructure::Lane*> lanes;
    std::unordered_map<Id, const MentalInfrastructure::TrafficSign*> trafficSigns;
};


struct StoppingPointData {
    std::map<Id,std::map<Id,std::map<StoppingPointType, StoppingPoint>>> stoppingPoints;
};


struct ConflictPoints {
    std::string currentOpenDriveRoadId;
    int64_t currentOpenDriveLaneId;
    std::string intersectionOpenDriveRoadId;
    int64_t intersectionOpenDriveLaneId;
    Common::Vector2d start;
    Common::Vector2d end;
};
struct InfrastructurePerception {

    /*!
     * \brief Return the next lane of given lane
     *
     */
    static const MentalInfrastructure::Lane* NextLane(IndicatorState indicatorState, bool movingInLaneDirection,
                                                      const MentalInfrastructure::Lane* lane);
    /*!
     * \brief return next lanes that follow on current lane (considering moving direction of agent)
     *
     * @param[in]     movingInLaneDirection
     * @param[in]     currentLane
     *
     * @return        next lanes
     */
    static std::optional<NextDirectionLanes> NextLanes(bool movingInLaneDirection, const MentalInfrastructure::Lane* currentLane);

    const std::vector<ConflictPoints>& GetConflicPoints() {
        return conflictPoints;
    }

    const std::map<StoppingPointType, StoppingPoint>& GetStoppingPoints(Id intersectionId, Id laneId) const {
        return stoppingPointData.stoppingPoints.at(intersectionId).at(laneId);
    }

    std::vector<std::shared_ptr<const MentalInfrastructure::Intersection>> intersections;
    std::vector<std::shared_ptr<const MentalInfrastructure::Road>> roads;
    std::vector<std::shared_ptr<const MentalInfrastructure::Lane>> lanes;
    std::vector<std::shared_ptr<const MentalInfrastructure::Section>> sections;
    std::vector<std::shared_ptr<const MentalInfrastructure::TrafficSign>> trafficSigns;
    RoadmapGraph::RoadmapGraph graph;
    StoppingPointData stoppingPointData;

    /*!
     * \brief map ids to infrastructure element
     */
    LookupTableRoadNetwork lookupTableRoadNetwork;

    std::vector<ConflictPoints> conflictPoints;
};

struct DynamicInfrastructurePerception {
    /**
     * @brief This method should clear all internally stored data since it should be overwritten at each time interval.
     */
    void Clear() { trafficSigns.clear(); }

    std::unordered_map<std::shared_ptr<MentalInfrastructure::Road>, MentalInfrastructure::TrafficSign> trafficSigns;
};

struct NavigationDecision {
    std::string odRoadID{"-999"};
    int odLaneID{-999};

    uint64_t roadID{InvalidId};
    uint64_t laneID{InvalidId};
    IndicatorState indicator = IndicatorState::IndicatorState_Off;
};


