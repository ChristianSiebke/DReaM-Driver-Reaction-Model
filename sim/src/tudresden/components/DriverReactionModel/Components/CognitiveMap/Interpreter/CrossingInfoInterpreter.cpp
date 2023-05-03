/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "CrossingInfoInterpreter.h"

#include <iostream>

#include "Common/TimeMeasurement.hpp"

TimeMeasurement timeMeasure3("CrossingInfoInterpreter.cpp");

namespace Interpreter {

void CrossingInfoInterpreter::Update(WorldInterpretation* interpretation, const WorldRepresentation& representation) {
    try {
        timeMeasure3.StartTimePoint("CrossingInfoInterpreter ");
        (interpretation, representation);
        Localize(representation);
        UpdateStoppingPoints(representation);
        interpretation->crossingInfo = crossingInfo;
        interpretation->targetVelocity = targetVelocityCalculation.Update(representation, crossingInfo);
        timeMeasure3.EndTimePoint();
    }
    catch (std::logic_error e) {
        std::string message = e.what();
        Log(message, error);
        throw std::logic_error(message);
    }
    catch (...) {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " Update CrossingInfo failed";
        Log(message, error);
        throw std::logic_error(message);
    };
}

void CrossingInfoInterpreter::UpdateStoppingPoints(const WorldRepresentation& representation) {
    auto egoJunction = representation.egoAgent->NextJunction();
    auto egoLane = representation.egoAgent->GetLanePosition().lane;

    // setting stopping points
    if (egoJunction && crossingInfo.phase <= CrossingPhase::Deceleration_TWO) {
        crossingInfo.egoStoppingPoints =
            representation.infrastructure->GetStoppingPoints(egoJunction->GetOpenDriveId(), egoLane->GetOwlId());
        crossingInfo.junctionOdId = egoJunction->GetOpenDriveId();
    }

    SetDistanceSP(representation.egoAgent, crossingInfo.egoStoppingPoints);
}

void CrossingInfoInterpreter::Localize(const WorldRepresentation &representation) {
    DetermineCrossingType(representation);
    DetermineCrossingPhase(representation);
}

void CrossingInfoInterpreter::DetermineCrossingType(const WorldRepresentation &representation) {
    // assigning Crossing Type  until entrance intersecion
    auto nextJunction = representation.egoAgent->NextJunction();
    auto road = representation.egoAgent->GetLanePosition().lane->GetRoad();

    if (nextJunction && nextJunction->GetIncomingRoads().size() > 2) {
        if (representation.egoAgent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Car ||
            representation.egoAgent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Bicycle) {
            if (representation.egoAgent->GetIndicatorState() == IndicatorState::IndicatorState_Left) {
                crossingInfo.type = CrossingType::Left;
            }
            else if (representation.egoAgent->GetIndicatorState() ==
                     IndicatorState::IndicatorState_Right) { // TODO move indicatorstate to DReaMDefinitions
                crossingInfo.type = CrossingType::Right;
            }
            else {
                crossingInfo.type = CrossingType::Straight;
            }
        }
        else if (representation.egoAgent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Pedestrian ||
                 representation.egoAgent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Bicycle) {
            crossingInfo.type = CrossingType::Straight;
        }
    }
    else if (road->IsOnJunction()) {
        // crossingInfo unchanged
    }
    else {
        crossingInfo.type = CrossingType::NA;
    }
}

void CrossingInfoInterpreter::DetermineCrossingPhase(const WorldRepresentation &representation) {
    auto road = representation.egoAgent->GetLanePosition().lane->GetRoad();
    auto refToFront = representation.egoAgent->GetDistanceReferencePointToLeadingEdge();
    auto sPositionFront = representation.egoAgent->GetLanePosition().sCoordinate + refToFront;
    bool frontExceedCurrentLane = sPositionFront > road->GetLength();
    sPositionFront = frontExceedCurrentLane ? sPositionFront - road->GetLength() : sPositionFront;
    auto nextJunction = representation.egoAgent->NextJunction();
    if ((nextJunction && nextJunction->GetIncomingRoads().size() == 2) ||
        (road->IsOnJunction() && road->GetJunction()->GetIncomingRoads().size() == 2)) {
        // connection of two roads
        crossingInfo.phase = CrossingPhase::NONE;
    }
    else if ((nextJunction && representation.egoAgent->GetJunctionDistance().toNext - refToFront < ApproachDistance) &&
             !frontExceedCurrentLane) {
        if (representation.egoAgent->GetJunctionDistance().toNext - refToFront < DecelerationTWODistance) {
            crossingInfo.phase = CrossingPhase::Deceleration_TWO;
        }
        else if (representation.egoAgent->GetJunctionDistance().toNext - refToFront < DecelerationONEDistance) {
            crossingInfo.phase = CrossingPhase::Deceleration_ONE;
        }
        else {
            crossingInfo.phase = CrossingPhase::Approach;
        }
    }
    else if (road->IsOnJunction() || (nextJunction && frontExceedCurrentLane)) {
        StoppingPoint entry = crossingInfo.egoStoppingPoints.at(StoppingPointType::Pedestrian_Crossing_ONE);
        StoppingPoint vehicle_oncoming_left = crossingInfo.egoStoppingPoints.at(StoppingPointType::Vehicle_Left);
        if (vehicle_oncoming_left.type == StoppingPointType::NONE && crossingInfo.type == CrossingType::Left) {
            // for T-intersections
            vehicle_oncoming_left = crossingInfo.egoStoppingPoints.at(StoppingPointType::Vehicle_Crossroad);
        }
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
            throw std::logic_error(message);
        }

        if (representation.egoAgent->GetJunctionDistance().on + refToFront > exit.sOffset) {
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
            throw std::logic_error(message);
        }
    }
    else {
        crossingInfo.phase = CrossingPhase::NONE;
    }
}

void CrossingInfoInterpreter::SetDistanceSP(const AgentRepresentation *agent, StoppingPointMap &stoppingpoints) {
    if (stoppingpoints.empty()) {
        return;
    }
    auto refToFront = agent->GetDistanceReferencePointToLeadingEdge();
    auto sPositionFront = agent->GetLanePosition().sCoordinate + refToFront;
    bool frontExceedCurrentLane = sPositionFront > agent->GetLanePosition().lane->GetRoad()->GetLength();
    sPositionFront = frontExceedCurrentLane ? sPositionFront - agent->GetLanePosition().lane->GetRoad()->GetLength() : sPositionFront;
    auto nextJunction = agent->NextJunction();
    for (auto &sp : stoppingpoints) {
        if (sp.second.type == StoppingPointType::NONE) {
            continue;
        }

        if (nextJunction && !frontExceedCurrentLane) {
            sp.second.distanceToEgoFront = sp.second.sOffset + (agent->GetLanePosition().lane->GetRoad()->GetLength() - sPositionFront);
        }
        else if (agent->GetLanePosition().lane->GetRoad()->IsOnJunction() || (nextJunction && frontExceedCurrentLane)) {
            sp.second.distanceToEgoFront = sp.second.sOffset - sPositionFront;
        }
    }
}

} // namespace Interpreter
