/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "LateralDecision.h"

#include <iostream>

#include "Common/Helper.h"
namespace LateralDecision {

LateralDecision::LateralDecision(const WorldRepresentation &worldRepresentation, const WorldInterpretation &worldInterpretation,
                                 int cycleTime, StochasticsInterface *stochastics, LoggerInterface *loggerInterface,
                                 const BehaviourData &behaviourData) :
    ComponentInterface(cycleTime, stochastics, loggerInterface, behaviourData),
    worldRepresentation{worldRepresentation},
    worldInterpretation{worldInterpretation} {
}

void LateralDecision::Update() {
    const_cast<WorldInterpretation &>(worldInterpretation).waitUntilTargetLaneIsFree = false;
    auto egoAgent = worldRepresentation.egoAgent;
    if (egoAgent->GetMainLocatorLane() != egoAgent->GetLanePosition().lane &&
        (!NeighborLaneIsFree(egoAgent->GetMainLocatorLane()) ||
         (worldInterpretation.targetLane && *worldInterpretation.targetLane != egoAgent->GetMainLocatorLane()))) {
        if (egoAgent->GetLanePosition().lane == egoAgent->GetMainLocatorLane()->GetRightLane()) {
            auto lateralDisplacement =
                -(egoAgent->GetLanePosition().lane->GetWidth() / 2 + (egoAgent->GetMainLocatorLane()->GetWidth() / 2));
            lateralAction.lateralDisplacement = lateralDisplacement;
            return;
        }
        if (egoAgent->GetLanePosition().lane == egoAgent->GetMainLocatorLane()->GetLeftLane()) {
            auto lateralDisplacement = egoAgent->GetLanePosition().lane->GetWidth() / 2 + (egoAgent->GetMainLocatorLane()->GetWidth() / 2);
            lateralAction.lateralDisplacement = lateralDisplacement;
            return;
        }
    }

    if (!worldInterpretation.targetLane) {
        lateralAction = ResetLateralAction(egoAgent->GetIndicatorState());
        return;
    }

    auto targetLane = *worldInterpretation.targetLane;

    if (targetLane == egoAgent->GetMainLocatorLane()->GetLeftLane() || targetLane == egoAgent->GetMainLocatorLane()->GetRightLane()) {
        if (NeighborLaneIsFree(targetLane)) {
            if (targetLane == egoAgent->GetMainLocatorLane()->GetLeftLane()) {
                auto lateralDisplacement = (egoAgent->GetMainLocatorLane()->GetWidth() / 2) + (targetLane->GetWidth() / 2);
                lateralAction.lateralDisplacement =
                    worldRepresentation.egoAgent->IsMovingInLaneDirection() ? lateralDisplacement : -lateralDisplacement;
                lateralAction.indicator = IndicatorState::IndicatorState_Left;
            }
            else if (targetLane == egoAgent->GetMainLocatorLane()->GetRightLane()) {
                auto lateralDisplacement = -((egoAgent->GetMainLocatorLane()->GetWidth() / 2) + (targetLane->GetWidth() / 2));
                lateralAction.lateralDisplacement =
                    worldRepresentation.egoAgent->IsMovingInLaneDirection() ? lateralDisplacement : -lateralDisplacement;
                lateralAction.indicator = IndicatorState::IndicatorState_Right;
            }
            else {
                const std::string msg = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                                        " next lane in path is not a neighbor";
                Log(msg, error);
                throw std::logic_error(msg);
            }
        }
        else {
            lateralAction.lateralDisplacement = 0;
            if (worldRepresentation.egoAgent->GetJunctionDistance().toNext < 20 &&
                worldRepresentation.egoAgent->GetJunctionDistance().toNext > 0 &&
                (!Common::AgentTouchesLane(egoAgent, egoAgent->GetLanePosition().lane->GetLeftLane()) &&
                 !Common::AgentTouchesLane(egoAgent, egoAgent->GetLanePosition().lane->GetRightLane()))) {
                const_cast<WorldInterpretation &>(worldInterpretation).waitUntilTargetLaneIsFree = true;
            }
        }
    }
    else {
        lateralAction = ResetLateralAction(egoAgent->GetIndicatorState());
        if (TurningAtJunction()) {
            lateralAction.indicator = SetIndicatorAtJunction(targetLane);
            return;
        }
    }
}
LateralAction LateralDecision::ResetLateralAction(IndicatorState currentIndicator) const {
    LateralAction lateralAction;
    lateralAction.lateralDisplacement = 0;
    lateralAction.indicator = currentIndicator;
    if (AgentIsTurningOnJunction()) {
        return lateralAction;
    }
    lateralAction.indicator = IndicatorState::IndicatorState_Off;
    return lateralAction;
}
bool LateralDecision::NeighborLaneIsFree(const MentalInfrastructure::Lane *targetLane) const {
    if (targetLane == nullptr) {
        return true;
    }
    auto egoS = worldRepresentation.egoAgent->GetLanePosition().sCoordinate;
    auto egoLane = worldRepresentation.egoAgent->GetLanePosition().lane;

    auto upperBound = std::numeric_limits<double>::infinity();
    auto lowerBound = -std::numeric_limits<double>::infinity();
    AgentInterpretation *upperAgent = nullptr;
    AgentInterpretation *lowerAgent = nullptr;

    for (const auto &entry : worldInterpretation.interpretedAgents) {
        const auto &agent = entry.second;

        auto agentLane = agent->agent->GetLanePosition().lane;
        auto agentLaneSuccessor = std::any_of(targetLane->GetSuccessors().begin(), targetLane->GetSuccessors().end(),
                                              [agentLane](const MentalInfrastructure::Lane *element) { return agentLane == element; });
        auto agentLanePredecessor = std::any_of(targetLane->GetPredecessors().begin(), targetLane->GetPredecessors().end(),
                                                [agentLane](const MentalInfrastructure::Lane *element) { return agentLane == element; });
        if (agentLane == targetLane || agentLaneSuccessor || agentLanePredecessor) {
            auto oAgentS = agent->agent->GetLanePosition().sCoordinate;
            if (agentLaneSuccessor) {
                oAgentS =
                    egoLane->GetLastPoint()->sOffset + (agent->agent->GetLanePosition().sCoordinate - agentLane->GetFirstPoint()->sOffset);
            }
            if (agentLanePredecessor) {
                oAgentS =
                    egoLane->GetFirstPoint()->sOffset - (agentLane->GetLastPoint()->sOffset - agent->agent->GetLanePosition().sCoordinate);
            }
            if (egoS <= oAgentS && oAgentS < upperBound) {
                upperBound = oAgentS;
                upperAgent = agent.get();
            }
            if (egoS > oAgentS && oAgentS > lowerBound) {
                lowerBound = oAgentS;
                lowerAgent = agent.get();
            }
        }
    }
    auto test = [](AgentInterpretation *a) -> std::string {
        if (a != nullptr) {
            return std::to_string(a->agent->GetID());
        }
        return " there is no agent";
    };
    double upperAgentDistanceReferenceToBack = 0;
    if (upperAgent)
        upperAgentDistanceReferenceToBack = upperAgent->agent->GetLength() - upperAgent->agent->GetDistanceReferencePointToLeadingEdge();
    double egoAgentDistanceReferenceToFront = worldRepresentation.egoAgent->GetDistanceReferencePointToLeadingEdge();
    double lowerAgentDistanceReferenceToFront = 0;
    if (lowerAgent)
        lowerAgentDistanceReferenceToFront = lowerAgent->agent->GetDistanceReferencePointToLeadingEdge();
    double egoAgentDistanceReferenceToBack =
        worldRepresentation.egoAgent->GetLength() - worldRepresentation.egoAgent->GetDistanceReferencePointToLeadingEdge();
    double distanceReferenceToEdgesUpperAgent = upperAgentDistanceReferenceToBack + egoAgentDistanceReferenceToFront;
    double distanceReferenceToEdgesLowerAgent = lowerAgentDistanceReferenceToFront + egoAgentDistanceReferenceToBack;
    double upperDistance = upperBound - egoS - distanceReferenceToEdgesUpperAgent;
    double lowerDistance = egoS - lowerBound - distanceReferenceToEdgesLowerAgent;

    if ((!upperAgent || (upperDistance > (worldRepresentation.egoAgent->GetVelocity() * 3) && upperDistance > 5)) &&
        (!lowerAgent || (lowerDistance > (lowerAgent->agent->GetVelocity() * 3) && lowerDistance > 5))) {
        return true;
    }
    return false;
}
bool LateralDecision::TurningAtJunction() const {
    return (worldInterpretation.crossingInfo.phase == CrossingPhase::Deceleration_TWO ||
            worldInterpretation.crossingInfo.phase == CrossingPhase::Crossing_Straight);
}
bool LateralDecision::AgentIsTurningOnJunction() const {
    return worldInterpretation.crossingInfo.phase >= CrossingPhase::Deceleration_TWO;
}

IndicatorState LateralDecision::SetIndicatorAtJunction(const MentalInfrastructure::Lane *targetLane) const {
    try {
        auto lanesPtr =
            worldRepresentation.egoAgent->GetMainLocatorLane()->NextLanes(worldRepresentation.egoAgent->IsMovingInLaneDirection());
        if (lanesPtr) {
            if (std::any_of(lanesPtr->leftLanes.begin(), lanesPtr->leftLanes.end(),
                            [targetLane](auto element) { return element == targetLane; })) {
                return IndicatorState::IndicatorState_Left;
            }
            else if (std::any_of(lanesPtr->rightLanes.begin(), lanesPtr->rightLanes.end(),
                                 [targetLane](auto element) { return element == targetLane; })) {
                return IndicatorState::IndicatorState_Right;
            }
            else if (std::any_of(lanesPtr->straightLanes.begin(), lanesPtr->straightLanes.end(),
                                 [targetLane](auto element) { return element == targetLane; })) {
                auto route = worldRepresentation.egoAgent->GetRoute();
                auto laneIter = std::find_if(route.begin(), route.end(),
                                             [targetLane](DReaMRoute::Waypoint element) { return element.lane == targetLane; });
                std::advance(laneIter, 1);

                if (route.end() == laneIter) {
                    return IndicatorState::IndicatorState_Off;
                }

                lanesPtr = targetLane->NextLanes(worldRepresentation.egoAgent->IsMovingInLaneDirection());
                if (targetLane->GetLength() + (worldRepresentation.egoAgent->GetLanePosition().lane->GetLength() -
                                               (worldRepresentation.egoAgent->GetLanePosition().sCoordinate +
                                                worldRepresentation.egoAgent->GetDistanceReferencePointToLeadingEdge())) <=
                    DecelerationTWODistance) {
                    if (std::any_of(lanesPtr->leftLanes.begin(), lanesPtr->leftLanes.end(),
                                    [laneIter](auto element) { return element == (*laneIter).lane; })) {
                        return IndicatorState::IndicatorState_Left;
                    }
                    else if (std::any_of(lanesPtr->rightLanes.begin(), lanesPtr->rightLanes.end(),
                                         [laneIter](auto element) { return element == (*laneIter).lane; })) {
                        return IndicatorState::IndicatorState_Right;
                    }
                }
                return IndicatorState::IndicatorState_Off;
            }
            else {
                const std::string msg = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                                        " Indicator cannot be calculated";
                Log(msg, error);
                throw std::logic_error(msg);
            }
        }
        const std::string msg =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " Indicator cannot be calculated";
        Log(msg, error);
        throw std::logic_error(msg);
    }
    catch (std::logic_error e) {
        const std::string msg =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + static_cast<std::string>(e.what());
        Log(msg, error);
        throw std::runtime_error(msg);
    }
    catch (...) {
        const std::string msg =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " error during route calculation";
        Log(msg, error);
        throw std::runtime_error(msg);
    }
}

} // namespace LateralDecision
