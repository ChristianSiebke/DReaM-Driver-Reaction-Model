/********************************************************************************
 * Copyright (c) 2018-2019 AMFD GmbH
 *               2016-2017 ITK Engineering GmbH
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include "common/lateralSignal.h"

/*!
* \brief
*
*
* \ingroup Algorithm_LateralDriver
*/
class SteeringController
{
public:
    /*!
    * \brief Calculates the steering angle for a given timestep.
    *
    * @param[in]     time           Current scheduling time
    *
    * return    steering angle
    */
    double CalculateSteeringAngle(int time);

    /*!
     * \brief Calculates the steering angle for a given timestep (especially for tight curves/ urban area)
     *
     * @param[in]     time           Current scheduling time
     *
     * return    steering angle
     */
    double CalculateSteeringAngleTUDresden(int time);

    /*!
    * \brief Sets the lateral input, which contains the desired lateral position.
    *
    * @param[in]     lateralSignal           LateralSignal
    */
    void SetLateralInput(const LateralSignal &lateralSignal)
    {
        in_lateralSignal = lateralSignal;
    }

    /*!
    * \brief Sets the vehicle parameters. This should ideally only be called once.
    *
    * @param[in]     steeringRatio                              steeringRatio
    * @param[in]     maximumSteeringWheelAngleAmplitude         maximumSteeringWheelAngleAmplitude
    * @param[in]     wheelbase                                  wheelbase
    */
    void SetVehicleParameter(const double &steeringRatio,
                             const double &maximumSteeringWheelAngleAmplitude,
                             const double &wheelbase)
    {
        in_steeringRatio = steeringRatio;
        in_steeringMax = maximumSteeringWheelAngleAmplitude;
        in_wheelBase = wheelbase;
    }

    /*!
    * \brief Sets the current velocity and the current steeringwheel angle of the vehicle.
    *
    * @param[in]     velocity                              Current velocity
    * @param[in]     steeringWheelAngle         Current steeringwheel angle
    */
    void SetVelocityAndSteeringWheelAngle(const double &velocity,
                                          const double &steeringWheelAngle)
    {
        in_velocity = velocity;
        in_steeringWheelAngle = steeringWheelAngle;
    }

protected:
    // --- module internal functions

    // --- module internal variables
    //  --- Inputs
    LateralSignal in_lateralSignal {};

    //! current velocity
    double in_velocity = 0.0;
    //! current angle of the steering wheel
    double in_steeringWheelAngle = 0.0;

    //! The steering ratio of the vehicle.
    double in_steeringRatio = 10.7;
    //! The maximum steering wheel angle of the car in both directions in degree.
    double in_steeringMax = 180.0;
    //! The wheelbase of the car in m.
    double in_wheelBase = 2.89;

    //  --- Internal Parameters

    //! Time to Average regulation over
    double tAverage {0.};
    /** @} @} */

    //! Previous scheduling time (for calculation of cycle time lenght).
    int timeLast {-100};
    //! running average of  mean curvature up to NearPoint
    double meanCurvatureToNearPointSmoothLast {0.};
    //! running average of  mean curvature from NearPoint up to FarPoint
    double meanCurvatureToFarPointSmoothLast {0.};
    //! running average of kappaRoad at referencepoint
    double curvatureRoadSmoothLast {0.};
};
