/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "Helper.h"

#include <iostream>

#include "boost/geometry.hpp"

namespace trans = boost::geometry::strategy::transform;
namespace bg = boost::geometry;

namespace Common {

Common::Vector2d CreatPointInDistance(double distance, Common::Vector2d startingPoint, double directionAngle)
{
    double xPos = distance;
    double yPos = 0;
    typedef bg::model::point<double, 2, bg::cs::cartesian> point_type;
    point_type p(xPos, yPos);
    point_type pRot;

    // boost rotates in mathematically negative direction -->  (- angle)!
    trans::rotate_transformer<bg::radian, double, 2, 2> rotate(-directionAngle);
    bg::transform(p, pRot, rotate);

    // Translate over (startingPoint)
    point_type pTrans;
    trans::translate_transformer<double, 2, 2> translate(startingPoint.x, startingPoint.y);
    bg::transform(pRot, pTrans, translate);
    xPos = bg::get<0>(pTrans);
    yPos = bg::get<1>(pTrans);

    return Common::Vector2d{xPos, yPos};
}

const AmbientAgentRepresentation *FindAgentById(int agentId, const AmbientAgentRepresentations &agents)
{
    for (const auto &agent : agents)
    {
        if (agent->GetInternalData().id == agentId)
        {
            return agent.get();
        }
    }
    return nullptr;
}

std::optional<CollisionPoint>
ClosestCollisionPointByTime(const std::unordered_map<int, std::unique_ptr<AgentInterpretation>> &interpretedAgents)
{
    if (interpretedAgents.size() > 0)
    {
        CollisionPoint mostInteresting;
        for (const auto &agent : interpretedAgents) {
            auto currentCP = agent.second->collisionPoint;
            if (currentCP && currentCP->timeToCollision < mostInteresting.timeToCollision)
            {
                mostInteresting = *currentCP;
            }
        }
        if (mostInteresting.oAgentID != maxInt) {
            return mostInteresting;
        }
    }
    return std::nullopt;
}

std::optional<int> AgentWithClosestConflictPoint(const std::unordered_map<int, std::unique_ptr<AgentInterpretation>> &interpretedAgents)
{
    if (interpretedAgents.size() > 0)
    {
        ConflictSituation mostInteresting;
        int agentID = maxInt;
        for (const auto &agent : interpretedAgents)
        {
            auto currentCS = agent.second->conflictSituation;
            if (currentCS && currentCS->egoDistance.vehicleFrontToCAStart > 0 &&
                currentCS->egoDistance.vehicleFrontToCAStart < mostInteresting.egoDistance.vehicleFrontToCAStart) {
                mostInteresting = *currentCS;
                agentID = agent.first;
            }
        }
        if (agentID != maxInt) {
            return agentID;
        }
    }
    return std::nullopt;
}

bool CollisionImminent(const std::unordered_map<int, std::unique_ptr<AgentInterpretation>> &interpretedAgents)
{
    if (auto collisionPoint = ClosestCollisionPointByTime(interpretedAgents))
    {
        return collisionPoint->collisionImminent;
    }
    return false;
}

std::optional<int> LeadingCarID(const std::unordered_map<int, std::unique_ptr<AgentInterpretation>> &worldInterpretation)
{
    if (!worldInterpretation.empty())
    {
        auto distance = maxDouble;
        auto leadingAgentId = maxInt;

        for (auto &agent : worldInterpretation)
        {
            if (auto currentDistance = agent.second->relativeDistance) {
                if (*currentDistance < distance)
                {
                    leadingAgentId = agent.first;
                    distance = *currentDistance;
                }
            }
        }
        if (leadingAgentId != maxInt)
        {
            return leadingAgentId;
        }
    }
    return std::nullopt;
}
double GetDistanceStoppingPoint(const AgentRepresentation *ego, const AgentInterpretation *observedAgent,
                                const WorldInterpretation &worldInterpretation) {
    StoppingPoint sp;
    if (worldInterpretation.crossingInfo.phase == CrossingPhase::Deceleration_TWO ||
        worldInterpretation.crossingInfo.phase == CrossingPhase::Deceleration_ONE) {
        if (IsVehicle(ego) && IsVehicle(observedAgent->agent)) {
            if (ego->GetIndicatorState() == IndicatorState::IndicatorState_Left) {
                sp = worldInterpretation.crossingInfo.egoStoppingPoints.at(StoppingPointType::Vehicle_Left);
                if (sp.type == StoppingPointType::NONE) {
                    // for T-Intersection
                    sp = worldInterpretation.crossingInfo.egoStoppingPoints.at(StoppingPointType::Vehicle_Crossroad);
                }
            }
            else if (ego->GetIndicatorState() == IndicatorState::IndicatorState_Right) {
                sp = worldInterpretation.crossingInfo.egoStoppingPoints.at(StoppingPointType::Pedestrian_Right);
            }
            else if (ego->GetIndicatorState() == IndicatorState::IndicatorState_Off) {
                sp = worldInterpretation.crossingInfo.egoStoppingPoints.at(StoppingPointType::Vehicle_Crossroad);
            }
        }
        else if ((IsVehicle(ego) && observedAgent->agent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Pedestrian) &&
                 observedAgent->rightOfWay.ego) {
            sp = worldInterpretation.crossingInfo.egoStoppingPoints.at(StoppingPointType::Pedestrian_Crossing_ONE);
        }
        else if ((IsVehicle(ego) && observedAgent->agent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Pedestrian) &&
                 (observedAgent->rightOfWay.observed && ego->GetIndicatorState() == IndicatorState::IndicatorState_Left)) {
            sp = worldInterpretation.crossingInfo.egoStoppingPoints.at(StoppingPointType::Pedestrian_Left);
        }
        else {
            auto msg = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " Agent type is unknown ";
            throw std::logic_error(msg);
        }
    }
    else if (worldInterpretation.crossingInfo.phase == CrossingPhase::Crossing_Left_ONE) {
        if (IsVehicle(ego) && IsVehicle(observedAgent->agent)) {
            sp = worldInterpretation.crossingInfo.egoStoppingPoints.at(StoppingPointType::Vehicle_Left);
            if (sp.type == StoppingPointType::NONE) {
                sp = worldInterpretation.crossingInfo.egoStoppingPoints.at(StoppingPointType::Vehicle_Crossroad);
            }
        }
        else if (IsVehicle(ego) && (observedAgent->agent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Pedestrian &&
                                    observedAgent->rightOfWay.observed)) {
            sp = worldInterpretation.crossingInfo.egoStoppingPoints.at(StoppingPointType::Pedestrian_Left);
        }
        else {
            auto msg = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " Agent type is unknown ";
            throw std::logic_error(msg);
        }
    }
    else if (worldInterpretation.crossingInfo.phase == CrossingPhase::Crossing_Straight) {
        sp = worldInterpretation.crossingInfo.egoStoppingPoints.at(StoppingPointType::Vehicle_Crossroad);
    }
    else if (worldInterpretation.crossingInfo.phase == CrossingPhase::Crossing_Right) {
        sp = worldInterpretation.crossingInfo.egoStoppingPoints.at(StoppingPointType::Pedestrian_Right);
    }
    else if (worldInterpretation.crossingInfo.phase == CrossingPhase::Crossing_Left_TWO) {
        sp = worldInterpretation.crossingInfo.egoStoppingPoints.at(StoppingPointType::Pedestrian_Left);
    }
    else {
        return std::numeric_limits<double>::infinity();
    }

    if (sp.type != StoppingPointType::NONE) {
        return sp.distanceToEgoFront;
    }
    else {
        return std::numeric_limits<double>::infinity();
    }
}

bool IsVehicle(const AgentRepresentation *agent) {
    return agent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Car ||
           agent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Truck ||
           agent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Motorbike;
}

double AngleBetween2d(const Vector2d &vectorA, const Vector2d &vectorB) {
    auto cosAngle = vectorA.Dot(vectorB) / (std::fabs(vectorA.Length()) * std::fabs(vectorB.Length()));
    if (cosAngle < -1.0) {
        cosAngle = -1.0;
    } else if (cosAngle > 1.0) {
        cosAngle = 1.0;
    }
    return std::acos(cosAngle);
}

std::optional<Vector2d> IntersectionPoint(Vector2d p1, Vector2d p2, Vector2d q1, Vector2d q2) {
    double m1 = (p2.y - p1.y) / (p2.x - p1.x);
    double n1 = ((p1.y * p2.x) - (p2.y * p1.x)) / (p2.x - p1.x);
    double m2 = (q2.y - q1.y) / (q2.x - q1.x);
    double n2 = ((q1.y * q2.x) - (q2.y * q1.x)) / (q2.x - q1.x);
    if (p1 == q1) {
        return std::optional<Vector2d>{{p1.x, p1.y}};
    }
    else if (p1 == q2) {
        return std::optional<Vector2d>{{p1.x, p1.y}};
    }
    else if (p2 == q1) {
        return std::optional<Vector2d>{{p2.x, p2.y}};
    }
    else if (p2 == q2) {
        return std::optional<Vector2d>{{p2.x, p2.y}};
    }

    if (m1 - m2 == 0.0)
        return std::nullopt;
    double x = (n2 - n1) / (m1 - m2);
    double y = m1 * x + n1;
    return std::optional<Vector2d>{{x, y}};
}

ConflictSituation DistanceToConflictArea(const std::pair<const MentalInfrastructure::ConflictArea *, OwlId> &egoCA,
                                         const std::pair<const MentalInfrastructure::ConflictArea *, OwlId> &observedCA,
                                         const EgoAgentRepresentation *ego, const AgentRepresentation &observedAgent) {
    ConflictSituation result;
    auto distanceEgoToStartCA = DistanceToConflictPoint(ego, egoCA.first->start, egoCA.second);
    auto distanceEgoToEndCA = DistanceToConflictPoint(ego, egoCA.first->end, egoCA.second);
    if (!ego->IsMovingInLaneDirection()) {
        auto temp = distanceEgoToStartCA;
        distanceEgoToStartCA = distanceEgoToEndCA;
        distanceEgoToEndCA = temp;
    }
    auto distanceObservedToStartCA = DistanceToConflictPoint(&observedAgent, observedCA.first->start, observedCA.second);
    auto distanceObservedToEndCA = DistanceToConflictPoint(&observedAgent, observedCA.first->end, observedCA.second);
    if (!observedAgent.IsMovingInLaneDirection()) {
        auto temp = distanceObservedToStartCA;
        distanceObservedToStartCA = distanceObservedToEndCA;
        distanceObservedToEndCA = temp;
    }
    result.egoCA = egoCA.first;
    result.oAgentCA = observedCA.first;
    result.egoDistance.vehicleReferenceToCAStart = distanceEgoToStartCA;
    result.egoDistance.vehicleReferenceToCAEnd = distanceEgoToEndCA;
    result.oAgentDistance.vehicleReferenceToCAStart = distanceObservedToStartCA;
    result.oAgentDistance.vehicleReferenceToCAEnd = distanceObservedToEndCA;
    result.egoDistance.vehicleFrontToCAStart = distanceEgoToStartCA - ego->GetDistanceReferencePointToLeadingEdge();
    result.egoDistance.vehicleBackToCAEnd = distanceEgoToEndCA + (ego->GetLength() - ego->GetDistanceReferencePointToLeadingEdge());
    result.oAgentDistance.vehicleFrontToCAStart = distanceObservedToStartCA - observedAgent.GetDistanceReferencePointToLeadingEdge();
    result.oAgentDistance.vehicleBackToCAEnd =
        distanceObservedToEndCA + (observedAgent.GetLength() - observedAgent.GetDistanceReferencePointToLeadingEdge());
    return result;
}

double DistanceToConflictPoint(const AgentRepresentation *agent, const MentalInfrastructure::LanePoint &conflictAreaBorder, OwlId laneId) {
    double distanceToPoint;
    auto nextLaneEgo = agent->GetNextLane();
    auto currentLaneEgo = agent->GetLanePosition().lane;

    if (laneId == currentLaneEgo->GetOwlId()) {
        distanceToPoint = conflictAreaBorder.sOffset - agent->GetLanePosition().sCoordinate;
        if (!agent->IsMovingInLaneDirection()) {
            distanceToPoint = agent->GetLanePosition().sCoordinate - conflictAreaBorder.sOffset;
        }
    }
    else {
        double distanceConnectionDistance = 0;
        while (nextLaneEgo->GetOwlId() != laneId) {
            distanceConnectionDistance += nextLaneEgo->GetLength();
            nextLaneEgo = nextLaneEgo->NextLane(agent->GetIndicatorState(), agent->IsMovingInLaneDirection());
        }

        auto distanceFromEgoToEndOfLane = currentLaneEgo->GetLastPoint()->sOffset - agent->GetLanePosition().sCoordinate;
        auto distanceFromStartOfLaneToPoint = conflictAreaBorder.sOffset - nextLaneEgo->GetFirstPoint()->sOffset;
        if (!agent->IsMovingInLaneDirection()) {
            distanceFromEgoToEndOfLane = agent->GetLanePosition().sCoordinate - currentLaneEgo->GetFirstPoint()->sOffset;
            distanceFromStartOfLaneToPoint = nextLaneEgo->GetLastPoint()->sOffset - conflictAreaBorder.sOffset;
        }
        distanceToPoint = distanceFromEgoToEndOfLane + distanceFromStartOfLaneToPoint + distanceConnectionDistance;
    }
    return distanceToPoint;
}

} // namespace Common
