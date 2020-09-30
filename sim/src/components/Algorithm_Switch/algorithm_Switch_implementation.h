/*********************************************************************
* Copyright (c) 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef ALGORITHM_SWITCH_IMPLEMENTATION_H
#define ALGORITHM_SWITCH_IMPLEMENTATION_H

#include <QString>
#include "include/modelInterface.h"
#include "common/primitiveSignals.h"
#include "common/vectorSignals.h"
#include "controlSignal.h"
#include "common/componentPorts.h"
#include "common/commonTools.h"
#include "switch.h"


/**
 * \addtogroup Components_Special openPASS components special
 * @{
 * \addtogroup Algorithm_Switch
 *
 * \brief Algorithm that switches between the assistance systems.
 *
 * \details This algorithm implements the decision made by the ECU about the
 * prioritized assistance system.
 *
 * @}
 */

/*!
 * \copydoc Algorithm_Switch
 * \ingroup Algorithm_Switch
 */
class Algorithm_Switch_Implementation : public AlgorithmInterface
{
public:
    const std::string COMPONENTNAME = "Algorithm_Switch";

    //! Constructor
    //!
    //! @param[in]     componentId    Corresponds to "id" of "Component"
    //! @param[in]     isInit         Corresponds to "init" of "Component"
    //! @param[in]     priority       Corresponds to "priority" of "Component"
    //! @param[in]     offsetTime     Corresponds to "offsetTime" of "Component"
    //! @param[in]     responseTime   Corresponds to "responseTime" of "Component"
    //! @param[in]     cycleTime      Corresponds to "cycleTime" of "Component"
    //! @param[in]     stochastics    Pointer to the stochastics class loaded by the framework
    //! @param[in]     parameters     Pointer to the parameters of the module
    //! @param[in]     evaluations    Pointer to the evaluations of the module
    //! @param[in]     callbacks      Pointer to the callbacks
    Algorithm_Switch_Implementation(std::string componentName,
                                         bool isInit,
                                         int priority,
                                         int offsetTime,
                                         int responseTime,
                                         int cycleTime,
                                         StochasticsInterface *stochastics,
                                         const ParameterInterface *parameters,
                                         PublisherInterface * const publisher,
                                         const CallbackInterface *callbacks,
                                         AgentInterface *agent);

    //! Destructor
    virtual ~Algorithm_Switch_Implementation();

    //! Function is called by framework when another component delivers a signal over
    //! a channel to this component (scheduler calls update taks of other component).
    //!
    //! @param[in]     localLinkId    Corresponds to "id" of "ComponentInput"
    //! @param[in]     data           Referenced signal (copied by sending component)
    //! @param[in]     time           Current scheduling time
    //! @return                       True on success
    virtual void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data,
                             int time);

    //! Function is called by framework when this component has to deliver a signal over
    //! a channel to another component (scheduler calls update task of this component).
    //!
    //! @param[in]     localLinkId    Corresponds to "id" of "ComponentOutput"
    //! @param[out]    data           Referenced signal (copied by this component)
    //! @param[in]     time           Current scheduling time
    //! @return                       True on success
    virtual void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time);

    //! Function is called by framework when the scheduler calls the trigger task
    //! of this component
    //!
    //! @param[in]     time           Current scheduling time
    //! @return                       True on success
    virtual void Trigger(int time);

private:

    std::map<int, ComponentPort *> inputPorts; //!< map for all InputPort

    /** \addtogroup Algorithm_Switch
     *  @{
     *      \name InputPort
     *      All input ports with PortId
     *      @{
     */
    InputPort<IntSignal, int> index {0, &inputPorts}; //!< select which of the assistance systems shall take control
    InputPort<ControlSignal, ControlData> driverControl {1, &inputPorts}; //!< control signals from driver
    InputPort<ControlSignal, ControlData> prio1Control {2, &inputPorts}; //!< control signals from assistSystem Nr 1
    InputPort<ControlSignal, ControlData> prio2Control {3, &inputPorts}; //!< control signals from assistSystem Nr 2
    InputPort<ControlSignal, ControlData> prio3Control {4, &inputPorts}; //!< control signals from assistSystem Nr 3
    /**
     *      @}
     *  @}
     */

    std::map<int, ComponentPort *> outputPorts; //!< map for all OutputPort
    /** \addtogroup Algorithm_Switch
     *  @{
     *      \name OutputPorts
     *      All output ports with PortId
     *      @{
     */
    OutputPort<ControlSignal, ControlData> resultingControl {0, &outputPorts}; //!< control signals
    /**
     *      @}
     *  @}
     */


    /**
     *    \name Internal objects
     *    @{
    */
    Switch* switchAssist;
    /**
     *    @}
    */
};

#endif // ALGORITHM_SWITCH_IMPLEMENTATION_H
