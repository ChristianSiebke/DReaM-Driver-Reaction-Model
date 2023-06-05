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

std::vector<std::pair<double, int>>
MinEmergencyBrakeDelay::ActivateIfNeeded(const std::unordered_map<int, std::unique_ptr<AgentInterpretation>> &agents) {
    for (auto it = begin(emergencyBrake); it != end(emergencyBrake);) {
        if (agents.find(it->first) == agents.end()) {
            it = emergencyBrake.erase(it);
        }
        else
            ++it;
    }

    std::vector<std::pair<double, int>> accelerations;
    std::for_each(emergencyBrake.begin(), emergencyBrake.end(), [=, &accelerations](std::pair<const int, EmergencyBrakeInfo> &element) {
        if (!element.second.emergencyState && element.second.timeEmergencyBrakeActive < minTimeEmergencyBrakeIsActive) {
            accelerations.push_back({element.second.deceleration, element.first});
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

bool AccelerationSorting(std::pair<double, int> d1, std::pair<double, int> d2) {
    return d1.first < d2.first;
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
    //---debugging---
    debuggingState = "";
    debuggingState += "TargetVelocity= " + std::to_string(static_cast<int>(worldInterpretation.targetVelocity)) + "|";
    //---debugging---
    std::vector<std::pair<double, int>> accelerations;
    accelerations.push_back({anticipation.CalculatePhaseAcceleration(), 999});
    minEmergencyBrakeDelay.ResetEmergencyState();
    for (auto &entry : worldInterpretation.interpretedAgents) {
        const auto &agent = entry.second;
        switch (actionStateHandler.GetState(agent)) {
        case ActionState::CollisionImminent: {
            //---debugging--
            debuggingState += "A:" + std::to_string(agent->agent->GetID()) + " Collision|";
            //---debugging--

            double deceleration = AgentCrashImminent(agent);
            minEmergencyBrakeDelay.InsertEmergencyBrakeEvent(agent->agent->GetID(), deceleration);
            accelerations.push_back({deceleration, agent->agent->GetID()});
            break;
        }
        case ActionState::Following: {
            //---debugging--
            debuggingState += "A:" + std::to_string(agent->agent->GetID()) + " Following|";
            //---debugging--
            if ((agent->conflictSituation && agent->conflictSituation->oAgentDistance.vehicleFrontToCAStart > 0) &&
                (agent->agent->GetVelocity() == 0 && !agent->laneInLineWithEgoLane)) {
                debuggingState += " XX|";
                break;
            }
            if (*agent->relativeDistance < 0) {
                // agent is behind ego
                break;
            }
            if (worldInterpretation.crossingInfo.phase > CrossingPhase::Approach) {
                accelerations.push_back(
                    {anticipation.AnticipationAccelerationToAchieveVelocityInDistance(*agent->relativeDistance, agent->agent->GetVelocity(),
                                                                                      worldRepresentation.egoAgent->GetVelocity()),
                     agent->agent->GetID()});
            }
            else {
                accelerations.push_back(
                    {anticipation.MaximumAccelerationWish(worldInterpretation.targetVelocity, worldRepresentation.egoAgent->GetVelocity(),
                                                          worldRepresentation.egoAgent->GetVelocity() - agent->agent->GetVelocity(),
                                                          *agent->relativeDistance),
                     agent->agent->GetID()});
            }
            break;
        }
        case ActionState::ReactToIntersectionSituation: {
            //---debugging--

            debuggingState += "A:" + std::to_string(agent->agent->GetID()) + " I " +
                              " EROW=" + std::to_string(static_cast<int>(agent->rightOfWay.ego)) +
                              " OROW=" + std::to_string(static_cast<int>(agent->rightOfWay.observed)) + "|";

            ////---debugging--
            accelerations.push_back({anticipation.IntersectionGap(agent), agent->agent->GetID()});
            break;
        }
        case ActionState::End: {
            break;
        }
        default: {
            std::string message =
                "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " ActionState is unkown ";
            throw std::logic_error(message);
        }
        }
    }
    auto emergencyDelay = minEmergencyBrakeDelay.ActivateIfNeeded(worldInterpretation.interpretedAgents);

    accelerations.insert(accelerations.end(), emergencyDelay.begin(), emergencyDelay.end());
    std::sort(accelerations.begin(), accelerations.end(), AccelerationSorting);
    debuggingState +=
        "Decelerate Agent = " + std::to_string(accelerations.front().second) +
        " Wish Decel=" + std::to_string(static_cast<int>(accelerations.front().first)) + ":" +
        std::to_string(static_cast<int>(((accelerations.front().first) - static_cast<int>(accelerations.front().first)) * 100));
 if(worldRepresentation.egoAgent->GetID()== 30)
    std::cout << "Ego" << worldRepresentation.egoAgent->GetID() << " | Acceleration: " << accelerations.front().first
              << " | oAgent=" << accelerations.front().second << std::endl;
    return accelerations.front().first;
}

double LongitudinalDecision::AgentCrashImminent(const std::unique_ptr<AgentInterpretation> &oAgent) const {
    if (oAgent->relativeDistance.has_value() && oAgent->relativeDistance < 0) {
        return anticipation.MaximumAccelerationWish(worldInterpretation.targetVelocity, worldRepresentation.egoAgent->GetVelocity(),
                                                    worldRepresentation.egoAgent->GetVelocity() - worldInterpretation.targetVelocity,
                                                    std::numeric_limits<double>::infinity());
    }
    else if (oAgent->relativeDistance.has_value() && oAgent->relativeDistance > 0) {
        return anticipation.MaximumAccelerationWish(worldInterpretation.targetVelocity, worldRepresentation.egoAgent->GetVelocity(),
                                                    worldRepresentation.egoAgent->GetVelocity() - oAgent->agent->GetVelocity(),
                                                    *oAgent->relativeDistance);
    }
    else {
        return anticipation.Deceleration(oAgent);
    }
}

} // namespace LongitudinalDecision
