//-----------------------------------------------------------------------------
//
// Copyright (c) TU Dresden.
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v1.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v10.html
//-----------------------------------------------------------------------------/
#pragma once

#include <algorithm>
#include <unordered_map>

#include "Common/vector2d.h"
#include "MentalInfrastructure/Junction.h"
#include "MentalInfrastructure/Lane.h"
#include "MentalInfrastructure/Road.h"
#include "MentalInfrastructure/RoadmapGraph/roadmap_graph.h"

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
    const MentalInfrastructure::Lane *newLane = nullptr;
    double newSCoordinate = -999;
};

struct JunctionDistance {
    double distanceOnJunction = -1;
    double distanceToNextJunction = -1;
};

struct NextDirectionLanes {
    std::vector<const MentalInfrastructure::Lane *> rightLanes;
    std::vector<const MentalInfrastructure::Lane *> leftLanes;
    std::vector<const MentalInfrastructure::Lane *> straightLanes;
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
    // REWORK move out of AgentPerception class
    // see CalculateJunctionDistance
    // could also be used to automatically set the corresponding bool --> then the metho would make sense at this point
    static bool IsMovingInLaneDirection(const MentalInfrastructure::Lane *agentLane, double yawAngle, double sCoordinate, double velocitz);

    /*!
     * \brief  calculates distance to next junction
     * @param[in]     agentRoad
     * @param[in]     agentLane
     *
     * @return        distance on/to junction
     */
    // REWORK move out of the AgentPerception class (maybe static)
    // VA: this code is not tied to the AgentPerception and should therefor not be tied to this class, it is more of a helper function
    JunctionDistance CalculateJunctionDistance(const MentalInfrastructure::Road *agentRoad,
                                               const MentalInfrastructure::Lane *agentLane) const;

    AgentVehicleType vehicleType{AgentVehicleType::NONE};

    double acceleration{-999};
    double velocity{-999};

    double distanceReferencePointToLeadingEdge{-999};

    bool brakeLight{true};
    IndicatorState indicatorState{IndicatorState::IndicatorState_Warn};
    // measured at reference point
    double sCoordinate{-999};
    LaneType laneType{LaneType::Undefined};
    bool movingInLaneDirection{false};
    // measured at reference point
    const MentalInfrastructure::Road *road;
    // measured at reference point
    const MentalInfrastructure::Lane *lane;
    // measured at reference point
    const MentalInfrastructure::Lane *nextLane;

    double distanceToNextJunction{-999};
    double distanceOnJunction{-999};
};

struct EgoPerception : AgentPerception {
    ~EgoPerception() override = default;

    //! position of the driver in the vehicle (global coordinate)
    Common::Vector2d driverPosition{-999, -999};

    //! additional vehicle information
    double steeringWheelAngle{-999};

    //! additional road information
    double laneWidth{-999};

    //! measured at mainLocaterPoint (vehicle front)
    const MentalInfrastructure::Lane *mainLocatorLane;
    // lateral deviation measured at mainLocaterPoint (vehicle front)
    double lateralDisplacement{-999};
    double curvature{-999};
    double heading{-999};
    DReaMRoute::Waypoints route{};
};

struct LookupTableRoadNetwork {
    std::unordered_map<OdId, const MentalInfrastructure::Road *> roads;
    std::unordered_map<OwlId, const MentalInfrastructure::Lane *> lanes;
    std::unordered_map<OdId, const MentalInfrastructure::TrafficSign *> trafficSigns;
};

struct StoppingPointData {
    std::unordered_map<OdId, std::unordered_map<OwlId, std::unordered_map<StoppingPointType, StoppingPoint>>> stoppingPoints;
};

struct InfrastructurePerception {
    /*!
     * \brief Return the next lane of given lane
     *
     */
    static const MentalInfrastructure::Lane *NextLane(IndicatorState indicatorState, bool movingInLaneDirection,
                                                      const MentalInfrastructure::Lane *lane);
    /*!
     * \brief return next lanes that follow on current lane (considering moving direction of agent)
     *
     * @param[in]     movingInLaneDirection
     * @param[in]     currentLane
     *
     * @return        next lanes
     */
    static std::optional<NextDirectionLanes> NextLanes(bool movingInLaneDirection, const MentalInfrastructure::Lane *currentLane);

    const std::unordered_map<OdId, std::vector<std::pair<MentalInfrastructure::ConflictArea, MentalInfrastructure::ConflictArea>>> &
    GetConflictAreas() {
        return conflictAreas;
    }

    const std::unordered_map<StoppingPointType, StoppingPoint> &GetStoppingPoints(OdId junctionId, OwlId laneId) const {
        return stoppingPointData.stoppingPoints.at(junctionId).at(laneId);
    }

    const StoppingPointData &GetStoppingPointData() const {
        return stoppingPointData;
    }

    std::vector<std::shared_ptr<const MentalInfrastructure::Junction>> junctions;
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
    /*!
     * \brief map conflic area pairs to conflict position (junction id)
     */
    std::unordered_map<OdId, std::vector<std::pair<MentalInfrastructure::ConflictArea, MentalInfrastructure::ConflictArea>>> conflictAreas;
};

struct DynamicInfrastructurePerception {
    /**
     * @brief This method should clear all internally stored data since it should be overwritten at each time interval.
     */
    void Clear() {
        trafficSigns.clear();
    }

    std::unordered_map<std::shared_ptr<MentalInfrastructure::Road>, MentalInfrastructure::TrafficSign> trafficSigns;
};

struct LateralAction {
    IndicatorState indicator = IndicatorState::IndicatorState_Off;
    double lateralDisplacement{0};
};
