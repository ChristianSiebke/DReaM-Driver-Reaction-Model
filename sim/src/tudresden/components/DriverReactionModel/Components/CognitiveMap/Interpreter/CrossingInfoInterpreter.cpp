/******************************************************************************
 * Copyright (c) 2020 TU Dresden
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
#include "CrossingInfoInterpreter.h"
#include <iostream>
namespace Interpreter {

void CrossingInfoInterpreter::Update(WorldInterpretation* interpretation, const WorldRepresentation& representation) {
    Localize(representation);
    UpdateStoppingPoints(representation);
    interpretation->crossingInfo = crossingInfo;
    interpretation->targetVelocity = targetVelocityCalculation.Update(representation, crossingInfo.phase);
}

void CrossingInfoInterpreter::UpdateStoppingPoints(const WorldRepresentation& representation) {
    auto egoJunction = representation.egoAgent->NextJunction();
    auto egoRoad = representation.egoAgent->GetRoad();
    auto egoLane = representation.egoAgent->GetLane();

    // setting stopping points
    if (crossingInfo.phase != CrossingPhase::NONE && crossingInfo.egoStoppingPoints.empty()) {
        crossingInfo.egoStoppingPoints =
            representation.infrastructure->GetStoppingPoints(egoJunction->GetOpenDriveId(), egoLane->GetOwlId());
        crossingInfo.junctionOdId = egoJunction->GetOpenDriveId();
    }
    else if (crossingInfo.phase == CrossingPhase::NONE && !crossingInfo.egoStoppingPoints.empty()) {
        crossingInfo.egoStoppingPoints.clear();
        crossingInfo.otherStoppingpoints.clear();
    }

    SetDistanceSP(representation.egoAgent, crossingInfo.egoStoppingPoints);

    for (auto &agent : *representation.agentMemory) {
        auto junction = agent->NextJunction();
        auto lane = agent->GetLane();

        if (junction && crossingInfo.phase != CrossingPhase::NONE) {
            if (crossingInfo.otherStoppingpoints.find(agent->GetID()) == crossingInfo.otherStoppingpoints.end()) {
                auto &stoppingpoints = representation.infrastructure->GetStoppingPoints(junction->GetOpenDriveId(), lane->GetOwlId());
                crossingInfo.otherStoppingpoints.insert(std::make_pair(agent->GetID(), stoppingpoints));
            }
        }
        if (std::any_of(crossingInfo.otherStoppingpoints.begin(), crossingInfo.otherStoppingpoints.end(),
                        [&agent](auto element) { return element.first == agent->GetID(); })) {
            SetDistanceSP(agent.get(), crossingInfo.otherStoppingpoints.at(agent->GetID()));
        }
    }
}

void CrossingInfoInterpreter::Localize(const WorldRepresentation &representation) {
    DetermineCrossingType(representation);
    DetermineCrossingPhase(representation);
    int i = 0;
}

void CrossingInfoInterpreter::DetermineCrossingType(const WorldRepresentation &representation) {
    // assigning Crossing Type  until entrance intersecion
    if (representation.egoAgent->NextJunction()) {
        if (representation.egoAgent->GetVehicleType() == AgentVehicleType::Car) {
            if (representation.egoAgent->GetIndicatorState() == IndicatorState::IndicatorState_Left) {
                crossingInfo.type = CrossingType::Left;
            }
            else if (representation.egoAgent->GetIndicatorState() == IndicatorState::IndicatorState_Right) {
                crossingInfo.type = CrossingType::Right;
            }
            else {
                crossingInfo.type = CrossingType::Straight;
            }
        }
        else if (representation.egoAgent->GetVehicleType() == AgentVehicleType::Pedestrian ||
                 representation.egoAgent->GetVehicleType() == AgentVehicleType::Bicycle) {
            crossingInfo.type = CrossingType::Straight;
        }
    }
}

void CrossingInfoInterpreter::DetermineCrossingPhase(const WorldRepresentation &representation) {
    auto road = representation.egoAgent->GetRoad();
    auto refToFront = representation.egoAgent->GetDistanceReferencePointToLeadingEdge();
    auto sPositionFront = representation.egoAgent->GetSCoordinate() + refToFront;
    bool frontExceedCurrentLane = sPositionFront > representation.egoAgent->GetRoad()->GetLength();
    sPositionFront = frontExceedCurrentLane ? sPositionFront - representation.egoAgent->GetRoad()->GetLength() : sPositionFront;
    auto nextJunction = representation.egoAgent->NextJunction();

    if ((nextJunction && representation.egoAgent->GetDistanceToNextJunction() - refToFront < 75) && !frontExceedCurrentLane) {
        if (representation.egoAgent->GetDistanceToNextJunction() - refToFront < 25) {
            crossingInfo.phase = CrossingPhase::Deceleration_TWO;
        }
        else if (representation.egoAgent->GetDistanceToNextJunction() - refToFront < 50) {
            crossingInfo.phase = CrossingPhase::Deceleration_ONE;
        }
        else {
            crossingInfo.phase = CrossingPhase::Approach;
        }
    }
    else if (road->IsOnJunction() || (nextJunction && frontExceedCurrentLane)) {
        StoppingPoint entry = crossingInfo.egoStoppingPoints.at(StoppingPointType::Pedestrian_Crossing_ONE);
        StoppingPoint vehicle_oncoming_left = crossingInfo.egoStoppingPoints.at(StoppingPointType::Vehicle_Left);
        StoppingPoint exit;

        if (crossingInfo.type == CrossingType::Left) {
            exit = crossingInfo.egoStoppingPoints.at(StoppingPointType::Pedestrian_Left);
        }
        else if (crossingInfo.type == CrossingType::Right) {
            exit = crossingInfo.egoStoppingPoints.at(StoppingPointType::Pedestrian_Right);
        }
        else if (crossingInfo.type == CrossingType::Straight) {
            exit = crossingInfo.egoStoppingPoints.at(StoppingPointType::Pedestrian_Crossing_TWO);
        }
        else {
            std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "can not determine StoppingPointType!";
            throw std::runtime_error(message);
        }

        if (representation.egoAgent->GetDistanceOnJunction() + refToFront > exit.sOffset) {
            crossingInfo.phase = CrossingPhase::Exit;
        }
        else if (sPositionFront < entry.sOffset) {
            crossingInfo.phase = CrossingPhase::Deceleration_TWO;
        }
        else if (crossingInfo.type == CrossingType::Left && sPositionFront < vehicle_oncoming_left.sOffset) {
            crossingInfo.phase = CrossingPhase::Crossing_Left_ONE;
        }
        else if (crossingInfo.type == CrossingType::Left && sPositionFront < exit.sOffset) {
            crossingInfo.phase = CrossingPhase::Crossing_Left_TWO;
        }
        else if (crossingInfo.type == CrossingType::Right && sPositionFront < exit.sOffset) {
            crossingInfo.phase = CrossingPhase::Crossing_Right;
        }
        else if (crossingInfo.type == CrossingType::Straight && sPositionFront < exit.sOffset) {
            crossingInfo.phase = CrossingPhase::Crossing_Straight;
        }
        else {
            std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "can not determine CrossingPhase!";
            throw std::runtime_error(message);
        }
    }
    else {
        crossingInfo.phase = CrossingPhase::NONE;
        crossingInfo.type = CrossingType::NA;
    }
}

void CrossingInfoInterpreter::SetDistanceSP(const AgentRepresentation *agent, StoppingPointMap &stoppingpoints) {
    if (stoppingpoints.empty()) {
        return;
    }
    auto refToFront = agent->GetDistanceReferencePointToLeadingEdge();
    auto sPositionFront = agent->GetSCoordinate() + refToFront;
    bool frontExceedCurrentLane = sPositionFront > agent->GetRoad()->GetLength();
    sPositionFront = frontExceedCurrentLane ? sPositionFront - agent->GetRoad()->GetLength() : sPositionFront;
    auto nextJunction = agent->NextJunction();
    for (auto &sp : stoppingpoints) {
        if (sp.second.type == StoppingPointType::NONE) {
            continue;
        }

        if (nextJunction && !frontExceedCurrentLane) {
            sp.second.distanceToEgoFront = sp.second.sOffset + (agent->GetRoad()->GetLength() - sPositionFront);
        }
        else if (agent->GetRoad()->IsOnJunction() || (nextJunction && frontExceedCurrentLane)) {
            sp.second.distanceToEgoFront = sp.second.sOffset - sPositionFront;
        }
    }
}

} // namespace Interpreter
