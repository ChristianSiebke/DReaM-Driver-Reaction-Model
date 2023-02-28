/******************************************************************************
 * Copyright (c) 2022 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#pragma once

#include <algorithm>
#include <limits>
#include <unordered_map>

#include "Common/vector2d.h"
#include "MentalInfrastructure/Junction.h"
#include "MentalInfrastructure/Lane.h"
#include "MentalInfrastructure/Road.h"
#include "MentalInfrastructure/RoadmapGraph/roadmap_graph.h"
#include "MentalInfrastructure/TrafficLight.h"

struct InfrastructurePerception;
struct DynamicInfrastructurePerception;

/**
 * @brief Generic perception data of an object
 *
 */
struct ObjectPerception {
    virtual ~ObjectPerception() = default;

    int id = std::numeric_limits<int>::min();
    double yaw = std::numeric_limits<double>::min();
    double width = std::numeric_limits<double>::min();
    double length = std::numeric_limits<double>::min();
    Common::Vector2d refPosition{std::numeric_limits<double>::min(), std::numeric_limits<double>::min()};
};

/**
 * @brief Represents a position on a lane
 *
 */
struct LanePosition {
    const MentalInfrastructure::Lane *lane = nullptr;
    double sCoordinate = std::numeric_limits<double>::min();
};

/**
 * @brief Distance to a next junction and on a current junction. Negative value mean no junction present.
 *
 */
struct JunctionDistance {
    double on = -1;
    double toNext = -1;
};

/**
 * @brief Wrapper for the next lanes an agent might take.
 *
 */
struct NextDirectionLanes {
    std::vector<const MentalInfrastructure::Lane *> rightLanes;
    std::vector<const MentalInfrastructure::Lane *> leftLanes;
    std::vector<const MentalInfrastructure::Lane *> straightLanes;
};

/**
 * @brief Wrapper for data collected at the main locator point (front of the vehicle / agent).
 *
 */
struct MainLocatorInformation {
    const MentalInfrastructure::Lane *mainLocatorLane = nullptr;
    double lateralDisplacement = std::numeric_limits<double>::min();
    double curvature = std::numeric_limits<double>::min();
    double heading = std::numeric_limits<double>::min();
};

/**
 * @brief General information collected about an agent. Will be passed to other agents and contains "public" data.
 *
 */
struct GeneralAgentPerception : ObjectPerception {
    // vehicle information [static]
    DReaMDefinitions::AgentVehicleType vehicleType = DReaMDefinitions::AgentVehicleType::NONE;
    double distanceReferencePointToLeadingEdge = std::numeric_limits<double>::min();

    // movement information
    double acceleration = std::numeric_limits<double>::min();
    double velocity = std::numeric_limits<double>::min();
    bool movingInLaneDirection = false;

    // light status
    bool brakeLight = true;
    IndicatorState indicatorState = IndicatorState::IndicatorState_Warn;

    // positional information (at reference point)
    LanePosition lanePosition{};
    const MentalInfrastructure::Lane *nextLane;
    JunctionDistance junctionDistance{};

public:
    /**
     * @brief Calculates position on a lane in a given distance
     *
     * @param distance distance from the current S-coordniate to the searched position
     * @return Position if lane is present in distance
     */
    virtual std::optional<LanePosition> FindNewPositionInDistance(double distance) const;

    /**
     * @brief Calculates a set of values for distance on and to next junction
     *
     * @param agentRoad
     * @param agentLane
     * @return JunctionDistance
     */
    // REWORK move out of the AgentPerception class (maybe static)
    // VA: this code is not tied to the AgentPerception and should therefor not be tied to this class, it is more of a helper function
    static JunctionDistance CalculateJunctionDistance(GeneralAgentPerception perception, const MentalInfrastructure::Road *agentRoad,
                                                      const MentalInfrastructure::Lane *agentLane);

    // TODO documentation
    static bool IsMovingInLaneDirection(const MentalInfrastructure::Lane *agentLane, double yawAngle, double sCoordinate, double velocity);
};

/**
 * @brief Detailed information collected on an agent. Will only be passed to the respective agent this data belongs to.
 *
 */
struct DetailedAgentPerception : GeneralAgentPerception {
    // position of the driver in the vehicle (global coordinate)
    Common::Vector2d globalDriverPosition{std::numeric_limits<double>::min(), std::numeric_limits<double>::min()};

    // additional vehicle information
    double steeringWheelAngle = std::numeric_limits<double>::min();

    // additional road information
    double laneWidth = std::numeric_limits<double>::min();
    MainLocatorInformation mainLocatorInformation{};

    DReaMRoute::Waypoints route{};
};

struct LookupTableRoadNetwork {
    std::unordered_map<OdId, const MentalInfrastructure::Road *> roads;
    std::unordered_map<OwlId, const MentalInfrastructure::Lane *> lanes;
    std::unordered_map<OdId, const MentalInfrastructure::TrafficSign *> trafficSigns;
    // not const since these need to be updated
    std::unordered_map<OdId, MentalInfrastructure::TrafficLight *> trafficLights;
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
    std::vector<std::shared_ptr<const MentalInfrastructure::TrafficLight>> trafficLights;

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
