/*********************************************************************
* Copyright (c) 2020, 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

/**
 * @defgroup module_tt Two-track vehicle model
 * The dynamics of the vehicle is modelled in 2 dimensions. In the sub items the principle function is described in order of execution.
 */
/** @addtogroup module_tt
 * Abbreviations:
 * - COG = center-of-gravity
 * - CS = coordinate system
*/

/**
 * @ingroup module_tt
 * @defgroup init_tt Initialization
*/
/**
 * @ingroup module_tt
 * @defgroup sim_step_00_tt Entry
*/
/**
 * @ingroup module_tt
 * @defgroup sim_step_10_tt Process
*/
/**
 * @ingroup module_tt
 * @defgroup sim_step_20_tt Output
*/

#include "dynamics_twotrack_implementation.h"

#include <memory>

#include <QString>
#include <qglobal.h>

#include "common/commonTools.h"
#include "components/common/vehicleProperties.h"

Dynamics_TwoTrack_Implementation::Dynamics_TwoTrack_Implementation(
        std::string componentName,
        bool isInit,
        int priority,
        int offsetTime,
        int responseTime,
        int cycleTime,
        StochasticsInterface *stochastics,
        WorldInterface *world,
        const ParameterInterface *parameters,
        PublisherInterface *const publisher,
        const CallbackInterface *callbacks,
        AgentInterface *agent) :
    UnrestrictedModelInterface(
        componentName,
        isInit,
        priority,
        offsetTime,
        responseTime,
        cycleTime,
        stochastics,
        world,
        parameters,
        publisher,
        callbacks,
        agent)
{

    LOGINFO(QString().sprintf("Constructing Dynamics_TwoTrack for agent %d...", agent->GetId()).toStdString());

    timeStep = (double)GetCycleTime() / 1000.0;

    try
    {
        auto parameterMapDoubleExternal = parameters->GetParametersDouble();
        foreach (auto &iterator, parameterMapDouble)
        {
            std::string id = iterator.first;
            parameterMapDouble.at(id)->SetValue(parameterMapDoubleExternal.at(id));
        }
    }
    catch (...)
    {
        const std::string msg = COMPONENTNAME + " could not init parameters";
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }

    torqueBrakeMin.SetValue(-std::fabs(torqueBrakeMin.GetValue()));

    yawVelocity = 0.0;
    yawAcceleration = 0.0;
    vehicle = new VehicleSimpleTT();

    auto weight = helper::map::query(GetAgent()->GetVehicleModelParameters().properties, vehicle::properties::Mass);
    THROWIFFALSE(weight.has_value(), "Mass was not defined in VehicleCatalog");

    /** @addtogroup init_tt
     * Define vehicle's body and engine characteristics:
     *  - total mass
     *  - power
     *  - maximum brake torque
    */
    vehicle->InitSetEngine(weight.value(), powerEngineMax.GetValue(), torqueBrakeMin.GetValue());

    /** @addtogroup init_tt
     * Define vehicle's geometry:
     *  - set the height of the COG
     *  - set the longitudinal position of the COG
     *  - set the wheelbase
     *  - set the track width
    */
    vehicle->InitSetGeometry(agent->GetVehicleModelParameters().frontAxle.positionX - agent->GetVehicleModelParameters().rearAxle.positionX,
                             0.0,//agent->GetVehicleModelParameters().wheelbase / 2.0 - agent->GetDistanceCOGtoFrontAxle(),
                             agent->GetVehicleModelParameters().frontAxle.trackWidth,
                             0.0);

    auto frictionCoeff = helper::map::query(GetAgent()->GetVehicleModelParameters().properties, vehicle::properties::FrictionCoefficient);
    THROWIFFALSE(frictionCoeff.has_value(), "FrictionCoefficient was not defined in VehicleCatalog");

    /** @addtogroup init_tt
     * Define vehicle's tire properties:
     *  - set initial velocity
     *  - set peak tire force
     *  - set force at full slide
     *  - set the slip corresponding to peak tire force
     *  - set the radius of the tire
     *  - set the road/tire friction coefficient
    */
    vehicle->InitSetTire(agent->GetVelocity(VelocityScope::Absolute),
                         muTireMax.GetValue(), muTireSlide.GetValue(),
                         slipTireMax.GetValue(), radiusTire.GetValue(), frictionCoeff.value());

    ControlData defaultControl = {0.0, 1.0, 0.0, {0.0, 0.0, 0.0, 0.0}};
    if (!control.SetDefaultValue(defaultControl))
    {
        std::stringstream log;
        log << COMPONENTNAME << " Default value set for control signal failed";
        LOG(CbkLogLevel::Error, log.str());
    }

    std::vector<double> defaultForceVertical = {
        vehicle->forceTireVerticalStatic[0],
        vehicle->forceTireVerticalStatic[1],
        vehicle->forceTireVerticalStatic[2],
        vehicle->forceTireVerticalStatic[3]};
    if (!forceWheelVertical.SetDefaultValue(defaultForceVertical))
    {
        std::stringstream log;
        log << COMPONENTNAME << " Default vertical force value set failed";
        LOG(CbkLogLevel::Error, log.str());
    }

    control.SetDefaultValue(defaultControl);

    LOGINFO("Constructing Dynamics_TwoTrack successful");

}

Dynamics_TwoTrack_Implementation::~Dynamics_TwoTrack_Implementation()
{
    delete (vehicle);
}

void Dynamics_TwoTrack_Implementation::UpdateInput(int localLinkId,
                                                   const std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    std::stringstream log;
    log << COMPONENTNAME << " UpdateInput";
    LOG(CbkLogLevel::Debug, log.str());
    log.str(std::string());

    bool success = inputPorts.at(localLinkId)->SetSignalValue(data);

    if (success) // no need for the brake superpose signal
    {
        log << COMPONENTNAME << " UpdateInput successful";
        LOG(CbkLogLevel::Debug, log.str());
    }
    else
    {
        log << COMPONENTNAME << " UpdateInput failed";
        LOG(CbkLogLevel::Error, log.str());
    }
}

void Dynamics_TwoTrack_Implementation::UpdateOutput(int localLinkId,
                                                    std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    std::stringstream log;
    log << COMPONENTNAME << " UpdateOutput";
    LOG(CbkLogLevel::Debug, log.str());
    log.str(std::string());

    bool success = outputPorts.at(localLinkId)->GetSignalValue(data);

    if (success)
    {
        log << COMPONENTNAME << " UpdateOutput successful";
        LOG(CbkLogLevel::Debug, log.str());
    }
    else
    {
        log << COMPONENTNAME << " UpdateOutput failed";
        LOG(CbkLogLevel::Error, log.str());
    }
}

void Dynamics_TwoTrack_Implementation::Trigger(int time)
{
    timeClock = (double)time;


    /** @addtogroup sim_step_00_tt
     * Read and update previous vehicle's state:
     *  - global position (cartesian coordinates)
     *  - direction of vehicle's longitudinal axes (angle in polar coordinates)
     *  - vehicle's longitudinal and lateral velocity in vehicle's CS
     *  - vehicle's rotational velociy
     *  - vehicle's longitudinal and lateral acceleration in vehicle's CS
     *  - vehicle's rotational acceleration
    */
    ReadPreviousState();
    vehicle->SetVelocity(velocityCar, yawVelocity);

    /** @addtogroup sim_step_10_tt
     * Apply acceleration/deceleration intentions:
     *  - calculate tire torques due to engine
     *  - calculate tire torques due to braking by
     *      - driver (pedal state)
     *      - assistant systems (selective single wheel braking)
    */
    vehicle->DriveTrain(std::clamp(control.GetValue().throttle, 0.0, 1.0),
                        std::clamp(control.GetValue().brakePedal, 0.0, 1.0),
                        control.GetValue().brakeSuperpose);
    LOGDEBUG(QString().sprintf("Driver Input for Dynamics_TwoTrack for agent %d: %f, %f, %f",
                               GetAgent()->GetId(), 180/M_PI*control.GetValue().steer, control.GetValue().throttle, control.GetValue().brakePedal).toStdString());

    /** @addtogroup sim_step_10_tt
     * Apply tire forces at the tire/road interface:
     *  - calculate longitudinal tire slips and forces
     *  - calculate lateral tire slips and forces
     *  - calculate friction forces
    */
    vehicle->ForceLocal(timeStep, control.GetValue().steer, forceWheelVertical.GetValue());
    LOGDEBUG(QString().sprintf("Vertical Force for Dynamics_TwoTrack for agent %d: %f, %f, %f, %f",
                               GetAgent()->GetId(),
                               forceWheelVertical.GetValue().at(0),
                               forceWheelVertical.GetValue().at(1),
                               forceWheelVertical.GetValue().at(2),
                               forceWheelVertical.GetValue().at(3)).toStdString());


    /** @addtogroup sim_step_10_tt
     * Combine local tire forces to a global force at the vehicle's body:
     *  - total longitudinal force
     *  - total lateral force
     *  - air drag
     *  - total rotational momentum
    */
    vehicle->ForceGlobal();

    /** @addtogroup sim_step_20_tt
     * Perform a translational Euler step:
     *  - calculate next vehicle's position from prevoius velocity values
     *  - calculate new velocity from previous acceleration values
     *  - calculate new acceleration from global forces
    */
    NextStateTranslation();

    /** @addtogroup sim_step_20_tt
     * Perform a rotational Euler step:
     *  - calculate vehicle's orientation from previous rotational velocity
     *  - calculate vehicle's rotational velocity from previous rotational acceleration
     *  - calculate vehicle's rotational acceleration from the total rotational momentum
    */
    NextStateRotation();

    /** @addtogroup sim_step_20_tt
     * Write actual vehicle's state:
     *  - global position (cartesian coordinates)
     *  - direction of vehicle's longitudinal axes (angle in polar coordinates)
     *  - vehicle's longitudinal and lateral velocity in vehicle's CS
     *  - vehicle's rotational velociy
     *  - vehicle's longitudinal and lateral acceleration in vehicle's CS
     *  - vehicle's rotational acceleration
     *  - inertia forces on vehicle's COG
    */
    NextStateSet();

}

void Dynamics_TwoTrack_Implementation::ReadPreviousState()
{

    positionCar.x = GetAgent()->GetPositionX(); // position of vehicle in global CS (reference point irrelevant)
    positionCar.y = GetAgent()->GetPositionY(); // position of vehicle in global CS (reference point irrelevant)
    yawAngle = GetAgent()->GetYaw(); // global CS

    velocityCar.x = GetAgent()->GetVelocity(VelocityScope::DirectionX); // global
    velocityCar.y = GetAgent()->GetVelocity(VelocityScope::DirectionY); // global
    velocityCar.Rotate(-yawAngle); // vehicle CS
    yawVelocity = GetAgent()->GetYawRate();

    accelerationCar.x = GetAgent()->GetTangentialAcceleration(); // vehicle CS
    accelerationCar.y = GetAgent()->GetCentripetalAcceleration(); // vehicle CS
    yawAcceleration = GetAgent()->GetYawAcceleration();
    LOGDEBUG(QString().sprintf("Prev Velocity for Dynamics_TwoTrack for agent %d: %f, %f, %f",
                              GetAgent()->GetId(), velocityCar.x, velocityCar.y, yawVelocity).toStdString());
    LOGDEBUG(QString().sprintf("Prev Acceleration for Dynamics_TwoTrack for agent %d: %f, %f, %f",
                               GetAgent()->GetId(), accelerationCar.x, accelerationCar.y, yawAcceleration).toStdString());

}

void Dynamics_TwoTrack_Implementation::NextStateTranslation()
{
    // update position (constant velocity step)
    velocityCar.Rotate(yawAngle); // global CS
    positionCar = positionCar + velocityCar * timeStep;

    velocityCar.Rotate(-yawAngle); // vehicle CS

    // update velocity
    Vector2d velocityCarNew = velocityCar + accelerationCar * timeStep;

    // update acceleration
    auto weight = helper::map::query(GetAgent()->GetVehicleModelParameters().properties, vehicle::properties::Mass);
    THROWIFFALSE(weight.has_value(), "Mass was not defined in VehicleCatalog");

    accelerationCar = vehicle->forceTotalXY * (1 / weight.value());

    // correct velocity and acceleration for zero-crossing
    if (velocityCarNew.Dot(velocityCar) < 0.0)
    {
        velocityCar.Scale(0.0);
        accelerationCar.Scale(0.0);
        LOGDEBUG(QString().sprintf("Zero crossing in velocity for agent %d!",
                                   GetAgent()->GetId()).toStdString());
    }
    else
    {
        velocityCar = velocityCarNew;
    }

}

void Dynamics_TwoTrack_Implementation::NextStateRotation()
{
    // preserve directions of velocity and acceleration
    velocityCar.Rotate(yawAngle);
    accelerationCar.Rotate(yawAngle);

    // update yaw angle
    yawAngle = std::fmod(yawAngle + timeStep * yawVelocity, 2*M_PI);

    // update yaw velocity
    double yawVelocityNew = yawVelocity + yawAcceleration * timeStep;

    // update acceleration
    auto momentInertiaYaw = helper::map::query(GetAgent()->GetVehicleModelParameters().properties, "MomentInertiaYaw");
    THROWIFFALSE(momentInertiaYaw.has_value(), "MomentInertiaYaw was not defined in VehicleCatalog");
    THROWIFFALSE(momentInertiaYaw != 0.0, "MomentInertiaYaw was defined as 0.0 in VehicleCatalog");

    yawAcceleration = vehicle->momentTotalZ / momentInertiaYaw.value();

    // correct velocity and acceleration for zero-crossing
    if (yawVelocityNew * yawVelocity < 0.0)
    {
        yawVelocity = 0.0;
        yawAcceleration = 0.0;
        LOGDEBUG(QString().sprintf("Zero crossing in w for agent %d!",
                                   GetAgent()->GetId()).toStdString());
    }
    else
    {
        yawVelocity = yawVelocityNew;
    }

    // reassign directions of velocity and acceleration
    velocityCar.Rotate(-yawAngle);
    accelerationCar.Rotate(-yawAngle);

}

void Dynamics_TwoTrack_Implementation::NextStateSet()
{
    // update position (constant acceleration step)
    GetAgent()->SetPositionX(positionCar.x);
    GetAgent()->SetPositionY(positionCar.y);

    GetAgent()->SetYaw(yawAngle);

    // update velocity
    velocityCar.Rotate(yawAngle); // global CS
    GetAgent()->SetVelocityVector(velocityCar.x, velocityCar.y, 0.0);
    GetAgent()->SetYawRate(yawVelocity);
    velocityCar.Rotate(-yawAngle); // vehicle CS

    // update forces
    // transfer to trajectory and/or world CS ??? -> NOPE !!!
    GetAgent()->SetTangentialAcceleration(accelerationCar.x);
    GetAgent()->SetCentripetalAcceleration(accelerationCar.y);
    GetAgent()->SetYawAcceleration(yawAcceleration);

    // for logging purposes: only longitudinal acceleration in vehicle CS
    GetAgent()->SetAcceleration(accelerationCar.x);

    // update outputs
    std::vector<double> forceInert = {-vehicle->forceTotalXY.x, -vehicle->forceTotalXY.y};
    forceGlobalInertia.SetValue(forceInert);

    LOGDEBUG(QString().sprintf("Setting Acceleration by Dynamics_TwoTrack for agent %d: %f, %f, %f",
                              GetAgent()->GetId(), accelerationCar.x, accelerationCar.y, yawAcceleration).toStdString());
}

