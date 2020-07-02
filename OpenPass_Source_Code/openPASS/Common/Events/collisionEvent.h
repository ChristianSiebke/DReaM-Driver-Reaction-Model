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

#include "Events/basicEvent.h"

namespace openpass::events {

//-----------------------------------------------------------------------------
/** This class implements all functionality of the CollisionEvent.
 *
 * \ingroup Events */
//-----------------------------------------------------------------------------
class CollisionEvent : public OpenPassEvent
{
public:
    CollisionEvent(int time, std::string source, bool collisionWithAgent, int collisionAgentId, int collisionOpponentId) :
        BasicEvent{time, "Collision", std::move(source), {{std::move(collisionAgentId), std::move(collisionOpponentId)}}, {}},
        collisionWithAgent{collisionWithAgent},
        collisionAgentId{collisionAgentId},
        collisionOpponentId{collisionOpponentId}
    {
        parameter.emplace("CollisionWithAgent", collisionWithAgent);
    }

    bool collisionWithAgent;
    int collisionAgentId;
    int collisionOpponentId;
};

} // namespace openpass::events
