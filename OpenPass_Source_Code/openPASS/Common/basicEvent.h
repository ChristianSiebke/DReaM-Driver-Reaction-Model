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
/** \addtogroup Event
* @{
* \file  BasicEvent.h
* \brief This file contains all basic functionality of events.
*
* \details This file contains the common event member variables and the basic functions of all events.
* @} */
//-----------------------------------------------------------------------------

#pragma once

#include "Interfaces/eventInterface.h"

//-----------------------------------------------------------------------------
/** Signal This class contains all functionality of the BasicEvent.
 *
 * \ingroup Event */
//-----------------------------------------------------------------------------
class BasicEvent : public EventInterface
{
public:
    BasicEvent(int time,
               std::string eventName,
               std::string source):
        time(time),
        name(eventName),
        source(source)
    {}
    BasicEvent(const BasicEvent&) = delete;
    BasicEvent(BasicEvent&&) = delete;
    BasicEvent& operator=(const BasicEvent&) = delete;
    BasicEvent& operator=(BasicEvent&&) = delete;
    virtual ~BasicEvent() override = default;

    /*!
    * \brief Sets the Id of the event.
    *
    * @param[in]	Id of the event.
    */
    virtual void SetId(const int eventId) override
    {
        this->eventId = eventId;
    }

    /*!
    * \brief Returns the Id of the event.
    *
    * @return	     Id.
    */
    virtual int GetId() const override
    {
        return eventId;
    }

    /*!
    * \brief Returns the time of the event.
    *
    * @return	     Time in milliseconds.
    */
    virtual int GetEventTime() const override
    {
        return time;
    }

    /*!
    * \brief Returns the category of the event.
    *
    * @return	     EventCategory.
    */
    virtual EventDefinitions::EventCategory GetCategory() const override
    {
        return EventDefinitions::EventCategory::Basic;
    }

    /*!
    * \brief Sets the Id of the event which triggered this event.
    *
    * @param[in]	     Event Id.
    */
    virtual void SetTriggeringEventId(const int triggeringEventId) override
    {
        this->triggeringEventId = triggeringEventId;
    }

    /*!
    * \brief Returns the Id of the event which triggered this event.
    *
    * @return	     Event Id.
    */
    virtual int GetTriggeringEventId() const override
    {
        return triggeringEventId;
    }

    /*!
    * \brief Returns the name of the event.
    *
    * @return	     Name of the event as string.
    */
    virtual std::string GetName() const override
    {
        return name;
    }

    /*!
    * \brief Returns the name of the source component.
    *
    * @return	     Name of source component as string.
    */
    virtual std::string GetSource() const override
    {
        return source;
    }

    /*!
    * \brief Returns all parameters of the event as string list.
    * \details Returns all parameters of the event as a generic string list.
    *          The parameter name and the value are returned as string.
    *
    * @return	     List of string pairs of the event parameters.
    */
    virtual EventParameters GetParametersAsString() override
    {
        return {};
    }

private:
    int eventId;
    const int time;
    int triggeringEventId {-1};
    const std::string name;
    const std::string source;
};
