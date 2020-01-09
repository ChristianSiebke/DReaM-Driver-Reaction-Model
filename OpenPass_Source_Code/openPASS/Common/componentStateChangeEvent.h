/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \addtogroup ComponentStateChangeEvent
* @{
* \file ComponentStateChangeEvent.h
* \brief This file contains the behaviours for ComponentStateChange events
*
* \details ComponentStateChange Events are intended to be used in conjunction
*          with the Vehicle Control Unit to toggle Vehicle Components
* @}
* */
//-----------------------------------------------------------------------------

#pragma once

#include "conditionalEvent.h"
#include "Interfaces/signalInterface.h"

//-----------------------------------------------------------------------------
/** This class implements all functionality of the ComponentStateChangeEvent.
 *
 * \ingroup Event */
//-----------------------------------------------------------------------------
class ComponentChangeEvent : public ConditionalEvent
{
public:
    ComponentChangeEvent(int time,
                         const std::string& eventName,
                         const std::string& source,
                         std::vector<int> triggeringAgents,
                         std::vector<int> actingAgents,
                         const std::string& componentName,
                         const std::string& goalStateName):
        ConditionalEvent(time,
                         eventName,
                         source,
                         triggeringAgents,
                         actingAgents),
        componentName(componentName),
        goalStateName(goalStateName)
    {
        goalState = ComponentStateMapping.at(goalStateName);
    }
    ComponentChangeEvent(const ComponentChangeEvent&) = delete;
    ComponentChangeEvent(ComponentChangeEvent&&) = delete;
    ComponentChangeEvent& operator=(const ComponentChangeEvent&) = delete;
    ComponentChangeEvent& operator=(ComponentChangeEvent&&) = delete;
    virtual ~ComponentChangeEvent() override = default;

    /*!
    * \brief Returns the category of the event.
    *
    * @return	     EventCategory.
    */
    virtual EventDefinitions::EventCategory GetCategory() const override
    {
        return EventDefinitions::EventCategory::ComponentStateChange;
    }

    /*!
     * \brief Returns the component name for which the event is targeted
     * @return Component Name
     */
    std::string GetComponentName() const {
        return componentName;
    }

    /*!
     * \brief Returns the goal state name for which the event is targeted
     * @return State Name
     */
    std::string GetGoalStateName() const {
        return goalStateName;
    }

    /*!
     * \brief Returns the goal state as a result of the event
     * @return State
     */
    ComponentState GetGoalState() const {
        return goalState;
    }

    /*!
    * \brief Returns all parameters of the event as string list.
    * \details Returns the agentId as string list.
    *
    * @return	     List of string pairs of the event parameters.
    */
    virtual EventParameters GetParametersAsString() override
    {
        EventParameters eventParameters = ConditionalEvent::GetParametersAsString();

        eventParameters.push_back({"ComponentName", componentName});
        eventParameters.push_back({"State", goalStateName});

        return eventParameters;
    }

private:
    const std::string componentName;
    const std::string goalStateName;
    ComponentState goalState = ComponentState::Undefined;
};

