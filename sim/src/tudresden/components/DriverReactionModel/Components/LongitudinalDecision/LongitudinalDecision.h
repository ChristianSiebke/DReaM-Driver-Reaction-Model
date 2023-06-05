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

#include <functional>
#include <iostream>

#include "ActionStateHandler.h"
#include "Anticipation.h"
#include "Common/Definitions.h"
#include "Common/Helper.h"
#include "Components/ComponentInterface.h"
#include "include/stochasticsInterface.h"

namespace LongitudinalDecision {
using emergencyState = bool;
using timeEmergencyBrakeActive = int;
struct EmergencyBrakeInfo {
    bool emergencyState;
    int timeEmergencyBrakeActive;
    double deceleration;
};

class MinEmergencyBrakeDelay {
public:
    MinEmergencyBrakeDelay(double minTimeEmergencyBrakeIsActive, int cycleTime);
    void ResetEmergencyState();
    void InsertEmergencyBrakeEvent(int agentID, double deceleration);

    std::vector<std::pair<double, int>> ActivateIfNeeded(const std::unordered_map<int, std::unique_ptr<AgentInterpretation>> &agents);

private:
    std::unordered_map<int, EmergencyBrakeInfo> emergencyBrake;
    double minTimeEmergencyBrakeIsActive;
    int cycleTime;
};

class LongitudinalDecision : public Component::ComponentInterface {
public:
    LongitudinalDecision(const WorldRepresentation &worldRepresentation, const WorldInterpretation &worldInterpretation, int cycleTime,
                         StochasticsInterface *stochastics, LoggerInterface *loggerInterface, const BehaviourData &behaviourData) :
        ComponentInterface(cycleTime, stochastics, loggerInterface, behaviourData),
        worldRepresentation{worldRepresentation},
        worldInterpretation{worldInterpretation},
        actionStateHandler(worldRepresentation, worldInterpretation),
        anticipation(worldRepresentation, worldInterpretation, stochastics, loggerInterface, behaviourData),
        minEmergencyBrakeDelay(GetBehaviourData().adBehaviour.minTimeEmergencyBrakeIsActive, GetCycleTime()) {
    }
    LongitudinalDecision(const LongitudinalDecision &) = delete;
    LongitudinalDecision(LongitudinalDecision &&) = delete;
    LongitudinalDecision &operator=(const LongitudinalDecision &) = delete;
    LongitudinalDecision &operator=(LongitudinalDecision &&) = delete;
    ~LongitudinalDecision() override = default;

    void Update() override;

    double GetAcceleration() const {
        return accelerationResult;
    }
    //--debugging
    std::string debuggingState;
    //--debugging

private:
    double DetermineAccelerationWish();

    double AgentCrashImminent(const std::unique_ptr<AgentInterpretation> &oAgent) const;
    
    double accelerationResult;

    const MentalInfrastructure::Road *currentRoad = nullptr;
    const WorldRepresentation &worldRepresentation;
    const WorldInterpretation &worldInterpretation;

    ActionStateHandler actionStateHandler;
    Anticipation anticipation;
    MinEmergencyBrakeDelay minEmergencyBrakeDelay;
};

} // namespace LongitudinalDecision
