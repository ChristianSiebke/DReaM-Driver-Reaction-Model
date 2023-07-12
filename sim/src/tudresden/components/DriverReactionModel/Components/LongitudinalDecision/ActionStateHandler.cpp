/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/

#include "ActionStateHandler.h"

    ActionState
    ActionStateHandler::GetState(const std::unique_ptr <AgentInterpretation> &agent) {
    currentState = ActionState::Start;
    while (currentState != ActionState::End) {
        if (DetermineNextState(agent)) {
            break;
        } else {
            IncrementState();
        }
    }
    if (currentState == ActionState::Following) {
        worldInterpretation.analysisData->following = true;
        if (agent->relativeDistance.has_value() && agent->relativeDistance.value() > 0) {
            double dist = agent->relativeDistance.value();
            if (worldRepresentation.egoAgent->GetVelocity() > 0) {
                double headway = dist / worldRepresentation.egoAgent->GetVelocity();
                if (headway < worldInterpretation.analysisData->timeHeadway) {
                    worldInterpretation.analysisData->timeHeadway = headway;
                    worldInterpretation.analysisData->followingTarget = agent->agent->GetID();
                }
            }
        }
    }
    if (currentState == ActionState::CollisionImminent || currentState == ActionState::ReactToIntersectionSituation)
        worldInterpretation.analysisData->obstruction = true;

    if (currentState == ActionState::ReactToIntersectionSituation && !EgoHasRightOfWay(agent))
        worldInterpretation.analysisData->hasROW = false;
    return currentState;
}

bool ActionStateHandler::DetermineNextState(const std::unique_ptr<AgentInterpretation>& agent) {
    try {
        switch (currentState) {
        case ActionState::Start:
            return false;
        case ActionState::CollisionImminent:
            return agent->collisionPoint.has_value() && agent->collisionPoint->collisionImminent;
        case ActionState::Following:
            if (agent->relativeDistance.has_value()) {
                if (agent->conflictSituation->oAgentDistance.vehicleFrontToCAStart < 0 ||
                    agent->conflictSituation->egoDistance.vehicleFrontToCAStart < 0) {
                    return true;
                }
                else if (agent->relativeDistance < 0 && !agent->laneInLineWithEgoLane) {
                    if (agent->conflictSituation.has_value() && !EgoHasRightOfWay(agent)) {
                        return false;
                    }
                }
                else if (agent->relativeDistance > 0 && !agent->laneInLineWithEgoLane) {
                    if (agent->conflictSituation.has_value() && EgoHasRightOfWay(agent)) {
                        return false;
                    }
                }
                return true;
            }
            return false;
        case ActionState::ReactToIntersectionSituation:
            return agent->conflictSituation.has_value() &&
                   (!EgoHasRightOfWay(agent) || CloseToConflictArea(agent, agent->conflictSituation));
        case ActionState::End:
            return true;
        }
        throw std::logic_error("ActionState is not valid");
    } catch (...) {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + "unexpected exception ";
        throw std::logic_error(message);
    }
}
bool ActionStateHandler::CloseToConflictArea(const std::unique_ptr<AgentInterpretation> &oAgent,
                                             std::optional<ConflictSituation> cA) const {
    if (cA) {
        if ((cA->junction == oAgent->agent->NextJunction() ||
             cA->junction == oAgent->agent->GetLanePosition().lane->GetRoad()->GetJunction()) &&
            cA->junction != nullptr) {
            return oAgent->agent->GetJunctionDistance().on > 0 ||
                   (oAgent->agent->GetJunctionDistance().toNext > -1 && oAgent->agent->GetJunctionDistance().toNext < 7);
        }
        if (cA->oAgentDistance.vehicleFrontToCAStart < 7) {
            return true;
        }
    }
    return false;
}

bool ActionStateHandler::EgoHasRightOfWay(const std::unique_ptr<AgentInterpretation> &agent) const {
    return (agent->rightOfWay.ego && !agent->rightOfWay.observed);
}

void ActionStateHandler::IncrementState() {
    switch (currentState) {
    case ActionState::Start:
        currentState = ActionState::CollisionImminent;
        break;
    case ActionState::CollisionImminent:
        currentState = ActionState::Following;
        break;
    case ActionState::Following:
        currentState = ActionState::ReactToIntersectionSituation;
        break;
    case ActionState::ReactToIntersectionSituation:
        currentState = ActionState::End;
        break;
    case ActionState::End:
        break;
    default:
        throw std::logic_error("ActionState is not valid");
    }
}
