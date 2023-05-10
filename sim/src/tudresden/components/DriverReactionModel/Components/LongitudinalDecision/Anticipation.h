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
#include "Common/Definitions.h"
#include "Common/Helper.h"
#include "Common/WorldRepresentation.h"
#include "Components/ComponentInterface.h"
namespace LongitudinalDecision {

struct TimeToConflictArea {
    double vehicleFrontToCAStart; // time until the front of the vehicle reach conflict area
    double vehicleBackToCAEnd;    // time until the end of the vehicle leaves the conflict area
};

class Anticipation {
  public:
    Anticipation(const WorldRepresentation& worldRepresentation, const WorldInterpretation& worldInterpretation,
                 StochasticsInterface* stochastics, const LoggerInterface* loggerInterface, const BehaviourData& behaviourData)
        : worldRepresentation{worldRepresentation}, worldInterpretation{worldInterpretation}, stochastics{stochastics},
          loggerInterface{loggerInterface}, behaviourData{behaviourData} {
        DistributionEntry maxEmergencyDecelDistribution = GetBehaviourData().adBehaviour.maxEmergencyDeceleration;
        double emergencyDec = -stochastics->GetLogNormalDistributed(std::abs(maxEmergencyDecelDistribution.mean),
                                                                    maxEmergencyDecelDistribution.std_deviation);
        maxEmergencyDeceleration =
            Common::ValueInBounds(maxEmergencyDecelDistribution.min, emergencyDec, maxEmergencyDecelDistribution.max);

        DistributionEntry comfortDecelerationDistribution = GetBehaviourData().adBehaviour.comfortDeceleration;
        double drawnComfortDec = -stochastics->GetLogNormalDistributed(std::abs(comfortDecelerationDistribution.mean),
                                                                       comfortDecelerationDistribution.std_deviation);
        comfortDeceleration =
            Common::ValueInBounds(comfortDecelerationDistribution.min, drawnComfortDec, comfortDecelerationDistribution.max);
    }
    double IntersectionGap(const std::unique_ptr<AgentInterpretation> &agent);

    /*!
     * \brief calculate acceleration according intelligent driver model Martin Treiben
     *
     * return acceleration
     *
     */
    double IDMAcceleration(double velTarget, double velCurrent, double dv, double sDiff) const;

    double ComfortAccelerationWish(double velTarget, double velCurrent, double dv, double sDiff) const;

    double MaximumAccelerationWish(double velTarget, double velCurrent, double dv, double sDiff) const;
    double CalculatePhaseAcceleration() const;

    double Deceleration(const std::unique_ptr<AgentInterpretation>& observedAgent) const;
    double GetMaxEmergencyAcceleration() const { return maxEmergencyDeceleration; }
    double IDMBrakeStrategy(double distance, double velTarget, double currentVelocity) const;

private:
     double AnticipationAccelerationToAchieveVelocityInDistance(double distance, double velTarget, double currentVelocity) const;

    void DeletePriorityAgent(int oAgentID);

    TimeToConflictArea CalculateTimeToConflictArea(DistanceToConflictArea distance, double velocity) const;
    double TravelTime(double distance, double velocity, double vTarget) const;

    double ApproachingStoppingPoint(double sFrontEgo, double tEndObserved, const AgentInterpretation *observedAgent) const;

    const BehaviourData &GetBehaviourData() const {
        return behaviourData;
        }

        double maxEmergencyDeceleration;
        double comfortDeceleration;
        std::unordered_map<int, const MentalInfrastructure::ConflictArea *> priorityAgents;
        const WorldRepresentation &worldRepresentation;
        const WorldInterpretation &worldInterpretation;
        StochasticsInterface *stochastics;
        const LoggerInterface *loggerInterface;
        const BehaviourData &behaviourData;
    };
} // namespace LongitudinalDecision
