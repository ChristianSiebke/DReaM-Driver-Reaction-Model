/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/

#include "RightOfWayRegulationInterface.h"

#include "common/Helper.h"

namespace RightOfWayRegulation {

RightOfWay RightOfWayRegulationInterface::RightOfWayDetermination(const AgentRepresentation& observedAgent,
                                                                  const WorldRepresentation& representation) {
    RightOfWay rightOfWay(true, true);

    if (IsVehicle(*representation.egoAgent)) {
        if (IsVehicle(observedAgent)) {
            return VehicleVsVehicleROW(observedAgent, representation);
        } else if (IsPedestrian(observedAgent)) {
            bool observedAgentROW = PedestrianVsVehicleROW(observedAgent, *representation.egoAgent);
            rightOfWay.observed = observedAgentROW;
            rightOfWay.ego = !observedAgentROW;
            return rightOfWay;
        }
    } else if (IsPedestrian(*representation.egoAgent)) {
        if (IsVehicle(observedAgent)) {
            bool egoAgentROW = PedestrianVsVehicleROW(*representation.egoAgent, observedAgent);
            rightOfWay.observed = !egoAgentROW;
            rightOfWay.ego = egoAgentROW;
            return rightOfWay;
        } else if (IsPedestrian(observedAgent)) {
            return rightOfWay;
        }
    }
    auto msg = __FILE__ " Line: " + std::to_string(__LINE__) + " Cannot determine right of way ";
    throw std::logic_error(msg);
}

template <typename T> bool RightOfWayRegulationInterface::IsVehicle(const T& agentdata) {
    return (agentdata.GetVehicleType() == DReaMDefinitions::AgentVehicleType::Car ||
            agentdata.GetVehicleType() == DReaMDefinitions::AgentVehicleType::Truck ||
            agentdata.GetVehicleType() == DReaMDefinitions::AgentVehicleType::Bicycle);
}

template <typename T> bool RightOfWayRegulationInterface::IsPedestrian(const T& agentdata) {
    return (agentdata.GetVehicleType() == DReaMDefinitions::AgentVehicleType::Pedestrian);
}

bool RightOfWayRegulationInterface::PedestrianVsVehicleROW(const AgentRepresentation& pedestrianAgent,
                                                           const AgentRepresentation& vehicleAgent) {
    // !Condition for algorithm: trajectories of agents overlap!
    if (vehicleAgent.GetIndicatorState() == IndicatorState::IndicatorState_Warn) {
        auto msg = __FILE__ " Line: " + std::to_string(__LINE__) + " Cannot determine right of way --> IndicatorState_Warn ";
        throw std::out_of_range(msg);
    }

    if (vehicleAgent.GetIndicatorState() == IndicatorState::IndicatorState_Off) {
        return false;
    }
    const auto vehicleLane = vehicleAgent.GetLanePosition().lane;
    const auto vehicleRoad = vehicleLane->GetRoad();
    const auto pedestrianLane = pedestrianAgent.GetLanePosition().lane;
    const auto pedestrianRoad = pedestrianLane->GetRoad();
    double vehicleLaneHDG;
    double pedestrianLaneHDG;

    // vehicles are not allowed to drive against lane direction near to intersection
    if (vehicleRoad->IsOnJunction()) {
        vehicleLaneHDG = vehicleLane->GetFirstPoint()->hdg;
    }
    else {
        vehicleLaneHDG = vehicleLane->GetLastPoint()->hdg;
    }

    if (pedestrianRoad->IsOnJunction()) {
        pedestrianLaneHDG = pedestrianLane->GetFirstPoint()->hdg;
    }
    else {
        if (pedestrianAgent.IsMovingInLaneDirection()) {
            pedestrianLaneHDG = pedestrianLane->GetLastPoint()->hdg;
        } else {
            pedestrianLaneHDG = -pedestrianLane->GetFirstPoint()->hdg;
        }
    }

    double diffAngle = std::fabs(std::fmod(pedestrianLaneHDG - vehicleLaneHDG, (M_PI))) * 180 / M_PI;

    if (parallelEpsilonDeg >= diffAngle || parallelEpsilonDeg >= std::fabs(180 - diffAngle)) {
        return true;
    }
    return false;
}
} // namespace RightOfWayRegulation
