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
typedef int NumberOfPriorityAgentsOnIntersection;
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
    double GetMaxEmergencyAcceleration() const {
        return maxEmergencyDeceleration;
    }
    double AnticipationAccelerationToAchieveVelocityInDistance(double distance, double velTarget, double currentVelocity) const;

    double IDMBrakeStrategy(double distance, double velTarget, double currentVelocity) const;

    void UpdateEqualPriorityCommunication(int egoAgentId, const std::unique_ptr<AgentInterpretation> &oAgent) {
        auto oAgentId = oAgent->agent->GetID();

        if (std::any_of(equalPriorityCommunication.begin(), equalPriorityCommunication.end(),
                        [egoAgentId](std::pair<int, std::unordered_map<int, std::pair<std::optional<NumberOfPriorityAgentsOnIntersection>,
                                                                                      std::optional<NumberOfPriorityAgentsOnIntersection>>>>
                                         element) { return element.first == egoAgentId; })) {
            if (!oAgent->conflictSituation.has_value()) {
                equalPriorityCommunication.at(egoAgentId).erase(oAgentId);
            }
            for (auto it = equalPriorityCommunication.at(egoAgentId).cbegin(); it != equalPriorityCommunication.at(egoAgentId).cend();) {
                if (std::none_of(worldInterpretation.interpretedAgents.begin(), worldInterpretation.interpretedAgents.end(),
                                 [it](const auto &element) { return it->first == element.first; })) {
                    it = equalPriorityCommunication.at(egoAgentId).erase(it);
                }
                else {
                    it++;
                }
            }

            if (equalPriorityCommunication.at(egoAgentId).empty()) {
                equalPriorityCommunication.erase(egoAgentId);
            }
        }
    }

private:
    void UpdatePriorityAgents(int oAgentID, const ConflictSituation &cA);
    void DeletePriorityAgent(int oAgentID);

    TimeToConflictArea CalculateTimeToConflictAreaEgo(DistanceToConflictArea distance, double velocity) const;
    TimeToConflictArea CalculateTimeToConflictAreaObserved(const ConflictSituation &situation,
                                                           const std::unique_ptr<AgentInterpretation> &oAgent) const;

    double TravelTimeTargetVelocity(double distance, double velocity, double vTarget) const;
    double TravelTimeObserved(double distance, bool egoInsideConflictArea, const std::unique_ptr<AgentInterpretation> &observedAgent) const;
    double TravelTime(double distance, double distanceAcceleration, double velocity, double acceleration, double vTarget) const;
    double ApproachingStoppingPoint(double sFrontEgo, double tEndObserved, const AgentInterpretation *observedAgent) const;

    double AccelerationIfPriorityAgentExist(const std::unique_ptr<AgentInterpretation> &observedAgent,
                                            const std::optional<ConflictSituation> &conflictSituation, const TimeToConflictArea &tObserved,
                                            double freeAccelerationEgo);
    const BehaviourData &GetBehaviourData() const {
        return behaviourData;
    }
    static std::unordered_map<int, std::unordered_map<int, std::pair<std::optional<NumberOfPriorityAgentsOnIntersection>,
                                                                     std::optional<NumberOfPriorityAgentsOnIntersection>>>>
        equalPriorityCommunication;
    double maxEmergencyDeceleration;
    double comfortDeceleration;
    std::unordered_map<int, ConflictSituation> priorityAgents{};
    const WorldRepresentation &worldRepresentation;
    const WorldInterpretation &worldInterpretation;
    StochasticsInterface *stochastics;
    const LoggerInterface *loggerInterface;
    const BehaviourData &behaviourData;
    };
} // namespace LongitudinalDecision
