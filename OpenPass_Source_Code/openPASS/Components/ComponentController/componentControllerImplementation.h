/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

/** \addtogroup ComponentController
* @{
* \brief This module
*
* @} */

#pragma once

#include "Interfaces/modelInterface.h"
#include "Common/compCtrlToAgentCompSignal.h"
#include "componentControllerCommon.h"
#include "stateManager.h"

using namespace ComponentControl;

/**
* \brief
*
* \ingroup ComponentController
*/
class ComponentControllerImplementation : public UnrestrictedEventModelInterface
{
public:
    const std::string COMPONENTNAME = "ComponentController";

    ComponentControllerImplementation(std::string componentName,
        bool isInit,
        int priority,
        int offsetTime,
        int responseTime,
        int cycleTime,
        StochasticsInterface *stochastics,
        WorldInterface *world,
        const ParameterInterface *parameters,
        PublisherInterface * const publisher,
        const CallbackInterface *callbacks,
        AgentInterface *agent,
        SimulationSlave::EventNetworkInterface * const eventNetwork);
    ComponentControllerImplementation(const ComponentControllerImplementation&) = delete;
    ComponentControllerImplementation(ComponentControllerImplementation&&) = delete;
    ComponentControllerImplementation& operator=(const ComponentControllerImplementation&) = delete;
    ComponentControllerImplementation& operator=(ComponentControllerImplementation&&) = delete;
    virtual ~ComponentControllerImplementation() = default;

    /*!
    * \brief Update Inputs
    *
    * Function is called by framework when another component delivers a signal over
    * a channel to this component (scheduler calls update taks of other component).
    *
    * Refer to module description for input channels and input ids.
    *
    * @param[in]     localLinkId    Corresponds to "id" of "ComponentInput"
    * @param[in]     data           Referenced signal (copied by sending component)
    * @param[in]     time           Current scheduling time
    */
    virtual void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time);

    /*!
    * \brief Update outputs.
    *
    * Function is called by framework when this component has to deliver a signal over
    * a channel to another component (scheduler calls update task of this component).
    *
    * Refer to module description for output channels and output ids.
    *
    * @param[in]     localLinkId    Corresponds to "id" of "ComponentOutput"
    * @param[out]    data           Referenced signal (copied by this component)
    * @param[in]     time           Current scheduling time
    */
    virtual void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time);

    /*!
    * \brief Process data within component.
    *
    * Function is called by framework when the scheduler calls the trigger task
    * of this component
    *
    * @param[in]     time           Current scheduling time
    */
    virtual void Trigger(int time);

private:
    template <typename T, typename Signal>
    const std::shared_ptr<T const> SignalCast(Signal&& signal, int linkId)
    {
        if (const auto castedSignal = std::dynamic_pointer_cast<T const>(std::forward<Signal>(signal)))
        {
            return castedSignal;
        }

        const std::string msg = COMPONENTNAME + " invalid signaltype on link " + std::to_string(linkId);
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }

    std::map<int, std::string> driverInputChannels;
    std::map<int, std::string> driverOutputChannels;

protected:
    Warnings driverWarnings;

    StateManager stateManager;
};
