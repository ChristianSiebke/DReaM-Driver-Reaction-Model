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
    auto egoAgent = worldRepresentation.egoAgent;
    if (!worldInterpretation.targetLane) {
        lateralAction = ResetLateralAction(egoAgent->GetIndicatorState());
        return;
    }
    auto targetLane = *worldInterpretation.targetLane;
    if (std::none_of(egoAgent->GetMainLocatorLane()->GetSuccessors().begin(), egoAgent->GetMainLocatorLane()->GetSuccessors().end(),
                     [targetLane](auto element) { return element == targetLane; })) {
        // New Lane free ?
        // TODO: implement Algorithm -->new lane free?
        // TODO: move to interpreter
        if (NewLaneIsFree()) {
            if (targetLane == egoAgent->GetMainLocatorLane()->GetLeftLane()) {
                auto lateralDisplacement = (egoAgent->GetMainLocatorLane()->GetWidth() / 2) + (targetLane->GetWidth() / 2);
                lateralAction.lateralDisplacement =
                    egoAgent->GetMainLocatorLane()->IsInRoadDirection() ? lateralDisplacement : -lateralDisplacement;
                lateralAction.indicator = egoAgent->GetMainLocatorLane()->IsInRoadDirection() ? IndicatorState::IndicatorState_Left
                                                                                              : IndicatorState::IndicatorState_Right;
            }
            else if (targetLane == egoAgent->GetMainLocatorLane()->GetRightLane()) {
                auto lateralDisplacement = -((egoAgent->GetMainLocatorLane()->GetWidth() / 2) + (targetLane->GetWidth() / 2));
                lateralAction.lateralDisplacement =
                    egoAgent->GetMainLocatorLane()->IsInRoadDirection() ? lateralDisplacement : -lateralDisplacement;
                lateralAction.indicator = egoAgent->GetMainLocatorLane()->IsInRoadDirection() ? IndicatorState::IndicatorState_Right
                                                                                              : IndicatorState::IndicatorState_Left;
            }
            else {
                const std::string msg = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                                        " next lane in path is not a neighbor";
                Log(msg, error);
                throw std::logic_error(msg);
            }
        }
        else {
            // TODO: slow down and wait at end of road/ after waiting a specific time choose new lane
            // TODO acceleration calcualtion in longitudinalAction
            // TODO: move to interpreter
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
bool LateralDecision::NewLaneIsFree() const {
    // TODO: implement
    return true;
}
bool LateralDecision::TurningAtJunction() const {
    return worldRepresentation.egoAgent->NextJunction() != nullptr &&
           worldInterpretation.crossingInfo.phase == CrossingPhase::Deceleration_TWO;
}
bool LateralDecision::AgentIsTurningOnJunction() const {
    return worldInterpretation.crossingInfo.phase >= CrossingPhase::Deceleration_TWO;
}

IndicatorState LateralDecision::SetIndicatorAtJunction(const MentalInfrastructure::Lane *targetLane) const {
    try {
        auto currentLane = worldRepresentation.egoAgent->GetLanePosition().lane;
        auto currentLanePoints = currentLane->GetLanePoints();
        auto secondLast = std::prev(currentLanePoints.end(), 2);
        Common::Vector2d CurrentDirection((currentLane->GetLastPoint()->x) - (secondLast->x),
                                          (currentLane->GetLastPoint()->y) - (secondLast)->y);
        auto nextLanePoints = targetLane->GetLanePoints();
        auto pointNL = std::prev((nextLanePoints).end(), 2);
        Common::Vector2d NextLaneDirection(targetLane->GetLastPoint()->x - pointNL->x, targetLane->GetLastPoint()->y - pointNL->y);

        auto angleDeg = AngleBetween2d(CurrentDirection, NextLaneDirection) * (180 / M_PI);

        if (10 >= angleDeg || std::fabs(angleDeg - 180) < 10) {
            return IndicatorState::IndicatorState_Off;
        }
        else if (10 < angleDeg && CurrentDirection.Cross(NextLaneDirection) > 0) {
            return IndicatorState::IndicatorState_Left;
        }
        else if (10 < angleDeg && CurrentDirection.Cross(NextLaneDirection) < 0) {
            return IndicatorState::IndicatorState_Right;
        }
        else {
            const std::string msg =
                static_cast<std::string>(__FILE__) + std::to_string(__LINE__) + " error during route calculation: cannot set CrossingType";
            Log(msg, error);
            throw std::runtime_error(msg);
        }
    }
    catch (std::runtime_error e) {
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
