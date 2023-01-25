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
    if (durationCounter >= currentGazeState.fixationDuration || Common::CollisionImminent(worldInterpretation.interpretedAgents)) {
        DetermineGazeState();
        durationCounter = 0;
    }
    UpdateUFOVAngle();
    durationCounter += GetCycleTime();
}

void GazeMovement::DetermineGazeState() {
    // TODO update old agents
    double random = GetStochastic()->GetUniformDistributed(0, 1);
    UpdateRoadSegment();

    if (auto collisionPoint = Common::ClosestCollisionPointByTime(worldInterpretation.interpretedAgents)) {
        if (collisionPoint->collisionImminent) {
            currentGazeState = PerformAgentObserveGlance(collisionPoint->oAgentID);
            return;
        }
    }

    if (auto possibleConflictAgent = Common::AgentWithClosestConflictPoint(worldInterpretation.interpretedAgents)) {
        // TODO set probability
        if (0.3 >= random) {
            currentGazeState = PerformAgentObserveGlance(*possibleConflictAgent);
            return;
        }
    }

    if (auto id = Common::LeadingCarID(worldInterpretation.interpretedAgents)) {
        // TODO min distance for leading condition?
        if (ProbabilityToFixateLeadCar() >= random) {
            currentGazeState = PerformAgentObserveGlance(*id);
            return;
        }
    }
    CrossingPhase phase = worldInterpretation.crossingInfo.phase;

    if (ProbabilityToPerformControlGlance() > random && phase != CrossingPhase::Exit) {
        currentGazeState = PerformControlGaze(phase);
        return;
    }
    currentGazeState = PerformScanGaze(phase);

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
        std::fabs(currentGazeState.openingAngle - (-999)) < 0.0001 || std::fabs(currentGazeState.fixationDuration - (-999)) < 0.0001) {
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + " Current GazeState is not complete";
        throw std::runtime_error(message);
    }
}

void GazeMovement::UpdateRoadSegment() {
    if (worldInterpretation.crossingInfo.phase >= CrossingPhase::Approach) {
        auto NextJunction = worldRepresentation.egoAgent->NextJunction();
        if (NextJunction != nullptr) {
            if (NextJunction->GetIncomingRoads().size() == 4) {
                if (currentSegmentType != SegmentType::XJunction) {
                    roadSegment = std::make_unique<XJunction>(worldRepresentation, GetStochastic(), GetBehaviourData());
                    currentSegmentType = SegmentType::XJunction;
                }
            }
            else if (NextJunction->GetIncomingRoads().size() == 3) {
                NextDirectionLanes nextLanes;
                // assumption movingInLaneDirection = true for now
                if (auto nextLanesPtr = InfrastructurePerception::NextLanes(true, worldRepresentation.egoAgent->GetLanePosition().lane)) {
                    if (nextLanesPtr.has_value()) {
                        nextLanes = nextLanesPtr.value();
                    }
                }
                TJunctionLayout layout;
                if (nextLanes.leftLanes.size() > 0 && nextLanes.straightLanes.size() > 0 && nextLanes.rightLanes.size() == 0) {
                    layout = TJunctionLayout::LeftStraight;
                }
                else if (nextLanes.leftLanes.size() > 0 && nextLanes.straightLanes.size() == 0 && nextLanes.rightLanes.size() > 0) {
                    layout = TJunctionLayout::LeftRight;
                }
                else if (nextLanes.leftLanes.size() == 0 && nextLanes.straightLanes.size() > 0 && nextLanes.rightLanes.size() > 0) {
                    layout = TJunctionLayout::StraightRight;
                }
                else {
                    std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + " invalid T-Junction layout";
                    throw std::runtime_error(message);
                }

                if (currentSegmentType != SegmentType::TJunction) {
                    roadSegment = std::make_unique<TJunction>(worldRepresentation, GetStochastic(), GetBehaviourData(), layout);
                    currentSegmentType = SegmentType::TJunction;
                }
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
