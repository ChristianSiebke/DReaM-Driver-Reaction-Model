#include "VelocityCalculation.h"
namespace ActionDecision {

void VelocityCalculation::Update() {
    phaseVelocity.clear();
    CalculatePhaseVelocities();
}

double VelocityCalculation::PrepareTargetVelocity() {
    double targetVelocity;

    auto mmLane = worldRepresentation.egoAgent->GetLane();

    if (worldRepresentation.egoAgent->GetVehicleType() == AgentVehicleType::Car) {
        if (worldInterpretation.crossingInfo.phase != CrossingPhase::NONE) {
            targetVelocity = phaseVelocity.at(worldInterpretation.crossingInfo.phase);
        } else {
            SpeedLimit speedLimit = {50, SpeedUnit::KilometersPerHour};
            speedLimit.convert(SpeedUnit::MetersPerSecond);
            targetVelocity = speedLimit.maxAllowedSpeed;
            // correction for misinterpretation
            if (targetVelocity <= 0.0) {
                const auto& roadId = mmLane->GetRoad()->GetOpenDriveId();
                std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) +
                                      " | SpeedLimit of Lane: " + std::to_string(mmLane->GetOpenDriveId()) +
                                      " on Road: " + std::to_string(roadId) + " <=0 !";
                throw std::logic_error(message);
            }
        }
    } else if (worldRepresentation.egoAgent->GetVehicleType() == AgentVehicleType::Pedestrian) {
        targetVelocity = 3.6;
    } else if (worldRepresentation.egoAgent->GetVehicleType() == AgentVehicleType::Bicycle) {
        targetVelocity = 9.0;
    } else {
        targetVelocity = 0.0; // TODO change this for trucks at some point
    }

    return targetVelocity;
}

void VelocityCalculation::CalculatePhaseVelocities() {
    if (worldRepresentation.egoAgent->GetIndicatorState() != IndicatorState::IndicatorState_Warn) {
        const auto& velocityDistributions =
            GetBehaviourData().adBehaviour.velocityStatistics.at(worldRepresentation.egoAgent->GetIndicatorState());

        for (const auto& entry : velocityDistributions) {
            const auto& de = entry.second;
            double velocity = GetStochastic()->GetNormalDistributed(de->mean, de->std_deviation);
            velocity = Common::ValueInBounds(de->min, velocity, de->max);
            phaseVelocity.insert(std::make_pair(entry.first, velocity));
        }
    }
}
} // namespace ActionDecision
