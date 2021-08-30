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
 * @defgroup module_rc Route Control
 * \image html RC_00_Overview.png "RouteControl function overview"
 * This algorithm receives a trajectory in two dimensions (x- and y-values in cartzesian coordinates with corresponding velocities and time steps).
 * Thus, the actual position and velocity of the agent are analyzed and the deviation to the desired values is obtained.
 * In order to correct vehicle's position and velocity, following outputs are generated:
 * - throttle pedal state [0; 1]
 * - brake pedal state [0; 1]
 * - steering angle [-pi/4; pi/4]
 */
/** @addtogroup module_rc
 * Furthermore, different driver types are considered. The driver type is described by an aggressivity parameter (a scaling factor between "0" and "1").
 * Lower aggressivity introduces scaled bounds to the three outputs decribed above.
 * \image html RC_30_Driver.png "RouteControl driver types"
 *
 * Abbreviations:
 * - CS = coordinate system
*/

/**
 * @ingroup module_rc
 * @defgroup init_rc Initialization
*/
/**
 * @ingroup module_rc
 * @defgroup sim_step_00_rc_start Simulation step entry
*/

/**
 * @ingroup module_rc
 * @defgroup sim_step_10_rc_error Calculation of errors
*/

/**
 * @ingroup module_rc
 * @defgroup sim_step_20_rc_steer Calculation of steering control
 * \image html RC_10_Steer.png "Steering control"
*/
/**
 * @ingroup module_rc
 * @defgroup sim_step_30_rc_accel Calculation of acceleration control
 * \image html RC_20_Accel.png "Acceleration control"
*/
/**
 * @ingroup module_rc
 * @defgroup sim_step_40_rc_out Simulation step output
*/

#include "algorithm_RouteControl_implementation.h"

#include <array>
#include <memory>

#include <qglobal.h>

Algorithm_Routecontrol_Implementation::Algorithm_Routecontrol_Implementation(
        std::string componentName,
        bool isInit,
        int priority,
        int offsetTime,
        int responseTime,
        int cycleTime,
        StochasticsInterface *stochastics,
        const ParameterInterface *parameters,
        PublisherInterface *const publisher,
        const CallbackInterface *callbacks,
        AgentInterface *agent) :
    AlgorithmInterface(
        componentName,
        isInit,
        priority,
        offsetTime,
        responseTime,
        cycleTime,
        stochastics,
        parameters,
        publisher,
        callbacks,
        agent)
{
    LOGINFO(QString().sprintf("Constructing %s for agent %d", COMPONENTNAME.c_str(), agent->GetId()).toStdString());

    mTimeStep = (double)cycleTime / 1000.0;

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

    LOGDEBUG(QString().sprintf("Construction of %s successful", COMPONENTNAME.c_str()).toStdString());

}

Algorithm_Routecontrol_Implementation::~Algorithm_Routecontrol_Implementation()
{
    LOGDEBUG(QString().sprintf("Destroying %s", COMPONENTNAME.c_str()).toStdString());

    delete waypoints;
    waypoints = nullptr;

    delete routeControl;
    routeControl = nullptr;

    LOGDEBUG(QString().sprintf("Destruction of %s successful", COMPONENTNAME.c_str()).toStdString());
}

void Algorithm_Routecontrol_Implementation::UpdateInput(int localLinkId,
                                                        const std::shared_ptr<SignalInterface const> &data,
                                                        int time)
{
    Q_UNUSED(time);
    Q_UNUSED(localLinkId);

    LOGDEBUG(QString().sprintf("%s UpdateInput", COMPONENTNAME.c_str()).toStdString());

    const auto trajectorySignal = std::dynamic_pointer_cast<TrajectorySignal const>(data);

    try
    {
        trajectory = trajectorySignal->trajectory;
        LOGDEBUG(QString().sprintf("%s UpdateInput successful", COMPONENTNAME.c_str()).toStdString());
    }
    catch (...)
    {
        LOGERROR(QString().sprintf("%s UpdateInput failed", COMPONENTNAME.c_str()).toStdString());
    }

}

void Algorithm_Routecontrol_Implementation::UpdateOutput(int localLinkId,
                                                         std::shared_ptr<SignalInterface const> &data,
                                                         int time)
{
    Q_UNUSED(time);
    LOGDEBUG(QString().sprintf("%s UpdateOutput", COMPONENTNAME.c_str()).toStdString());

    bool success = outputPorts.at(localLinkId)->GetSignalValue(data);
    if (success)
    {
        LOGDEBUG(QString().sprintf("%s UpdateOutput successful", COMPONENTNAME.c_str()).toStdString());
    }
    else
    {
        LOGERROR(QString().sprintf("%s UpdateOutput failed", COMPONENTNAME.c_str()).toStdString());
    }
}

void Algorithm_Routecontrol_Implementation::Trigger(int time_ms)
{

    /** @addtogroup init_rc
     * Read trajectory to be followed:
     *  - time steps
     *  - x-position in cartesian coordinates
     *  - y-position in cartesian coordinates
     *  - vehicle's longitudinal velocity in vehicle's CS
    */
    if (!routeControl) // only allocate and initialize once
    {
        routeControl = new RouteControl(mTimeStep);
        /** @addtogroup init_rc
         * Define vehicle's and driver's characteristics:
         *  - total vehicle mass
         *  - engine power
         *  - maximum brake torque
         *  - driver aggressivity
        */
        routeControl->SetVehicleProperties(GetAgent()->GetVehicleModelParameters().weight,
                                           maxpower.GetValue(),
                                           mintorque.GetValue(),
                                           drivingAgressivity.GetValue());

        routeControl->SetPIDParameters(pedalsKp.GetValue(), pedalsKi.GetValue(), pedalsKd.GetValue(),
                                       steeringKp.GetValue(), steeringKi.GetValue(), steeringKd.GetValue());
    }

    if (!waypoints) // only allocate once
    {
        unsigned int n = trajectory.points.size();
        waypoints = new std::vector<WaypointData>(n);
        ReadWayPointData(); // for periodic trajectory update: move this line outside the IF statement
    }

    routeControl->SetRequestedTrajectory(*waypoints);

    if (routeControl == nullptr)
    {
        LOGERROR(QString().sprintf("%s Trigger not callable", COMPONENTNAME.c_str()).toStdString());
        return;
    }

    // perform calculations
    double velAbs = std::sqrt(
                std::pow(GetAgent()->GetVelocity(VelocityScope::DirectionX),2) +
                std::pow(GetAgent()->GetVelocity(VelocityScope::DirectionY),2));
    routeControl->Perform(time_ms, GetAgent()->GetPositionX(), GetAgent()->GetPositionY(),
                          GetAgent()->GetYaw(), velAbs);

    LOGDEBUG(QString().sprintf("%s output (agent %d) : FrontWheelAngle = %f, ThrottlePedal = %f, BrakePedal = %f",
                               COMPONENTNAME.c_str(), GetAgent()->GetId(),
                               180/3.14*routeControl->GetFrontWheelAngle(),
                               routeControl->GetThrottlePedal(),
                               routeControl->GetBrakePedal()).toStdString());

    /** @addtogroup sim_step_40_rc_out
     * Write new output signals:
     * - steering angle
     * - throttle pedal state
     * - brake pedal state
     * - brake superposition state
    */
    double brake = routeControl->GetBrakePedal();
    std::array<double, 4> brakeSuperpose{0.0, 0.0, 0.0, 0.0};
    ControlData result{routeControl->GetFrontWheelAngle(), routeControl->GetThrottlePedal(), brake, brakeSuperpose};
    control.SetValue(result);

    LOGDEBUG(QString().sprintf("%s Trigger successful", COMPONENTNAME.c_str()).toStdString());
    return;
}


void Algorithm_Routecontrol_Implementation::ReadWayPointData()
{
    unsigned int n = trajectory.points.size();

    double vel = 0.0;

    for (unsigned int i = 0; i < n; ++i)
    {
        openScenario::TrajectoryPoint point = trajectory.points.at(i);
        waypoints->at(i).time = point.time; // s
        waypoints->at(i).position.x = point.x;
        waypoints->at(i).position.y = point.y;
        if (i<n-1)
        {
            openScenario::TrajectoryPoint pointNext = trajectory.points.at(i+1);
            vel = std::sqrt(std::pow(pointNext.x-point.x,2) + std::pow(pointNext.y-point.y,2)) / mTimeStep; // uniform motion approximation
        }
        waypoints->at(i).longVelocity = vel;
    }
}
