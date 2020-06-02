/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \addtogroup EventNetwork
* @{
* \file  EventNetwork.h
*	\brief The EventNetwork manages events and can trigger functions in different framework modules.
*
*   \details The EventNetwork stores all events. Other modules can access the events through the EventNetwork.
*            The EventNetwork can trigger functions in different framework modules.
*            This allows the Manipulators to influence the framework modules.
* @} */
//-----------------------------------------------------------------------------

#pragma once

#include <map>
#include <vector>

#include "Common/conditionalEvent.h"
#include "Common/componentStateChangeEvent.h"
#include "Common/componentWarningEvent.h"
#include "Common/collisionEvent.h"
#include "Common/laneChangeEvent.h"
#include "Common/vehicleComponentEvent.h"
#include "Interfaces/eventNetworkInterface.h"
#include "Interfaces/runResultInterface.h"
#include "Interfaces/worldInterface.h"

namespace SimulationSlave
{
using namespace EventDefinitions;

//-----------------------------------------------------------------------------
/** \brief Implements the functionality of the interface.
*
* 	\ingroup EventNetwork */
//-----------------------------------------------------------------------------
class EventNetwork : public EventNetworkInterface
{
public:
    EventNetwork() = default;
    virtual ~EventNetwork() override = default;

    /*!
    * \brief Returns the activeEvents.
    *
    * @return	     Map of activ events.
    */
    virtual Events *GetActiveEvents();

    /*!
    * \brief Returns the archivedEvents.
    *
    * @return	     Map of archived events.
    */
    virtual Events *GetArchivedEvents();

    /*!
    * \brief Returns the active events of a specific category.
    *
    * @return	     List of active events.
    */
    virtual EventContainer GetActiveEventCategory(const EventCategory eventCategory);

    /*!
    * \brief Removes archived events which are older than a certain time stamp.
    *
    * \details Removes all archived events which are older than a certain time stamp.
    *
    *
    * @param[in]     time       Time stamp.
    */
    virtual void RemoveOldEvents(int time);

    /*!
    * \brief Inserts an event into the activeEvents.
    *
    * \details Inserts an event into the activeEvents if the event has a valid category.
    *
    *
    * @param[in]     event    Shared pointer of the event.
    */
    virtual void InsertEvent(std::shared_ptr<EventInterface> event);

    /*!
    * \brief Empties the active events map and stores them as archived events.
    *
    * \details Empties the current active events and inserts them into the archived events.
    *          This method gets called once per cycle time.
    */
    virtual void ClearActiveEvents();

    /*!
    * \brief Clears the event maps and resets pointers.
    */
    virtual void Clear();

    /*!
    * \brief Adds a collision id to the RunResult
    *
    * \details Adds a collision id to the RunResult
    *
    *
    * @param[in]     agentId     Id of the collision agent
    */
    virtual void AddCollision(const int agentId);

    /*!
    * \brief Initalizes the EventNetwork
    *
    * \details Initalizes the EventNetwork.
    *
    *
    * @param[in]     runResult    Pointer to the runResult.
    */
    void Initialize(RunResultInterface *runResult);

private:
    Events activeEvents;
    Events archivedEvents;
    RunResultInterface *runResult {nullptr};

    int eventId {0};
};

} //namespace SimulationSlave

