/*********************************************************************
* Copyright (c) 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef ALGORITHM_ECU_IMPLEMENTATION_H
#define ALGORITHM_ECU_IMPLEMENTATION_H

#include <QString>
#include "include/modelInterface.h"
#include "include/observationInterface.h"
#include "common/primitiveSignals.h"
#include "common/componentPorts.h"
#include "ecu.h"

/**
 * \addtogroup Components_Special openPASS components special
 * @{
 * \addtogroup Algorithm_ECU
 *
 * \brief Algorithm that decides which of the assistance systems takes control.
 *
 * \details The decision about the used assistance system depends on the
 * detection of a collision and the activity of prioritized assistance
 * systems.
 *
 * ECU receives activity flags from assistant systems.
 *
 * The priority of assistant systems is defined by the user by connecting
 * the most important assistant at the top inputs and the least at the
 * bottom in the SystemEditor.
 *
 * @}
 */

/*!
 * \copydoc Algorithm_ECU
 * \ingroup Algorithm_ECU
 */
class Algorithm_ECU_Implementation : public AlgorithmInterface
{
public:
    const std::string COMPONENTNAME = "Algorithm_ECU";

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
    Algorithm_ECU_Implementation(std::string componentName,
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
    virtual ~Algorithm_ECU_Implementation();

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

    /** \addtogroup Algorithm_ECU
     *  @{
     *      \name InputPort
     *      All input ports with PortId
     *      @{
     */
    InputPort<BoolSignal, bool> collisionOccurred {0, &inputPorts}; //!< flags if collision has happened already
    InputPort<BoolSignal, bool> prio1AssistActivity {1, &inputPorts}; //!< current activity of assistSystem Nr 1
    InputPort<BoolSignal, bool> prio2AssistActivity {2, &inputPorts}; //!< current activity of assistSystem Nr 2
    InputPort<BoolSignal, bool> prio3AssistActivity {3, &inputPorts}; //!< current activity of assistSystem Nr 3
    /**
     *      @}
     *  @}
     */

    std::map<int, ComponentPort *> outputPorts; //!< map for all OutputPort
    /** \addtogroup Algorithm_ECU
     *  @{
     *      \name OutputPorts
     *      All output ports with PortId
     *      @{
     */
    OutputPort<IntSignal, int> index {0, &outputPorts}; //!< select which of the assistance systems shall take control
    /**
     *      @}
     *  @}
     */


    /**
     *    \name Internal objects
     *    @{
    */
    Ecu* ecu = nullptr;
    /**
     *    @}
    */
};

#endif // ALGORITHM_ECU_IMPLEMENTATION_H
