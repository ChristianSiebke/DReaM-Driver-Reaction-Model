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
    auto oAgentID = oAgent->GetID();
    auto conflictSituation = observedAgent->conflictSituation;
    double freeAccelerationEgo = CalculatePhaseAcceleration();

    UpdatePriorityAgents(oAgentID, {*conflictSituation, observedAgent->rightOfWay});
    if (conflictSituation->oAgentDistance.vehicleBackToCAEnd <= 0 || conflictSituation->egoDistance.vehicleBackToCAEnd <= 0) {
        // observed or ego agent has passed conflict area
        DeletePriorityAgent(oAgentID);
        return freeAccelerationEgo;
    }

    auto tEgo = CalculateTimeToConflictAreaEgo(conflictSituation->egoDistance, worldRepresentation.egoAgent->GetVelocity());
    auto tObserved = CalculateTimeToConflictAreaObserved(*conflictSituation, observedAgent);

    if (tEgo.vehicleFrontToCAStart - tObserved.vehicleBackToCAEnd >= GetBehaviourData().adBehaviour.timeGapAcceptance) {
        // observed agent pass conflict area until ego reaches conflict area
        return freeAccelerationEgo;
    }

    if (Common::AgentTouchesLane(observedAgent->agent, conflictSituation->egoCA->lane)) {
        return ApproachingStoppingPoint(conflictSituation->egoDistance.vehicleFrontToCAStart, tObserved.vehicleBackToCAEnd,
                                        observedAgent.get());
    }

    if ((oAgent->GetAcceleration() == 0.0 && oAgent->GetVelocity() == 0.0) && observedAgent->rightOfWay.ego) {
        //  check whether agent can pass
        if (observedAgent->collisionPoint) {
            return ApproachingStoppingPoint(conflictSituation->egoDistance.vehicleFrontToCAStart, tObserved.vehicleBackToCAEnd,
                                            observedAgent.get());
        }
        return freeAccelerationEgo;
    }

    if (!(tObserved.vehicleFrontToCAStart < std::numeric_limits<double>::infinity()) && observedAgent->rightOfWay.ego) {
        // observed agent stops
        DeletePriorityAgent(oAgentID);
        return freeAccelerationEgo;
    }

    if ((oAgent->GetAcceleration() == 0.0 && oAgent->GetVelocity() == 0.0) &&
        std::any_of(priorityAgents.begin(), priorityAgents.end(), [this, conflictSituation](auto element) {
            return Common::AgentTouchesLane(worldRepresentation.egoAgent, element.second.conflictSituation.oAgentCA->lane) ||
                   (!Common::AgentTouchesLane(worldRepresentation.egoAgent, element.second.conflictSituation.oAgentCA->lane) &&
                    (conflictSituation->egoDistance.vehicleFrontToCAStart < -worldRepresentation.egoAgent->GetLength() &&
                     conflictSituation->egoDistance.vehicleBackToCAEnd <
                         (conflictSituation->egoCA->end.sOffset - conflictSituation->egoCA->start.sOffset) / 2));
        })) {
        // if ego block intersection
        return freeAccelerationEgo;
    }
    if (std::any_of(priorityAgents.begin(), priorityAgents.end(), [oAgentID, conflictSituation](auto element) {
            return element.first == oAgentID && element.second.conflictSituation.egoCA == conflictSituation->egoCA;
        })) {
        return AccelerationIfPriorityAgentExist(observedAgent, conflictSituation, tObserved, freeAccelerationEgo);
    }

    if (!(tEgo.vehicleBackToCAEnd < std::numeric_limits<double>::infinity())) {
        // ego stops
        return ApproachingStoppingPoint(conflictSituation->egoDistance.vehicleFrontToCAStart, tObserved.vehicleBackToCAEnd,
                                        observedAgent.get());
    }

    if (tEgo.vehicleFrontToCAStart - tObserved.vehicleBackToCAEnd >= GetBehaviourData().adBehaviour.timeGapAcceptance ||
        tObserved.vehicleFrontToCAStart - tEgo.vehicleBackToCAEnd >= GetBehaviourData().adBehaviour.timeGapAcceptance ||
        (tObserved.vehicleFrontToCAStart - tEgo.vehicleBackToCAEnd >= 1 && observedAgent->rightOfWay.ego) ||
        (tObserved.vehicleFrontToCAStart - tEgo.vehicleBackToCAEnd > 0 &&
         (conflictSituation->egoDistance.vehicleFrontToCAStart < 1 ||
          GetBehaviourData().adBehaviour.comfortDeceleration.min > Deceleration(observedAgent))) ||
        (tObserved.vehicleFrontToCAStart - tEgo.vehicleBackToCAEnd > 0 &&
         (conflictSituation->egoDistance.vehicleBackToCAEnd - worldRepresentation.egoAgent->GetLength() < 0 ||
          GetBehaviourData().adBehaviour.comfortDeceleration.min > Deceleration(observedAgent)))) {
        return freeAccelerationEgo;
    }
    else {
        Priority priority = {*conflictSituation, observedAgent->rightOfWay};
        priorityAgents.insert_or_assign(oAgentID, priority);
        return ApproachingStoppingPoint(conflictSituation->egoDistance.vehicleFrontToCAStart, tObserved.vehicleBackToCAEnd,
                                        observedAgent.get());
    }
}

void Anticipation::UpdatePriorityAgents(int oAgentID, Priority priority) {
    for (auto it = priorityAgents.cbegin(); it != priorityAgents.cend();) {
        const auto &iter = worldInterpretation.interpretedAgents.find(it->first);
        if (iter != worldInterpretation.interpretedAgents.end() && !iter->second->conflictSituation.has_value()) {
            it = priorityAgents.erase(it);
        }
        else {
            ++it;
        }
    }

    const auto &itUpdate = priorityAgents.find(oAgentID);
    if (itUpdate != priorityAgents.end() && itUpdate->second != priority) {
        // Priority changed
        priorityAgents.erase(itUpdate);
    }
    else if (itUpdate != priorityAgents.end() && itUpdate->second == priority) {
        itUpdate->second = priority;
    }
}

void Anticipation::DeletePriorityAgent(int oAgentID) {
    priorityAgents.erase(oAgentID);
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
    return MaximumAccelerationWish(0, egoAgent->GetVelocity(), egoAgent->GetVelocity(), s);
}

TimeToConflictArea Anticipation::CalculateTimeToConflictAreaEgo(DistanceToConflictArea distance, double velocity) const {
    TimeToConflictArea result;
    result.vehicleFrontToCAStart = TravelTimeTargetVelocity(distance.vehicleFrontToCAStart, velocity, worldInterpretation.targetVelocity);
    result.vehicleBackToCAEnd = TravelTimeTargetVelocity(distance.vehicleBackToCAEnd, velocity, worldInterpretation.targetVelocity);
    return result;
}
TimeToConflictArea Anticipation::CalculateTimeToConflictAreaObserved(const ConflictSituation &situation,
                                                                     const std::unique_ptr<AgentInterpretation> &observedAgent) const {
    TimeToConflictArea result;
    result.vehicleFrontToCAStart =
        TravelTimeObserved(situation.oAgentDistance.vehicleFrontToCAStart, 0 >= situation.egoDistance.vehicleFrontToCAStart, observedAgent);
    result.vehicleBackToCAEnd =
        TravelTimeObserved(situation.oAgentDistance.vehicleBackToCAEnd, 0 >= situation.egoDistance.vehicleFrontToCAStart, observedAgent);
    return result;
}

double Anticipation::TravelTimeTargetVelocity(double distance, double velocity, double vTarget) const {
    double acceleration = ComfortAccelerationWish(vTarget, velocity, velocity - vTarget, std::numeric_limits<double>::infinity());
    // distance to reach target velocity
    double distanceAcceleration = 0;
    if (acceleration != 0.0) {
        distanceAcceleration =
            (acceleration / 2) * std::pow((vTarget - velocity) / acceleration, 2) + velocity * ((vTarget - velocity) / acceleration);
        if (distanceAcceleration < 0) {
            std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                                  "  Error in calculation TimeToConflictArea  ";
            throw std::logic_error(message);
        }
    }
    return TravelTime(distance, distanceAcceleration, velocity, acceleration, vTarget);
}

double Anticipation::TravelTime(double distance, double distanceAcceleration, double velocity, double acceleration, double vTarget) const {
    double result;
    double interimResult = (velocity * velocity) + (2 * acceleration * distanceAcceleration);
    double t1;
    if (distanceAcceleration <= 0) {
        t1 = 0;
    }
    else if (acceleration == 0.0) {
        t1 = distanceAcceleration / velocity;
    }
    else if (interimResult < 0) {
        t1 = std::numeric_limits<double>::infinity();
    }
    else if (interimResult > 0) {
        t1 = (std::sqrt(interimResult) - velocity) / acceleration;
    }
    else if (interimResult == 0.0) {
        t1 = velocity / acceleration;
    }
    else {
        std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                              "  Error in calculation TimeToConflictArea  ";
        throw std::logic_error(message);
    }

    if (distanceAcceleration >= distance) {
        interimResult = (velocity * velocity) + (2 * acceleration * distance);
        if (distance <= 0) {
            return 0;
        }
        else if (acceleration == 0.0) {
            return distance / velocity;
        }
        else if (interimResult < 0) {
            return std::numeric_limits<double>::infinity();
        }
        else if (interimResult >= 0) {
            return (std::sqrt(interimResult) - velocity) / acceleration;
        }
        else {
            std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                                  "  Error in calculation TimeToConflictArea  ";
            throw std::logic_error(message);
        }
    }

    double sConstantSpeed = distance - distanceAcceleration;
    double t2 = sConstantSpeed / vTarget;
    result = t1 + t2;

    std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                          "  Error in calculation: time to conflict area is negative ";
    return result >= 0 ? result : throw std::logic_error(message);
}

double Anticipation::TravelTimeObserved(double distance, bool egoInsideConflictArea,
                                        const std::unique_ptr<AgentInterpretation> &observedAgent) const {
    double acceleration = observedAgent->agent->GetAcceleration();
    double velocity = observedAgent->agent->GetVelocity();
    if ((acceleration == 0.0 && velocity == 0.0) && !egoInsideConflictArea) {
        // ego anticipate  observed vehicle could accelerate unless ego is inside conflict area
        acceleration = GetBehaviourData().adBehaviour.maxAcceleration;
    }
    if (distance <= 0) {
        return 0;
    }
    // limit estiamated target velocity for bicycle
    double estimatedTargetVelocityObsevedAgent =
        observedAgent->agent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Bicycle && worldInterpretation.targetVelocity > 6
            ? 6
            : worldInterpretation.targetVelocity;
    if (estimatedTargetVelocityObsevedAgent < velocity && acceleration > -0.1) {
        estimatedTargetVelocityObsevedAgent = velocity;
    }
    double decelerationStandstill = (velocity * velocity) / (2 * distance);
    if (decelerationStandstill < GetBehaviourData().adBehaviour.comfortDeceleration.min || acceleration >= 0 ||
        observedAgent->rightOfWay.observed) {
        // observed will probably not slow down for ego
        return TravelTimeTargetVelocity(distance, velocity, estimatedTargetVelocityObsevedAgent);
    }
    else {
        // observed will probably stop
        if (velocity < estimatedTargetVelocityObsevedAgent &&
            (acceleration < 0 &&
             GetBehaviourData().adBehaviour.comfortDeceleration.min / 2 < ComfortAccelerationWish(0, velocity, velocity, distance))) {
            return std::numeric_limits<double>::infinity();
        }

        double interimResult = (velocity * velocity) + (2 * acceleration * distance);

        if (acceleration == 0.0) {
            return distance / velocity;
        }
        else if (interimResult < 0) {
            return std::numeric_limits<double>::infinity();
        }
        else if (interimResult >= 0) {
            return (std::sqrt(interimResult) - velocity) / acceleration;
        }
        else {
            std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                                  "  Error in calculation TimeToConflictArea  ";
            throw std::logic_error(message);
        }
    }
}

double Anticipation::IDMAcceleration(double velTarget, double velCurrent, double dv, double sDiff) const {
    double aWish;
    double distance = sDiff - GetBehaviourData().adBehaviour.minDistanceStationaryTraffic;
    if (distance <= 0.0) {
        aWish = maxEmergencyDeceleration;
    }
    else if (velTarget == 0.0) {
        double b = (velCurrent * velCurrent) / (2.0 * (distance));
        aWish = -1 * ((b * b) / std::abs(comfortDeceleration));
    }
    else {
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

double Anticipation::ComfortAccelerationWish(double velTarget, double velCurrent, double dv, double sDiff) const {
    return Common::ValueInBounds(GetBehaviourData().adBehaviour.comfortDeceleration.min, IDMAcceleration(velTarget, velCurrent, dv, sDiff),
                                 GetBehaviourData().adBehaviour.maxAcceleration);
}

double Anticipation::MaximumAccelerationWish(double velTarget, double velCurrent, double dv, double sDiff) const {
    return Common::ValueInBounds(maxEmergencyDeceleration, IDMAcceleration(velTarget, velCurrent, dv, sDiff),
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
    else if (worldInterpretation.crossingInfo.type == CrossingType::Straight &&
             worldRepresentation.egoAgent->GetIndicatorState() != IndicatorState::IndicatorState_Off) {
        distance = worldRepresentation.egoAgent->GetLanePosition().lane->GetLength() -
                   (worldRepresentation.egoAgent->GetLanePosition().sCoordinate + refToFront) +
                   worldRepresentation.egoAgent->GetNextLane()->GetLength();
    }
    else {
        distance = std::numeric_limits<double>::infinity();
    }
    return AnticipationAccelerationToAchieveVelocityInDistance(distance, velTarget, v);
}

double Anticipation::AnticipationAccelerationToAchieveVelocityInDistance(double distance, double velTarget, double currentVelocity) const {
    if (distance < std::numeric_limits<double>::infinity() && currentVelocity >= velTarget) {
        return IDMBrakeStrategy(distance, velTarget, currentVelocity);
    }
    else {
        return ComfortAccelerationWish(velTarget, currentVelocity, currentVelocity - velTarget, std::numeric_limits<double>::infinity());
    }
}

double Anticipation::IDMBrakeStrategy(double distance, double velTarget, double currentVelocity) const {
    auto s_star = (GetBehaviourData().adBehaviour.minDistanceStationaryTraffic) +
                  (currentVelocity * GetBehaviourData().adBehaviour.desiredFollowingTimeHeadway) +
                  ((currentVelocity * (currentVelocity - velTarget)) /
                   (2.0 * std::sqrt(GetBehaviourData().adBehaviour.maxAcceleration * std::abs(comfortDeceleration))));

    auto a = -GetBehaviourData().adBehaviour.maxAcceleration * ((s_star * s_star) / (distance * distance));
    return Common::ValueInBounds(GetBehaviourData().adBehaviour.comfortDeceleration.min, a, GetBehaviourData().adBehaviour.maxAcceleration);
}

double Anticipation::ApproachingStoppingPoint(double sFrontEgo, double tEndObserved, const AgentInterpretation *observedAgent) const {
    auto egoAgent = worldRepresentation.egoAgent;
    double s = Common::GetDistanceStoppingPoint(egoAgent, observedAgent, worldInterpretation);
    if (s > sFrontEgo || s < 0) {
        s = sFrontEgo;
    }

    double aIDM = MaximumAccelerationWish(0, egoAgent->GetVelocity(), egoAgent->GetVelocity(), s);
    if (aIDM < behaviourData.adBehaviour.comfortDeceleration.min && observedAgent->rightOfWay.ego) {
        // ego slows down so that when it enters the conflict area, the observed agent has already passed the conflict area.
        double a = (2 * (sFrontEgo - (worldRepresentation.egoAgent->GetVelocity() * tEndObserved))) / (tEndObserved * tEndObserved);
        a = Common::ValueInBounds(maxEmergencyDeceleration, a, GetBehaviourData().adBehaviour.maxAcceleration);
        if ((tEndObserved < std::numeric_limits<double>::infinity() && a > aIDM) &&
            (a > 0 && std::abs(worldRepresentation.egoAgent->GetVelocity() / a)) > tEndObserved) {
            return a;
        }
        double aIDM2 = MaximumAccelerationWish(0, egoAgent->GetVelocity(), egoAgent->GetVelocity(), sFrontEgo);
        double deceleration = IDMBrakeStrategy(sFrontEgo, 0, egoAgent->GetVelocity());
        if (deceleration >= comfortDeceleration / 3 && sFrontEgo > GetBehaviourData().adBehaviour.minDistanceStationaryTraffic + 2) {
            return CalculatePhaseAcceleration();
        }
        return aIDM2;
    }

    double deceleration = IDMBrakeStrategy(s, 0, egoAgent->GetVelocity());
    if (deceleration >= comfortDeceleration / 3 && s > GetBehaviourData().adBehaviour.minDistanceStationaryTraffic + 2) {
        return CalculatePhaseAcceleration();
    }
    return aIDM;
}

double Anticipation::AccelerationIfPriorityAgentExist(const std::unique_ptr<AgentInterpretation> &observedAgent,
                                                      const std::optional<ConflictSituation> &conflictSituation,
                                                      const TimeToConflictArea &tObserved, double freeAccelerationEgo) {
    if (observedAgent->rightOfWay.ego == false && observedAgent->rightOfWay.observed == false) {
        if ((observedAgent->agent->GetAcceleration() == 0.0 && observedAgent->agent->GetVelocity() == 0.0) ||
            worldRepresentation.egoAgent->GetJunctionDistance().on > observedAgent->agent->GetJunctionDistance().on ||
            (worldRepresentation.egoAgent->GetJunctionDistance().toNext < observedAgent->agent->GetJunctionDistance().toNext)) {
            return freeAccelerationEgo;
        }
        else {
            return ApproachingStoppingPoint(conflictSituation->egoDistance.vehicleFrontToCAStart, tObserved.vehicleBackToCAEnd,
                                            observedAgent.get());
        }
    }
    return ApproachingStoppingPoint(conflictSituation->egoDistance.vehicleFrontToCAStart, tObserved.vehicleBackToCAEnd,
                                    observedAgent.get());
}

} // namespace LongitudinalDecision
