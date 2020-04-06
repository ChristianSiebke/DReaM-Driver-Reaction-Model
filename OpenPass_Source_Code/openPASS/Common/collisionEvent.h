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
/** @file  CollisionEvent.h
* @brief This file contains all functions for collision based events.
*
* This class contains all functionality of the module. */
//-----------------------------------------------------------------------------
#pragma once

#include "basicEvent.h"

//-----------------------------------------------------------------------------
/** This class implements all functionality of the CollisionEvent.
 *
 * \ingroup Events */
//-----------------------------------------------------------------------------
class CollisionEvent : public BasicEvent
{
public:
    CollisionEvent(int time, std::string source, bool collisionWithAgent, int collisionAgentId, int collisionOpponentId) :
        BasicEvent{time, "Collision", source, {collisionAgentId, collisionOpponentId}, {}},
        collisionWithAgent{collisionWithAgent},
        collisionAgentId{collisionAgentId},
        collisionOpponentId{collisionOpponentId}
    {
        parameter.emplace("CollisionWithAgent", collisionWithAgent);
    }

    ~CollisionEvent() override = default;

    EventDefinitions::EventCategory GetCategory() const override
    {
        return EventDefinitions::EventCategory::OpenPASS;
    }

    EventParameters GetParametersAsString() override
    {
        EventParameters eventParameters = BasicEvent::GetParametersAsString();
        eventParameters.push_back({"CollisionWithAgent", collisionWithAgent ? "true" : "false"});
        eventParameters.push_back({"CollisionAgentId", std::to_string(collisionAgentId)});
        eventParameters.push_back({"CollisionOpponentId", std::to_string(collisionOpponentId)});
        return eventParameters;
    }

    [[deprecated("Will be replaced by BasicEvent::parameter")]] bool collisionWithAgent;
    [[deprecated("Will be replaced by BasicEvent::parameter")]] int collisionAgentId;
    [[deprecated("Will be replaced by BasicEvent::parameter")]] int collisionOpponentId;
};
