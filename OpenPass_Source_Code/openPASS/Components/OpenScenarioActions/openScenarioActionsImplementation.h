/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

/** \addtogroup OpenScenarioActions
* @{
* \brief This module
*
* @} */

#pragma once

#include "Interfaces/modelInterface.h"
#include "Common/trajectoryEvent.h"
#include "Common/laneChangeEvent.h"

/**
* \brief
*
* \ingroup OpenScenarioActions
*/
class OpenScenarioActionsImplementation : public UnrestrictedEventModelInterface
{
public:
    const std::string COMPONENTNAME = "OpenScenarioActions";

    OpenScenarioActionsImplementation(std::string componentName,
        bool isInit,
        int priority,
        int offsetTime,
        int responseTime,
        int cycleTime,
        StochasticsInterface *stochastics,
        WorldInterface *world,
        const ParameterInterface *parameters,
        const std::map<int, ObservationInterface*> *observations,
        const CallbackInterface *callbacks,
        AgentInterface *agent,
        SimulationSlave::EventNetworkInterface * const eventNetwork);
    OpenScenarioActionsImplementation(const OpenScenarioActionsImplementation&) = delete;
    OpenScenarioActionsImplementation(OpenScenarioActionsImplementation&&) = delete;
    OpenScenarioActionsImplementation& operator=(const OpenScenarioActionsImplementation&) = delete;
    OpenScenarioActionsImplementation& operator=(OpenScenarioActionsImplementation&&) = delete;
    virtual ~OpenScenarioActionsImplementation() = default;

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
    std::shared_ptr<TrajectoryEvent> trajectoryEvent;
    std::shared_ptr<LaneChangeEvent> laneChangeEvent;
};
