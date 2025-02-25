/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
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
    double vehicleReferenceToCAStart = maxDouble; // distance from the Reference of the vehicle to the start of the conflict area
    double vehicleReferenceToCAEnd = maxDouble;   // distance from the Reference of the vehicle to the end of the conflict area
    double vehicleFrontToCAStart = maxDouble; // distance from the front of the vehicle to the start of the conflict area
    double vehicleBackToCAEnd = maxDouble;    // distance from the back of the vehicle to the end of the conflict area
};

struct ConflictSituation {
    ConflictSituation() {
    }
    ~ConflictSituation() = default;
    DistanceToConflictArea egoDistance;
    DistanceToConflictArea oAgentDistance;
    const MentalInfrastructure::ConflictArea *egoCA = nullptr;
    const MentalInfrastructure::ConflictArea *oAgentCA = nullptr;
    const MentalInfrastructure::Junction *junction = nullptr;
};

struct AgentInterpretation {
    AgentInterpretation(const AmbientAgentRepresentation* agent) : agent{agent} {}
    ~AgentInterpretation() {}

    const AmbientAgentRepresentation *agent{nullptr};
    std::optional<CollisionPoint> collisionPoint{std::nullopt};
    std::optional<ConflictSituation> conflictSituation{std::nullopt};
    RightOfWay rightOfWay;
    std::optional<double> relativeDistance{std::nullopt};
    bool laneInLineWithEgoLane {false};
};

struct MemorizedTrafficSignal {
    const MentalInfrastructure::TrafficSignal *trafficSignal{nullptr};
    int firstTimeStamp{-999};
    int lastTimeStamp{-999};
};

struct VisibleTrafficSignals {
    // lane DReaMId -> TrafficSignals on that lane
    std::unordered_map<DReaMId, std::list<const MentalInfrastructure::TrafficSignal *>> laneTrafficSignalMap;

    // complete memory, signal DReaMId -> TrafficSignal
    std::unordered_map<DReaMId, MemorizedTrafficSignal> *memory{nullptr};

    // speed limit information
    const MentalInfrastructure::TrafficSign *upcomingSpeedLimitSign{nullptr};
    const MentalInfrastructure::TrafficSign *currentSpeedLimitSign{nullptr};
    const MentalInfrastructure::TrafficSign *previousSpeedLimitSign{nullptr};

    std::optional<const MentalInfrastructure::TrafficSign *> GetSignForLane(DReaMId laneId,
                                                                            MentalInfrastructure::TrafficSignType type) const {
        if (laneTrafficSignalMap.find(laneId) != laneTrafficSignalMap.end()) {
            for (const auto signal : laneTrafficSignalMap.at(laneId)) {
                auto trafficSign = dynamic_cast<const MentalInfrastructure::TrafficSign *>(signal);
                if (trafficSign != nullptr && trafficSign->GetType() == type)
                    return trafficSign;
            }
            return std::nullopt;
        }
        return std::nullopt;
    }

    std::optional<const MentalInfrastructure::TrafficLight *> GetTrafficLightForLane(DReaMId laneId) const {
        if (laneTrafficSignalMap.find(laneId) != laneTrafficSignalMap.end()) {
            for (const auto signal : laneTrafficSignalMap.at(laneId)) {
                auto trafficLight = dynamic_cast<const MentalInfrastructure::TrafficLight *>(signal);
                if (trafficLight != nullptr)
                    return trafficLight;
            }
            return std::nullopt;
        }
        return std::nullopt;
    }

    std::vector<const MentalInfrastructure::TrafficSign *> GetSignsForLane(DReaMId laneId) const {
        std::vector<const MentalInfrastructure::TrafficSign *> toReturn;
        if (laneTrafficSignalMap.find(laneId) != laneTrafficSignalMap.end()) {
            for (const auto signal : laneTrafficSignalMap.at(laneId)) {
                auto trafficSign = dynamic_cast<const MentalInfrastructure::TrafficSign *>(signal);
                if (trafficSign != nullptr)
                    toReturn.push_back(trafficSign);
            }
        }
        return toReturn;
    }
    const MentalInfrastructure::TrafficSign *GetRightOfWaySignsForLane(const MentalInfrastructure::Lane *lane) const {
        assert(lane != nullptr);
        auto laneSigns = GetSignsForLane(lane->GetDReaMId());
        auto iter = std::find_if(laneSigns.begin(), laneSigns.end(), [](const MentalInfrastructure::TrafficSign *element) {
            return static_cast<int>(element->GetType()) > 0 && static_cast<int>(element->GetType()) <= 100;
        });
        if (iter != laneSigns.end()) {
            return *iter;
        }
        auto newLane = lane;
        while (newLane->IsJunctionLane()) {
            newLane = newLane->GetPredecessors().front();
        }
        laneSigns = GetSignsForLane(newLane->GetDReaMId());
        iter = std::find_if(laneSigns.begin(), laneSigns.end(), [](const MentalInfrastructure::TrafficSign *element) {
            return static_cast<int>(element->GetType()) > 0 && static_cast<int>(element->GetType()) <= 100;
        });
        if (iter != laneSigns.end()) {
            return *iter;
        }
        return nullptr;
    }
};

struct WorldRepresentation {
    const EgoAgentRepresentation *egoAgent{nullptr};
    const InfrastructureRepresentation *infrastructure{nullptr};
    const AmbientAgentRepresentations *agentMemory{nullptr};
    const VisibleTrafficSignals *trafficSignalMemory{nullptr};
    std::vector<std::shared_ptr<GeneralAgentPerception>>
        processedAgents; // Perceived agents detected after the reaction time and latency period have elapsed (needed for analysis)
};

struct WorldInterpretation {
    WorldInterpretation() {}
    //! Interpretation of the observed agents from the ego agent point of view
    std::unordered_map<int, std::unique_ptr<AgentInterpretation>> interpretedAgents;
    CrossingInfo crossingInfo;
    //! map holds right of way for observed agent
    std::unordered_map<int, RightOfWay> rightOfWayMap;
    //! target velocity of ego agent
    double targetVelocity = std::numeric_limits<double>::max();
    //! next lane on route graph
    std::optional<const MentalInfrastructure::Lane *> targetLane = std::nullopt;
    // waiting till lane change is possible
    bool waitUntilTargetLaneIsFree = false;
    ScanAOI triggerShoulderCheckDecision = ScanAOI::NONE;
    //! data for DroneAnalysis
    std::unique_ptr<AnalysisSignal> analysisData{nullptr};
};

namespace CognitiveMap {

// TODO maybe make this class a child of GeneralAgentPerception and override the relevant fields / add getter methods for extrapolated
// fields
class AgentRepresentation {
  public:
    AgentRepresentation() {}
    AgentRepresentation(std::shared_ptr<GeneralAgentPerception> perceptionData) : internalData(perceptionData) {
    }
    virtual ~AgentRepresentation() = default;

    /*!
     * \brief Return Position in distance on lane
     *
     * @param[in]     distance     distance from the current S-coordniate to the searched position
     *
     * @return        Position if lane is present in distance
     *
     */
    std::optional<LanePosition> FindNewPositionInDistance(double distance) const;

    /*!
     * \brief Checks if the observed vehicle came from a road that is to the right of the ego agent
     *        (vehicles are only allowed to drive in lane direction)
     *
     * @param[in]     oAgentPerceptionData
     *
     * @return        true if observed agent came from rigth side
     */
    virtual bool ObservedVehicleCameFromRight(const AgentRepresentation& oAgentPerceptionData) const;

    /*!
     * \brief  return  junction (only consider next road --> TODO: Re-implementation to go over more than one road )
     * @param[in]     infrastructure
     *
     * @return        junction
     */
    const MentalInfrastructure::Junction *NextJunction() const;

    /*!
     * \brief  extrapolate distance (in lane direction distance >0 against lane direction distance <0)
     *
     * @param[in]     timeStep
     *
     * @return        distance
     */
    double ExtrapolateDistanceAlongLane(double timeStep) const;

    //*********Get-functions****//
    virtual const GeneralAgentPerception &GetInternalData() const {
        return *internalData;
    }

    int GetID() const { return internalData->id; }

    double GetYawAngle() const {
        return internalData->yaw;
    }

    double GetWidth() const { return internalData->width; }
    double GetLength() const { return internalData->length; }
    Common::Vector2d GetRefPosition() const { return internalData->refPosition; }
    DReaMDefinitions::AgentVehicleType GetVehicleType() const {
        return internalData->vehicleType;
    }
    double GetAcceleration() const { return internalData->acceleration; }
    double GetVelocity() const { return internalData->velocity; }
    double GetDistanceReferencePointToLeadingEdge() const { return internalData->distanceReferencePointToLeadingEdge; }
    bool GetBrakeLight() const { return internalData->brakeLight; }
    IndicatorState GetIndicatorState() const { return internalData->indicatorState; }
    LanePosition GetLanePosition() const {
        return internalData->lanePosition;
    }
    const std::map<std::string, RoadInterval> &GetTouchedRoads() const {
        return internalData->touchedRoads;
    }
    const MentalInfrastructure::Lane* GetNextLane() const { return internalData->nextLane; }
    JunctionDistance GetJunctionDistance() const {
        return internalData->junctionDistance;
    }
    virtual bool IsMovingInLaneDirection() const { return internalData->movingInLaneDirection; }

  protected:
      std::optional<const MentalInfrastructure::Lane *> GetJunctionConnectionLane(const AgentRepresentation &oAgentPerceptionData) const;

      /*!
       * \brief  return  right of way signs
       * @param[in]     trafficSignMap
       *
       * @return         right of way signs
       */
      const std::unordered_map<MentalInfrastructure::TrafficSignType, std::vector<MentalInfrastructure::TrafficSign>> FilterROWTrafficSigns(
          const std::unordered_map<MentalInfrastructure::TrafficSignType, std::vector<MentalInfrastructure::TrafficSign>> &trafficSignMap)
          const;

      //! the internal information of the agent representation
      std::shared_ptr<GeneralAgentPerception> internalData{nullptr};
};

class AmbientAgentRepresentation : public AgentRepresentation {
  public:
      AmbientAgentRepresentation(std::shared_ptr<GeneralAgentPerception> perceptionData, double lifetime = 0) :
          AgentRepresentation(perceptionData), lifetime{lifetime} {
      }
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

    void UpdateInternalData(std::shared_ptr<DetailedAgentPerception> perceptionData) {
        internalData = perceptionData;
    }

    Common::Vector2d GetGlobalDriverPosition() const {
        auto egoInternalData = std::static_pointer_cast<DetailedAgentPerception>(internalData);
        return egoInternalData->globalDriverPosition;
    }
    double GetSteeringWheelAngle() const {
        auto egoInternalData = std::static_pointer_cast<DetailedAgentPerception>(internalData);
        return egoInternalData->steeringWheelAngle;
    }
    double GetLaneWidth() const {
        auto egoInternalData = std::static_pointer_cast<DetailedAgentPerception>(internalData);
        return egoInternalData->laneWidth;
    }
    const MentalInfrastructure::Lane *GetMainLocatorLane() const {
        auto egoInternalData = std::static_pointer_cast<DetailedAgentPerception>(internalData);
        return egoInternalData->mainLocatorInformation.mainLocatorLane;
    }
    double GetLateralDisplacement() const {
        auto egoInternalData = std::static_pointer_cast<DetailedAgentPerception>(internalData);
        return egoInternalData->mainLocatorInformation.lateralDisplacement;
    }
    double GetCurvature() const {
        auto egoInternalData = std::static_pointer_cast<DetailedAgentPerception>(internalData);
        return egoInternalData->mainLocatorInformation.curvature;
    }
    double GetHeading() const {
        auto egoInternalData = std::static_pointer_cast<DetailedAgentPerception>(internalData);
        return egoInternalData->mainLocatorInformation.heading;
    }
    DReaMRoute::Waypoints GetRoute() const {
        auto egoInternalData = std::static_pointer_cast<DetailedAgentPerception>(internalData);
        return egoInternalData->route;
    }
};

class InfrastructureRepresentation {
  public:
    InfrastructureRepresentation() {}

    void UpdateInternalData(std::shared_ptr<InfrastructurePerception> perceptionData) { infrastructure = perceptionData; }

    const std::unordered_map<StoppingPointType, StoppingPoint> &GetStoppingPoints(OdId junctionId, OwlId laneId) const {
        return infrastructure->GetStoppingPoints(junctionId, laneId);
    }

    const std::list<const RoadmapGraph::RoadmapNode*> FindShortestPath(const MentalInfrastructure::Lane* start,
                                                                       const MentalInfrastructure::Lane* end) const {
        return infrastructure->graph.FindShortestPath(start, end);
    }

    const RoadmapGraph::RoadmapNode *NavigateToTargetNode(OdId targetRoadOdId, OwlId targetLaneOdId) const;

    const std::unordered_map<OdId, std::vector<std::pair<MentalInfrastructure::ConflictArea, MentalInfrastructure::ConflictArea>>> &GetConflictAreas() const {
        return infrastructure->GetConflictAreas();
    }

private:
    std::shared_ptr<InfrastructurePerception> infrastructure{nullptr};
};

} // namespace CognitiveMap
