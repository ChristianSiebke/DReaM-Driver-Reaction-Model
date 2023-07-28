/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#pragma once
#include "Common/BehaviourData.h"
#include "Common/Helper.h"
#include "Common/WorldRepresentation.h"
#include "LoggerInterface.h"
#include "include/stochasticsInterface.h"

namespace Interpreter {

class TargetVelocityCalculation {
public:
    TargetVelocityCalculation(LoggerInterface *logger, const BehaviourData &behaviourData, StochasticsInterface *stochastics) :
        stochastics{stochastics}, loggerInterface{logger}, behaviourData{behaviourData} {
        for (const auto &entry : behaviourData.adBehaviour.velocityStatisticsSpecificRoads) {
            const auto &de = entry.second;
            double velocity = stochastics->GetNormalDistributed(de->mean, de->std_deviation);
            velocity = Common::ValueInBounds(de->min, velocity, de->max);
            velocityStatisticsSpecificRoads.insert(std::make_pair(entry.first, velocity));
        }
        const auto &de = behaviourData.adBehaviour.defaultVelocity;
        double velocity = stochastics->GetNormalDistributed(de.mean, de.std_deviation);
        defaultVelocity = Common::ValueInBounds(de.min, velocity, de.max);
    }
    TargetVelocityCalculation(const TargetVelocityCalculation &) = delete;
    TargetVelocityCalculation(TargetVelocityCalculation &&) = delete;
    TargetVelocityCalculation &operator=(const TargetVelocityCalculation &) = delete;
    TargetVelocityCalculation &operator=(TargetVelocityCalculation &&) = delete;
    virtual ~TargetVelocityCalculation() = default;

    double Update(const WorldRepresentation &worldRepresentation, CrossingInfo crossingInfo);
    double GetVelDistOffset() {
        return velocityDistributionOffset;
    }

private:
    void CalculatePhaseVelocities(const WorldRepresentation &worldRepresentation, CrossingInfo crossingInfo);
    double CalculateTargetVelocity(const WorldRepresentation &worldRepresentation, CrossingInfo crossingInfo);
    void Log(const std::string &message, DReaMLogLevel level = info) const {
        loggerInterface->Log(message, level);
    }
    void CalculateVelDistOffset(double targetVelocity, DistributionEntry activeTargetDistribution);

    std::map<IntersectionSpot, double> phaseVelocities{};
    std::map<std::string, double> velocityStatisticsSpecificRoads{};
    double defaultVelocity;
    const MentalInfrastructure::Road *lastRoad = nullptr;
    IndicatorState lastIndicatorState = IndicatorState::IndicatorState_Off;
    StochasticsInterface *stochastics;
    const LoggerInterface *loggerInterface;
    const BehaviourData &behaviourData;
    const std::map<IntersectionSpot, std::shared_ptr<DistributionEntry>> *activeVelocityDistributions = nullptr;
    double velocityDistributionOffset = std::numeric_limits<double>::max();
};
} // namespace Interpreter
