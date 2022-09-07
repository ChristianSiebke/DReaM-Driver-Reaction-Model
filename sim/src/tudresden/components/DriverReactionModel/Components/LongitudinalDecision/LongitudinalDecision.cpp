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
#include "LongitudinalDecision.h"

#include <math.h>
#include <string>

namespace LongitudinalDecision {

MinEmergencyBrakeDelay::MinEmergencyBrakeDelay(double minTimeEmergencyBrakeIsActive, int cycleTime) :
    minTimeEmergencyBrakeIsActive{minTimeEmergencyBrakeIsActive}, cycleTime{cycleTime} {
}

void MinEmergencyBrakeDelay::ResetEmergencyState() {
    std::for_each(emergencyBrake.begin(), emergencyBrake.end(),
                  [](std::pair<const int, EmergencyBrakeInfo> &element) { element.second.emergencyState = false; });
}
void MinEmergencyBrakeDelay::InsertEmergencyBrakeEvent(int agentID, double deceleration) {
    EmergencyBrakeInfo info;
    info.deceleration = deceleration;
    info.emergencyState = true;
    info.timeEmergencyBrakeActive = cycleTime;

    auto insert = emergencyBrake.insert({agentID, info});
    if (!insert.second) {
        insert.first->second.emergencyState = true;
        insert.first->second.timeEmergencyBrakeActive += cycleTime;
        insert.first->second.deceleration = deceleration;
    }
}

std::vector<double> MinEmergencyBrakeDelay::ActivateIfNeeded(const std::unordered_map<int, std::unique_ptr<AgentInterpretation>> &agents) {
    for (auto it = begin(emergencyBrake); it != end(emergencyBrake);) {
        if (agents.find(it->first) == agents.end()) {
            it = emergencyBrake.erase(it);
        }
        else
            ++it;
    }

    std::vector<double> accelerations;
    std::for_each(emergencyBrake.begin(), emergencyBrake.end(), [=, &accelerations](std::pair<const int, EmergencyBrakeInfo> &element) {
        if (!element.second.emergencyState && element.second.timeEmergencyBrakeActive < minTimeEmergencyBrakeIsActive) {
            accelerations.push_back(element.second.deceleration);
            element.second.timeEmergencyBrakeActive += cycleTime;
        }
    });

    for (auto it = begin(emergencyBrake); it != end(emergencyBrake);) {
        if (!it->second.emergencyState && it->second.timeEmergencyBrakeActive >= minTimeEmergencyBrakeIsActive) {
            it = emergencyBrake.erase(it);
        }
        else
            ++it;
    }

    return accelerations;
}

bool AccelerationSorting(double d1, double d2) {
    return d1 < d2;
}

void LongitudinalDecision::Update() {
    try {
        accelerationResult = DetermineAccelerationWish();
    }
    catch (const std::logic_error &er) {
        std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " | " + er.what();
        Log(message, error);
        throw std::runtime_error(message);
    }
    catch (...) {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + "unexpected exception ";
        throw std::runtime_error(message);
    }
}

double LongitudinalDecision::DetermineAccelerationWish() {
    std::cout << "Agent: " << worldRepresentation.egoAgent->GetID() << "| START " << std::endl;

    std::vector<double> accelerations;
    accelerations.push_back(anticipation.CalculatePhaseAcceleration());
    minEmergencyBrakeDelay.ResetEmergencyState();
    for (auto &entry : worldInterpretation.interpretedAgents) {
        const auto &agent = entry.second;
        switch (actionStateHandler.GetState(agent)) {
        case ActionState::CollisionImminent:
            double deceleration;
            //-----
            std::cout << "Agent: " << worldRepresentation.egoAgent->GetID() << "| Collision "
                      << " | Collision agent :" << agent->agent->GetID() << std::endl;
            //-----
            deceleration = AgentCrashImminent(agent);
            minEmergencyBrakeDelay.InsertEmergencyBrakeEvent(agent->agent->GetID(), deceleration);
            accelerations.push_back(deceleration);
            break;
        case ActionState::Following:
            //-----
            std::cout << "Agent: " << worldRepresentation.egoAgent->GetID() << "| Following " << std::endl;
            //-----
            accelerations.push_back(anticipation.MaximumAccelerationWish(
                worldInterpretation.targetVelocity, worldRepresentation.egoAgent->GetVelocity() - agent->agent->GetVelocity(),
                *agent->followingDistanceToLeadingVehicle));
            break;
        case ActionState::IntersectionSituation:
            //-----
            std::cout << "Agent: " << worldRepresentation.egoAgent->GetID() << "| IntersectionSituation " << std::endl;
            //-----
            if (!EgoHasRightOfWay(agent) || CloseToConlictArea()) {
                accelerations.push_back(anticipation.IntersectionGap(agent));
            }
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
    std::cout << " accelerations.front(): " << accelerations.front() << std::endl;
    return accelerations.front();
}

double LongitudinalDecision::AgentCrashImminent(const std::unique_ptr<AgentInterpretation> &oAgent) const {
    if (observedAgentIsbehindEgoAgent(oAgent)) {
        return anticipation.MaximumAccelerationWish(worldInterpretation.targetVelocity,
                                                    worldRepresentation.egoAgent->GetVelocity() - worldInterpretation.targetVelocity,
                                                    std::numeric_limits<double>::infinity());
    }
    else if (oAgent->followingDistanceToLeadingVehicle.has_value()) {
        return anticipation.MaximumAccelerationWish(worldInterpretation.targetVelocity,
                                                    worldRepresentation.egoAgent->GetVelocity() - oAgent->agent->GetVelocity(),
                                                    *oAgent->followingDistanceToLeadingVehicle);
    }
    else {
        return anticipation.Deceleration(oAgent);
    }
}

bool LongitudinalDecision::EgoHasRightOfWay(const std::unique_ptr<AgentInterpretation> &agent) const {
    return (agent->rightOfWay.ego && !agent->rightOfWay.observed);
}

bool LongitudinalDecision::observedAgentIsbehindEgoAgent(const std::unique_ptr<AgentInterpretation> &oAgent) const {
    return (oAgent->agent->GetNextLane() == worldRepresentation.egoAgent->GetLane() &&
            oAgent->agent->IsMovingInLaneDirection() == worldRepresentation.egoAgent->IsMovingInLaneDirection()) ||
           ((oAgent->agent->IsMovingInLaneDirection() == true && worldRepresentation.egoAgent->IsMovingInLaneDirection() == true) &&
            (oAgent->agent->GetRoad() == worldRepresentation.egoAgent->GetRoad() &&
             oAgent->agent->GetSCoordinate() < worldRepresentation.egoAgent->GetSCoordinate())) ||
           ((oAgent->agent->IsMovingInLaneDirection() == false && worldRepresentation.egoAgent->IsMovingInLaneDirection() == false) &&
            (oAgent->agent->GetRoad() == worldRepresentation.egoAgent->GetRoad() &&
             oAgent->agent->GetSCoordinate() > worldRepresentation.egoAgent->GetSCoordinate()));
}

bool LongitudinalDecision::CloseToConlictArea() const {
    return worldRepresentation.egoAgent->GetDistanceOnJunction() > 0 || (worldRepresentation.egoAgent->GetDistanceToNextJunction() > -1 &&
                                                                         worldRepresentation.egoAgent->GetDistanceToNextJunction() < 5);
}

} // namespace LongitudinalDecision
