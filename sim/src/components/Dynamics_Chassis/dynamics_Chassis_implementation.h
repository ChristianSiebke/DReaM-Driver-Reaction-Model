/*********************************************************************
* Copyright (c) 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef DYNAMICS_CHASSIS_IMPLEMENTATION_H
#define DYNAMICS_CHASSIS_IMPLEMENTATION_H

#include <cassert>
#include <qglobal.h>
#include "include/modelInterface.h"
#include "include/observationInterface.h"
#include "common/primitiveSignals.h"
#include "common/vectorSignals.h"
#include "common/componentPorts.h"
#include "include/parameterInterface.h"

#include "VehicleBasics.h"
#include "ForceWheelZ.h"
#include "WheelOscillation.h"

/**
 * \addtogroup Components_Special openPASS components special
 * @{
 * \addtogroup Dynamics_Chassis
 *
 * \brief suspension and deformation simulation
 *
 * \details Component simulates suspension as well as deformation of a vehicle due to inertia forces and calcuates the vertical forces on each wheel.
 *
 * @}
 */

/*!
 * \copydoc Dynamics_Chassis
 * \ingroup Dynamics_Chassis
 */
class Dynamics_Chassis_Implementation : public DynamicsInterface
{
public:
    const std::string COMPONENTNAME = "Dynamics_Chassis";

    //! Constructor
    //!
    //! @param[in]     componentName  Name of the component
    //! @param[in]     isInit         Corresponds to "init" of "Component"
    //! @param[in]     priority       Corresponds to "priority" of "Component"
    //! @param[in]     offsetTime     Corresponds to "offsetTime" of "Component"
    //! @param[in]     responseTime   Corresponds to "responseTime" of "Component"
    //! @param[in]     cycleTime      Corresponds to "cycleTime" of "Component"
    //! @param[in]     stochastics    Pointer to the stochastics class loaded by the framework
    //! @param[in]     parameters     Pointer to the parameters of the module
    //! @param[in]     publisher      Pointer to the publisher instance
    //! @param[in]     callbacks      Pointer to the callbacks
    //! @param[in]     agent          Pointer to agent instance
    Dynamics_Chassis_Implementation(std::string componentName,
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
                                    AgentInterface *agent);
    virtual ~Dynamics_Chassis_Implementation();

    /*!
     * \brief Update Inputs
     *
     * Function is called by framework when another component delivers a signal over
     * a channel to this component (scheduler calls update taks of other component).
     *
     * Refer to module description for input channels and input ids.
     *
     * \param[in]     localLinkId    Corresponds to "id" of "ComponentInput"
     * \param[in]     data           Referenced signal (copied by sending component)
     * \param[in]     time           Current scheduling time
     */
    virtual void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data,
                             int time);

    /*!
     * \brief Update outputs.
     *
     * Function is called by framework when this component has to deliver a signal over
     * a channel to another component (scheduler calls update task of this component).
     *
     * Refer to module description for output channels and output ids.
     *
     * \param[in]     localLinkId    Corresponds to "id" of "ComponentOutput"
     * \param[out]    data           Referenced signal (copied by this component)
     * \param[in]     time           Current scheduling time
     */
    virtual void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time);

    /*!
     * \brief Process data within component.
     *
     * Function is called by framework when the scheduler calls the trigger task
     * of this component.
     *
     * Refer to module description for information about the module's task.
     *
     * \param[in]     time           Current scheduling time
     */
    virtual void Trigger(int time);

private:
    //-----------------------------------------------------------------------------

    std::map<int, ComponentPort *> inputPorts; //!< map for all InputPort
    /** \addtogroup Dynamics_Chassis
     *  @{
     *      \name InputPort
     *      All input ports with PortId
     *      @{
     */
    InputPort<SignalVectorDouble, std::vector<double>> inertiaForce {0, &inputPorts}; //!< inertia forces along the X-axis and Y-axis
    /**
     *      @}
     *  @}
     */

    std::map<int, ComponentPort *> outputPorts; //!< map for all OutputPort
    /** \addtogroup Dynamics_Chassis
     *  @{
     *      \name OutputPorts
     *      All output ports with PortId
     *      @{
     */
    OutputPort<SignalVectorDouble, std::vector<double>> verticalForce {0, &outputPorts}; //!< perpendicular force of each wheel against inertia
    /**
     *      @}
     *  @}
     */

    std::map<std::string, externalParameter<double>*> parameterMapDouble;
    /** \addtogroup Dynamics_Chassis
     *  @{
     *      \name External Parameter
     *      Parameter which are set externally in agentConfiguration file.
     *      @{
     */
    externalParameter<double> springCoefficient {0, &parameterMapDouble }; //!< Spring coefficient of the suspension system
    externalParameter<double> damperCoefficient {1, &parameterMapDouble }; //!< Damper coefficient of the suspension system
    /**
     *      @}
     *  @}
     */

    /**
     *    \name Internal objects
     *    @{
    */

    VehicleBasics carParam;
    ForceWheelZ wheelForces;
    WheelOscillation oscillations[NUMBER_WHEELS];

    double pitchZ;
    double rollZ;
    /**
     *    @}
    */
};

#endif // DYNAMICS_CHASSIS_IMPLEMENTATION_H
