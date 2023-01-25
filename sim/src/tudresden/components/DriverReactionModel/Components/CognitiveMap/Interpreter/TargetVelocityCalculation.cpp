/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "TargetVelocityCalculation.h"

#include "Helper.h"

namespace Interpreter {

double TargetVelocityCalculation::Update(const WorldRepresentation &worldRepresentation, CrossingPhase phase) {
    try {
        if (currentRoad != worldRepresentation.egoAgent->GetLanePosition().lane->GetRoad()) {
            currentRoad = worldRepresentation.egoAgent->GetLanePosition().lane->GetRoad();
            phaseVelocities.clear();
            CalculatePhaseVelocities(worldRepresentation);
        }
        return CalculateTargetVelocity(worldRepresentation, phase);
    }
    catch (...) {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " error in update target velocity";
        Log(message, error);
    }
};

double TargetVelocityCalculation::CalculateTargetVelocity(const WorldRepresentation &worldRepresentation, CrossingPhase phase) const {
    double targetVelocity;

    auto mmLane = worldRepresentation.egoAgent->GetLanePosition().lane;

    if (worldRepresentation.egoAgent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Car ||
        worldRepresentation.egoAgent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Motorbike ||
        worldRepresentation.egoAgent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Truck) {
        if (phase != CrossingPhase::NONE) {
            targetVelocity = phaseVelocities.at(phase);
        }
        else {
            // TODO: set speed limit via traffic sign or road/lane speed
            SpeedLimit speedLimit = {50, SpeedUnit::KilometersPerHour};
            speedLimit.convert(SpeedUnit::MetersPerSecond);
            targetVelocity = speedLimit.maxAllowedSpeed;
            // correction for misinterpretation
            if (targetVelocity <= 0.0) {
                const auto &roadId = mmLane->GetRoad()->GetOpenDriveId();
                std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                                      " | SpeedLimit of Lane: " + mmLane->GetOpenDriveId() + " on Road: " + roadId + " <=0 !";
                throw std::logic_error(message);
            }
        }
    }
    else if (worldRepresentation.egoAgent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Pedestrian) {
        targetVelocity = 3.6;
    }
    else if (worldRepresentation.egoAgent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Bicycle) {
        targetVelocity = 9.0;
    }
    else {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + "  unknown vehicle type ";
        throw std::logic_error(message);
    }

    return targetVelocity;
}

void TargetVelocityCalculation::CalculatePhaseVelocities(const WorldRepresentation &worldRepresentation) {
    if (worldRepresentation.egoAgent->GetIndicatorState() != IndicatorState::IndicatorState_Warn) {
        const auto &velocityDistributions =
            behaviourData.adBehaviour.velocityStatistics.at(worldRepresentation.egoAgent->GetIndicatorState());

        for (const auto &entry : velocityDistributions) {
            const auto &de = entry.second;
            double velocity = stochastics->GetNormalDistributed(de->mean, de->std_deviation);
            velocity = Common::ValueInBounds(de->min, velocity, de->max);
            phaseVelocities.insert(std::make_pair(entry.first, velocity));
        }
    }
}
} // namespace Interpreter
