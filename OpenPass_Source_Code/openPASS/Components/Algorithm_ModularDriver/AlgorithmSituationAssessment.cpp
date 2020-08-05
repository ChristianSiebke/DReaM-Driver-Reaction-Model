/******************************************************************************
* Copyright (c) 2019 TU Dresden
* Copyright (c) 2019 AMFD GmbH
*
* This program and the accompanying materials are made available under the
* terms of the Eclipse Public License 2.0 which is available at
* https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*****************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AlgorithmSituationAssessment.cpp
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

//#include <cassert>
//#include <cmath>
//#include <array>
//#include <limits>
//#include <map>
#include <iostream>
#include <list>
#include <set>
#include "AlgorithmSituationAssessment.h"
#include "boundingBoxCalculation.h"


SituationAssessment::SituationAssessment(StochasticsInterface *stochastics, int cycleTime, ObservationInterface *observer):
    _stochastic(stochastics),
    cycleTime(cycleTime),
    situationCalculation(cycleTime, stochastics),
    situationlogging(cycleTime, &situationCalculation),
    observation(observer)
{
    v_y_Max = _stochastic->GetNormalDistributed(1,0.2); //TODO
}

void SituationAssessment::Initialize()
{
    vWish = _stochastic->GetLogNormalDistributed(vWish, vWish_deviation);
    tGapWish = _stochastic->GetLogNormalDistributed(tGapWish, tGapWish_deviation); //std::min(0.7, _stochastic->GetLogNormalDistributed(tGapWish, tGapWish_deviation));
    thw_Wish_out = tGapWish;
    commonSpeedLimit_Violation = _stochastic->GetNormalDistributed(speedLimit_Violation, speedLimit_Violation_deviation);
    situationCalculation.Initialize(vehicleParameters);
    loggingGroups = SplitLoggingStates(LoggingGroups);
    loggingActivated = situationlogging.Initialize(loggingGroups, observation);
}

void SituationAssessment::SetSituationAssessment_Input(SitationAssessment_Input *Input)
{
    SA_Input = {};
    SA_Input = Input;
}

void SituationAssessment::SetEnvironmentInformation(SituationAssessment_Output_BU *SA_Output_BU)
{
    SA_Output_BU->AssessedEnvironment.StaticEnvironment = SA_Input->MM_O->EnvironmentInfo;
}

SituationAssessment_Output_TD *SituationAssessment::GetSituationAssessment_Output_TD()
{
    return &SA_Output_TD;
}

void SituationAssessment::AssessEgoSituationAndWish(SituationAssessment_Output_BU *SA_Output_BU)
{
    SA_Output_BU->Ego = SA_Input->MM_O->Ego;
    speedLimit SpeedLimit = situationCalculation.CheckSignsForRelevantSpeedLimit();
    double currSpeedLimit = SpeedLimit.GetCurrentSpeedLimit();
    double distance = SpeedLimit.GetCurrentDistanceToSign();

    stopSign StopSign = situationCalculation.CheckSignsForRelevantStopSign();

    SA_Output_BU->AssessedEnvironment.SpeedLimit = SpeedLimit;
    SA_Output_BU->AssessedEnvironment.StopSign = StopSign;

    v_Wish_out = AdaptionOnSpeedLimit(currSpeedLimit, distance);

    driverInformation DriverInformation;
    DriverInformation.v_Wish = &v_Wish_out;
    DriverInformation.thw_Wish = &thw_Wish_out;
    DriverInformation.v_y_Max = &v_y_Max;
    DriverInformation.MinGap = &minDistance;
    DriverInformation.commonSpeedLimit_Violation = &commonSpeedLimit_Violation;
    SA_Output_BU->Ego->SetDriverInformation(DriverInformation);
}

void SituationAssessment::Pigeonhole_SurroundingMovingObjectsToEgo(SituationAssessment_Output_BU *SA_Output_BU, int time)
{
    double mindistfront             = {std::numeric_limits<double>::max()};
    double mindistback              = {std::numeric_limits<double>::max()};
    double mindistneighfrontleft    = {std::numeric_limits<double>::max()};
    double mindistneighfollowleft   = {std::numeric_limits<double>::max()};
    double mindistneighfrontright   = {std::numeric_limits<double>::max()};
    double mindistneighfollowright  = {std::numeric_limits<double>::max()};

    egoData *Ego = SA_Input->MM_O->Ego;
    state *EgoState = Ego->GetState();
    bool lanerightexists = SA_Output_BU->AssessedEnvironment.StaticEnvironment->roadGeometry.laneRight.exists;
    bool laneleftexists = SA_Output_BU->AssessedEnvironment.StaticEnvironment->roadGeometry.laneLeft.exists;

    IndicatorState indState = EgoState->indicatorstate;
    int *laneId = &Ego->GetState()->laneid;
    int rightlaneId = *laneId - 1;
    int leftlaneId = *laneId + 1;

    std::map<RelationType, SurroundingMovingObjectsData> *NearTraffic = &SA_Output_BU->NearTraffic;
    NearTraffic->clear();

    for (auto&& it = SA_Input->MM_O->SurroundingMovingObjects->begin() ; it != SA_Input->MM_O->SurroundingMovingObjects->end(); it++)
    {
        SurroundingMovingObjectsData ItInternal = it->get()->Get_internal_Data();
        state *ItState = it->get()->Get_internal_Data().GetState();
        properties *ItProperties = it->get()->Get_internal_Data().GetProperties();

        int itlaneId = ItState->laneid;
        double front_s_it = ItState->roadPos.s + ItProperties->distanceReftoLeadingEdge;
        double front_s_Ego = EgoState->roadPos.s + vehicleParameters->distanceReferencePointToLeadingEdge;
        double frontEgo_back_it_distance = ItState->roadPos.s + ItProperties->distanceReftoLeadingEdge - ItProperties->lx - EgoState->roadPos.s - vehicleParameters->distanceReferencePointToLeadingEdge;
        double backEgo_front_it_distance = EgoState->roadPos.s + vehicleParameters->distanceReferencePointToLeadingEdge - vehicleParameters->length - ItState->roadPos.s - ItProperties->distanceReftoLeadingEdge;

        if ((itlaneId == *laneId) || (CheckItOnViewedLane(*it->get(), *laneId)))
        {
            if ((front_s_it > front_s_Ego) && (frontEgo_back_it_distance < mindistfront))
            {
                SetNearVehicle(Leader, &ItInternal, ItState, &mindistfront, frontEgo_back_it_distance, SA_Output_BU);
            }

            if ((front_s_it < front_s_Ego) && (backEgo_front_it_distance < mindistback))
            {
                SetNearVehicle(Follower, &ItInternal, ItState, &mindistback, backEgo_front_it_distance, SA_Output_BU);
            }
        }

        if ((lanerightexists)
            && ((itlaneId == (rightlaneId)) || CheckItOnViewedLane(*it->get(), rightlaneId)))
        {
            if ((front_s_it > front_s_Ego) && (frontEgo_back_it_distance < mindistneighfrontright))
            {
                SetNearVehicle(LeaderRight, &ItInternal, ItState, &mindistneighfrontright, frontEgo_back_it_distance, SA_Output_BU);
            }
            if ((front_s_it < front_s_Ego) && (backEgo_front_it_distance < mindistneighfollowright))
            {
                SetNearVehicle(FollowerRight, &ItInternal, ItState, &mindistneighfollowright, backEgo_front_it_distance, SA_Output_BU);
            }
        }

        if ((laneleftexists)
            && ((itlaneId == (leftlaneId)) || CheckItOnViewedLane(*it->get(), leftlaneId)))
        {
            if ((front_s_it > front_s_Ego) && (frontEgo_back_it_distance < mindistneighfrontleft))
            {
                SetNearVehicle(LeaderLeft, &ItInternal, ItState, &mindistneighfrontleft, frontEgo_back_it_distance, SA_Output_BU);
            }
            if ((front_s_it < front_s_Ego) && (backEgo_front_it_distance < mindistneighfollowleft))
            {
                SetNearVehicle(FollowerLeft, &ItInternal, ItState, &mindistneighfollowleft, backEgo_front_it_distance, SA_Output_BU);
            }
        }                
    }

    CheckReaction();
    SetLastNearVehicles(NearTraffic);

}

void SituationAssessment::SetRelationType(RelationType RelationType, double MinDistance, const SurroundingMovingObjectsData *it, SituationAssessment_Output_BU *SA_Output_BU)
{
    if (SA_Output_BU->NearTraffic.find(RelationType)!=SA_Output_BU->NearTraffic.end())
    {
        SA_Output_BU->NearTraffic.at(RelationType) = *it;
    }
    else
    {
        SA_Output_BU->NearTraffic.emplace(RelationType, *it);
    }
    SA_Output_BU->NearTraffic.at(RelationType).SetDistanceToEgo(MinDistance);
}


bool SituationAssessment::CheckItOnViewedLane(AgentRepresentation &it, int targetlaneid)
{
    IndicatorState indState = it.Get_internal_Data().GetState()->indicatorstate;
    int itlaneid = it.Get_internal_Data().GetState()->laneid;

    if ((itlaneid + 1 == targetlaneid) && (indState==IndicatorState::IndicatorState_Left))
    {
        return true;
    }
    if ((itlaneid - 1 == targetlaneid) && (indState==IndicatorState::IndicatorState_Right))
    {
        return true;
    }
    else
    {
        std::set<int> secondaryLanes = it.Get_internal_Data().GetState()->secondarycoveredlanes;
        for (auto lane = secondaryLanes.begin(); lane != secondaryLanes.end(); ++lane)
        {
            if (*lane == targetlaneid)
                return true;
        }
            return false;
    }
}

double SituationAssessment::AdaptionOnSpeedLimit(double SpeedLimit, double Distance)
{
    double v_Wish_new = vWish;
    if (vWish > SpeedLimit)
    {                      
        if (Distance > 0 && Distance < 200)
        {
            lastSpeedLimit = SpeedLimit;
            double v_Wish_target = SpeedLimit + commonSpeedLimit_Violation;
            const double v_ego = SA_Input->MM_O->Ego->GetState_Ego()->velocity_long;
            v_Wish_new = v_ego + (v_Wish_target - v_ego)/Distance * (cycleTime*0.001);
            return v_Wish_new;
        }
        if (Distance < 0)
        {
            lastSpeedLimit = SpeedLimit;
        }
    }
    if (vWish > lastSpeedLimit)
    {
        return lastSpeedLimit + commonSpeedLimit_Violation;
    }
    return v_Wish_new;
}

void SituationAssessment::UpdateSituationCalculations()
{
    situationCalculation.UpdateMembers(SA_Input->MM_O->Ego, SA_Input->MM_O->SurroundingMovingObjects, SA_Input->MM_O->EnvironmentInfo);
}

void SituationAssessment::CheckForInternalLogging(int time)
{
    if (loggingActivated)
    {
        situationlogging.CheckForLoggingStatesAndLog(time, "Ego:");
    }
}

void SituationAssessment::SetNearVehicle(RelationType RelationType,
                                         const SurroundingMovingObjectsData *it,
                                         state *ItState,
                                         double *mindistance,
                                         double currdist,
                                         SituationAssessment_Output_BU *SA_Output_BU)
{
    if (GetLastNearVehicleId(RelationType) == -999
        || GetLastNearVehicleId(RelationType)==ItState->id)
    {
        *mindistance = currdist;
        SetRelationType(RelationType, *mindistance, it, SA_Output_BU);
        DeactivateReaction(RelationType);
    }
    else
    {
        ActivateReaction(RelationType);
    }
    if (IsReactive(RelationType) && GetCurrReactionTrigger(RelationType) >= GetCurrReactionTime(RelationType))
    {
        *mindistance = currdist;
        SetRelationType(RelationType, *mindistance, it, SA_Output_BU);
    }
}

void SituationAssessment::LogSetValues(int time)
{
    observation->Insert(time,
                        SA_Input->MM_O->Ego->GetState()->id,
                        LoggingGroup::Driver,
                        "VWish",
                        std::to_string(vWish));
    observation->Insert(time,
                        SA_Input->MM_O->Ego->GetState()->id,
                        LoggingGroup::Driver,
                        "ThwWish",
                        std::to_string(tGapWish));
    observation->Insert(time,
                        SA_Input->MM_O->Ego->GetState()->id,
                        LoggingGroup::Driver,
                        "CommonSpeedlimitViolation",
                        std::to_string(commonSpeedLimit_Violation));

    Logged = true;
}




