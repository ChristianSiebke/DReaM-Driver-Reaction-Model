/******************************************************************************
* Copyright (c) 2019 AMFD GmbH
*
* This program and the accompanying materials are made available under the
* terms of the Eclipse Public License 2.0 which is available at
* https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*****************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AlgorithmSituationAssessment.h
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief provide data and calculate driver's situation assessment parameters.
//!
//! This Class contains the assessment of the current situation, regarding the surrounding agents
//! and select the leader, follower, neighleader and neighfollower and checking the criticality of the situation
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_ModularDriver
//-----------------------------------------------------------------------------

#pragma once

//#include <vector>
#include "Interfaces/stochasticsInterface.h"
#include "Interfaces/observationInterface.h"
#include <ContainerStructures.h>
#include "SituationCalculation.h"
#include "SituationLogging.h"

//! \ingroup AlgorithmSituationAssessment


class SituationAssessment
{
public:

    //!
    //! \brief SituationAssessment
    //! \param stochastics          Stochastics module
    //! \param cycleTime            defined cycleTime of the current simulation run
    //!
    SituationAssessment(StochasticsInterface *stochastics, int cycleTime, ObservationInterface *observer); // avoid access of constructor of singleton outside class

    //!
    //! \brief Initialize                       initializes the static values for the class
    //! \param vWish                            mean velocity-wish of the current agent
    //! \param vWish_deviation                  std deviation of the velocity wish (lognormal)
    //! \param tGapWish                         mean prefered time-gap to the leader agent
    //! \param tGapWish_deviation               std deviation of the prefered time-gap (lognormal)
    //! \param speedLimit_violation             mean violation difference to the current speed-limit (speedlimit 120 km/h, violation 10 km/h --> vwish 130 +- deviation)
    //! \param speedLimit_violation_deviation   std deviation of the violation difference
    //! \param minDistance                      minimal accepted distance between the leader and ego
    //! \param vehicleparameters                the static vehicle parameters of the current agent
    //! \param observer                         current observation instance
    //!
    void Initialize();

    void LogSetValues(int time);

    bool InitialValuesLogged()
    {
        return Logged;
    }

    //!
    //! \brief SetSituationAssessment_Input sets the input of the class
    //! \param Input
    //!
    void SetSituationAssessment_Input(SitationAssessment_Input *Input);

    SituationAssessment_Output_TD *GetSituationAssessment_Output_TD();

    void SetMeanReactionTime(int MeanReactionTime)
    {
        meanReactionTime = MeanReactionTime;
    }
    void SetReactionTimeDeviation(int ReactionTimeDeviation)
    {
        reactionTimeDeviation = ReactionTimeDeviation;
    }
    void SetMinReactionTime(int MinReactionTime)
    {
        minReactionTime = MinReactionTime;
    }

    void SetEnvironmentInformation(SituationAssessment_Output_BU *SA_Output_BU);
    void AssessEgoSituationAndWish(SituationAssessment_Output_BU *SA_Output_BU);
    void Pigeonhole_SurroundingMovingObjectsToEgo(SituationAssessment_Output_BU *SA_Output_BU, int time);

    void SetNearVehicle(RelationType RelationType,
                        const SurroundingMovingObjectsData *it,
                        State *ItState,
                        double *mindistance,
                        double currdist,
                        SituationAssessment_Output_BU *SA_Output_BU);

    void SetRelationType(RelationType RelationType,
                           double MinDistance,
                           const SurroundingMovingObjectsData *it,
                           SituationAssessment_Output_BU *SA_Output_BU);

    // Checks if current viewed vehicle is eighter changing its lane to the viewed targetlane or covering it secondary
    bool CheckItOnViewedLane(AgentRepresentation &it, int targetlaneid);
    double AdaptionOnSpeedLimit(double SpeedLimit, double Distance);

    void UpdateSituationCalculations();

    void CheckForInternalLogging(int time);

    std::vector<std::string> SplitLoggingStates(std::string input)
    {
        if (!input.empty())
        {
            std::vector<std::string> output;
            std::string token;
            std::istringstream tokenStream(input);
            while (std::getline(tokenStream, token, ','))
               {
                  output.push_back(token);
               }
            return output;
        }
        else
        {
            std::vector<std::string> output{};
            return output;
        }
    }

    void SetVWish(double vWish)
    {
        this->vWish = vWish;
    }
    void SetVWishDeviation(double vWish_deviation)
    {
        this->vWish_deviation = vWish_deviation;
    }
    void SetTGapWish(double tGapWish)
    {
        this->tGapWish = tGapWish;
    }
    void SetTGapWishDeviation(double tGapWish_deviation)
    {
        this->tGapWish_deviation = tGapWish_deviation;
    }
    void SetSpeedLimit_Violation(double speedLimit_Violation)
    {
        this->speedLimit_Violation = speedLimit_Violation;
    }
    void SetSpeedLimit_ViolationDeviation(double speedLimit_Violation_deviation)
    {
        this->speedLimit_Violation_deviation = speedLimit_Violation_deviation;
    }
    void SetMinDistance(double minDistance)
    {
        this->minDistance = minDistance;
    }
    void SetLoggingGroups(std::string LoggingGroups)
    {
        this->LoggingGroups = LoggingGroups;
    }
    void SetVehicleParameters(const VehicleModelParameters *vehicleParameters)
    {
        this->vehicleParameters = vehicleParameters;
    }
    void ResetReactionTime(RelationType relationType)
    {
        int time = std::max(minReactionTime,(int) std::round(_stochastic->GetLogNormalDistributed(meanReactionTime,reactionTimeDeviation)/10)*10);
        if (ReactionTrigger.find(relationType)!=ReactionTrigger.end())
        {
            ReactionTrigger.at(relationType) = 0;
            ReactionTime.at(relationType) = time;
        }
        else
        {
            ReactionTrigger.emplace(relationType, 0);
            ReactionTime.emplace(relationType, time);
        }
    }
    void IncrementReactionTime(RelationType relationType)
    {
        if (ReactionTrigger.find(relationType)!=ReactionTrigger.end())
        {
            ReactionTrigger.at(relationType) += cycleTime;
        }
        else
        {
            ReactionTrigger.emplace(relationType, cycleTime);
        }
    }
    void ActivateReaction(RelationType relationType)
    {
        if (React.find(relationType)!=React.end())
        {
            React.at(relationType) = true;
        }
        else
        {
           React.emplace(relationType, true);
        }
    }
    void DeactivateReaction(RelationType relationType)
    {
        if (React.find(relationType)!=React.end())
        {
            React.at(relationType) = false;
        }
        else
        {
           React.emplace(relationType, false);
        }
    }
    bool IsReactive(RelationType relationType)
    {
        if (React.find(relationType)!=React.end())
        {
            return React.at(relationType);
        }
        return false;
    }
    int GetCurrReactionTrigger(RelationType relationType)
    {
        if (ReactionTrigger.find(relationType)!=ReactionTrigger.end())
        {
            return ReactionTrigger.at(relationType);
        }
        return 0;
    }
    int GetCurrReactionTime(RelationType relationType)
    {
        if (ReactionTime.find(relationType)!=ReactionTime.end())
        {
            return ReactionTime.at(relationType);
        }
        return 100;
    }
    int GetLastNearVehicleId(RelationType relationType)
    {
        if (lastNearTraffic.find(relationType)!=lastNearTraffic.end())
        {
            return lastNearTraffic.at(relationType);
        }
        return -999;
    }
    void SetLastNearVehicleId(RelationType relationType, int id)
    {
        if (lastNearTraffic.find(relationType)!=lastNearTraffic.end())
        {
            lastNearTraffic.at(relationType) = id;
        }
        else
        {
            lastNearTraffic.emplace(relationType, id);
        }
    }
    void ClearLastNearVehicleId(RelationType relationType)
    {
        if (lastNearTraffic.find(relationType)!=lastNearTraffic.end())
        {
            lastNearTraffic.erase(relationType);
        }
    }
    void SetLastNearVehicles(std::map<RelationType, SurroundingMovingObjectsData> *NearTraffic)
    {
        for (int reltypeint = Leader ; reltypeint != FollowerLeft ; reltypeint++)
        {
            RelationType relationType = static_cast<RelationType>(reltypeint);
            if (NearTraffic->find(relationType)!=NearTraffic->end())
                SetLastNearVehicleId(relationType, NearTraffic->at(relationType).GetState()->id);
            else
            {
                ClearLastNearVehicleId(relationType);
            }
        }
    }
    void CheckReaction()
    {
        for (int reltypeint = Leader ; reltypeint != FollowerLeft ; reltypeint++)
        {
            RelationType relationType = static_cast<RelationType>(reltypeint);
            if (IsReactive(relationType))
            {
                IncrementReactionTime(relationType);
            }
            else
            {
                ResetReactionTime(relationType);
            }
        }
    }

protected:


private:


    SitationAssessment_Input *SA_Input;
    SituationAssessment_Output_TD SA_Output_TD;

    StochasticsInterface *_stochastic;
    int cycleTime;
    ObservationInterface *observation;

    bool Logged = false;

    const VehicleModelParameters *vehicleParameters;

    int meanReactionTime = 0;
    int reactionTimeDeviation = 0;
    int minReactionTime = 0;

    std::string LoggingGroups {};
    std::vector<std::string> loggingGroups;

    SituationCalculation situationCalculation;
    SituationLogging situationlogging;

    double v_y_Max;

    double vWish = 140 / 3.6; //mean of lognormal-distribution for the wish-velocity in km/h
    double vWish_deviation = 20 / 3.6; //std deviation for the lognormal-distribution
    double tGapWish = 1.5; //mean of lognormal-distribution for the wish-netto-timegap between leader and ego in s
    double tGapWish_deviation = 0.3; //std deviation for the lognormal-distribution
    double speedLimit_Violation = 5 / 3.6; // mean difference (normal-distribution) between the chosen speed and the current speed-limit
    double speedLimit_Violation_deviation = 3 / 3.6; //std deviation of the speedLimit_Violation
    double minDistance = 2; //min
    double lastSpeedLimit = 300/3.6;

    std::map<RelationType, int> lastNearTraffic;
    std::map<RelationType, int> ReactionTrigger;
    std::map<RelationType, bool> React;
    std::map<RelationType, int> ReactionTime;

    double commonSpeedLimit_Violation;
    double v_Wish_out;
    double thw_Wish_out;

    bool loggingActivated = false;
    bool initialisationSpeedLimit = false;

    double collisionDetectionLongitudinalBoundary {0.3}; ///!< Additional length added to the vehicle boundary when checking for collision detection
    double collisionDetectionLateralBoundary {0.3}; ///!< Additional width added to the vehicle boundary when checking for collision detection
    double ttcBrake{2}; ///!< The minimum Time-To-Collision before the AEB component activates


};


