/*******************************************************************************
* Copyright (c) 2019 AMFD GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AlgorithmActionExecution.h
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief This file contains the calculations for the action-execution
//!
//! This class contains the data calculations for the execution of actions from the previous cognition.
//! Singleton such that data calculations is generated only once and not seperately
//! for each instance.
//!
//-----------------------------------------------------------------------------

#pragma once

//#include <vector>
#include <ContainerStructures.h>
#include "algorithm_longitudinalCalculations.h"
#include "Interfaces/stochasticsInterface.h"

//! \brief provide data and calculate driver's action parameters.
//!
//! \ingroup AlgorithmActionExecution

class ActionExecution
{
public:

    ActionExecution(StochasticsInterface *stochastics); // avoid access of constructor of singleton outside class

    void SetActionExecution_Input (ActionExecution_Input *Input);
    ActionExecution_Output_TD * GetActionExecution_Output_TD();

    void CalculatePedalPosAndGear(ActionExecution_Output_BU *AE_Output_BU);
    double GetPedalChangeTime();
    bool IsPedalChangeFinished(double pedalChangeTime, double cycleTime);

    //-----------------------------------------------------------------------------
    /*!
    * \brief Returns the lateral acceleration wish.
    *
    * \details Returns the lateral acceleration wish.
    *
    * @param[in]    DeltaY  Lateral displacement.
    *
    * @return       lateral acceleration wish.
    */
    //-----------------------------------------------------------------------------
    double ALatWish(double DeltaW);

    //-----------------------------------------------------------------------------
    /*!
    * \brief Returns the desired curvature.
    *
    * \details Returns the desired curvature.
    *
    * @param[in]    accLatTarget         Desired acceleration.
    *
    * @return       Desired curvature.
    */
    //-----------------------------------------------------------------------------
    double CurvatureTarget(double accLatTarget);

    void CalculateSteeringWheelAngle(ActionExecution_Output_BU *AE_Output_BU, int time);

    void SetIndicatorStateDirection(ActionExecution_Output_BU *AE_Output_BU);

    void SetVehicleParameters (const VehicleModelParameters *vehicleParameters)
    {
        this->vehicleParameters = vehicleParameters;
    }

private:

    ActionExecution_Input *AE_Input;
    ActionExecution_Output_TD AE_Output_TD;

    // --- module internal variables
    ObservationInterface* _observer = nullptr;

    // Stochastics
    StochasticsInterface* _stochastic;

    // --- module internal variables
    //  --- Inputs
    //! Current lateral deviation regarding trajectory [m].
    double in_lateralDeviation = 0;
    //! Gain for lateral deviation controller [-].
    double in_gainLateralDeviation = 20.0;
    //! Current heading error regarding trajectory [rad].
    double in_headingError = 0;
    //! Gain for heading error controller [-].
    double in_gainHeadingError = 7.5;
    //! Set value for trajectory curvature [1/m].
    double in_kappaSet = 0;
    //! current velocity
    double velocity = 0.0;
    //! current angle of the steering wheel
    double steeringWheelAngle = 0.0;

    //  --- Outputs
    //! The steering wheel angle wish of the driver in degree.
    double out_desiredSteeringWheelAngle{0};
    /** @} @} */

    //  --- Internal Parameters

    //! Helper constant to convert radiant into degree.
    const double RadiantToDegree = 57.295779513082320876798154814105;
    /** @} @} */

    bool isActive{false};

    //! Previous scheduling time (for calculation of cycle time lenght).
    int timeLast{-100};

    double pedalchangetime;

    const VehicleModelParameters *vehicleParameters;

};


