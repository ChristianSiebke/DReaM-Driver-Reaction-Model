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

double TargetVelocityCalculation::CalculateTargetVelocity(const WorldRepresentation &worldRepresentation, CrossingInfo crossingInfo) const {
    double targetVelocity;
    auto mmLane = worldRepresentation.egoAgent->GetLanePosition().lane;

    auto velocitySpecificRoad =
        velocityStatisticsSpecificRoads.find(worldRepresentation.egoAgent->GetLanePosition().lane->GetRoad()->GetOpenDriveId());

    if (worldRepresentation.egoAgent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Car ||
        worldRepresentation.egoAgent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Motorbike ||
        worldRepresentation.egoAgent->GetVehicleType() == DReaMDefinitions::AgentVehicleType::Truck) {
        if ((crossingInfo.phase <= CrossingPhase::Deceleration_TWO && crossingInfo.phase > CrossingPhase::Approach) ||
            (crossingInfo.type == CrossingType::Straight &&
             worldRepresentation.egoAgent->GetIndicatorState() != IndicatorState::IndicatorState_Off)) {
            targetVelocity = phaseVelocities.at(IntersectionSpot::IntersectionEntry);
        }
        else if (crossingInfo.phase > CrossingPhase::Deceleration_TWO) {
            targetVelocity = phaseVelocities.at(IntersectionSpot::IntersectionExit);
        }
        else if (velocitySpecificRoad != velocityStatisticsSpecificRoads.end()) {
            targetVelocity = velocitySpecificRoad->second;
        }
        else {
            targetVelocity = defaultVelocity;
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
    std::cout << "Agent ID =" << worldRepresentation.egoAgent->GetID() << " | targetVelocity==" << targetVelocity << std::endl;
    return targetVelocity;
}

void TargetVelocityCalculation::CalculatePhaseVelocities(const WorldRepresentation &worldRepresentation, CrossingInfo crossingInfo) {
    if (worldRepresentation.egoAgent->GetIndicatorState() != IndicatorState::IndicatorState_Warn) {
        auto nextJunction = worldRepresentation.egoAgent->NextJunction();
        auto nextNextLane = worldRepresentation.infrastructure->NextLane(worldRepresentation.egoAgent->GetIndicatorState(),
                                                                         worldRepresentation.egoAgent->IsMovingInLaneDirection(),
                                                                         worldRepresentation.egoAgent->GetNextLane());
        auto nextNextJunction = nextNextLane != nullptr ? nextNextLane->GetRoad()->GetJunction() : nullptr;
        std::string nextNextJunctionValue = "not exist";
        nextNextJunctionValue;
        if (nextNextJunction) {
            nextNextJunctionValue = "exist";
        }

        if (nextJunction || nextNextJunction) {
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

            phaseVelocities.clear();
            for (const auto &entry : *velocityDistributions) {
                const auto &de = entry.second;
                double velocity = stochastics->GetNormalDistributed(de->mean, de->std_deviation);
                velocity = Common::ValueInBounds(de->min, velocity, de->max);
                phaseVelocities.insert(std::make_pair(entry.first, velocity));
            }
        }
    }
}

} // namespace Interpreter
