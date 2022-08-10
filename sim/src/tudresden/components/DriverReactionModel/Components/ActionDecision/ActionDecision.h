/******************************************************************************
 * Copyright (c) 2019 TU Dresden
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

#include <functional>
#include <iostream>

#include "ActionStateHandler.h"
#include "Anticipation.h"
#include "Common/Definitions.h"
#include "Common/Helper.h"
#include "Components/ComponentInterface.h"
#include "include/stochasticsInterface.h"

namespace ActionDecision {
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

    std::vector<double> ActivateIfNeeded(const std::unordered_map<int, std::unique_ptr<AgentInterpretation>>& agents);

  private:
    std::unordered_map<int, EmergencyBrakeInfo> emergencyBrake;
    double minTimeEmergencyBrakeIsActive;
    int cycleTime;
};

class ActionDecision : public Component::ComponentInterface {
  public:
      ActionDecision(const WorldRepresentation &worldRepresentation, const WorldInterpretation &worldInterpretation, int cycleTime,
                     StochasticsInterface *stochastics, LoggerInterface *loggerInterface, const BehaviourData &behaviourData) :
          ComponentInterface(cycleTime, stochastics, loggerInterface, behaviourData),
          worldRepresentation{worldRepresentation},
          worldInterpretation{worldInterpretation},
          actionStateHandler(worldRepresentation, worldInterpretation),
          anticipation(worldRepresentation, worldInterpretation, stochastics, loggerInterface, behaviourData),
          minEmergencyBrakeDelay(GetBehaviourData().adBehaviour.minTimeEmergencyBrakeIsActive, GetCycleTime()) {
      }
    ActionDecision(const ActionDecision&) = delete;
    ActionDecision(ActionDecision&&) = delete;
    ActionDecision& operator=(const ActionDecision&) = delete;
    ActionDecision& operator=(ActionDecision&&) = delete;
    ~ActionDecision() override = default;

    void Update() override;

    double GetAcceleration() const { return accelerationResult; }

  private:
    double DetermineAccelerationWish();

    bool observedAgentIsbehindEgoAgent(const std::unique_ptr<AgentInterpretation>& oAgent) const;

    double AgentCrashImminent(const std::unique_ptr<AgentInterpretation> &oAgent) const;

    bool CloseToConlictArea() const;
    bool EgoHasRightOfWay(const std::unique_ptr<AgentInterpretation>& agent) const;


    double accelerationResult;

    const MentalInfrastructure::Road *currentRoad = nullptr;
    const WorldRepresentation& worldRepresentation;
    const WorldInterpretation& worldInterpretation;

    ActionStateHandler actionStateHandler;
    Anticipation anticipation;
    MinEmergencyBrakeDelay minEmergencyBrakeDelay;
};

} // namespace ActionDecision
