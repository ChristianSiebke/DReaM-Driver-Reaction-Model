/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <vector>

#include "basicEvent.h"

//-----------------------------------------------------------------------------
/** This class implements all functionality of the ConditionalEvent.
 *
 * \ingroup Event */
//-----------------------------------------------------------------------------
class ConditionalEvent : public BasicEvent
{
public:
    ConditionalEvent(int time, std::string eventName, std::string source,
                     std::vector<int> triggeringAgents, std::vector<int> actingAgents) :
        BasicEvent{time, eventName, source, triggeringAgents, actingAgents}
    {
    }

    ~ConditionalEvent() override = default;

    EventDefinitions::EventCategory GetCategory() const override
    {
        return EventDefinitions::EventCategory::OpenSCENARIO;
    }
};
