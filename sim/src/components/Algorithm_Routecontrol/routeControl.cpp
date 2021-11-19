/********************************************************************************
 * Copyright (c) 2020-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "routeControl.h"

// Constructor RouteControl
RouteControl::RouteControl(double dT):
    mDT(dT)
{
    // Initialize current Position Data
    mCurrentPosData.position.x = 0.0;
    mCurrentPosData.position.y = 0.0;
    mCurrentPosData.longVelocity = 0.0;
    mCurrentPosData.angle = 0.0;

    // Initialize outputs (throttle, brake and front wheel angle)
    mThrottlePedal = 0.0;
    mThrottlePedalPrev = 0.0;
    mBrakePedal = 0.0;
    mBrakePedalPrev = 0.0;
    mFrontWheelAngle = 0.0;

    // Initialize support variables and paremeters
    mSteeringFactorDriverAction = 0.0;

    mlookAheadTime = C_lookAheadTimeDefault / 1000.0;    // C_lookAheadTimeDefault = 270 ms
    mLastUsedTrajectoryIndex = 0;

    mDrivingAggressivity = C_MaxDrivingAggressivity;

    // ************ Pedals PID Controller Initialization ***************************

    // Initialize pedals PID controller
    mPedalsIntegralError = 0.0;
    mPreVelocityError = 0.0;

    mPedalsKp = C_PedalsKp;   // C_PedalsKp = -2.0
    mPedalsKi = C_PedalsKi;   // C_PedalsKi = -1.5
    mPedalsKd = C_PedalsKd;   // C_PedalsKd = 0.0

    // ************ Steering PID Controller Initialization ***************************

    // Initialize steering PID controller
    mLimitSteeringAngle = C_MaxSteeringAngle;
    mSteeringIntegralError = 0.0;
    mPrevAngleError = 0.0;

    mSteeringKp = C_SteeringKp;  // C_SteeringKp = -18.0
    mSteeringKi = C_SteeringKi;  // C_SteeringKi = -0.6
    mSteeringKd = C_SteeringKd;  // C_SteeringKd = 0.0

}

// Set Vehicle properties and parameters
void RouteControl::SetVehicleProperties(double weight, double maxPower, double minTorque, double drivingAggressivity)
{
    // Set wheel radius, limit steerin angle, weight, max throttle power, min brake torque
    mWheelRadius = C_WheelRadius;               // C_WheelRadius = 0.3m
    mLimitSteeringAngle = C_MaxSteeringAngle;   // C_MaxSteeringAngle = PI/4
    mWeight = weight;
    mMaxEnginePower = maxPower;
    mMinBrakeTorque = minTorque;

    if (drivingAggressivity > C_MaxDrivingAggressivity)
    {
        // driving agressivity parameter maximum value = 1
        mDrivingAggressivity = C_MaxDrivingAggressivity;
    }
    else if (drivingAggressivity < C_MinDrivingAggressivity)
    {
        // driving agressivity parameter minimum value = 0
        mDrivingAggressivity = C_MinDrivingAggressivity;
    }
    else
    {
        mDrivingAggressivity = drivingAggressivity;
    }
}

// set parameters for the PID controllers
void RouteControl::SetPIDParameters(double pedalsKp, double pedalsKi, double pedalsKd, double steeringKp, double steeringKi, double steeringKd)
{
    mPedalsKp = pedalsKp;
    mPedalsKi = pedalsKi;
    mPedalsKd = pedalsKd;
    mSteeringKp = steeringKp;
    mSteeringKi = steeringKi;
    mSteeringKd = steeringKd;
}

// Function to calculate the Throttle pedal value to apply
void RouteControl::CalculateThrottlePedal()
{
    // Limit the calculated throttle pedal with a MinThrottlePedal value and a MaxThrottlePedal value
    double throttlePedalPreAggrFactor = std::clamp(mThrottlePedal, C_MinThrottlePedal, C_MaxThrottlePedal);

    double throttleChangeAllowed = 0.0;
    // Calculate the throttle change from the previous cycle
    double signThrottleChange = C_SignPositive;
    double throttleChange = throttlePedalPreAggrFactor - mThrottlePedalPrev;

    if ((fabs(throttleChange)) > mDrivingAggressivity) // using DrivingAggressivity as the threshold of throttleChange
    {
        if (throttleChange < 0.0)
        {
            signThrottleChange = C_SignNegative;
        }
        // being the absolute value of throttle change bigger than the driving aggressivity parameter,
        // the throttle change allowed is limited to the driving aggressivity value,
        // with the sign negative if the throttle change is negative, otherwise with positive sign
        throttleChangeAllowed = signThrottleChange * mDrivingAggressivity;
    }
    else
    {
        // if the absolute value of throttle change is smaller/equal than the driving aggressivity parameter,
        // use the throttle change as throttle change allowed
        throttleChangeAllowed = throttleChange;
    }

    // the throttle pedal value to apply as the output, will be the previous throttle pedal value added to the throttle change allowed value
    mThrottlePedal = mThrottlePedalPrev + throttleChangeAllowed;

    // the throttle pedal value delivered is remembered for the next calculation cycle
    mThrottlePedalPrev = mThrottlePedal;
}

double RouteControl::GetThrottlePedal()
{
    return mThrottlePedal;
}

// Function to calculate the Brake pedal value to apply
void RouteControl::CalculateBrakePedal()
{
    double brakeDrivingAggressivity = C_MaxDrivingAggressivity;
    if (mDrivingAggressivity < C_MaxDrivingAggressivity)
    {
        // when the driving agressivity parameter is smaller than its maximum value (1.0),
        // the considered driving aggressivity value, used for the brake pedal action is scaled in a 0.01 factor,
        // being set as a new brake driving aggressivity parameter
        brakeDrivingAggressivity = mDrivingAggressivity * C_DrivAggresSensitivityForBrake;
    }

    // the calculated brake pedal value from the PID controller is previously limited with a MinBrakePedal value and a MaxBrakePedal value
    double brakePedalPreAggrFactor = std::clamp(mBrakePedal, C_MinBrakePedal, C_MaxBrakePedal);

    double brakeChangeAllowed = 0.0;
    // Calculate the brake change from the previous cycle
    double signBrakeChange = C_SignPositive;
    double brakeChange = brakePedalPreAggrFactor - mBrakePedalPrev;

    if ((fabs(brakeChange)) > (brakeDrivingAggressivity))
    {
        if (brakeChange < 0.0)
        {
            signBrakeChange = C_SignNegative;
        }
        // when the absolute value of brake change is bigger than the scaled driving aggressivity (brake driving aggressivity) parameter,
        // the brake change allowed is limited to brake driving aggressivity value,
        // with the sign negative if the brake change is negative, otherwise with positive sign
        brakeChangeAllowed = signBrakeChange * brakeDrivingAggressivity;
    }
    else
    {
        // if the absolute value of brake change is smaller/equal than the brake driving aggressivity parameter,
        // use the brake change as brake change allowed
        brakeChangeAllowed = brakeChange;
    }

    // the brake pedal value to apply as the output, will be the previous brake pedal value added to the brake change allowed value
    mBrakePedal = mBrakePedalPrev + brakeChangeAllowed;

    // the brake pedal value delivered is remembered for the next calculation cycle
    mBrakePedalPrev = mBrakePedal;
}

double RouteControl::GetBrakePedal()
{
    return mBrakePedal;
}

// Function to calculate the Front Wheel Angle value to apply
void RouteControl::CalculateFrontWheelAngle()
{
    // The default limit steering angle is PI/4
    mLimitSteeringAngle = C_MaxSteeringAngle;
    if (mDrivingAggressivity < C_MaxDrivingAggressivity)
    {
        // when driving aggressivity parameter is smaller than its maximum value (1.0),
        // limit steering angle is scaled (0.01 * driving aggressivity value) from its possible maximum value (PI/4)
        mLimitSteeringAngle = mDrivingAggressivity * C_MaxSteeringAngle * C_DrivAggresSensitivityForSteering;
    }

    // the calculated front wheel angle value from the PID controller is limited with a the limit steering angle in both sides (left and right)
    mFrontWheelAngle = std::clamp(mFrontWheelAngle, -mLimitSteeringAngle, mLimitSteeringAngle);
}

double RouteControl::GetFrontWheelAngle()
{
    return mFrontWheelAngle;
}

// Set the requested trajectory (route) to follow
void RouteControl::SetRequestedTrajectory(std::vector<WaypointData> &trajFoll)
{
    // Copy the received array of waypoints and extracts the size of it
    mRequestedTrajectory = trajFoll;
    mTrajectorySize = mRequestedTrajectory.size();
}

// Set the current position of the vehicle
void RouteControl::SetCurrentPosition(double positionX, double positionY, double yawAngle, double longVelocity)
{
    // Sets the current x-position, y-position, longitudinal velocity and the yaw angle of the vehicle
    mCurrentPosData.position.x = positionX;
    mCurrentPosData.position.y = positionY;
    mCurrentPosData.longVelocity = longVelocity;
    mCurrentPosData.angle = yawAngle;
}

// Function returning the next relevant goal waypoint of the trajectory to use as a reference
WaypointData& RouteControl::GetGoalWaypoint(double lookAheadTime, bool& isLastTrajectoryPoint)
{
    double distance;
    double minDistance = INFINITY;
    int nextPointIndex = -1;

    // Get the index of the next point to the nearest point to the current position
    // (no matter if the point time matches with the current time)
    for (int i = 0; i < mTrajectorySize; i++)
    {
        // Calculate the distance between the current position and a waypoint in trajectory
        Common::Vector2d distanceVec(mRequestedTrajectory[i].position - mCurrentPosData.position);
        distance = distanceVec.Length();

        if (distance < minDistance)
        {
            // update the minDistance
            minDistance = distance;
            nextPointIndex = i;

            if (i < (mTrajectorySize-1)) // check if the waypoint is the last one in trajectory
            {
                nextPointIndex = i + 1;
            }
            else
            {
                // in case it is the last trajectory point, take it as the next point
                nextPointIndex = i;
                isLastTrajectoryPoint = true;
            }
        }
    }

    if (lookAheadTime > 0) // check if lookAheadTime is meaningful (bigger than 0)
    {
        // starting from the trajectory waypoint with minimum distance until the current position
        for (int i = nextPointIndex; i < mTrajectorySize; i++)
        {
            if ((mRequestedTrajectory[i].time - (double)mTime / 1000.0) >= lookAheadTime)
            {
                // when the time difference between an analyzed trajectory waypoint and the waypoint with the minimum distance (until the current position of the vehicle)
                // is bigger or equal than the look ahead time, returns that analyzed waypoint as the next waypoint to use
                return mRequestedTrajectory[i];
            }
        }
        // if no waypoint was returned yet, the last trajectory point will be returned as the next waypoint to use
        return mRequestedTrajectory[mTrajectorySize-1];
        // Note : When the current position is further than the last trajectory point
        // or 'lookAheadTime' is further than the time to last trajectory point,
        // the last point is returned
    }
    else // lookAheadTime is meaningless (smaller or equal than 0)
    {
        // use the next waypoint with minimum distance as the goal waypoint point
        return mRequestedTrajectory[nextPointIndex];
    }
}

// Function returning the angle from the current position to the given goal waypoint
double RouteControl::GetAngleToGoalWaypoint(WaypointData& goalWaypointAngle)
{
    Common::Vector2d distanceToGoalPoint(goalWaypointAngle.position - mCurrentPosData.position);
    return distanceToGoalPoint.Angle();
}

// Function returning the next relevant goal waypoint of the trajectory to use as a reference for the Velocity control
WaypointData& RouteControl::GetGoalWaypointVelocity()
{
    bool isLastTrajectoryPoint;
    Q_UNUSED(isLastTrajectoryPoint);
    // lastTrajectoryPoint flag is true, when the returned goal waypoint is the last one
    return GetGoalWaypoint(0.0, isLastTrajectoryPoint);   // lookAheadTime trasfered here is equal 0
}

// Function returning the next relevant goal waypoint of the trajectory to use as a reference for the Steering Angle control
WaypointData& RouteControl::GetGoalWaypointAngle(double lookAheadTime, bool& lastTrajectoryPoint)
{
    // lastTrajectoryPoint flag is true, when the returned goal waypoint is the last one
    // endOfRoute flag is true, when lastTrajectoryPoint was previously set to true and the vehicle keeps on driving far from last trajectory point
    return GetGoalWaypoint(lookAheadTime, lastTrajectoryPoint);
}

void RouteControl::Perform(int time, double positionX, double positionY, double yawAngle, double longVelocity)
{
    /** @addtogroup sim_step_00_rc_start
     * Read and update previous vehicle's state:
     *  - global position (cartesian coordinates)
     *  - direction of vehicle's longitudinal axes (angle in polar coordinates)
     *  - vehicle's longitudinal velocity in vehicle's CS
    */
    SetCurrentPosition(positionX, positionY, yawAngle, longVelocity);
    mTime = time;

    /** @addtogroup sim_step_10_rc_error
     * Obtain reference indice in the trajectory data for:
     * - velocity goal
     * - cartesian coordinates
    */
    // get the goal point to reach considering velocity control
    WaypointData goalWaypointVelocity = GetGoalWaypointVelocity();

    // get the goal point to reach considering angle control
    WaypointData goalWaypointAngle = GetGoalWaypointAngle(mlookAheadTime, lastTrajectoryPoint);

    double steeringError = 0.0;
    if (lastTrajectoryPoint)
    {
        // by last trajectory point as goal, the angle 0 should be the one considered
        SetSteering(0.0);
    }
    else
    {
        /** @addtogroup sim_step_10_rc_error
         * Obtain the pointing deviation between the extrapolated vehicle translation and the trajectory point to be reached.
        */
        double angleToGoalWaypoint = GetAngleToGoalWaypoint(goalWaypointAngle);

        // compare current steering angle with the one it should have
        double angleDelta = mCurrentPosData.angle - angleToGoalWaypoint;
        if (angleDelta > M_PI)  // make sure the angle is in the proper range (-PI, PI)
        {
            angleDelta -= (2 * M_PI);
        }
        else if (angleDelta < -M_PI)
        {
            angleDelta += (2 * M_PI);
        }

        /** @addtogroup sim_step_10_rc_error
         * Scale steering control according to absolute velocity.
        */
        steeringError = angleDelta/mCurrentPosData.longVelocity;

        // calculate the steering and velocity control
        SteeringControl(steeringError);
    }

    /** @addtogroup sim_step_10_rc_error
     * Obtain the deviation between the actual vehicle velocity and the desired velocity value.
    */
    double velocityError = mCurrentPosData.longVelocity - goalWaypointVelocity.longVelocity;
    PedalControl( velocityError);

    CalculateFrontWheelAngle();
    CalculateThrottlePedal();
    CalculateBrakePedal();

}

// Function SteeringControl executes the PID controller to compensate the steering error
void RouteControl::SteeringControl(double steeringError)
{
    double steeringPIDControllerAction = 0.0;

    double steeringProportionalTerm = 0.0;
    double steeringIntegralTerm = 0.0;
    double steeringDerivativeTerm = 0.0;

    /** @addtogroup sim_step_20_rc_steer
     * Calculate the coefficient of the steering PID controller:
     * - proportional
     * - integral
     * - derivative
    */
    // calculate the Proportional part of the PID controller
    steeringProportionalTerm = mSteeringKp * steeringError;

    // calculate the Integral part of the PID controller
    mSteeringIntegralError += steeringError;
    steeringIntegralTerm = mSteeringKi * mSteeringIntegralError * mDT;

    // calculate the Derivative part of the PID controller
    double steeringDerivativeError = steeringError - mPrevAngleError;
    if (mDT > 0.0)    // avoids division by 0 if dT equals 0
    {
        steeringDerivativeTerm = mSteeringKd * (steeringDerivativeError / mDT);
    }

    /** @addtogroup sim_step_20_rc_steer
     * Apply steering PID values.
    */
    // Calculate the driver steering reaction (P+I+D)
    steeringPIDControllerAction = steeringProportionalTerm + steeringIntegralTerm + steeringDerivativeTerm;
    // set the steering action to be scaled
    SetSteering(steeringPIDControllerAction);

    // remember the angle error for the next cycle
    mPrevAngleError = steeringError;
}

// Function PedalControl executes the PID controller to compensate the forward velocity error
void RouteControl::PedalControl(double velocityError)
{
    double pedalPIDControllerAction;

    double pedalProportionalTerm = 0.0;
    double pedalIntegralTerm = 0.0;
    double pedalDerivativeTerm = 0.0;

    /** @addtogroup sim_step_30_rc_accel
     * Calculate the coefficient of the acceleration PID controller:
     * - proportional
     * - integral
     * - derivative
    */
    // calculate the Proportional part of the PID controller
    pedalProportionalTerm = mPedalsKp * velocityError;

    // calculate the Integral part of the PID controller
    mPedalsIntegralError += velocityError;
    pedalIntegralTerm = mPedalsKi * mPedalsIntegralError * mDT;
    // the integral term is limited, to avoid that it becomes extremly high with the time
    if (pedalIntegralTerm > C_PedalIntegratorMax)
    {
        pedalIntegralTerm = C_PedalIntegratorMax;
    }
    else if (pedalIntegralTerm < C_PedalIntegratorMin)
    {
        pedalIntegralTerm = C_PedalIntegratorMin;
    }

    // calculate the Derivative part of the PID controller
    double pedalsDerivativeError = velocityError - mPreVelocityError;
    if (mDT > 0.0)    // when the dT is equal 0, avoids division by 0
    {
        pedalDerivativeTerm = mPedalsKd * (pedalsDerivativeError / mDT);
    }

    /** @addtogroup sim_step_30_rc_accel
     * Apply acceleration PID values.
    */
    // Calculate the driver pedal reaction (P+I+D)
    pedalPIDControllerAction = pedalProportionalTerm + pedalIntegralTerm + pedalDerivativeTerm;
    // set the pedal action to be scaled
    SetPedals(pedalPIDControllerAction);

    // remember the velocity error for the next cycle
    mPreVelocityError = velocityError;
}

// Function SetSteering sets the PID controller action, transfered to the steering of the vehicle
void RouteControl::SetSteering(double pidAction)
{
    mSteeringFactorDriverAction = C_SteeringWheelSensitivity;
    mFrontWheelAngle = pidAction * mSteeringFactorDriverAction;
}

// Function SetPedals sets the PID controller action, transfered to the pedals of the vehicle
void RouteControl::SetPedals(double pidAction)
{
    if (pidAction > 0.0) // when driving too slow, driver compensates with acceleration
    {
        if (mCurrentPosData.longVelocity <= 0.0)
        {
            // if the velocity is 0 or negative, vehicle must press the full throttle pedal
            mThrottlePedal = C_MaxThrottlePedal;
        }
        else
        {
            // otherwise the throttle pedal action from the pid controller is scaled by a formula including the parameters (weight, velocity, maxPower) of the vehicle
            double throttleFactorDriverAction = (mWeight * mCurrentPosData.longVelocity) / (mMaxEnginePower * mDT);   // (velocity * 0.02 / dT)
            mThrottlePedal = pidAction * throttleFactorDriverAction;
        }
        // brakePedal is not pressed at all
        mBrakePedal = C_MinBrakePedal;
    }
    else if (pidAction < 0.0)    // when driving too fast, driver compensates with deceleration
    {
        // the brake pedal action from the pid controller is scaled by a formula including the parameters (weight, wheel radius, brakeTorque) of the vehicle
        double brakeFactorDriverAction = (mWheelRadius * mWeight) / (mDT * mMinBrakeTorque); // (-0.06 / dT)
        mBrakePedal = pidAction * brakeFactorDriverAction * GetBrakeSensitivity();
        // Throttle pedal is not pressed at all
        mThrottlePedal = C_MinThrottlePedal;
    }
    else    // when driving at right speed no pedal is pressed
    {
        mThrottlePedal = C_MinThrottlePedal;
        mBrakePedal = C_MinBrakePedal;
    }
}

// Function GetBrakeSensitivity returns the brake sensitivity, used to scale the brake pedal action
double RouteControl::GetBrakeSensitivity()
{
    // by velocity equals 0, the brake sensitivity is 0
    if (mCurrentPosData.longVelocity == 0.0)
    {
        return 0.0;
    }
    else
    {
        // the higher the velocity, the smaller will be the brake sensitivity
        return (1 / mCurrentPosData.longVelocity * C_BrakeSensitivity);
    }
}
