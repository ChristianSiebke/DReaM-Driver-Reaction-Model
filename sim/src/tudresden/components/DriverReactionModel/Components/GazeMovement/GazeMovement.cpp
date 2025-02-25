/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/

#include "GazeMovement.h"
#include <cmath>

namespace GazeMovement {

void GazeMovement::Update() {
    UpdateRoadSegment();
    if (durationCounter >= currentGazeState.fixationDuration || Common::CollisionImminent(worldInterpretation.interpretedAgents)) {
        currentGazeState = DetermineGazeState();
        durationCounter = 0;
    }
    UpdateUFOV();
    durationCounter += GetCycleTime();

    if (currentGazeState.fixationState.first == GazeType::NONE ||
        (currentGazeState.fixationState.first == GazeType::ScanGlance &&
         currentGazeState.fixationState.second == static_cast<int>(ScanAOI::NONE)) ||
        (currentGazeState.fixationState.first == GazeType::ControlGlance &&
         currentGazeState.fixationState.second == static_cast<int>(ControlAOI::NONE)) ||
        (currentGazeState.fixationState.first == GazeType::ObserveGlance &&
         currentGazeState.fixationState.second == static_cast<int>(ObservationAOI::NONE)) ||
        (currentGazeState.fixationState.first == GazeType::ObserveGlance && currentGazeState.target.fixationAgent == -999) ||
        (currentGazeState.fixationState.first == GazeType::ControlGlance &&
         std::fabs(currentGazeState.target.fixationPoint.x - (-999)) < 0.0001 &&
         std::fabs(currentGazeState.target.fixationPoint.y - (-999)) < 0.0001) ||
        std::fabs(currentGazeState.openingAngle - (-999)) < 0.0001 || std::fabs(currentGazeState.fixationDuration - (-999)) < 0.0001 ||
        std::fabs(currentGazeState.directionUFOV - (-999)) < 0.0001 || std::fabs(currentGazeState.viewDistance - (-999) < 0.0001) ||
        (currentGazeState.startPosUFOV.x == std::numeric_limits<double>::max() &&
         currentGazeState.startPosUFOV.y == std::numeric_limits<double>::max())) {
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + " Current GazeState is not complete";
        throw std::runtime_error(message);
    }
}

GazeState GazeMovement::DetermineGazeState() {
    // TODO update old agents
    double random = GetStochastic()->GetUniformDistributed(0, 1);

    if (auto collisionPoint = Common::ClosestCollisionPointByTime(worldInterpretation.interpretedAgents)) {
        if (collisionPoint->collisionImminent) {
            return PerformAgentObserveGlance(collisionPoint->oAgentID);
        }
    };
    if ((!turningAtJunctionShoulderCheckDecision && worldInterpretation.targetLane) &&
        worldRepresentation.egoAgent->GetNextLane() == *worldInterpretation.targetLane) {
        turningAtJunctionShoulderCheckDecision = true;
        if (0.494 >= random || worldRepresentation.egoAgent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Bicycle) {
            if (currentGazeState.fixationState.second != static_cast<int>(ScanAOI::ShoulderCheckLeft) &&
                worldInterpretation.triggerShoulderCheckDecision == ScanAOI::ShoulderCheckLeft) {
                return PerformShoulderCheckLeft(worldInterpretation.triggerShoulderCheckDecision);
            }
            else if (currentGazeState.fixationState.second != static_cast<int>(ScanAOI::ShoulderCheckRight) &&
                     worldInterpretation.triggerShoulderCheckDecision == ScanAOI::ShoulderCheckRight) {
                return PerformShoulderCheckRight(worldInterpretation.triggerShoulderCheckDecision);
            }
        }
    }
    if (0.494 >= random || worldRepresentation.egoAgent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Bicycle) {
        if (currentGazeState.fixationState.second != static_cast<int>(ScanAOI::ShoulderCheckLeft) &&
            worldInterpretation.triggerShoulderCheckDecision == ScanAOI::ShoulderCheckLeft) {
            return PerformShoulderCheckLeft(worldInterpretation.triggerShoulderCheckDecision);
        }
        else if (currentGazeState.fixationState.second != static_cast<int>(ScanAOI::ShoulderCheckRight) &&
                 worldInterpretation.triggerShoulderCheckDecision == ScanAOI::ShoulderCheckRight) {
            return PerformShoulderCheckRight(worldInterpretation.triggerShoulderCheckDecision);
        }
    }

    if (currentGazeState.fixationState.second != static_cast<int>(ScanAOI::OuterLeftRVM) &&
        worldInterpretation.triggerShoulderCheckDecision == ScanAOI::OuterLeftRVM && 0.8 >= random) {
        return PerformMirrowGaze(worldInterpretation.triggerShoulderCheckDecision);
    }
    if (currentGazeState.fixationState.second != static_cast<int>(ScanAOI::OuterRightRVM) &&
        worldInterpretation.triggerShoulderCheckDecision == ScanAOI::OuterRightRVM && 0.8 >= random) {
        return PerformMirrowGaze(worldInterpretation.triggerShoulderCheckDecision);
    }

    if (auto id = Common::LeadingCarID(worldInterpretation.interpretedAgents)) {
        if (ProbabilityToFixateLeadCar() >= random) {
            return PerformAgentObserveGlance(*id);
        }
    }

    CrossingPhase phase = worldInterpretation.crossingInfo.phase;
    if (ProbabilityToPerformControlGlance() > random && phase != CrossingPhase::Exit) {
        return PerformControlGaze(phase);
    }
    return PerformScanGaze(phase);
}

void GazeMovement::UpdateRoadSegment() {
    if (worldInterpretation.crossingInfo.phase >= CrossingPhase::Approach) {
        auto nextJunction = worldRepresentation.egoAgent->NextJunction();
        if (nextJunction != nullptr && lastJunctionID != nextJunction->GetOpenDriveId()) {
            if (nextJunction->GetIncomingRoads().size() == 4) {
                if (currentSegmentType != SegmentType::XJunction) {
                    turningAtJunctionShoulderCheckDecision = false;
                    roadSegment = std::make_unique<XJunction>(worldRepresentation, GetStochastic(), GetBehaviourData());
                    currentSegmentType = SegmentType::XJunction;
                }
            }
            else if (nextJunction->GetIncomingRoads().size() == 3) {
                auto lanes = worldRepresentation.egoAgent->GetLanePosition().lane->GetRoad()->GetLanes();
                auto findCorrespondinglaneToId = [lanes](std::string id) {
                    return std::find_if(lanes.begin(), lanes.end(), [id](auto element) { return element->GetOpenDriveId() == id; });
                };

                auto laneiter = std::stoi(worldRepresentation.egoAgent->GetLanePosition().lane->GetOpenDriveId()) < 0
                                    ? findCorrespondinglaneToId("-1")
                                    : findCorrespondinglaneToId("1");
                if (laneiter == lanes.end()) {
                    std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + " invalid T-Junction layout";
                    throw std::runtime_error(message);
                }
                auto nextLanes = (*laneiter)->NextLanes(worldRepresentation.egoAgent->IsMovingInLaneDirection());
                if (nextLanes == std::nullopt) {
                    std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "unknown successor lanes";
                    throw std::runtime_error(message);
                }
                TJunctionLayout layout;
                if (nextLanes->leftLanes.size() > 0 && nextLanes->straightLanes.size() > 0 && nextLanes->rightLanes.size() == 0) {
                    layout = TJunctionLayout::LeftStraight;
                }
                else if (nextLanes->leftLanes.size() > 0 && nextLanes->straightLanes.size() == 0 && nextLanes->rightLanes.size() > 0) {
                    layout = TJunctionLayout::LeftRight;
                }
                else if (nextLanes->leftLanes.size() == 0 && nextLanes->straightLanes.size() > 0 && nextLanes->rightLanes.size() > 0) {
                    layout = TJunctionLayout::StraightRight;
                }
                else {
                    std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + " invalid T-Junction layout";
                    throw std::runtime_error(message);
                }

                turningAtJunctionShoulderCheckDecision = false;
                roadSegment = std::make_unique<TJunction>(worldRepresentation, GetStochastic(), GetBehaviourData(), layout);
                currentSegmentType = SegmentType::TJunction;
                lastJunctionID = nextJunction->GetOpenDriveId();
                
            }
            else if (nextJunction->GetIncomingRoads().size() == 2) {
                // connection of two roads
                roadSegment = std::make_unique<StandardRoad>(worldRepresentation, GetStochastic(), GetBehaviourData());
                currentSegmentType = SegmentType::StandardRoad;
            }
            else {
                std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "unknown node segment";
                throw std::runtime_error(message);
            }
        }
    }
    else if (currentSegmentType != SegmentType::StandardRoad) {
        roadSegment = std::make_unique<StandardRoad>(worldRepresentation, GetStochastic(), GetBehaviourData());
        currentSegmentType = SegmentType::StandardRoad;
    }
    else {
        // TODO default message --> more specific description for standardRoad needed
        // std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "unknown node segment";
        // throw std::runtime_error(message);
    }
}
} // namespace GazeMovement
