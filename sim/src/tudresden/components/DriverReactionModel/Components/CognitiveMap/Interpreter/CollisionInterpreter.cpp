/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/

#include "CollisionInterpreter.h"

#include <future>
#include <iostream>

#include "common/Helper.h"

namespace Interpreter {

// time step size for collision detection (s)
const double TIME_STEP = 0.1;
// maximum amounts of lanes an agent will look into
// only works with a maximum of 2 lanes!
const int MAX_LANES_AHEAD = 2;
// amount of vertices for generating a polygon based on an circle (for
// pedestrians) e.g. 4 vertices will result in a rectangle
const int CIRCLE_POLYGON_PRECISION = 8;
// used for debug output
const bool DEBUG_OUT = true;

void CollisionInterpreter::Update(WorldInterpretation *interpretation, const WorldRepresentation &representation) {
    DetermineCollisionPoints(interpretation, representation);
};

void CollisionInterpreter::DetermineCollisionPoints(WorldInterpretation *interpretation, const WorldRepresentation &representation) {
    try {
        Log("Calculating Collisionpoints");
        numberCollisionPoints = 0;
        if (representation.agentMemory->empty()) {
            Log("No surrounding agents... returning...", DReaMLogLevel::warning);
            return;
        }
        for (const auto &agent : *representation.agentMemory) {
            auto possibleCollisionPoint = CalculationCollisionPoint(representation, *agent);
            auto &agentInterpretation = interpretation->interpretedAgents.at(agent->GetID());
            agentInterpretation->collisionPoint = possibleCollisionPoint;

            if (possibleCollisionPoint) {
                if (DEBUG_OUT)
                    Log("No Collision with Agent:" + std::to_string(agent->GetID()) + " should occur.");
            }
            else {
                if (DEBUG_OUT)
                    Log("Collision with Agent:" + std::to_string(agent->GetID()) + " might occur!");
                numberCollisionPoints++;
            }
        }
        Log("Finished calculating collision points" + std::to_string(numberCollisionPoints) + " collision points, returning...");
    }
    catch (...) {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " Update collision points failed";
        Log(message, error);
    }
}

std::optional<CollisionPoint> CollisionInterpreter::CalculationCollisionPoint(const WorldRepresentation &representation,
                                                                              const AgentRepresentation &observedAgent) const {
    std::vector<std::future<std::optional<CollisionPoint>>> futures;
    std::vector<CollisionPoint> resultCP;

    if (representation.egoAgent->GetVelocity() <= 0.05 && observedAgent.GetVelocity() <= 0.05) {
        // no agent move --> no crash
        return std::nullopt;
    }
    double endTime = GetBehaviourData().adBehaviour.collisionImminentMargin;
    unsigned int numberThreads = 8;
    for (double startTime = 0; startTime < endTime; startTime += endTime / numberThreads) {
        double endTime = startTime + endTime / numberThreads;
        futures.push_back(std::async(std::launch::async, &CollisionInterpreter::PerformCollisionPointCalculation, this, startTime, endTime,
                                     representation, observedAgent));
    }

    for (auto &f : futures) {
        auto cp = f.get();
        if (cp) {
            resultCP.push_back(*cp);
        }
    }
    if (resultCP.empty()) {
        return std::nullopt;
    }
    CollisionPoint crashPoint =
        *std::min_element(resultCP.begin(), resultCP.end(), [](auto a, auto b) { return a.timeToCollision < b.timeToCollision; });
    return crashPoint;
}

std::optional<CollisionPoint> CollisionInterpreter::PerformCollisionPointCalculation(double timeStart, double timeEnd,
                                                                                     const WorldRepresentation &representation,
                                                                                     const AgentRepresentation &observedAgent) const {
    for (auto time = timeStart; time <= timeEnd; time += TIME_STEP) {
        double egoDistance = representation.egoAgent->ExtrapolateDistanceAlongLane(time);
        double observedDistance = observedAgent.ExtrapolateDistanceAlongLane(time);
        auto positionEgoAgent = representation.egoAgent->FindNewPositionInDistance(egoDistance);
        auto positionObservedAgent = observedAgent.FindNewPositionInDistance(observedDistance);

        if (!positionEgoAgent || !positionObservedAgent) {
            // agent out of road map
            return std::nullopt;
        }

        auto ego = positionEgoAgent->newLane->InterpolatePoint(positionEgoAgent->newSCoordinate);
        auto observed = positionObservedAgent->newLane->InterpolatePoint(positionObservedAgent->newSCoordinate);

        double hdgEgo = representation.egoAgent->IsMovingInLaneDirection() ? ego.hdg : ego.hdg + M_PI;
        double hdgObserved = observedAgent.IsMovingInLaneDirection() ? observed.hdg : observed.hdg + M_PI;
        // Take the actual position and yaw angle for small distances
        hdgEgo = egoDistance < 0.3 ? representation.egoAgent->GetYawAngle() : hdgEgo;
        hdgObserved = observedDistance < 0.3 ? observedAgent.GetYawAngle() : hdgObserved;

        auto shapeEgo = ConstructAgentPolygonRepresentation(*representation.egoAgent, {ego.x, ego.y}, hdgEgo);
        auto shapeObserved = ConstructAgentPolygonRepresentation(observedAgent, {observed.x, observed.y}, hdgObserved);

        if (boost::geometry::intersects(shapeEgo, shapeObserved)) {
            CollisionPoint possibleCollisionPoint;
            possibleCollisionPoint.distanceCP = egoDistance;
            possibleCollisionPoint.oAgentID = observedAgent.GetID();
            possibleCollisionPoint.timeToCollision = time;
            double decelTime = std::abs(representation.egoAgent->GetVelocity() / GetBehaviourData().adBehaviour.comfortDeceleration.mean);
            possibleCollisionPoint.collisionImminent = time <= GetBehaviourData().adBehaviour.collisionImminentMargin;
            return possibleCollisionPoint;
        }
    }
    return std::nullopt;
}

polygon_t CollisionInterpreter::ConstructAgentPolygonRepresentation(const AgentRepresentation &data, const Common::Vector2d pos,
                                                                    const double hdg) const {
    switch (data.GetVehicleType()) {
    case AgentVehicleType::Car:
        return ConstructPolygonRepresentation(data, pos, hdg);
    case AgentVehicleType::Truck:
        return ConstructPolygonRepresentation(data, pos, hdg);
    case AgentVehicleType::Pedestrian:
        return ConstructPolygonRepresentation(data, pos, hdg);
    default:
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "AgentType does not exist";
        Log(message, DReaMLogLevel::error);
        throw std::runtime_error(message);
    }
}

polygon_t CollisionInterpreter::ConstructPolygonRepresentation(const AgentRepresentation &agent, const Common::Vector2d pos,
                                                               const double hdg) const {
    // Initial bounding box in local coordinate system
    double safetyMargin = 0.05;
    double lengthHalf = (agent.GetLength() / 2.0) + safetyMargin;
    double widthHalf = (agent.GetWidth() / 2.0) + safetyMargin;

    double boxPoints[][2]{
        {-lengthHalf, -widthHalf}, {lengthHalf, -widthHalf}, {lengthHalf, widthHalf}, {-lengthHalf, widthHalf}, {-lengthHalf, -widthHalf}};
    polygon_t box;
    bg::append(box, boxPoints);

    //// translate offset of reference point
    polygon_t referencePointBox;
    double offsetReferencePoint = agent.GetDistanceReferencePointToLeadingEdge() - agent.GetLength() / 2;
    bt::translate_transformer<double, 2, 2> translateOffset(offsetReferencePoint, 0);
    bg::transform(box, referencePointBox, translateOffset);

    // rotation in mathematical negative orientation (boost) -> inverte to match
    polygon_t agentBoxRot;
    bt::rotate_transformer<bg::radian, double, 2, 2> rotate(-hdg);
    bg::transform(referencePointBox, agentBoxRot, rotate);

    // translate by locatedPoint
    polygon_t agentBox;
    bt::translate_transformer<double, 2, 2> translate(pos.x, pos.y);
    bg::transform(agentBoxRot, agentBox, translate);

    return agentBox;
}

} // namespace Interpreter
