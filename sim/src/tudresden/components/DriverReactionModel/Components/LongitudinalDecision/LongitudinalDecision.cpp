/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
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

            deceleration = AgentCrashImminent(agent);
            minEmergencyBrakeDelay.InsertEmergencyBrakeEvent(agent->agent->GetID(), deceleration);
            accelerations.push_back(deceleration);
            //-----
            std::cout << "Agent: " << worldRepresentation.egoAgent->GetID() << "| Collision acceleration: " << deceleration
                      << " | Collision agent :" << agent->agent->GetID() << std::endl;
            //-----
            break;
        case ActionState::Following:
            if ((agent->conflictSituation && agent->conflictSituation->oAgentDistance.vehicleFrontToCAStart > 0) &&
                (agent->agent->GetVelocity() == 0)) {
                break;
            }
            if (*agent->relativeDistance < 0) {
                // agent is behind ego
                break;
            }
            if (worldInterpretation.crossingInfo.phase > CrossingPhase::Approach) {
                double v = worldInterpretation.targetVelocity < agent->agent->GetVelocity() ? worldInterpretation.targetVelocity
                                                                                            : agent->agent->GetVelocity();
                accelerations.push_back(anticipation.AnticipationAccelerationToAchieveVelocityInDistance(
                    *agent->relativeDistance, v, worldRepresentation.egoAgent->GetVelocity()));
            }
            else {
                accelerations.push_back(anticipation.MaximumAccelerationWish(
                    worldInterpretation.targetVelocity, worldRepresentation.egoAgent->GetVelocity() - agent->agent->GetVelocity(),
                    *agent->relativeDistance));
            }
            //-----
            std::cout << "Agent: " << worldRepresentation.egoAgent->GetID() << " | relative distance=" << *agent->relativeDistance
                      << " | Following acceleration:" << accelerations.back() << " | Observed Agent=" << agent->agent->GetID() << std::endl;
            //-----
            break;
        case ActionState::ReactToIntersectionSituation:
            accelerations.push_back(anticipation.IntersectionGap(agent));

            //-----
            std::cout << "Agent: " << worldRepresentation.egoAgent->GetID()
                      << "| ReactToIntersectionSituation  acceleration: " << accelerations.back()
                      << " | Observed Agent=" << agent->agent->GetID() << std::endl;
            //-----

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
    else if (oAgent->relativeDistance.has_value()) {
        return anticipation.MaximumAccelerationWish(worldInterpretation.targetVelocity,
                                                    worldRepresentation.egoAgent->GetVelocity() - oAgent->agent->GetVelocity(),
                                                    *oAgent->relativeDistance);
    }
    else {
        return anticipation.Deceleration(oAgent);
    }
}

bool LongitudinalDecision::observedAgentIsbehindEgoAgent(const std::unique_ptr<AgentInterpretation> &oAgent) const {
    return (oAgent->agent->GetNextLane() == worldRepresentation.egoAgent->GetLanePosition().lane &&
            oAgent->agent->IsMovingInLaneDirection() == worldRepresentation.egoAgent->IsMovingInLaneDirection()) ||
           ((oAgent->agent->IsMovingInLaneDirection() == true && worldRepresentation.egoAgent->IsMovingInLaneDirection() == true) &&
            (oAgent->agent->GetLanePosition().lane->GetRoad() == worldRepresentation.egoAgent->GetLanePosition().lane->GetRoad() &&
             oAgent->agent->GetLanePosition().sCoordinate < worldRepresentation.egoAgent->GetLanePosition().sCoordinate)) ||
           ((oAgent->agent->IsMovingInLaneDirection() == false && worldRepresentation.egoAgent->IsMovingInLaneDirection() == false) &&
            (oAgent->agent->GetLanePosition().lane->GetRoad() == worldRepresentation.egoAgent->GetLanePosition().lane->GetRoad() &&
             oAgent->agent->GetLanePosition().sCoordinate > worldRepresentation.egoAgent->GetLanePosition().sCoordinate));
}

} // namespace LongitudinalDecision
