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

double TargetVelocityCalculation::Update(const WorldRepresentation &worldRepresentation, CrossingInfo crossingInfo) {
    try {
        if (lastRoad != worldRepresentation.egoAgent->GetLanePosition().lane->GetRoad() ||
            lastIndicatorState != worldRepresentation.egoAgent->GetIndicatorState()) {
            lastRoad = worldRepresentation.egoAgent->GetLanePosition().lane->GetRoad();
            lastIndicatorState = worldRepresentation.egoAgent->GetIndicatorState();
            CalculatePhaseVelocities(worldRepresentation, crossingInfo);
        }

        return CalculateTargetVelocity(worldRepresentation, crossingInfo);
    }
    catch (...) {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " error in update target velocity";
        Log(message, error);
        throw std::logic_error(message);
    }
};

double TargetVelocityCalculation::CalculateTargetVelocity(const WorldRepresentation &worldRepresentation, CrossingInfo crossingInfo) {
    double targetVelocity;
    DistributionEntry activeTargetDistribution(0, 0, 0, 0);
    auto mmLane = worldRepresentation.egoAgent->GetLanePosition().lane;

    auto velocitySpecificRoad =
        velocityStatisticsSpecificRoads.find(worldRepresentation.egoAgent->GetLanePosition().lane->GetRoad()->GetOpenDriveId());

    if (worldRepresentation.egoAgent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Car ||
        worldRepresentation.egoAgent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Motorbike ||
        worldRepresentation.egoAgent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Truck ||
        worldRepresentation.egoAgent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Pedestrian ||
        worldRepresentation.egoAgent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Bicycle) {
        if ((crossingInfo.phase <= CrossingPhase::Deceleration_TWO && crossingInfo.phase > CrossingPhase::Approach) ||
            (crossingInfo.type == CrossingType::Straight &&
             worldRepresentation.egoAgent->GetIndicatorState() != IndicatorState::IndicatorState_Off)) {
            targetVelocity = phaseVelocities.at(IntersectionSpot::IntersectionEntry);
            activeTargetDistribution = *activeVelocityDistributions->at(IntersectionSpot::IntersectionEntry).get();
        }
        else if (crossingInfo.phase > CrossingPhase::Deceleration_TWO) {
            targetVelocity = phaseVelocities.at(IntersectionSpot::IntersectionExit);
            activeTargetDistribution = *activeVelocityDistributions->at(IntersectionSpot::IntersectionExit).get();
        }
        else if (velocitySpecificRoad != velocityStatisticsSpecificRoads.end()) {
            targetVelocity = velocitySpecificRoad->second;
            activeTargetDistribution = *behaviourData.adBehaviour.velocityStatisticsSpecificRoads
                                            .at(worldRepresentation.egoAgent->GetLanePosition().lane->GetRoad()->GetOpenDriveId())
                                            .get();
        }
        else {
            targetVelocity = defaultVelocity;
            activeTargetDistribution = behaviourData.adBehaviour.defaultVelocity;
        }
    }
    else {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + "  unknown vehicle type ";
        throw std::logic_error(message);
    }
    CalculateVelDistOffset(targetVelocity, activeTargetDistribution);

    return targetVelocity;
}

void TargetVelocityCalculation::CalculateVelDistOffset(double targetVelocity, DistributionEntry activeTargetDistribution) {
    if (activeTargetDistribution.std_deviation == 0.0) {
        velocityDistributionOffset = 0;
        return;
    }
    velocityDistributionOffset = (activeTargetDistribution.mean - targetVelocity) / activeTargetDistribution.std_deviation;
}

void TargetVelocityCalculation::CalculatePhaseVelocities(const WorldRepresentation &worldRepresentation, CrossingInfo crossingInfo) {
    if (worldRepresentation.egoAgent->GetIndicatorState() != IndicatorState::IndicatorState_Warn) {
        auto nextJunction = worldRepresentation.egoAgent->NextJunction();
        auto nextNextLane =
            worldRepresentation.egoAgent->GetNextLane()
                ? worldRepresentation.egoAgent->GetNextLane()->NextLane(worldRepresentation.egoAgent->GetIndicatorState(),
                                                                        worldRepresentation.egoAgent->IsMovingInLaneDirection())
                : nullptr;
        auto nextNextJunction = nextNextLane != nullptr ? nextNextLane->GetRoad()->GetJunction() : nullptr;
        auto currentJunction = worldRepresentation.egoAgent->GetLanePosition().lane->GetRoad()->GetJunction();

        if (nextJunction || nextNextJunction || currentJunction) {
            auto velocityDistributions = &behaviourData.adBehaviour.velocityStatisticsIntersection.at("default").at("default").at(
                worldRepresentation.egoAgent->GetIndicatorState());
            if (nextJunction) {
                auto iter = behaviourData.adBehaviour.velocityStatisticsIntersection.find(nextJunction->GetOpenDriveId());
                if (iter != behaviourData.adBehaviour.velocityStatisticsIntersection.end()) {
                    auto roadID = worldRepresentation.egoAgent->GetLanePosition().lane->GetRoad()->GetOpenDriveId();
                    velocityDistributions = &behaviourData.adBehaviour.velocityStatisticsIntersection.at(nextJunction->GetOpenDriveId())
                                                 .at(roadID)
                                                 .at(worldRepresentation.egoAgent->GetIndicatorState());
                }
            }
            else if (nextNextJunction && (crossingInfo.type == CrossingType::Straight &&
                                          worldRepresentation.egoAgent->GetIndicatorState() != IndicatorState::IndicatorState_Off)) {
                auto roadID = worldRepresentation.egoAgent->GetNextLane()->GetRoad()->GetOpenDriveId();
                velocityDistributions = &behaviourData.adBehaviour.velocityStatisticsIntersection.at(nextNextJunction->GetOpenDriveId())
                                             .at(roadID)
                                             .at(worldRepresentation.egoAgent->GetIndicatorState());
            }
            else if (currentJunction) {
                auto iter = behaviourData.adBehaviour.velocityStatisticsIntersection.find(currentJunction->GetOpenDriveId());
                if (iter != behaviourData.adBehaviour.velocityStatisticsIntersection.end()) {
                    auto roadID =
                        worldRepresentation.egoAgent->GetLanePosition().lane->GetPredecessors().front()->GetRoad()->GetOpenDriveId();
                    velocityDistributions = &behaviourData.adBehaviour.velocityStatisticsIntersection.at(currentJunction->GetOpenDriveId())
                                                 .at(roadID)
                                                 .at(worldRepresentation.egoAgent->GetIndicatorState());
                }
            }

            phaseVelocities.clear();
            for (const auto &entry : *velocityDistributions) {
                const auto &de = entry.second;
                double velocity = stochastics->GetNormalDistributed(de->mean, de->std_deviation);
                velocity = Common::ValueInBounds(de->min, velocity, de->max);
                phaseVelocities.insert(std::make_pair(entry.first, velocity));
            }
            activeVelocityDistributions = velocityDistributions;
        }
    }
}

} // namespace Interpreter
