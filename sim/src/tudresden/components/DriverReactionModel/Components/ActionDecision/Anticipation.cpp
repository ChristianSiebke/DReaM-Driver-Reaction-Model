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

#include "Anticipation.h"
#include "Common/Definitions.h"
#include "Common/Helper.h"

namespace ActionDecision {

double Anticipation::IntersectionGap(const std::unique_ptr<AgentInterpretation>& observedAgent, double vTargetEgo) {
    auto egoAgent = worldRepresentation.egoAgent;
    auto oAgent = observedAgent->agent;
    auto oAgentID = oAgent->GetID();
    auto conflictSituation = observedAgent->conflictSituation;
    double freeAccelerationEgo = CalculatePhaseAcceleration(vTargetEgo, egoAgent->GetVelocity());

    // ego is already in conflict area and observed agent not
    if (conflictSituation->egoDistance.vehicleFrontToCAStart <= 0 && conflictSituation->oAgentDistance.vehicleFrontToCAStart >= 0) {
        return 0;
    }
    // observed or ego agent has passed conflict area
    if (conflictSituation->oAgentDistance.vehicleBackToCAEnd <= 0 || conflictSituation->egoDistance.vehicleBackToCAEnd <= 0) {
        DeletePriorityAgent(oAgentID);
        // TODO: delete also obstacle agent
        return freeAccelerationEgo;
    }
    // both agents in conflict area
    if (conflictSituation->egoDistance.vehicleFrontToCAStart < 0 && conflictSituation->oAgentDistance.vehicleFrontToCAStart < 0) {
        return maxEmergencyDeceleration;
    }
    auto tEgo = CalculateTimeToConflictAreaEgo(conflictSituation->egoDistance, egoAgent->GetVelocity(), vTargetEgo);
    auto tObserved =
        CalculateTimeToConflictAreaObserved(conflictSituation->oAgentDistance, oAgent->GetAcceleration(), oAgent->GetVelocity());

    // do not consider potential crash events that are more than X seconds in the future
    if (tEgo.vehicleFrontToCAStart > 5) {
        return freeAccelerationEgo;
    }
    //  check whether ego (if he has right of way) can pass
    if ((oAgent->GetAcceleration() == 0.0 && oAgent->GetVelocity() == 0.0) && observedAgent->rightOfWay.ego) {
        if (observedAgent->collisionPoint ||
            std::any_of(obstacles.begin(), obstacles.end(), [oAgentID](int id) { return id == oAgentID; })) {
            obstacles.push_back(oAgentID);
            return CalculateDeceleration(conflictSituation->egoDistance.vehicleFrontToCAStart, tObserved.vehicleBackToCAEnd, observedAgent);
        }
        return freeAccelerationEgo;
    }
    // observed agent will not reach conflict area
    if (!(tObserved.vehicleFrontToCAStart < std::numeric_limits<double>::infinity())) {
        DeletePriorityAgent(oAgentID);
    }
    // observed agent pass conflict area until ego reaches conflict area
    if (tEgo.vehicleFrontToCAStart - tObserved.vehicleBackToCAEnd >= 0) {
        return freeAccelerationEgo;
    }

    if (std::any_of(priorityAgents.begin(), priorityAgents.end(), [oAgentID](int id) { return id == oAgentID; })) {
        return CalculateDeceleration(conflictSituation->egoDistance.vehicleFrontToCAStart, tObserved.vehicleBackToCAEnd, observedAgent);
    }
    // ego will not pass conflict area
    if (!(tEgo.vehicleFrontToCAStart < std::numeric_limits<double>::infinity()) ||
        !(tEgo.vehicleBackToCAEnd < std::numeric_limits<double>::infinity())) {
        return CalculateDeceleration(conflictSituation->egoDistance.vehicleFrontToCAStart, tObserved.vehicleBackToCAEnd, observedAgent);
    }

    if (tEgo.vehicleFrontToCAStart - tObserved.vehicleBackToCAEnd >= 0 ||
        tObserved.vehicleFrontToCAStart - tEgo.vehicleBackToCAEnd >= GetBehaviourData().adBehaviour.timeGapAcceptance) {
        return freeAccelerationEgo;
    }
    else {
        priorityAgents.push_back(oAgentID);
        return CalculateDeceleration(conflictSituation->egoDistance.vehicleFrontToCAStart, tObserved.vehicleBackToCAEnd, observedAgent);
    }
}

void Anticipation::DeletePriorityAgent(int oAgentID) {
    priorityAgents.erase(std::remove_if(priorityAgents.begin(), priorityAgents.end(), [oAgentID](int id) { return id == oAgentID; }),
                         priorityAgents.end());
}

double Anticipation::Deceleration(const std::unique_ptr<AgentInterpretation>& observedAgent) const {
    auto egoAgent = worldRepresentation.egoAgent;
    double s = Common::GetDistanceStoppingPoint(egoAgent->GetVehicleType(), worldInterpretation);

    if (observedAgent->collisionPoint && s > observedAgent->collisionPoint->distanceCP) {
        s = observedAgent->collisionPoint->distanceCP;
    }

    if (observedAgent->conflictSituation && s > observedAgent->conflictSituation->egoDistance.vehicleFrontToCAStart) {
        s = observedAgent->conflictSituation->egoDistance.vehicleFrontToCAStart;
    }
    return MaximumAccelerationWish(0, egoAgent->GetVelocity(), s);
}

TimeToConflictArea Anticipation::CalculateTimeToConflictAreaEgo(DistanceToConflictArea distance, double velocity, double vTarget) const {
    TimeToConflictArea result;
    result.vehicleFrontToCAStart = TravelTimeEgo(distance.vehicleFrontToCAStart, velocity, vTarget);
    result.vehicleBackToCAEnd = TravelTimeEgo(distance.vehicleBackToCAEnd, velocity, vTarget);
    return result;
}

double Anticipation::TravelTimeEgo(double distance, double velocity, double vTarget) const {
    double result;
    auto egoAgent = worldRepresentation.egoAgent;
    double acceleration = ComfortAccelerationWish(vTarget, egoAgent->GetVelocity() - vTarget, std::numeric_limits<double>::infinity());
    // distance to reach target velocity
    double sAcceleration = 0;
    if (acceleration != 0.0) {
        sAcceleration =
            (acceleration / 2) * std::pow((vTarget - velocity) / acceleration, 2) + velocity * ((vTarget - velocity) / acceleration);
        if (sAcceleration < 0) {
            std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                                  "  Error in calculation TimeToConflictArea  ";
            throw std::logic_error(message);
        }
    }

    double interimResult = (velocity * velocity) + (2 * acceleration * sAcceleration);
    double t1;
    if (sAcceleration <= 0) {
        t1 = 0;
    } else if (acceleration == 0.0) {
        t1 = sAcceleration / velocity;
    } else if (interimResult < 0) {
        t1 = std::numeric_limits<double>::infinity();
    } else if (interimResult > 0) {
        t1 = (std::sqrt(interimResult) - velocity) / acceleration;
    } else if (interimResult == 0.0) {
        t1 = velocity / acceleration;
    } else {
        std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                              "  Error in calculation TimeToConflictArea  ";
        throw std::logic_error(message);
    }

    if (sAcceleration >= distance) {
        interimResult = (velocity * velocity) + (2 * acceleration * distance);
        if (distance <= 0) {
            return 0;
        } else if (acceleration == 0.0) {
            return distance / velocity;
        } else if (interimResult < 0) {
            return std::numeric_limits<double>::infinity();
        } else if (interimResult >= 0) {
            return (std::sqrt(interimResult) - velocity) / acceleration;
        } else {
            std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                                  "  Error in calculation TimeToConflictArea  ";
            throw std::logic_error(message);
        }
    }

    double sConstantSpeed = distance - sAcceleration;
    double t2 = sConstantSpeed / vTarget;
    result = t1 + t2;

    std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                          "  Error in calculation: time to conflict area is negative ";
    return result >= 0 ? result : throw std::logic_error(message);
}

TimeToConflictArea Anticipation::CalculateTimeToConflictAreaObserved(DistanceToConflictArea distance, double acceleration,
                                                                     double velocity) const {
    TimeToConflictArea result;
    result.vehicleFrontToCAStart = TravelTimeObserved(distance.vehicleFrontToCAStart, acceleration, velocity);
    result.vehicleBackToCAEnd = TravelTimeObserved(distance.vehicleBackToCAEnd, acceleration, velocity);
    return result;
}

double Anticipation::TravelTimeObserved(double distance, double acceleration, double velocity) const {
    double result;
    if (acceleration == 0.0 && velocity == 0.0) {
        // anticipate  acceleration
        acceleration = GetBehaviourData().adBehaviour.maxAcceleration;
    }

    double interimResult = (velocity * velocity) + (2 * acceleration * distance);
    if (distance <= 0) {
        result = 0;
    } else if (acceleration == 0.0) {
        result = distance / velocity;
    } else if (interimResult < 0) {
        result = std::numeric_limits<double>::infinity();
    } else if (interimResult >= 0) {
        result = (std::sqrt(interimResult) - velocity) / acceleration;
    } else {
        std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                              "  Error in calculation TimeToConflictArea  ";
        throw std::logic_error(message);
    }

    std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                          "  Error in calculation: time to conflict area is negative ";
    return result >= 0 ? result : throw std::logic_error(message);
}

double Anticipation::IDMAcceleration(double velTarget, double dv, double sDiff) const {
    double aWish;
    double velCurrent = worldRepresentation.egoAgent->GetVelocity();
    if (sDiff <= 0.0) {
        aWish = maxEmergencyDeceleration;
    } else if (velTarget == 0.0) {
        double b = (velCurrent * velCurrent) / (2.0 * (sDiff - GetBehaviourData().adBehaviour.minDistanceStationaryTraffic));
        aWish = -1 * ((b * b) / std::abs(comfortDeceleration));
    } else {
        // free term
        double exponent = 4.0;
        double aFree = 1.0 - std::pow((velCurrent / velTarget), exponent);
        // s*(v, dv) part of the differential equation
        double sWish =
            std::max(0.0, velCurrent * GetBehaviourData().adBehaviour.desiredFollowingTimeHeadway +
                              ((velCurrent * dv) /
                               (2.0 * std::sqrt(GetBehaviourData().adBehaviour.maxAcceleration * std::abs(comfortDeceleration)))));
        sWish += GetBehaviourData().adBehaviour.minDistanceStationaryTraffic;
        // interaction term
        double aInt = std::pow((sWish / sDiff), 2.0);
        aWish = GetBehaviourData().adBehaviour.maxAcceleration * (aFree - aInt);
    }
    return aWish;
}

double Anticipation::ComfortAccelerationWish(double velTarget, double dv, double sDiff) const {
    return Common::ValueInBounds(GetBehaviourData().adBehaviour.comfortDeceleration.min, IDMAcceleration(velTarget, dv, sDiff),
                                 GetBehaviourData().adBehaviour.maxAcceleration);
}

double Anticipation::MaximumAccelerationWish(double velTarget, double dv, double sDiff) const {
    return Common::ValueInBounds(maxEmergencyDeceleration, IDMAcceleration(velTarget, dv, sDiff),
                                 GetBehaviourData().adBehaviour.maxAcceleration);
}

double Anticipation::CalculatePhaseAcceleration(double velTarget, double v) {
    double distance = std::numeric_limits<double>::infinity();
    if (worldInterpretation.crossingInfo.phase == CrossingPhase::Deceleration_ONE) {
        distance = worldRepresentation.egoAgent->GetDistanceToNextJunction();
    }
    else if (worldInterpretation.crossingInfo.phase == CrossingPhase::Deceleration_TWO) {
        distance = worldRepresentation.egoAgent->GetDistanceToNextJunction();
    }
    else if (CrossingPhase::Deceleration_TWO < worldInterpretation.crossingInfo.phase &&
             worldInterpretation.crossingInfo.phase < CrossingPhase::Exit) {
        distance = worldRepresentation.egoAgent->GetLane()->GetLength() - worldRepresentation.egoAgent->GetDistanceOnJunction();
    }

    if (distance < std::numeric_limits<double>::infinity()) {
        double a = (GetBehaviourData().adBehaviour.maxAcceleration * v * v * ((velTarget - v) * (velTarget - v))) /
                   (4 * GetBehaviourData().adBehaviour.maxAcceleration * std::abs(comfortDeceleration) * (distance * distance));
        a = velTarget - v < 0 ? -a : a;
        return Common::ValueInBounds(GetBehaviourData().adBehaviour.comfortDeceleration.min, a,
                                     GetBehaviourData().adBehaviour.maxAcceleration);
    } else {
        return ComfortAccelerationWish(velTarget, v - velTarget, std::numeric_limits<double>::infinity());
    }
}

double Anticipation::CalculateDeceleration(double sFrontEgo, double tEndObserved,
                                           const std::unique_ptr<AgentInterpretation>& observedAgent) const {
    auto egoAgent = worldRepresentation.egoAgent;
    if (egoAgent->GetVelocity() == 0.0) {
        return 0;
    }

    double s = Common::GetDistanceStoppingPoint(egoAgent->GetVehicleType(), worldInterpretation);
    if (s > sFrontEgo) {
        s = sFrontEgo;
    }

    double aIDM = MaximumAccelerationWish(0, egoAgent->GetVelocity(), s);
    if (aIDM < behaviourData.adBehaviour.comfortDeceleration.min && observedAgent->rightOfWay.ego) {
        if (0 < sFrontEgo - ((worldRepresentation.egoAgent->GetAcceleration() / 2 * (tEndObserved * tEndObserved)) +
                             worldRepresentation.egoAgent->GetVelocity() * tEndObserved)) {
            double a = (2 * (sFrontEgo - (worldRepresentation.egoAgent->GetVelocity() * tEndObserved))) / (tEndObserved * tEndObserved);
            if (tEndObserved < std::numeric_limits<double>::infinity() && a > aIDM) {
                return a;
            }
        }
        return MaximumAccelerationWish(0, egoAgent->GetVelocity(), sFrontEgo);
    }
    return aIDM;
}

} // namespace ActionDecision
