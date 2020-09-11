/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

/** \addtogroup EventDetector
* @{
* \brief This file contains the basic EventDetector functionality.
*
* \details This file contains the basic EventDetector functionality.
* An EventDetector acts as Trigger for specific events. Those events are then forwarded
* to the EventNetwork.
* The EventDetector uses its access to the world and the framework to find events.
* Each EventDetector can have its own cycle time.
*
* \section MODULENAME_ExternalParameters External parameters
* name | meaning
* -----|------
* triggeringAgents  | Determines what agents can trigger the EventDetector.
* sequenceName      | Name of the sequence the EventDetector belongs to.
* agentGroupName    | In case there are no triggeringAgents an EventDetector can trigger to a ceratin agent group.
*
*
* @} */

#pragma once

#include "include/callbackInterface.h"
#include "include/eventDetectorInterface.h"
#include "include/parameterInterface.h"
#include "include/stochasticsInterface.h"
#include "include/worldInterface.h"

#include "common/events/basicEvent.h"
#include "common/openScenarioDefinitions.h"
#include "common/eventDetectorDefinitions.h"

const std::unordered_map<std::string, AgentCategory> agentCategoryMap = {{"Ego", AgentCategory::Ego},
                                                                         {"Scenario", AgentCategory::Scenario},
                                                                         {"Common", AgentCategory::Common},
                                                                         {"Any", AgentCategory::Any}};

//-----------------------------------------------------------------------------
/** \brief This is the parent class for all EventDetectors providing the basic functionality.
*
* 	\ingroup EventDetector */
//-----------------------------------------------------------------------------
class EventDetectorCommonBase : public EventDetectorInterface
{
public:
    EventDetectorCommonBase(WorldInterface *world,
                            SimulationSlave::EventNetworkInterface *eventNetwork,
                            const CallbackInterface *callbacks,
                            StochasticsInterface *stochastics):
    world(world),
    eventNetwork(eventNetwork),
    callbacks(callbacks),
    stochastics(stochastics)
    {}

    /*!
    * \brief Triggers the functionality of this class
    *
    * \details Trigger gets called each cycle timestep.
    * This function is repsonsible for creating events
    *
    * @param[in]     time    Current time.
    */
    void Trigger(int time) override = 0;

    /*!
    * \brief Returns the cycle time.
    *
    * @return     cycle time.
    */
    virtual int GetCycleTime() override;

    /*!
    * \brief Resets the initial state of the EventDetector.
    *
    */
    virtual void Reset() override;

protected:
    /*!
     * \brief Log
     * Provides callback to LOG() macro
     *
     * \param[in] logLevel          Importance of log
     * \param[in] file              Name of file where log is called
     * \param[in] line              Line within file where log is called
     * \param[in] message           Message to log
     */
    void Log(CbkLogLevel logLevel,
             const char *file,
             int line,
             const std::string &message);

    WorldInterface *world {nullptr};
    SimulationSlave::EventNetworkInterface* eventNetwork {nullptr};
    const CallbackInterface *callbacks {nullptr};
    StochasticsInterface* stochastics {nullptr};

    int cycleTime = 1000;

    const std::string COMPONENTNAME {"EventDetector"};
};


