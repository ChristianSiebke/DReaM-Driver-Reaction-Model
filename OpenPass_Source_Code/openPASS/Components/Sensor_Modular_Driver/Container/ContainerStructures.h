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
//! @file  ContainerStructures.h
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief provide Container Structures for the driver's cognition
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_ModularDriver
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include "globalDefinitions.h"
#include "complexsignals.h"
#include <list>
#include <memory>
#include "surroundingmovingobjectsdata.h"
#include "staticenvironmentdata.h"
#include "egodata.h"
#include "agent_representation.h"

class AgentRepresentation;

class speedLimit
{
public:

    double TransformSignToSpeed (CommonTrafficSign::Entity *sign)//match type of the signal (e.g. code according to StVO) to value
    {
        switch (sign->type)
        {
        case (274): //MaximumSpeedLimit
            return sign->value / 3.6;
        case (278): //EndOfMaximumSpeedLimit
            return INFINITY;
        case (310): //TownBegin
            return 50 / 3.6;
        case (311): //TownEnd
            return 100 / 3.6;
        case (2741): //Zone30Begin
            return 30 / 3.6;
        case (2742): //Zone30End
            return 50 / 3.6;
        case (3251): //TrafficCalmedDistrictBegin
            return 4 / 3.6;
        case (3252): //TrafficCalmedDistrictEnd
        {
            std::list<double>::iterator sl = SpeedLimits.begin();
            std::advance(sl, 1);
            if (*sl == 30 / 3.6)
                return 30 / 3.6;
            else
                return 50 / 3.6;
        }
        default:
            return -1;
        }
    }
    void UpdateSpeedLimitAndDistance(double SpeedLimit, double Distance)
    {
        if (SpeedLimit != SpeedLimits.front())
        {
            SpeedLimits.push_front(SpeedLimit);
            DistanceToSpeedLimits.push_front(Distance);
        }
        else
        {
            DistanceToSpeedLimits.front()=Distance;
        }
    }
    double GetCurrentSpeedLimit()
    {
        if(SpeedLimits.size()==0)
        {
            SpeedLimits.push_front(INFINITY);
        }
        return SpeedLimits.front();
    }
    double GetCurrentDistanceToSign()
    {
        if(DistanceToSpeedLimits.size()==0)
        {
            DistanceToSpeedLimits.push_front(INFINITY);
        }
        return DistanceToSpeedLimits.front();
    }

private:

    std::list<double> SpeedLimits;
    std::list<double> DistanceToSpeedLimits;
};

struct stopSign
{
    bool exists = false;
    CommonTrafficSign::Entity *Sign;
};

struct assessedEnvironment
{
    StaticEnvironmentData *StaticEnvironment;
    speedLimit SpeedLimit;
    stopSign StopSign;
};

struct InformationAcquisition_Input_BU
{
    StaticEnvironmentData StaticEnvironment;
    std::list<SurroundingMovingObjectsData> SurroundingMovingObjects;
    egoData EgoData;
};

struct InformationAcquisition_Output
{
    egoData* Ego;
    std::list<SurroundingMovingObjectsData> *SurroundingMovingObjects;
    StaticEnvironmentData *EnvironmentInfo;
};

struct MentalModel_Input_BU
{
    InformationAcquisition_Output *IA_O;
};

struct MentalModel_Output
{
    egoData* Ego;
    std::list <std::unique_ptr<AgentRepresentation>> *SurroundingMovingObjects={};
    StaticEnvironmentData *EnvironmentInfo;
};

struct SitationAssessment_Input
{
    MentalModel_Output *MM_O;
};

struct SituationAssessment_Output_TD //Top Down
{

};

struct SituationAssessment_Output_BU //Bottom Up
{
    egoData* Ego;
    std::map<RelationType, SurroundingMovingObjectsData> NearTraffic;
    assessedEnvironment AssessedEnvironment;
};

struct ActionDeduction_Input
{
    SituationAssessment_Output_BU *SA_O_BU;
    MentalModel_Output *MM_O;
};

struct ActionDeduction_Output_TD //Top Down
{

};

struct ActionDeduction_Output_BU //Bottom Up
{
    //! This flag states whether a collision has freshly occurred in the current time step.
    bool notifyCollision;
    //! the current gear of the current vehicle.
    int *gear;
    //! The velocity wish for the next time-step of the current vehicle in m/s.
    double velocity_long_wish;
    //! The acceleration wish for the next time-step of the current vehicle in m/s.
    double acceleration_long_wish;

    //! The lateral velocity of the current vehicle [m/s].
    double out_lateral_speed = 0;
    //! The relative lateral position of the vehicle [m].
    double out_lateral_displacement = 0;
    //! The gain for lateral displacement error controller [-].
    double out_lateral_gain_displacement = 20.0;
    //! The damping constant of the lateral oscillation of the vehicle [].
    double out_lateral_damping = 0;
    //! The lateral oscillation frequency of the vehicle [1/s].
    double out_lateral_frequency = 0;
    //! The heading angle error of the vehicle [rad].
    double out_lateral_heading_error = 0;
    //! The gain for heading error controller [-].
    double out_lateral_gain_heading_error = 7.5;

    double GainHeadingError;
    double out_curvature;

    double *steeringWheelAngle;

    LaneChangeState LCState;
};

struct ActionExecution_Input
{
    ActionDeduction_Output_BU *AD_O_BU;
    MentalModel_Output *MM_O;
};

struct ActionExecution_Output_TD
{

};

struct ActionExecution_Output_BU
{
    int out_gear = {0};
    //! The state of the turning indicator [-].
    int out_indicatorState = static_cast<int>(IndicatorState::IndicatorState_Off);

    double out_accPP;
    double out_brkPP;
    double out_desiredSteeringWheelAngle;
    double out_velocityX;
    double out_YawAngle;
    double out_YawVelocity;
};

struct MentalModel_Input_TD
{
    SituationAssessment_Output_TD *SA_O_TD;
    ActionDeduction_Output_TD *AD_O_TD;
    ActionExecution_Output_TD *AE_O_TD;
};

struct InformationAcquisition_Input_TD
{
    SituationAssessment_Output_TD *SA_O_TD;
    ActionDeduction_Output_TD *AD_O_TD;
    ActionExecution_Output_TD *AE_O_TD;
};


