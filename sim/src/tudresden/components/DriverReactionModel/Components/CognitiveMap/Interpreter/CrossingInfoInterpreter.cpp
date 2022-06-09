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
}

void CrossingInfoInterpreter::UpdateStoppingPoints(const WorldRepresentation& representation) {
    auto nextJunction = representation.egoAgent->NextJunction();
    auto road = representation.egoAgent->GetRoad();
    auto lane = representation.egoAgent->GetLane();

    // setting stopping points
    if (crossingInfo.phase != CrossingPhase::NONE && crossingInfo.egoStoppingPoints.empty()) {
        crossingInfo.egoStoppingPoints = representation.infrastructure->GetStoppingPoints(nextJunction->GetOpenDriveId(), lane->GetOwlId());
        crossingInfo.junctionOdId = nextJunction->GetOpenDriveId();
    } else if (crossingInfo.phase == CrossingPhase::NONE && !crossingInfo.egoStoppingPoints.empty()) {
        crossingInfo.egoStoppingPoints.clear();
        crossingInfo.otherStoppingpoints.clear();
    }

    SetDistanceSP(representation.egoAgent, road, nextJunction, crossingInfo.egoStoppingPoints);

    for (auto& agent : *representation.agentMemory) {
        auto a_nextJunction = agent->NextJunction();
        auto a_road = agent->GetRoad();
        auto a_lane = agent->GetLane();

        if (a_nextJunction && crossingInfo.phase != CrossingPhase::NONE) {
            if (crossingInfo.otherStoppingpoints.find(agent->GetID()) == crossingInfo.otherStoppingpoints.end()) {
                auto &stoppingpoints =
                    representation.infrastructure->GetStoppingPoints(a_nextJunction->GetOpenDriveId(), a_lane->GetOwlId());
                crossingInfo.otherStoppingpoints.insert(std::make_pair(agent->GetID(), stoppingpoints));
            }
            SetDistanceSP(agent.get(), a_road, a_nextJunction, crossingInfo.otherStoppingpoints.at(agent->GetID()));
        }
    }
}

void CrossingInfoInterpreter::Localize(const WorldRepresentation& representation) {
    DetermineCrossingType(representation);
    DetermineCrossingPhase(representation);
    int i = 0;
}

void CrossingInfoInterpreter::DetermineCrossingType(const WorldRepresentation& representation) {
    // assigning Crossing Type  until entrance intersecion
    if (representation.egoAgent->NextJunction() != nullptr) {
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
    auto nextJunction = representation.egoAgent->NextJunction();

    if (nextJunction != nullptr && representation.egoAgent->GetDistanceToNextJunction() < 75) {
        if (representation.egoAgent->GetDistanceToNextJunction() < 25) {
            crossingInfo.phase = CrossingPhase::Deceleration_TWO;
        }
        else if (representation.egoAgent->GetDistanceToNextJunction() < 50) {
            crossingInfo.phase = CrossingPhase::Deceleration_ONE;
        }
        else {
            crossingInfo.phase = CrossingPhase::Approach;
        }
    }
    else if (road->IsOnJunction()) {
        StoppingPoint entry = crossingInfo.egoStoppingPoints.at(StoppingPointType::Pedestrian_Crossing_ONE);
        StoppingPoint vehicle_oncoming_left = crossingInfo.egoStoppingPoints.at(StoppingPointType::Vehicle_Left);
        StoppingPoint exit;

        if (crossingInfo.type == CrossingType::Left) {
            exit = crossingInfo.egoStoppingPoints.at(StoppingPointType::Pedestrian_Left);
        } else if (crossingInfo.type == CrossingType::Right) {
            exit = crossingInfo.egoStoppingPoints.at(StoppingPointType::Pedestrian_Right);
        } else if (crossingInfo.type == CrossingType::Straight) {
            exit = crossingInfo.egoStoppingPoints.at(StoppingPointType::Pedestrian_Crossing_TWO);
        } else {
            std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "can not determine StoppingPointType!";
            throw std::runtime_error(message);
        }

        if (representation.egoAgent->GetDistanceOnJunction() < entry.sOffset) {
            crossingInfo.phase = CrossingPhase::Deceleration_TWO;
        }
        else if (crossingInfo.type == CrossingType::Left &&
                 representation.egoAgent->GetDistanceOnJunction() < vehicle_oncoming_left.sOffset) {
            crossingInfo.phase = CrossingPhase::Crossing_Left_ONE;
        }
        else if (representation.egoAgent->GetDistanceOnJunction() > exit.sOffset) {
            crossingInfo.phase = CrossingPhase::Exit;
        }
        else if (crossingInfo.type == CrossingType::Left && representation.egoAgent->GetDistanceOnJunction() < exit.sOffset) {
            crossingInfo.phase = CrossingPhase::Crossing_Left_TWO;
        }
        else if (crossingInfo.type == CrossingType::Right && representation.egoAgent->GetDistanceOnJunction() < exit.sOffset) {
            crossingInfo.phase = CrossingPhase::Crossing_Right;
        }
        else if (crossingInfo.type == CrossingType::Straight && representation.egoAgent->GetDistanceOnJunction() < exit.sOffset) {
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

void CrossingInfoInterpreter::SetDistanceSP(const AgentRepresentation *representation, const MentalInfrastructure::Road *road,
                                            const MentalInfrastructure::Junction *nextJunction, StoppingPointMap stoppingpoints) {
    if (stoppingpoints.empty()) {
        return;
    }

    for (auto& sp : stoppingpoints) {
        if (sp.second.type == StoppingPointType::NONE) {
            continue;
        }

        if (nextJunction) {
            sp.second.distanceToEgo =
                sp.second.sOffset + representation->GetDistanceToNextJunction() - representation->GetDistanceReferencePointToLeadingEdge();
        }
        else if (road->IsOnJunction()) {
            sp.second.distanceToEgo =
                sp.second.sOffset - representation->GetSCoordinate() - representation->GetDistanceReferencePointToLeadingEdge();
        }
    }
}

} // namespace Interpreter
