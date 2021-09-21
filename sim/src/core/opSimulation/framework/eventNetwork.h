/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020, 2021 in-tech GmbH
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

#include <unordered_map>
#include <vector>

#include "common/opExport.h"
#include "common/coreDataPublisher.h"
#include "include/eventNetworkInterface.h"
#include "include/runResultInterface.h"
#include "include/worldInterface.h"

namespace core {

//-----------------------------------------------------------------------------
/** \brief Implements the functionality of the interface.
*
* 	\ingroup EventNetwork */
//-----------------------------------------------------------------------------
class SIMULATIONCOREEXPORT EventNetwork final : public EventNetworkInterface
{
public:
    EventNetwork(DataBufferWriteInterface *const dataBuffer) :
        publisher{dataBuffer}
    {
    }

    /*!
    * \brief Returns the active events of a specific category.
    *
    * @return	     List of active events.
    */
    virtual EventContainer GetEvents(const EventDefinitions::EventCategory eventCategory) override;

    /*!
    * \brief Inserts an event for the manipulators
    *
    * @param[in]     event    Shared pointer of the event.
    */
    virtual void InsertEvent(SharedEvent event) override;

    /*!
    * \brief Clears the event maps and resets pointers.
    */
    virtual void Clear() override;

    /*!
    * \brief Adds a collision id to the RunResult
    *
    * \details Adds a collision id to the RunResult
    *
    *
    * @param[in]     agentId     Id of the collision agent
    */
    virtual void AddCollision(const int agentId) override;

    /*!
    * \brief Initalizes the EventNetwork
    *
    * \details Initalizes the EventNetwork.
    *
    *
    * @param[in]     runResult    Pointer to the runResult.
    */
    virtual void Initialize(RunResultInterface *runResult) override;

    /// \brief Add a trigger, which is being relayed to the components
    /// \param topic for identification of the specific trigger
    /// \param event The according event, which is being sent
    void InsertTrigger(const std::string &topic, std::unique_ptr<EventInterface> event) override;

    /// \brief Returns a collection of active trigger
    /// \param topic for identification of the specific triggers
    /// \return Collection of active trigger
    std::vector<EventInterface const *> GetTrigger(const std::string &topic) const override;

private:
    openpass::publisher::CoreDataPublisher publisher;
    std::unordered_map<std::string, std::vector<std::unique_ptr<EventInterface>>> trigger;
    std::unordered_map<EventDefinitions::EventCategory, EventContainer> events;
    RunResultInterface *runResult{nullptr};
};

} //namespace core
