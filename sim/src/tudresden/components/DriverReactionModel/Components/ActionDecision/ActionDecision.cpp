/******************************************************************************
 * Copyright (c) 2019 TU Dresden
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
#include "ActionDecision.h"
#include <math.h>
#include <string>

namespace ActionDecision {

MinEmergencyBrakeDelay::MinEmergencyBrakeDelay(double minTimeEmergencyBrakeIsActive, int cycleTime)
    : minTimeEmergencyBrakeIsActive{minTimeEmergencyBrakeIsActive}, cycleTime{cycleTime} {}

void MinEmergencyBrakeDelay::ResetEmergencyState() {
    std::for_each(emergencyBrake.begin(), emergencyBrake.end(),
                  [](std::pair<const int, EmergencyBrakeInfo>& element) { element.second.emergencyState = false; });
}
void MinEmergencyBrakeDelay::InsertEmergencyBrakeEvent(const AmbientAgentRepresentation* agentData, double deceleration) {
    EmergencyBrakeInfo info;
    info.deceleration = deceleration;
    info.emergencyState = true;
    info.timeEmergencyBrakeActive = cycleTime;

    auto insert = emergencyBrake.insert({agentData->GetID(), info});
    if (!insert.second) {
        insert.first->second.emergencyState = true;
        insert.first->second.timeEmergencyBrakeActive += cycleTime;
        insert.first->second.deceleration = deceleration;
    }
}

std::vector<double> MinEmergencyBrakeDelay::ActivateIfNeeded(const std::unordered_map<int, std::unique_ptr<AgentInterpretation>>& agents) {
    for (auto it = begin(emergencyBrake); it != end(emergencyBrake);) {
        if (agents.find(it->first) == agents.end()) {
            it = emergencyBrake.erase(it);
        } else
            ++it;
    }

    std::vector<double> accelerations;
    std::for_each(emergencyBrake.begin(), emergencyBrake.end(), [=, &accelerations](std::pair<const int, EmergencyBrakeInfo>& element) {
        if (!element.second.emergencyState && element.second.timeEmergencyBrakeActive < minTimeEmergencyBrakeIsActive) {
            accelerations.push_back(element.second.deceleration);
            element.second.timeEmergencyBrakeActive += cycleTime;
        }
    });

    for (auto it = begin(emergencyBrake); it != end(emergencyBrake);) {
        if (!it->second.emergencyState && it->second.timeEmergencyBrakeActive >= minTimeEmergencyBrakeIsActive) {
            it = emergencyBrake.erase(it);
        } else
            ++it;
    }

    return accelerations;
}

bool AccelerationSorting(double d1, double d2) { return d1 < d2; }

void ActionDecision::Update() {
    try {
        if (currentRoad != worldRepresentation.egoAgent->GetRoad()) {
            currentRoad = worldRepresentation.egoAgent->GetRoad();
            velocityCalculator.Update();
        }
        accelerationResult = DetermineAccelerationWish();
    } catch (const std::logic_error& er) {
        std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " | " + er.what();
        Log(message, error);
        throw std::runtime_error(message);
    } catch (...) {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + "unexpected exception ";
        throw std::runtime_error(message);
    }
}

double ActionDecision::DetermineAccelerationWish() {
    double targetVelocity = velocityCalculator.PrepareTargetVelocity();
    std::vector<double> accelerations;
    accelerations.push_back(anticipation.CalculatePhaseAcceleration(targetVelocity, worldRepresentation.egoAgent->GetVelocity()));
    minEmergencyBrakeDelay.ResetEmergencyState();
    for (auto& entry : worldInterpretation.interpretedAgents) {
        auto& agent = entry.second;
        auto& agentData = agent->agent;

        switch (actionStateHandler.GetState(agent)) {
        case ActionState::Collision:
            double deceleration;
            if (agent->collisionPoint->collisionImminent) {
                deceleration = anticipation.GetMaxEmergencyAcceleration();
            }
            deceleration = AgentCrashImminent(agent, targetVelocity);
            minEmergencyBrakeDelay.InsertEmergencyBrakeEvent(agentData, deceleration);
            accelerations.push_back(deceleration);
            break;
        case ActionState::Following:
            accelerations.push_back(
                anticipation.MaximumAccelerationWish(targetVelocity, worldRepresentation.egoAgent->GetVelocity() - agentData->GetVelocity(),
                                                     *agent->followingDistanceToLeadingVehicle));
            break;
        case ActionState::EgoRoW:
            if (ObservedAgentIsInConlictArea(agentData)) {
                accelerations.push_back(anticipation.IntersectionGap(agent, targetVelocity));
            }
            break;
        case ActionState::IntersectionSituation:
            accelerations.push_back(anticipation.IntersectionGap(agent, targetVelocity));
            break;
        case ActionState::End:
            break;
        default:
            std::string message =
                "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " ActionState is unkown ";
            throw std::logic_error(message);
        }
    }
    auto emergencyDelay = minEmergencyBrakeDelay.ActivateIfNeeded(worldInterpretation.interpretedAgents);

    accelerations.insert(accelerations.end(), emergencyDelay.begin(), emergencyDelay.end());
    std::sort(accelerations.begin(), accelerations.end(), AccelerationSorting);
    return accelerations.front();
}

double ActionDecision::AgentCrashImminent(const std::unique_ptr<AgentInterpretation>& oAgent, double targetVelocity) const {
    if (observedAgentIsbehindEgoAgent(oAgent)) {
        return anticipation.MaximumAccelerationWish(targetVelocity, worldRepresentation.egoAgent->GetVelocity() - targetVelocity,
                                                    std::numeric_limits<double>::infinity());
    } else {
        return anticipation.Deceleration(oAgent);
    }
}
bool ActionDecision::observedAgentIsbehindEgoAgent(const std::unique_ptr<AgentInterpretation>& oAgent) const {
    return (oAgent->agent->GetNextLane() == worldRepresentation.egoAgent->GetLane() &&
            oAgent->agent->IsMovingInLaneDirection() == worldRepresentation.egoAgent->IsMovingInLaneDirection()) ||
           ((oAgent->agent->IsMovingInLaneDirection() == true && worldRepresentation.egoAgent->IsMovingInLaneDirection() == true) &&
            (oAgent->agent->GetRoad() == worldRepresentation.egoAgent->GetRoad() &&
             oAgent->agent->GetSCoordinate() < worldRepresentation.egoAgent->GetSCoordinate())) ||
           ((oAgent->agent->IsMovingInLaneDirection() == false && worldRepresentation.egoAgent->IsMovingInLaneDirection() == false) &&
            (oAgent->agent->GetRoad() == worldRepresentation.egoAgent->GetRoad() &&
             oAgent->agent->GetSCoordinate() > worldRepresentation.egoAgent->GetSCoordinate()));
}

bool ActionDecision::ObservedAgentIsInConlictArea(const AmbientAgentRepresentation* agent) const {
    return agent->GetDistanceOnJunction() > 0 ||
           (agent->GetDistanceToNextJunction() > -1 && agent->GetDistanceToNextJunction() < 5);
}

} // namespace ActionDecision
