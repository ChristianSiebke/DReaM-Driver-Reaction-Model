/*********************************************************************
* Copyright (c) 2020, 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef ROUTECONTROL_H
#define ROUTECONTROL_H

#include <vector>
#include <iostream>
#include <math.h>

#include <QFile>
#include <QTextStream>

#include "common/vector2d.h"

//! Lightweight structure comprising all information on a way point within a trajectory
struct WaypointData
{
    Common::Vector2d position;         //!< position (x- and y-coordinates)
    double longVelocity;        //!< velocity
    double time;            //!< timestamp

    //! Standard constructor
    WaypointData():
        position(), longVelocity(0), time(0)
    {}

    //! Constructor
    //!
    //! @param[in]     x            x-coordinate
    //! @param[in]     y            y-coordinate
    //! @param[in]     velocity
    //! @param[in]     time
    WaypointData(double x, double y, double velocity, double time):
        position(x, y), longVelocity(velocity), time(time)
    {}
};

//! Lightweight structure comprising all information on positioning a vehicle
struct PositionData
{
    Common::Vector2d position;         //!< 2d vector with x- and y-coordinates
    double angle;           //!< angle
    double longVelocity;        //!< longitudinal velocity

    //! Standard constructor
    PositionData(): position(), angle(0), longVelocity(0) {}

    //! Constructor
    //!
    //! @param[in]     position         2d vector with x- and y-coordinates
    //! @param[in]     angle
    //! @param[in]     velocity
    PositionData(Common::Vector2d position, double angle, double velocity):
        position(position), angle(angle), longVelocity(velocity)
    {}

    //! Constructor
    //!
    //! @param[in]     x                x-coordinate
    //! @param[in]     y                y-coordinate
    //! @param[in]     angle
    //! @param[in]     velocity
    PositionData(double x, double y, double angle, double velocity):
        angle(angle), longVelocity(velocity)
    {
        position.x = x;
        position.y = y;
    }
};

class RouteControl
{

    const double C_MaxThrottlePedal = 1.0;                               //!< constant value to indicate that the throttle pedal is completely pressed
    const double C_MinThrottlePedal = 0.0;                               //!< constant value to indicate that the throttle pedal is completely released

    const double C_MaxBrakePedal = 1.0;                             //!< constant value to indicate that the brake pedal is completely pressed
    const double C_MinBrakePedal = 0.0;                             //!< constant value to indicate that the brake pedal is completely released

    const double C_DrivAggresSensitivityForBrake = 0.01;            //!< constant value to scale the driving aggressivity factor, which is used to limit the brake action, according to the driving aggressivity value
    const double C_DrivAggresSensitivityForSteering = 0.01;         //!< constant value to scale the driving aggressivity factor, which is used to limit the steering action, according to the driving aggressivity value

    const double C_PedalIntegratorMin = -10.0;                       //!< constant parameter for the pedals PID control algorithms, limits the max value used as Integrator part of the PID
    const double C_PedalIntegratorMax = 10.0;                      //!< constant parameter for the pedals PID control algorithms, limits the min value used as Integrator part of the PID
    const double C_BrakeSensitivity = 1.5;                          //!< constant value used to scale the influence of the velocity in the brake sensitivity
    const double C_SteeringWheelSensitivity = 1.0;                  //!< constant value to scale the steering PID control result, which is transfered to be used as steering action

    const double C_MaxSteeringAngle = M_PI_4;                       //!< typical maximal steering constant value, indicates the steering angle limit
    const double C_WheelRadius = 0.3;                               //!< typical wheel radius constant value

    const double C_PedalsKp = -2.0;                                 //!< constant value for the Kp parameter of the pedals PID control
    const double C_PedalsKi = -1.5;                                 //!< constant value for the Ki parameter of the pedals PID control
    const double C_PedalsKd = 0.0;                                  //!< constant value for the Kd parameter of the pedals PID control

    const double C_SteeringKp = -18.0;                              //!< constant value for the Kp parameter of the steering PID control
    const double C_SteeringKi = -0.6;                               //!< constant value for the Ki parameter of the steering PID control
    const double C_SteeringKd = 0.0;                                //!< constant value for the Kd parameter of the steering PID control

    const double C_lookAheadTimeDefault = 270;                      //!< default look ahead time value considered [in ms] to select the next relevant trajectory point used by the steering control
    const double C_MaxDrivingAggressivity = 1.0;                    //!< constant value for the maximum value of DrivingAggressivity Parameter
    const double C_MinDrivingAggressivity = 0.0;                    //!< constant value for the minimum value of DrivingAggressivity Parameter

    const double C_SignPositive = 1.0;                              //!< indicates a sign positive
    const double C_SignNegative = -1.0;                             //!< indicates a sign negative

public:
    //! Constructor
    //!
    //! @param[in]     dT               cycletime used
    RouteControl(double dT = 0.01);

    //! Function that triggers the algorithm logic, selecting the next relevant waypoint and executing the pedals and steering control
    //!
    void Perform(int time, double positionX, double positionY, double yawAngle, double longVelocity);

    //! Function setting the vehicle properties needed for the calculations
    //!
    //! @param[in]     weight                   weight of the vehicle
    //! @param[in]     maxPower                 maximum engine power
    //! @param[in]     minTorque                minimum brake torque
    //! @param[in]     drivingAggressivity      driving aggressivity level
    void SetVehicleProperties(double weight, double maxPower, double minTorque, double drivingAggressivity);

    //! Function setting parameters needed for the PID controllers
    //!
    //! @param[in]     pedalsKp                Kp parameter of the pedals PID control
    //! @param[in]     pedalsKi                Ki parameter of the pedals PID control
    //! @param[in]     pedalsKd                Kd parameter of the pedals PID control
    //! @param[in]     steeringKp              Kp parameter of the steering PID control
    //! @param[in]     steeringKi              Ki parameter of the steering PID control
    //! @param[in]     steeringKd              Kd parameter of the steering PID control
    void SetPIDParameters(double pedalsKp, double pedalsKi, double pedalsKd, double steeringKp, double steeringKi, double steeringKd);

    //! Function returning the required throttle pedal value
    //!
    //! @return                             the required throttle pedal value
    double GetThrottlePedal();

    //! Function returning the required brake pedal value
    //!
    //! @return                             the required brake pedal value
    double GetBrakePedal();

    //! Function returning the required front wheel angle
    //!
    //! @return                             the required front wheel angle
    double GetFrontWheelAngle();

    //! Function setting the requested trajectory (route) to follow
    //!
    //! @param[in]     trajFoll         vector of waypoints of the given trajectory
    void SetRequestedTrajectory(std::vector<WaypointData> &trajFoll);

    //! Function returning the angle to the given goal waypoint
    //!
    //! @param[in]     goalWaypointAngle        goal Waypoint position
    //! @return                                 angle to the given goal waypoint
    double GetAngleToGoalWaypoint(WaypointData& goalWaypointAngle);

private:
    int mTime;                                          //!< time of the current similuation cycle in ms
    double mDT;                                         //!< cycle time

    // Vehicle Properties
    double mMaxEnginePower;                                //!< maximum engine power
    double mMinBrakeTorque;                             //!< minimum brake torque
    double mWeight;                                     //!< weight of the vehicle
    double mWheelRadius;                                //!< wheel radius
    double mDrivingAggressivity;                        //!< driving aggressivity level

    // Requested Trajectory
    std::vector<WaypointData> mRequestedTrajectory;     //!< vector of waypoints of the given trajectory
    int mTrajectorySize;                                //!< number of waypoints that the requested trajectory contains
    double mlookAheadTime;                              //!< time to look ahead, relevant for calculating the next trajectory point to consider in the route (trajectory)

    // Current Position
    PositionData mCurrentPosData;                       //!< the current position data
    int mLastUsedTrajectoryIndex;                       //!< last trajectory waypoint index used as a reference for the calculations

    // Pedals Action
    double mThrottlePedal;                                   //!< the required throttle pedal value
    double mBrakePedal;                                 //!< the required brake pedal value
    double mThrottlePedalPrev;                               //!< the required throttle pedal value from the previous cycle
    double mBrakePedalPrev;                             //!< the required brake pedal value from the previous cycle

    // Pedals PID Controller
    double mPedalsKp;                                   //!< proportional part of the pedals PID controller
    double mPedalsKi;                                   //!< integral part of the pedals PID controller
    double mPedalsKd;                                   //!< derivative part of the pedals PID controller
    double mPedalsIntegralError;                        //!< pedal-integral-error calculated for the pedals controller
    double mPreVelocityError;                              //!< previous forward velocity error used in the pedals PID controller

    // Steering Action
    double mFrontWheelAngle;                            //!< the required steering angle value
    double mSteeringFactorDriverAction;                 //!< scale factor applied to the steering PID control result

    // Steering PID Controller
    double mSteeringKp;                                 //!< proportional part of the steering PID controller
    double mSteeringKi;                                 //!< integral part of the steering PID controller
    double mSteeringKd;                                 //!< derivative part of the steering PID controller
    double mSteeringIntegralError;                      //!< angle-integral-error calculated for the steering controller
    double mPrevAngleError;                             //!< previous angle-error used in the steering PID controller
    double mLimitSteeringAngle;                         //!< contains the value to indicate the limit angle of the steering

    //! Function setting the current position data
    //!
    //! @param[in]     positionX        position on the x axle of the vehicle in global CS
    //! @param[in]     positionY        position on the y axle of the vehicle in global CS
    //! @param[in]     yawAngle         yaw angle of the vehicle in global CS
    //! @param[in]     longVelocity     longitudinal velocity of the vehicle, in car's CS
    void SetCurrentPosition(double positionX, double positionY, double yawAngle, double longVelocity);

    //! Function returning the next relevant goal waypoint of the trajectory to use as a reference
    //!
    //! @param[in]     lookAheadTime            time to look ahead, relevant for calculating the next trajectory point to consider in the route (trajectory)
    //! @param[in]     lastTrajectoryPoint      flag that indicates if the returned waypoint is the last one of the requested trajectory
    //! @param[in]     endOfRoute               flag that indicates if the vehicle is driving away from the last route (trajectory) point, because it was previously reached
    //! @return                                 the next relevant goal waypoint of the trajectory to use as a reference
    WaypointData& GetGoalWaypoint(double lookAheadTime, bool& lastTrajectoryPoint);

    //! Function returning the next relevant goal waypoint of the trajectory to use as a reference for the Velocity control
    //!
    //! @return                                 the next relevant goal waypoint of the trajectory to use as a reference for the Velocity control,
    //!                                         lookAheadTime is here considered as 0.0;
    WaypointData& GetGoalWaypointVelocity();

    //! Function returning the next relevant goal waypoint of the trajectory to use as a reference for the Steering Angle control
    //!
    //! @param[in]     lookAheadTime            time to look ahead, relevant for calculating the next trajectory point to consider in the route (trajectory)
    //! @param[in]     lastTrajectoryPoint      flag that indicates if the returned waypoint is the last one of the requested trajectory
    //! @param[in]     endOfRoute               flag that indicates if the vehicle is driving away from the last route (trajectory) point, because it was previously reached
    //! @return                                 the next relevant goal waypoint of the trajectory to use as a reference for the Steering Angle control
    WaypointData& GetGoalWaypointAngle(double lookAheadTime, bool& lastTrajectoryPoint);

    //! Function executing the PID controller to compensate the forward velocity error
    //!
    //! @param[in]     velFwError               forward velocity error used in the pedals PID controller
    void PedalControl(double velocityError);

    //! Function executing the PID controller to compensate the steering error
    //!
    //! @param[in]     steeringError            angle steering error used in the pedals PID controller
    void SteeringControl(double steeringError);

    //! Function setting the PID controller action, transfered to the pedals of the vehicle
    //!
    //! @param[in]     pidAction                the pedals PID controller result
    void SetPedals(double pidAction);

    //! Function setting the PID controller action, transfered to the steering of the vehicle
    //!
    //! @param[in]     pidAction                the steering PID controller result
    void SetSteering(double pidAction);

    //! Function returning the brake sensitivity, used to scale the brake pedal action
    //!
    //! @return                                 the brake sensitivity, which depends of the velocity
    double GetBrakeSensitivity();

    //! Function calculating the required throttle pedal value
    //!
    void CalculateThrottlePedal();

    //! Function calculating the required brake pedal value
    //!
    void CalculateBrakePedal();

    //! Function calculating the required front wheel angle
    //!
    void CalculateFrontWheelAngle();

    bool lastTrajectoryPoint = false; //!< flag for reaching end of trajectory

};


#endif // ROUTECONTROL_H
