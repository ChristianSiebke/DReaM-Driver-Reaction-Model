/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

/** \addtogroup Manipulator
* @{
* \brief This file contains the basic Manipulator functionality.
*
* \details This file contains the basic Manipulator functionality.
* A Manipulator acts upon a specific event. The Manipulator usually influences agents or
* framework modules.
* Each Manipulator can have its own cycle time.
*
* \section MODULENAME_ExternalParameters External parameters
* name | meaning
* -----|------
* eventTypes    | Event types which trigger the manipulator.
* actorNames    | Names of acting agents.
* sequenceName  | Name of the sequence the Manipulator belongs to.
*
*
* @} */

#pragma once

#include "Interfaces/eventNetworkInterface.h"
#include "Interfaces/manipulatorInterface.h"
#include "Interfaces/scenarioActionInterface.h"
#include "Interfaces/worldInterface.h"

//Event Categories
#include "Common/agentBasedManipulatorEvent.h"
#include "Common/collisionEvent.h"

#define EVENT_DETECTOR "EventDetector"

//-----------------------------------------------------------------------------
/** \brief This is the parent class for all Manipulators providing the basic functionality.
*
* 	\ingroup Manipulator */
//-----------------------------------------------------------------------------
class ManipulatorCommonBase : public ManipulatorInterface
{
public:
    ManipulatorCommonBase(WorldInterface *world,
                          SimulationSlave::EventNetworkInterface *eventNetwork,
                          const CallbackInterface *callbacks):
        world(world),
        eventNetwork(eventNetwork),
        callbacks(callbacks)
    {}

    ManipulatorCommonBase(WorldInterface *world,
                          std::shared_ptr<ScenarioActionInterface> action,
                          SimulationSlave::EventNetworkInterface *eventNetwork,
                          const CallbackInterface *callbacks);

    ManipulatorCommonBase(const ManipulatorCommonBase&) = delete;
    ManipulatorCommonBase(ManipulatorCommonBase&&) = delete;
    ManipulatorCommonBase& operator=(const ManipulatorCommonBase&) = delete;
    ManipulatorCommonBase& operator=(ManipulatorCommonBase&&) = delete;

    /*!
    * \brief Triggers the functionality of this class
    *
    * \details Trigger gets called each cycle timestep.
    * This function is repsonsible for creating events
    *
    * @param[in]     time    Current time.
    */
    virtual void Trigger(int time) = 0;

    /*!
    * \brief Returns the cycle time.
    *
    * @return     cycle time.
    */
    virtual int GetCycleTime();

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

    /*!
    * \brief Returns all current triggering events.
    * \details Returns all triggering events which are currently in the event network.
    *          Filters according to the defined event types.
    *
    * param[in]     manipulated event
    */
    virtual EventContainer GetEvents() = 0;

    std::list<EventDefinitions::EventType> eventTypes;

    WorldInterface *world = nullptr;
    ParameterInterface *parameters = nullptr;
    SimulationSlave::EventNetworkInterface* eventNetwork = nullptr;
    const CallbackInterface *callbacks = nullptr;

    int cycleTime {};
    std::string sequenceName {""};

    EventDefinitions::EventType eventType = EventDefinitions::EventType::Undefined;

    const std::string COMPONENTNAME {"Manipulator"};
};
