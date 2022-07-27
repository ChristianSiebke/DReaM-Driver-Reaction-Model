/******************************************************************************
 * Copyright (c) 2020 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *****************************************************************************/

#pragma once
#include "Common/PerceptionData.h"

namespace CognitiveMap {
class AgentRepresentation;
class AmbientAgentRepresentation;
class EgoAgentRepresentation;
class InfrastructureRepresentation;
} // namespace CognitiveMap
using AgentRepresentation = CognitiveMap::AgentRepresentation;
using AmbientAgentRepresentation = CognitiveMap::AmbientAgentRepresentation;
using AmbientAgentRepresentations = std::vector<std::unique_ptr<AmbientAgentRepresentation>>;
using EgoAgentRepresentation = CognitiveMap::EgoAgentRepresentation;
using InfrastructureRepresentation = CognitiveMap::InfrastructureRepresentation;
enum class JunctionSituation;

struct DistanceToConflictArea {
    double vehicleFrontToCAStart = maxDouble; // distance from the front of the vehicle to the start of the conflict area
    double vehicleBackToCAEnd = maxDouble;    // distance from the back of the vehicle to the end of the conflict area
};

struct ConflictSituation {
    ConflictSituation() {
    }
    ~ConflictSituation() = default;
    int oAgentID = maxInt;
    DistanceToConflictArea egoDistance;
    DistanceToConflictArea oAgentDistance;
};

struct AgentInterpretation {
    AgentInterpretation(const AmbientAgentRepresentation* agent) : agent{agent} {}
    ~AgentInterpretation() {}

    const AmbientAgentRepresentation* agent;
    std::optional<CollisionPoint> collisionPoint;
    std::optional<ConflictSituation> conflictSituation;
    RightOfWay rightOfWay;
    std::optional<double> followingDistanceToLeadingVehicle;
};

struct VisibleTrafficSigns {
    std::unordered_map<OdId, std::unordered_map<MentalInfrastructure::TrafficSignType,
                                                std::unordered_map<OwlId, const MentalInfrastructure::TrafficSign *>>>
        trafficSigns;

    std::unordered_map<MentalInfrastructure::TrafficSignType, std::unordered_map<OwlId, const MentalInfrastructure::TrafficSign *>>
    GetSignsForRoad(OdId roadId) {
        auto findIter = trafficSigns.find(roadId);
        if (findIter != trafficSigns.end())
            return findIter->second;

        std::unordered_map<MentalInfrastructure::TrafficSignType, std::unordered_map<OwlId, const MentalInfrastructure::TrafficSign *>>
            empty;
        return empty;
    }
};

struct WorldRepresentation {
    const EgoAgentRepresentation* egoAgent;
    const InfrastructureRepresentation* infrastructure;
    const AmbientAgentRepresentations* agentMemory;
    const VisibleTrafficSigns* trafficSignMemory;
};

struct WorldInterpretation {
    WorldInterpretation() {}
    //! Interpretation of the observed agents from the point of view of the ego
    std::unordered_map<int, std::unique_ptr<AgentInterpretation>> interpretedAgents;
    CrossingInfo crossingInfo;
    //! map holds right of way for observed agent
    std::unordered_map<int, RightOfWay> rightOfWayMap;
};

namespace CognitiveMap {

class AgentRepresentation {
  public:
    AgentRepresentation() {}
    AgentRepresentation(std::shared_ptr<AgentPerception> perceptionData) : internalData(perceptionData) {}
    virtual ~AgentRepresentation() = default;

    /*!
     * \brief Return Position in distance on lane
     *
     * @param[in]     distance     distance from the current S-coordniate to the searched position
     *
     * @return        Position if lane is present in distance
     *
     */
    std::optional<PositionAlongRoad> FindNewPositionInDistance(double distance) const;

    /*!
     * \brief Checks if the observed vehicle came from a road that is to the right of the ego agent
     *        (vehicles are only allowed to drive in lane direction close junctions)
     *
     * @param[in]     oAgentPerceptionData
     *
     * @return        true if observed agent came from rigth side
     */
    virtual bool ObservedVehicleCameFromRight(const AgentRepresentation& oAgentPerceptionData) const;

    /*!
     * \brief  return next junction
     * @param[in]     infrastructure
     *
     * @return        junction
     */
    const MentalInfrastructure::Junction *NextJunction() const;

    /*!
     * \brief  return next right of way sign
     *
     * @return        next right of way sign
     */
    virtual std::optional<MentalInfrastructure::TrafficSign> NextROWSign() const;

    /*!
     * \brief  return junction situation --> Constellation of agents to each other at the junction
     * @param[in]     observedAgent
     *
     * @return        CollisionSituationType
     */
    std::optional<JunctionSituation> JunctionSituation(const AgentRepresentation &observedAgent) const;

    /*!
     * \brief  check if agent is moving towards specific junction
     * @param[in]     junction
     *
     * @return        true if agent is moving towards junction
     */
    bool IsMovingTowardsJunction(const MentalInfrastructure::Junction *junction) const;
    /*!
     * \brief  extrapolate distance (in lane direction distance >0 against lane direction distance <0)
     *
     * @param[in]     timeStep
     *
     * @return        distance
     */
    double ExtrapolateDistanceAlongLane(double timeStep) const;

    /*!
     * \brief  returns the distance to conflict area if current or next lane of the agents intersect
     * @param[in]     observedAgent
     *
     * @return        distance to conflict area
     */
    std::optional<ConflictSituation> PossibleConflictAreaAlongLane(const AgentRepresentation &observedAgent) const;

    //*********Get-functions****//
    virtual const AgentPerception& GetInternalData() const { return *internalData; }

    int GetID() const { return internalData->id; }

    double GetYawAngle() const { return internalData->yawAngle; }

    double GetWidth() const { return internalData->width; }
    double GetLength() const { return internalData->length; }
    Common::Vector2d GetRefPosition() const { return internalData->refPosition; }
    AgentVehicleType GetVehicleType() const { return internalData->vehicleType; }
    double GetAcceleration() const { return internalData->acceleration; }
    double GetVelocity() const { return internalData->velocity; }
    double GetDistanceReferencePointToLeadingEdge() const { return internalData->distanceReferencePointToLeadingEdge; }
    bool GetBrakeLight() const { return internalData->brakeLight; }
    IndicatorState GetIndicatorState() const { return internalData->indicatorState; }
    double GetSCoordinate() const { return internalData->sCoordinate; }
    LaneType GetLaneType() const { return internalData->laneType; }
    const MentalInfrastructure::Road* GetRoad() const { return internalData->road; }
    const MentalInfrastructure::Lane* GetLane() const { return internalData->lane; }
    const MentalInfrastructure::Lane* GetNextLane() const { return internalData->nextLane; }
    double GetDistanceToNextJunction() const {
        return internalData->distanceToNextJunction;
    }
    double GetDistanceOnJunction() const {
        return internalData->distanceOnJunction;
    }
    virtual bool IsMovingInLaneDirection() const { return internalData->movingInLaneDirection; }

  protected:
    /*!
     * \brief  return  right of way signs
     * @param[in]     trafficSignMap
     *
     * @return         right of way signs
     */
    const std::unordered_map<MentalInfrastructure::TrafficSignType, std::vector<MentalInfrastructure::TrafficSign>> FilterROWTrafficSigns(
        const std::unordered_map<MentalInfrastructure::TrafficSignType, std::vector<MentalInfrastructure::TrafficSign>>& trafficSignMap)
        const;

    ConflictSituation DistanceToConflictArea(std::pair<const MentalInfrastructure::ConflictArea &, OwlId> egoCA,
                                             std::pair<const MentalInfrastructure::ConflictArea &, OwlId> observedCA,
                                             const AgentRepresentation &observedAgent) const;

    double DistanceToConflictPoint(const AgentRepresentation &agent, const MentalInfrastructure::LanePoint &junctionPoint,
                                   OwlId laneId) const;

    //! the internal information of the agent representation
    std::shared_ptr<AgentPerception> internalData;
};

class AmbientAgentRepresentation : public AgentRepresentation {
  public:
    AmbientAgentRepresentation(std::shared_ptr<AgentPerception> perceptionData) : AgentRepresentation(perceptionData) { lifetime = 0; }
    virtual ~AmbientAgentRepresentation() = default;

    //! increase the life time of the agent representation by one cycletime
    void IncrementLifeTimeTicker(double cycletime) { lifetime += cycletime; }
    double GetLifeTime() const { return lifetime; }

  private:
    //!  represents the time since the agent representation is in the Cognitive Map
    double lifetime;
};

class EgoAgentRepresentation : public AgentRepresentation {
  public:
    EgoAgentRepresentation() {}
    virtual ~EgoAgentRepresentation() = default;

    void UpdateInternalData(std::shared_ptr<EgoPerception> perceptionData) { internalData = perceptionData; }

    Common::Vector2d GetDriverPosition() const {
        auto egoInternalData = std::static_pointer_cast<EgoPerception>(internalData);
        return egoInternalData->driverPosition;
    }
    double GetSteeringWheelAngle() const {
        auto egoInternalData = std::static_pointer_cast<EgoPerception>(internalData);
        return egoInternalData->steeringWheelAngle;
    }
    double GetLaneWidth() const {
        auto egoInternalData = std::static_pointer_cast<EgoPerception>(internalData);
        return egoInternalData->laneWidth;
    }
    const MentalInfrastructure::Lane *GetMainLocatorLane() const {
        auto egoInternalData = std::static_pointer_cast<EgoPerception>(internalData);
        return egoInternalData->mainLocatorLane;
    }
    double GetLateralDisplacement() const {
        auto egoInternalData = std::static_pointer_cast<EgoPerception>(internalData);
        return egoInternalData->lateralDisplacement;
    }
    double GetCurvature() const {
        auto egoInternalData = std::static_pointer_cast<EgoPerception>(internalData);
        return egoInternalData->curvature;
    }
    double GetHeading() const {
        auto egoInternalData = std::static_pointer_cast<EgoPerception>(internalData);
        return egoInternalData->heading;
    }
    DReaMRoute::Waypoints GetRoute() const {
        auto egoInternalData = std::static_pointer_cast<EgoPerception>(internalData);
        return egoInternalData->route;
    }
};

class InfrastructureRepresentation {
  public:
    InfrastructureRepresentation() {}

    void UpdateInternalData(std::shared_ptr<InfrastructurePerception> perceptionData) { infrastructure = perceptionData; }

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
    static const std::optional<NextDirectionLanes> NextLanes(bool movingInLaneDirection, const MentalInfrastructure::Lane* currentLane);

    const std::unordered_map<StoppingPointType, StoppingPoint> &GetStoppingPoints(OdId junctionId, OwlId laneId) const {
        return infrastructure->GetStoppingPoints(junctionId, laneId);
    }

    const std::list<const RoadmapGraph::RoadmapNode*> FindShortestPath(const MentalInfrastructure::Lane* start,
                                                                       const MentalInfrastructure::Lane* end) const {
        return infrastructure->graph.FindShortestPath(start, end);
    }

    const RoadmapGraph::RoadmapNode *NavigateToTargetNode(OdId targetRoadOdId, OwlId targetLaneOdId) const;

    const std::vector<std::pair<MentalInfrastructure::ConflictArea, MentalInfrastructure::ConflictArea>> &GetConflictAreas() const {
        return infrastructure->GetConflictAreas();
    }

  private:
    std::shared_ptr<InfrastructurePerception> infrastructure;
};

} // namespace CognitiveMap
