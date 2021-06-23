/*********************************************************************
* Copyright (c) 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

/**
 * @defgroup module_3dc 3D Chassis
 * The dynamics component for simulating suspension and deformation of a vehicle due to inertia forces.
 * \image html 3DT_Pitch.png "Force Analysis in Pitch with Deformation"
 * \image html 3DT_Roll.png "Force Analysis in Roll with Deformation"
 */

/**
 * @ingroup module_3dc
 * @defgroup init_3dc Initialization
*/
/**
 * @ingroup module_3dc
 * @defgroup sim_step_3dc Entry
*/
/**
 * @ingroup module_3dc
 * @defgroup sim_step_3dc_2 Process
*/
/**
 * @ingroup module_3dc
 * @defgroup sim_step_3 Output
*/

#include "dynamics_Chassis_implementation.h"
#include <QString>

Dynamics_Chassis_Implementation::Dynamics_Chassis_Implementation(std::string componentName,
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
    DynamicsInterface(componentName,
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

    LOGINFO(QString().sprintf("Constructing Dynamics_Chassis for agent %d...", agent->GetId()).toStdString());

    /** @addtogroup init_3dc
     * Get predefined parameters from the component's XML file.
    */
    const std::map<std::string, double> parameterMapDoubleExternal =
        GetParameters()->GetParametersDouble();
    foreach (auto &iterator, parameterMapDouble)
    {
        std::string id = iterator.first;
        parameterMapDouble.at(id)->SetValue(parameterMapDoubleExternal.at(id));
    }

    /** @addtogroup init_3dc
     * Get basic parameters of the ego vehicle.
    */
    double wheelBase = GetAgent()->GetVehicleModelParameters().wheelbase;
    double lenFront = wheelBase / 2.0;
    double lenRear = wheelBase - lenFront;
    double trackWidth = GetAgent()->GetVehicleModelParameters().trackwidth;
    double hCOG = GetAgent()->GetVehicleModelParameters().height/2.0;
    double mass = GetAgent()->GetVehicleModelParameters().weight;

    // define car parameters including lLeft, lRight, lFront, lRear, hMC, mass
    carParam = VehicleBasics(trackWidth*0.5, trackWidth*0.5, lenFront, lenRear, hCOG, mass);

    /** @addtogroup init_3dc
     * For each wheel, initialize the correspondig oscillation object for later simulation.
    */
    for(int i=0; i < NUMBER_WHEELS; i++)
    {
        oscillations[i].Init(i, GetCycleTime()*0.001, springCoefficient.GetValue(), damperCoefficient.GetValue());
    }

    LOGDEBUG(QString().sprintf("Chassis: agent info: DistanceCOGtoFrontAxle %.2f, DistanceCOGtoRearAxle %.2f, trackWidth %.2f, heightCOG %.2f, mass %.2f",
                   lenFront, lenRear, trackWidth, hCOG, mass).toStdString());

    pitchZ = 0.0;
    rollZ = 0.0;

    LOGINFO("Constructing Dynamics_Chassis successful");

}

Dynamics_Chassis_Implementation::~Dynamics_Chassis_Implementation()
{

}

void Dynamics_Chassis_Implementation::UpdateInput(int localLinkId,
                                                       const std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    bool success = inputPorts.at(localLinkId)->SetSignalValue(data);
    if (success)
        LOGDEBUG(QString().sprintf("Dynamics_Chassis: Update input #%d successful", localLinkId).toStdString());
    else
        LOGERROR(QString().sprintf("Dynamics_Chassis: Update input #%d failed", localLinkId).toStdString());
}

void Dynamics_Chassis_Implementation::UpdateOutput(int localLinkId,
                                                        std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    bool success = outputPorts.at(localLinkId)->GetSignalValue(data);
    if (success)
        LOGDEBUG(QString().sprintf("Dynamics_Chassis: Update output #%d successful", localLinkId).toStdString());
    else
        LOGERROR(QString().sprintf("Dynamics_Chassis: Update output #%d failed", localLinkId).toStdString());
}

void Dynamics_Chassis_Implementation::Trigger(int time)
{
    Q_UNUSED(time);

    /** @addtogroup sim_step_3dc
     * Get the inputs of inertia forces.
    */
    double fInertiaX = 0.0;
    double fInertiaY = 0.0;
    if(inertiaForce.GetValue().size() == 2)
    {
        fInertiaX = inertiaForce.GetValue().at(0);
        fInertiaY = inertiaForce.GetValue().at(1);
    }

    /** @addtogroup sim_step_3dc_2
     * Calculate the vehicle deformation and additional vertical forces against inertia.
    */
    // calculate the vertical force
    if(!wheelForces.CalForce(fInertiaX, fInertiaY, pitchZ, rollZ, carParam))
        return;

    double originalForce = -carParam.mass / NUMBER_WHEELS * GRAVITY_ACC;

    /** @addtogroup sim_step_3dc_2
     * For each wheel, calculate the total vertical force as well as the Z-position.
    */
    double forceZ;
    double wheelMass;
    std::vector<double> resForces;
    for(int i=0; i < NUMBER_WHEELS; i++)
    {

        // calculate deformation due to the vertical force
        wheelMass = carParam.GetWheelMass(i); // the mass is distributed based on deformation
        forceZ = -wheelMass * GRAVITY_ACC + wheelForces.GetForce(i);
        if (forceZ>-1.0)
        {
            forceZ = -1.0; // glue the tires to the ground and limit to at least 1N
        }
        resForces.push_back(forceZ);

        oscillations[i].Perform(forceZ, wheelMass);
    }

    LOGDEBUG(QString().sprintf("Chassis %d at time %d: inertia force %f, %f, weight force %f",
                               GetAgent()->GetId(), time,
                               fInertiaX, fInertiaY, originalForce).toStdString());
    LOGDEBUG(QString().sprintf("Chassis %d at time %d: zForce %f, %f, %f, %f",
                               GetAgent()->GetId(), time,
                               resForces.at(0), resForces.at(1), resForces.at(2), resForces.at(3)).toStdString());
    LOGDEBUG(QString().sprintf("Chassis %d at time %d: zPos %f, %f, %f, %f",
                               GetAgent()->GetId(), time,
                               oscillations[0].GetCurZPos(), oscillations[1].GetCurZPos(), oscillations[2].GetCurZPos(), oscillations[3].GetCurZPos()).toStdString());

    /** @addtogroup sim_step_3dc_2
     * Calculate the Z-position of the vehicle in both longitudinal and lateral directions.
    */
    // the pitchZ is positive if pitching backward, otherwise negative
    // the rollZ is positive if rolling right, otherwise negative
    pitchZ = (oscillations[0].GetCurZPos() + oscillations[1].GetCurZPos() - oscillations[2].GetCurZPos() - oscillations[3].GetCurZPos()) / 2.0;
    rollZ = (oscillations[0].GetCurZPos() + oscillations[2].GetCurZPos() - oscillations[1].GetCurZPos() - oscillations[3].GetCurZPos()) / 2.0;

    /** @addtogroup sim_step_3
     * Output local vertical forces for the two-track dynamics.
    */
    verticalForce.SetValue(resForces);

    return;
}
