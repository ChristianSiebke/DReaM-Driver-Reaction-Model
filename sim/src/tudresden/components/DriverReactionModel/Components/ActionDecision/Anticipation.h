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
#include "Common/Definitions.h"
#include "Common/Helper.h"
#include "Common/WorldRepresentation.h"
#include "Components/ComponentInterface.h"
namespace ActionDecision {

struct TimeToConflictArea {
    double start; // time until the front of the vehicle reach conflict area
    double end;   // time until the end of the vehicle leaves the conflict area
};

class Anticipation {
  public:
    Anticipation(const WorldRepresentation& worldRepresentation, const WorldInterpretation& worldInterpretation,
                 StochasticsInterface* stochastics, const LoggerInterface* loggerInterface, const BehaviourData& behaviourData)
        : worldRepresentation{worldRepresentation}, worldInterpretation{worldInterpretation}, stochastics{stochastics},
          loggerInterface{loggerInterface}, behaviourData{behaviourData} {
        DistributionEntry maxEmergencyDecelDistribution = GetBehaviourData().adBehaviour.maxEmergencyDeceleration;
        double emergencyDec =
            stochastics->GetNormalDistributed(maxEmergencyDecelDistribution.mean, maxEmergencyDecelDistribution.std_deviation);
        maxEmergencyDeceleration =
            Common::ValueInBounds(maxEmergencyDecelDistribution.min, emergencyDec, maxEmergencyDecelDistribution.max);
    }
    double IntersectionGap(const std::unique_ptr<AgentInterpretation>& agent, double targetVelocity);

    /*!
     * \brief calculate acceleration according intelligent driver model Martin Treiben
     *
     * return acceleration
     *
     */
    double IDMAcceleration(double velTarget, double dv, double sDiff) const;

    double ComfortAccelerationWish(double velTarget, double dv, double sDiff) const;

    double MaximumAccelerationWish(double velTarget, double dv, double sDiff) const;
    double CalculatePhaseAcceleration(double velTarget, double v);

    double Deceleration(const std::unique_ptr<AgentInterpretation>& observedAgent) const;
    double GetMaxEmergencyAcceleration() const { return maxEmergencyDeceleration; }

  private:
    void DeletePriorityAgent(int oAgentID);

    TimeToConflictArea CalculateTimeToConflictAreaEgo(DistanceToConflictArea distance, double velocity, double vTarget) const;

    TimeToConflictArea CalculateTimeToConflictAreaObserved(DistanceToConflictArea distance, double acceleration, double velocity) const;

    double TravelTimeEgo(double distance, double velocity, double vTarget) const;
    double TravelTimeObserved(double distance, double acceleration, double velocity) const;

    double CalculateDeceleration(double sFrontEgo, double tEndObserved, const std::unique_ptr<AgentInterpretation>& observedAgent) const;

    const BehaviourData& GetBehaviourData() const { return behaviourData; }

    double maxEmergencyDeceleration;

    std::vector<int> priorityAgents;
    std::vector<int> obstacles;
    const WorldRepresentation& worldRepresentation;
    const WorldInterpretation& worldInterpretation;
    StochasticsInterface* stochastics;
    const LoggerInterface* loggerInterface;
    const BehaviourData& behaviourData;
};
} // namespace ActionDecision
