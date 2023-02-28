/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/

#include "Anticipation.h"
#include "Common/Definitions.h"
#include "Common/Helper.h"

namespace LongitudinalDecision {

double Anticipation::IntersectionGap(const std::unique_ptr<AgentInterpretation> &observedAgent) {
    auto oAgent = observedAgent->agent;
    std::cout << "observed velocity: " << oAgent->GetVelocity() << " | observed Acceleration " << oAgent->GetAcceleration()
              << " | observed sCoordinate: " << oAgent->GetLanePosition().sCoordinate << " | distanceToStartCA "
              << observedAgent->conflictSituation->oAgentDistance.vehicleFrontToCAStart << " | distanceToENDCA "
              << observedAgent->conflictSituation->oAgentDistance.vehicleBackToCAEnd << std::endl;
    auto oAgentID = oAgent->GetID();
    auto conflictSituation = observedAgent->conflictSituation;
    double freeAccelerationEgo = CalculatePhaseAcceleration();
    // observed or ego agent has passed conflict area
    if (conflictSituation->oAgentDistance.vehicleBackToCAEnd <= 0 || conflictSituation->egoDistance.vehicleBackToCAEnd <= 0) {
        DeletePriorityAgent(oAgentID);
        return freeAccelerationEgo;
    }

    auto tEgo = CalculateTimeToConflictAreaEgo(conflictSituation->egoDistance, worldRepresentation.egoAgent->GetVelocity());
    auto tObserved = CalculateTimeToConflictAreaObserved(*conflictSituation, oAgent);
    std::cout << " t ego front to CA start: " << tEgo.vehicleFrontToCAStart << std::endl;
    std::cout << " t ego back to CA end: " << tEgo.vehicleBackToCAEnd << std::endl;
    std::cout << " t observed front to CA start: " << tObserved.vehicleFrontToCAStart << std::endl;
    std::cout << " t observed back to CA end: " << tObserved.vehicleBackToCAEnd << std::endl;
    //  check whether ego (if he has right of way) can pass
    if ((oAgent->GetAcceleration() == 0.0 && oAgent->GetVelocity() == 0.0) && observedAgent->rightOfWay.ego) {
        if (observedAgent->collisionPoint || observedAgent->conflictSituation->oAgentDistance.vehicleFrontToCAStart <= 0) {
            return CalculateDeceleration(conflictSituation->egoDistance.vehicleFrontToCAStart, tObserved.vehicleBackToCAEnd,
                                         observedAgent.get());
        }
        std::cout << "Agent: " << worldRepresentation.egoAgent->GetID() << " | RoW and oagent do not move " << std::endl;
        return freeAccelerationEgo;
    }
    // observed agent stops
    if (!(tObserved.vehicleFrontToCAStart < std::numeric_limits<double>::infinity()) && observedAgent->rightOfWay.ego) {
        DeletePriorityAgent(oAgentID);
    }
    // observed agent pass conflict area until ego reaches conflict area
    if (tEgo.vehicleFrontToCAStart - tObserved.vehicleBackToCAEnd >= GetBehaviourData().adBehaviour.timeGapAcceptance) {
        std::cout << "Agent: " << worldRepresentation.egoAgent->GetID() << " | observed agent pass CA until ego reach CA" << std::endl;
        return freeAccelerationEgo;
    }

    if (std::any_of(priorityAgents.begin(), priorityAgents.end(), [oAgentID](int id) { return id == oAgentID; })) {
        std::cout << "Agent: " << worldRepresentation.egoAgent->GetID() << " |  let pass other agent " << std::endl;
        return CalculateDeceleration(conflictSituation->egoDistance.vehicleFrontToCAStart, tObserved.vehicleBackToCAEnd,
                                     observedAgent.get());
    }
    // ego stops
    if (!(tEgo.vehicleBackToCAEnd < std::numeric_limits<double>::infinity())) {
        std::cout << "Agent: " << worldRepresentation.egoAgent->GetID() << " | stop  " << std::endl;
        return CalculateDeceleration(conflictSituation->egoDistance.vehicleFrontToCAStart, tObserved.vehicleBackToCAEnd,
                                     observedAgent.get());
    }

    if (conflictSituation->egoDistance.vehicleFrontToCAStart < 0) {
        return freeAccelerationEgo;
    }

    if (tEgo.vehicleFrontToCAStart - tObserved.vehicleBackToCAEnd >= GetBehaviourData().adBehaviour.timeGapAcceptance ||
        tObserved.vehicleFrontToCAStart - tEgo.vehicleBackToCAEnd >= GetBehaviourData().adBehaviour.timeGapAcceptance) {
        std::cout << "Agent: " << worldRepresentation.egoAgent->GetID() << " | save to pass"
                  << " |Gap 1: " << tEgo.vehicleFrontToCAStart - tObserved.vehicleBackToCAEnd
                  << " |Gap 2: " << tObserved.vehicleFrontToCAStart - tEgo.vehicleBackToCAEnd
                  << " |  Acceleration : " << freeAccelerationEgo << std::endl;

        return freeAccelerationEgo;
    }
    else {
        priorityAgents.push_back(oAgentID);
        std::cout << "Agent: " << worldRepresentation.egoAgent->GetID() << " | gap too small" << std::endl;
        return CalculateDeceleration(conflictSituation->egoDistance.vehicleFrontToCAStart, tObserved.vehicleBackToCAEnd,
                                     observedAgent.get());
    }
}

void Anticipation::DeletePriorityAgent(int oAgentID) {
    priorityAgents.erase(std::remove_if(priorityAgents.begin(), priorityAgents.end(), [oAgentID](int id) { return id == oAgentID; }),
                         priorityAgents.end());
}

double Anticipation::Deceleration(const std::unique_ptr<AgentInterpretation>& observedAgent) const {
    auto egoAgent = worldRepresentation.egoAgent;
    double s = Common::GetDistanceStoppingPoint(egoAgent, observedAgent.get(), worldInterpretation);

    if (observedAgent->collisionPoint && s > observedAgent->collisionPoint->distanceCP) {
        s = observedAgent->collisionPoint->distanceCP;
    }

    if (observedAgent->conflictSituation && s > observedAgent->conflictSituation->egoDistance.vehicleFrontToCAStart) {
        s = observedAgent->conflictSituation->egoDistance.vehicleFrontToCAStart;
    }
    return MaximumAccelerationWish(0, egoAgent->GetVelocity(), s);
}

TimeToConflictArea Anticipation::CalculateTimeToConflictAreaEgo(DistanceToConflictArea distance, double velocity) const {
    TimeToConflictArea result;
    result.vehicleFrontToCAStart = TravelTimeEgo(distance.vehicleFrontToCAStart, velocity, worldInterpretation.targetVelocity);
    result.vehicleBackToCAEnd = TravelTimeEgo(distance.vehicleBackToCAEnd, velocity, worldInterpretation.targetVelocity);
    return result;
}

double Anticipation::TravelTimeEgo(double distance, double velocity, double vTarget) const {
    double result;
    auto egoAgent = worldRepresentation.egoAgent;
    double acceleration = ComfortAccelerationWish(vTarget, velocity - vTarget, std::numeric_limits<double>::infinity());
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

TimeToConflictArea Anticipation::CalculateTimeToConflictAreaObserved(const ConflictSituation &situation,
                                                                     const AmbientAgentRepresentation *oAgent) const {
    TimeToConflictArea result;
    result.vehicleFrontToCAStart =
        TravelTimeObserved(situation.oAgentDistance.vehicleFrontToCAStart, 0 >= situation.egoDistance.vehicleFrontToCAStart, oAgent);
    result.vehicleBackToCAEnd =
        TravelTimeObserved(situation.oAgentDistance.vehicleBackToCAEnd, 0 >= situation.egoDistance.vehicleFrontToCAStart, oAgent);
    return result;
}

double Anticipation::TravelTimeObserved(double distance, bool egoInsideConflictArea, const AmbientAgentRepresentation *oAgent) const {
    double result;
    double acceleration = oAgent->GetAcceleration();
    double velocity = oAgent->GetVelocity();
    if ((acceleration == 0.0 && velocity == 0.0) && !egoInsideConflictArea) {
        // ego anticipate  observed vehicle could accelerate unless ego is inside conflict area
        acceleration = GetBehaviourData().adBehaviour.maxAcceleration;
    }

    double interimResult = (velocity * velocity) + (2 * acceleration * distance);
    if (distance <= 0) {
        result = 0;
    }
    else if (acceleration == 0.0 && velocity > 0) {
        result = distance / velocity;
    }
    else if (acceleration == 0.0 && velocity == 0) {
        result = std::numeric_limits<double>::infinity();
    }
    else if (interimResult < 0) {
        result = std::numeric_limits<double>::infinity();
    }
    else if (interimResult >= 0) {
        result = (std::sqrt(interimResult) - velocity) / acceleration;
    }
    else {
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

double Anticipation::CalculatePhaseAcceleration() const {
    double v = worldRepresentation.egoAgent->GetVelocity();
    double velTarget = worldInterpretation.targetVelocity;
    double distance;
    double distanceToIntersection =
        worldRepresentation.egoAgent->GetJunctionDistance().toNext - worldRepresentation.egoAgent->GetDistanceReferencePointToLeadingEdge();
    auto refToFront = worldRepresentation.egoAgent->GetDistanceReferencePointToLeadingEdge();
    auto sPositionFront = worldRepresentation.egoAgent->GetLanePosition().sCoordinate + refToFront;

    auto egoAgentRoad = worldRepresentation.egoAgent->GetLanePosition().lane->GetRoad();
    bool frontExceedCurrentRoad = sPositionFront > egoAgentRoad->GetLength();
    auto distanceRefPointToEndOfRoad =
        frontExceedCurrentRoad && !egoAgentRoad->IsOnJunction()
            ? egoAgentRoad->GetLength() - worldRepresentation.egoAgent->GetLanePosition().sCoordinate + egoAgentRoad->GetLength()
            : egoAgentRoad->GetLength() - worldRepresentation.egoAgent->GetLanePosition().sCoordinate;

    if (CrossingPhase::Approach <= worldInterpretation.crossingInfo.phase && distanceToIntersection >= 0) {
        distance = distanceToIntersection;
    }
    else if (CrossingPhase::Deceleration_TWO <= worldInterpretation.crossingInfo.phase &&
             worldInterpretation.crossingInfo.phase <= CrossingPhase::Exit) {
        distance = distanceRefPointToEndOfRoad;
    }
    else {
        distance = std::numeric_limits<double>::infinity();
    }

    if (distance < std::numeric_limits<double>::infinity() && v >= velTarget) {
        //  IDM brake strategy
        auto s_star =
            GetBehaviourData().adBehaviour.minDistanceStationaryTraffic + (v * GetBehaviourData().adBehaviour.desiredFollowingTimeHeadway) +
            ((v * (v - velTarget)) / (2.0 * std::sqrt(GetBehaviourData().adBehaviour.maxAcceleration * std::abs(comfortDeceleration))));

        auto a = -GetBehaviourData().adBehaviour.maxAcceleration * ((s_star * s_star) / (distance * distance));
        return Common::ValueInBounds(GetBehaviourData().adBehaviour.comfortDeceleration.min, a,
                                     GetBehaviourData().adBehaviour.maxAcceleration);
    }
    else {
        return ComfortAccelerationWish(velTarget, v - velTarget, std::numeric_limits<double>::infinity());
    }
}

double Anticipation::CalculateDeceleration(double sFrontEgo, double tEndObserved, const AgentInterpretation *observedAgent) const {
    auto egoAgent = worldRepresentation.egoAgent;
    if (egoAgent->GetVelocity() == 0.0) {
        return 0;
    }

    double s = Common::GetDistanceStoppingPoint(egoAgent, observedAgent, worldInterpretation);
    std::cout << "distance to stopping point: " << s << std::endl;
    if (s > sFrontEgo || s < 0) {
        s = sFrontEgo;
    }
    std::cout << "distance Deceleration: " << s << std::endl;

    double aIDM = MaximumAccelerationWish(0, egoAgent->GetVelocity(), s);
    if (aIDM < behaviourData.adBehaviour.comfortDeceleration.min && observedAgent->rightOfWay.ego) {
        // ego slows down so that when he enters the conflict area, the observed agent has already passed the conflict area.
        double a = (2 * (sFrontEgo - (worldRepresentation.egoAgent->GetVelocity() * tEndObserved))) / (tEndObserved * tEndObserved);

        std::cout << " a: " << a << " | aIDM: " << aIDM << " | std::abs(worldRepresentation.egoAgent->GetVelocity() / a:"
                  << std::abs(worldRepresentation.egoAgent->GetVelocity() / a) << " | tEndObserved" << tEndObserved << std::endl;
        if ((tEndObserved < std::numeric_limits<double>::infinity() && a > aIDM) &&
            std::abs(worldRepresentation.egoAgent->GetVelocity() / a) > tEndObserved) {
            return a;
        }

        return MaximumAccelerationWish(0, egoAgent->GetVelocity(), sFrontEgo);
    }
    return aIDM;
}

} // namespace LongitudinalDecision
