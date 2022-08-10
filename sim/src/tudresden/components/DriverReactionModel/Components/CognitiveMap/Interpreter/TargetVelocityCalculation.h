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
#pragma once
#include "Common/BehaviourData.h"
#include "Common/WorldRepresentation.h"
#include "LoggerInterface.h"
#include "include/stochasticsInterface.h"

namespace Interpreter {

class TargetVelocityCalculation {
public:
    TargetVelocityCalculation(LoggerInterface *logger, const BehaviourData &behaviourData, StochasticsInterface *stochastics) :
        stochastics{stochastics}, loggerInterface{loggerInterface}, behaviourData{behaviourData} {
    }
    TargetVelocityCalculation(const TargetVelocityCalculation &) = delete;
    TargetVelocityCalculation(TargetVelocityCalculation &&) = delete;
    TargetVelocityCalculation &operator=(const TargetVelocityCalculation &) = delete;
    TargetVelocityCalculation &operator=(TargetVelocityCalculation &&) = delete;
    virtual ~TargetVelocityCalculation() = default;

    double Update(const WorldRepresentation &worldRepresentation, CrossingPhase phase);

private:
    void CalculatePhaseVelocities(const WorldRepresentation &worldRepresentation);
    double CalculateTargetVelocity(const WorldRepresentation &worldRepresentation, CrossingPhase phase) const;
    void Log(const std::string &message, DReaMLogLevel level = info) const {
        loggerInterface->Log(message, level);
    }

    std::map<CrossingPhase, double> phaseVelocities;
    const MentalInfrastructure::Road *currentRoad = nullptr;
    StochasticsInterface *stochastics;
    const LoggerInterface *loggerInterface;
    const BehaviourData &behaviourData;
};
} // namespace Interpreter
