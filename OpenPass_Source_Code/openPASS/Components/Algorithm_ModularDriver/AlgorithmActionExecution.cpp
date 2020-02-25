/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*               2016, 2017 ITK Engineering GmbH
*               2019 AMFD GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AlgorithmActionExecution.cpp
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief This file contains the calculations for the action-execution
//!
//! This class contains the data calculations for the execution of actions from the previous cognition.
//! Singleton such that data calculations is generated only once and not seperately
//! for each instance.
//!
//-----------------------------------------------------------------------------

#include <map>
#include <iostream>
#include "AlgorithmActionExecution.h"
#include "commonTools.h"


ActionExecution::ActionExecution(StochasticsInterface *stochastics):
      _stochastic(stochastics)
{
    pedalchangetime = (_stochastic->GetLogNormalDistributed(0.29,0.073)*1000);
}

void ActionExecution::SetActionExecution_Input (ActionExecution_Input *Input)
{
    AE_Input = Input;
}

ActionExecution_Output_TD * ActionExecution::GetActionExecution_Output_TD()
{
    return &AE_Output_TD;
}

void ActionExecution::CalculatePedalPosAndGear(ActionExecution_Output_BU *AE_Output_BU)
{
    // get current values from agent
    double *currentVelocity = &AE_Input->MM_O->Ego->GetState_Ego()->velocity_long; // the current velocity of the vehicle
    double *xAcc = &AE_Input->MM_O->Ego->GetState_Ego()->acceleration_long; // the current acceleration of the vehicle
    int *lastGear = &AE_Input->MM_O->Ego->GetState_Ego()->currentGear;
    double accelerationWish = AE_Input->AD_O_BU->acceleration_long_wish;

    //CHECK
    //if (AE_Input->MM_O->Ego->GetState()->id==14)
    //    std::cout << "acc_wish: " << accelerationWish << std::endl;


    // reset output variables
    AE_Output_BU->out_accPP = 0;
    AE_Output_BU->out_brkPP = 0;
    AE_Output_BU->out_gear = *lastGear;

    if (*currentVelocity == 0 && accelerationWish == 0)
    {
            AE_Output_BU->out_accPP = 0;
            AE_Output_BU->out_brkPP = 0;
            AE_Output_BU->out_gear = 0;
    }
    else
    {
        AlgorithmLongitudinalCalculations calculations(*currentVelocity,
                                                       accelerationWish,
                                                       *vehicleParameters);
        calculations.CalculateGearAndEngineSpeed();
        calculations.CalculatePedalPositions();

        AE_Output_BU->out_gear = calculations.GetGear();
        AE_Output_BU->out_accPP = calculations.GetAcceleratorPedalPosition();
        AE_Output_BU->out_brkPP = calculations.GetBrakePedalPosition();
    }
}

void ActionExecution::CalculateSteeringWheelAngle(ActionExecution_Output_BU *AE_Output_BU, int time)
{
    // Time step length
    double dt{(time - timeLast) * 0.001};

    // Scale gains to current velocity. Linear interpolation between 0 and default values at 200km/h.
    double velocityFactor = 3.6 / 200. * AE_Input->MM_O->Ego->GetState_Ego()->velocity_long;
    AE_Input->AD_O_BU->out_lateral_gain_displacement *= velocityFactor;
    AE_Input->AD_O_BU->out_lateral_heading_error *= velocityFactor;

    // Controller for lateral deviation
    double deltaHLateralDeviation = AE_Input->AD_O_BU->out_lateral_gain_displacement
            * vehicleParameters->steeringRatio * vehicleParameters->wheelbase / (AE_Input->MM_O->Ego->GetState_Ego()->velocity_long * AE_Input->MM_O->Ego->GetState_Ego()->velocity_long)
            * AE_Input->AD_O_BU->out_lateral_displacement * RadiantToDegree;

    // Controller for heading angle error
    double deltaHHeadingError = AE_Input->AD_O_BU->out_lateral_gain_heading_error
            * vehicleParameters->steeringRatio * vehicleParameters->wheelbase / AE_Input->MM_O->Ego->GetState_Ego()->velocity_long
            * AE_Input->AD_O_BU->out_lateral_heading_error * RadiantToDegree;

    // Controller for curvature
    double deltaHkappa = std::atan(AE_Input->AD_O_BU->out_curvature * vehicleParameters->wheelbase)
            * vehicleParameters->steeringRatio * RadiantToDegree;

    // Total steering wheel angle
    double deltaH = deltaHLateralDeviation + deltaHHeadingError + deltaHkappa;

    // Limit steering wheel velocity. Human limit set to 160°/s.
    if (std::fabs(deltaH - AE_Input->MM_O->Ego->GetState_Ego()->steeringWheelAngle) > 160. * dt)
    {
        deltaH = deltaH / std::fabs(deltaH) * 160. * dt + AE_Input->MM_O->Ego->GetState_Ego()->steeringWheelAngle;
    }

    // Calculate lateral position of next time step
    double lateralDeviationNext = std::sin(deltaH / vehicleParameters->steeringRatio / RadiantToDegree) * AE_Input->MM_O->Ego->GetState_Ego()->velocity_long * dt + AE_Input->AD_O_BU->out_lateral_displacement;
    if (AE_Input->AD_O_BU->out_lateral_displacement * lateralDeviationNext < 0.)  // negative --> sign changed
    {
        deltaH = std::asin(AE_Input->AD_O_BU->out_lateral_displacement / (AE_Input->MM_O->Ego->GetState_Ego()->velocity_long * dt)) * vehicleParameters->steeringRatio * RadiantToDegree;
    }

    AE_Output_BU->out_desiredSteeringWheelAngle = TrafficHelperFunctions::ValueInBounds(-vehicleParameters->maximumSteeringWheelAngleAmplitude, deltaH, vehicleParameters->maximumSteeringWheelAngleAmplitude);

    timeLast = time;
    }

void ActionExecution::SetIndicatorStateDirection(ActionExecution_Output_BU *AE_Output_BU)
{
    LaneChangeState ChangeState = AE_Input->AD_O_BU->LCState;

    if (ChangeState == LaneChangeState::NoLaneChange)
    {
        AE_Output_BU->out_indicatorState = static_cast<int>(IndicatorState::IndicatorState_Off);
    }
    else if (ChangeState == LaneChangeState::LaneChangeRight)
    {
        AE_Output_BU->out_indicatorState = static_cast<int>(IndicatorState::IndicatorState_Right);
    }
    else if (ChangeState == LaneChangeState::LaneChangeLeft)
    {
        AE_Output_BU->out_indicatorState = static_cast<int>(IndicatorState::IndicatorState_Left);
    }
    else
    {
        AE_Output_BU->out_indicatorState = static_cast<int>(IndicatorState::IndicatorState_Off);
    }

}



