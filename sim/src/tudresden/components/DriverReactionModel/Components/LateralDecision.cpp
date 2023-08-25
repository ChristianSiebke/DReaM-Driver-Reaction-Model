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
    try {
        const_cast<WorldInterpretation &>(worldInterpretation).waitUntilTargetLaneIsFree = false;
        auto egoAgent = worldRepresentation.egoAgent;
        // abort lane change TODO: encapsulate in function
        if (egoAgent->GetMainLocatorLane() != egoAgent->GetLanePosition().lane &&
            (!NeighborLaneIsFree(egoAgent->GetMainLocatorLane()) ||
             (worldInterpretation.targetLane && *worldInterpretation.targetLane != egoAgent->GetMainLocatorLane()))) {
            if (egoAgent->GetLanePosition().lane == egoAgent->GetMainLocatorLane()->GetRightLane()) {
                auto lateralDisplacement =
                    -((egoAgent->GetLanePosition().lane->GetWidth() / 2) + (egoAgent->GetMainLocatorLane()->GetWidth() / 2));
                lateralAction.lateralDisplacement = lateralDisplacement;
                return;
            }
            if (egoAgent->GetLanePosition().lane == egoAgent->GetMainLocatorLane()->GetLeftLane()) {
                auto lateralDisplacement =
                    (egoAgent->GetLanePosition().lane->GetWidth() / 2) + (egoAgent->GetMainLocatorLane()->GetWidth() / 2);
                lateralAction.lateralDisplacement = lateralDisplacement;
                return;
            }
        }
        const_cast<WorldInterpretation &>(worldInterpretation).triggerShoulderCheckDecision = TriggerLateralGaze();
        if (!worldInterpretation.targetLane || *worldInterpretation.targetLane == worldRepresentation.egoAgent->GetLanePosition().lane) {
            lateralAction = ResetLateralAction(egoAgent->GetIndicatorState());
            return;
        }

        auto targetLane = *worldInterpretation.targetLane;
        assert(targetLane != nullptr);

        if (targetLane == egoAgent->GetMainLocatorLane()->GetLeftLane() || targetLane == egoAgent->GetMainLocatorLane()->GetRightLane()) {
            if (NeighborLaneIsFree(targetLane)) {
                if (targetLane == egoAgent->GetMainLocatorLane()->GetLeftLane()) {
                    auto lateralDisplacement = (egoAgent->GetMainLocatorLane()->GetWidth() / 2) + (targetLane->GetWidth() / 2);
                    lateralAction.lateralDisplacement =
                        worldRepresentation.egoAgent->IsMovingInLaneDirection() ? lateralDisplacement : -lateralDisplacement;
                    lateralAction.indicator = IndicatorState::IndicatorState_Left;
                    const_cast<WorldInterpretation &>(worldInterpretation).triggerShoulderCheckDecision = ScanAOI::ShoulderCheckLeft;
                }
                else if (targetLane == egoAgent->GetMainLocatorLane()->GetRightLane()) {
                    auto lateralDisplacement = -((egoAgent->GetMainLocatorLane()->GetWidth() / 2) + (targetLane->GetWidth() / 2));
                    lateralAction.lateralDisplacement =
                        worldRepresentation.egoAgent->IsMovingInLaneDirection() ? lateralDisplacement : -lateralDisplacement;
                    lateralAction.indicator = IndicatorState::IndicatorState_Right;
                    const_cast<WorldInterpretation &>(worldInterpretation).triggerShoulderCheckDecision = ScanAOI::ShoulderCheckRight;
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
    catch (std::logic_error &e) {
        const std::string msg =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + static_cast<std::string>(e.what());
        Log(msg, error);
        throw std::runtime_error(msg);
    }
    catch (std::runtime_error &e) {
        const std::string msg =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + static_cast<std::string>(e.what());
        Log(msg, error);
        throw std::runtime_error(msg);
    }
    catch (...) {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + "unexpected exception LateralDecision ";
        throw std::runtime_error(message);
    }
}
ScanAOI LateralDecision::TriggerLateralGaze() const {
    if (!worldInterpretation.targetLane.has_value())
        return ScanAOI::NONE;

    auto egoAgent = worldRepresentation.egoAgent;
    auto targetLane = *worldInterpretation.targetLane;
    auto nextLanes = egoAgent->GetLanePosition().lane->NextLanes(egoAgent->IsMovingInLaneDirection());
    // turning at intersections
    if (nextLanes &&
        std::any_of(nextLanes->rightLanes.begin(), nextLanes->rightLanes.end(),
                    [targetLane](const MentalInfrastructure::Lane *lane) { return lane == targetLane; }) &&
        !nextLanes->straightLanes.empty() && (egoAgent->GetJunctionDistance().toNext < 5 && egoAgent->GetJunctionDistance().toNext > 0)) {
        return ScanAOI::ShoulderCheckRight;
    }

    // lane change
    auto route = worldRepresentation.egoAgent->GetRoute();
    auto laneIterStart =
        std::find_if(route.begin(), route.end(), [targetLane](DReaMRoute::Waypoint element) { return element.lane == targetLane; });

    if (route.end() == laneIterStart) {
        const std::string msg =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " can not find target lane in route";
        Log(msg, error);
        throw std::logic_error(msg);
    }

    if (laneIterStart->lane == egoAgent->GetLanePosition().lane->GetLeftLane()) {
        return ScanAOI::OuterLeftRVM;
    }
    if (laneIterStart->lane == egoAgent->GetLanePosition().lane->GetRightLane()) {
        return ScanAOI::OuterRightRVM;
    }

    auto laneIterNext = laneIterStart;
    std::advance(laneIterNext, 1);

    if (route.end() == laneIterNext) {
        return ScanAOI::NONE;
    }

    if (egoAgent->GetLanePosition().lane->GetLeftLane() == laneIterNext->lane &&
        5 > (laneIterStart->s - egoAgent->GetLanePosition().sCoordinate)) {
        return ScanAOI::OuterLeftRVM;
    }
    if (egoAgent->GetLanePosition().lane->GetRightLane() == laneIterNext->lane &&
        5 > (laneIterStart->s - egoAgent->GetLanePosition().sCoordinate)) {
        return ScanAOI::OuterRightRVM;
    }

    if (egoAgent->GetNextLane() && egoAgent->GetNextLane()->GetLeftLane() == laneIterNext->lane &&
        2 > (laneIterStart->s - laneIterStart->lane->GetFirstPoint()->sOffset) +
                (egoAgent->GetLanePosition().lane->GetLength() - egoAgent->GetLanePosition().sCoordinate)) {
        return ScanAOI::OuterLeftRVM;
    }
    if (egoAgent->GetNextLane() && egoAgent->GetNextLane()->GetRightLane() == laneIterNext->lane &&
        2 > (laneIterStart->s - laneIterStart->lane->GetFirstPoint()->sOffset) +
                (egoAgent->GetLanePosition().lane->GetLength() - egoAgent->GetLanePosition().sCoordinate)) {
        return ScanAOI::OuterRightRVM;
    }
    return ScanAOI::NONE;
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

    if ((!upperAgent || (upperDistance > (worldRepresentation.egoAgent->GetVelocity() * 2) && upperDistance > 5)) &&
        (!lowerAgent || (lowerDistance > (lowerAgent->agent->GetVelocity() * 3) && lowerDistance > 5))) {
        return true;
    }
    return false;
}
bool LateralDecision::TurningAtJunction() const {
    return ((worldInterpretation.crossingInfo.phase == CrossingPhase::Deceleration_TWO &&
             worldRepresentation.egoAgent->GetJunctionDistance().toNext > 0) ||
            worldInterpretation.crossingInfo.phase == CrossingPhase::Crossing_Straight);
}
bool LateralDecision::AgentIsTurningOnJunction() const {
    return worldInterpretation.crossingInfo.phase >= CrossingPhase::Deceleration_TWO;
}

IndicatorState LateralDecision::SetIndicatorAtJunction(const MentalInfrastructure::Lane *targetLane) const {
    try {
        auto lanesPtr =
            worldRepresentation.egoAgent->GetLanePosition().lane->NextLanes(worldRepresentation.egoAgent->IsMovingInLaneDirection());
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

                if (route.end() == laneIter) {
                    const std::string msg = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                                            " can not find target lane in route";
                    Log(msg, error);
                    throw std::logic_error(msg);
                }
                std::advance(laneIter, 1);

                if (route.end() == laneIter) {
                    return IndicatorState::IndicatorState_Off;
                }

                lanesPtr = targetLane->NextLanes(worldRepresentation.egoAgent->IsMovingInLaneDirection());

                if (lanesPtr.has_value() &&
                    (targetLane->GetLength() + (worldRepresentation.egoAgent->GetLanePosition().lane->GetLength() -
                                                (worldRepresentation.egoAgent->GetLanePosition().sCoordinate +
                                                 worldRepresentation.egoAgent->GetDistanceReferencePointToLeadingEdge())) <=
                     DecelerationTWODistance)) {
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
                return worldRepresentation.egoAgent->GetIndicatorState();
            }
        }
        const std::string msg =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " Indicator cannot be calculated";
        Log(msg, error);
        throw std::logic_error(msg);
    }
    catch (std::logic_error &e) {
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
