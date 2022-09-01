#include "ActionStateHandler.h"

ActionState ActionStateHandler::GetState(const std::unique_ptr<AgentInterpretation>& agent) {
    currentState = ActionState::Start;
    while (currentState != ActionState::End) {
        if (DetermineNextState(agent)) {
            break;
        } else {
            IncrementState();
        }
    }
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
            return agent->followingDistanceToLeadingVehicle.has_value();
        case ActionState::IntersectionSituation:
            return agent->conflictSituation.has_value();
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

void ActionStateHandler::IncrementState() {
    switch (currentState) {
    case ActionState::Start:
        currentState = ActionState::CollisionImminent;
        break;
    case ActionState::CollisionImminent:
        currentState = ActionState::Following;
        break;
    case ActionState::Following:
        currentState = ActionState::IntersectionSituation;
        break;
    case ActionState::IntersectionSituation:
        currentState = ActionState::End;
        break;
    case ActionState::End:
        break;
    default:
        throw std::logic_error("ActionState is not valid");
    }
}
