/******************************************************************************
* Copyright (c) 2019 TU Dresden
* Copyright (c) 2019 AMFD GmbH
*
* This program and the accompanying materials are made available under the
* terms of the Eclipse Public License 2.0 which is available at
* https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*****************************************************************************/

//-----------------------------------------------------------------------------
//! @file  sensor_criticalityImplementation.h
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief provide all for the bird-view criticality calculation needed information
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_ModularDriver
//-----------------------------------------------------------------------------

#pragma once

#include "Interfaces/modelInterface.h"
#include "Interfaces/observationInterface.h"
#include "ContainerStructures.h"
#include "SituationCalculation.h"
#include "SituationLogging.h"

/*!
 * \brief records the state of the agent.
 *
 * This component is used to record the state of agent in every single timestep.
 *
 * \ingroup SensorCriticality
 */

//ToDo This module needs a major refactoring, when the new observation/logging concept is implemented. Until then it uses the first ObservationInterface of the ObservationNetwork.
class SensorCriticalityImplementation : public SensorInterface
{
public:
    const std::string COMPONENTNAME = "SensorCriticality";

    SensorCriticalityImplementation(
            std::string componentName,
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
            AgentInterface *agent);

    SensorCriticalityImplementation(const SensorCriticalityImplementation&) = delete;
    SensorCriticalityImplementation(SensorCriticalityImplementation&&) = delete;
    SensorCriticalityImplementation& operator=(const SensorCriticalityImplementation&) = delete;
    SensorCriticalityImplementation& operator=(SensorCriticalityImplementation&&) = delete;
    virtual ~SensorCriticalityImplementation() = default;

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
    void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time);

    /*!
     * \brief Update outputs.
     *
     * Function is called by framework when this Component.has to deliver a signal over
     * a channel to another component (scheduler calls update task of this component).
     *
     * Refer to module description for output channels and output ids.
     *
     * \param[in]     localLinkId    Corresponds to "id" of "ComponentOutput"
     * \param[out]    data           Referenced signal (copied by this component)
     * \param[in]     time           Current scheduling time
     */
    void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time);

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
    void Trigger(int time);

    void UpdateEgoData(AgentInterface *agent);
    void SetMovingObjects(AgentInterface *agent, const std::map<int, AgentInterface*> *Agents, int time);

    LaneInformationTrafficRules GetTrafficRuleLaneInformationEgo();


private:

    bool LoggingActivated = false;
    int agentId = 0;
    int timeMSec = 0;
    int indexLaneEgo{0};
    ObservationInterface* observerInstance {nullptr};
    std::vector<std::string> loggingGroups;

    BoundingBox Ego;
    std::list <std::unique_ptr<AgentRepresentation>> SurroundingMovingObjects;
    StaticEnvironmentData Environment;

    SituationLogging situationlogging;
    SituationCalculation situationCalculation;
};


