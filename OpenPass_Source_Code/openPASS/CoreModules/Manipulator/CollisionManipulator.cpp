/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  CollisionManipulator.cpp */
//-----------------------------------------------------------------------------

#include "CollisionManipulator.h"

#include <iostream>
#include "Common/Events/collisionEvent.h"

CollisionManipulator::CollisionManipulator(WorldInterface *world,
                                           SimulationSlave::EventNetworkInterface *eventNetwork,
                                           const CallbackInterface *callbacks) :
    ManipulatorCommonBase(world,
                          eventNetwork,
                          callbacks)
{
    cycleTime = 100;
}

void CollisionManipulator::Trigger([[maybe_unused]] int time)
{
    for (std::shared_ptr<EventInterface> eventInterface : GetEvents())
    {
        if (auto event = std::dynamic_pointer_cast<openpass::events::CollisionEvent>(eventInterface))
        {
            AgentInterface *collisionAgent = world->GetAgent(event->collisionAgentId);

            eventNetwork->AddCollision(event->collisionAgentId);

            if (event->collisionWithAgent)
            {
                AgentInterface *collisionOpponent = world->GetAgent(event->collisionOpponentId);
                UpdateCollision(collisionAgent, collisionOpponent);
                eventNetwork->AddCollision(event->collisionOpponentId);
            }
            else
            {
                const auto pair = std::make_pair(ObjectTypeOSI::Object, event->collisionOpponentId);
                for (auto partner : collisionAgent->GetCollisionPartners())
                {
                    if (partner.first == ObjectTypeOSI::Vehicle)
                    {
                        AgentInterface *partnerAgent = world->GetAgent(partner.second);
                        partnerAgent->UpdateCollision(pair);
                    }
                }

                collisionAgent->UpdateCollision(pair);
            }
        }
    }
}

void CollisionManipulator::UpdateCollision(AgentInterface *agent, AgentInterface *opponent)
{
    if (!agent || !opponent || agent->GetId() == opponent->GetId())
    {
        return;
    }

    // check if object is already in stored collision partners
    for (const auto &[objectType, id] : agent->GetCollisionPartners())
    {
        if (id == opponent->GetId() && objectType == ObjectTypeOSI::Vehicle)
        {
            return;
        }
    }

    //Update the collision partners
    agent->UpdateCollision({opponent->GetType(), opponent->GetId()});
    opponent->UpdateCollision({agent->GetType(), agent->GetId()});

    //Recursion for agent
    for (const auto &partner : agent->GetCollisionPartners())
    {
        if (partner.first == ObjectTypeOSI::Object)
        {
            opponent->UpdateCollision({ObjectTypeOSI::Object, partner.second});
        }
        else
        {
            UpdateCollision(world->GetAgent(partner.second), opponent);
        }
    }

    //Recursion for opponent
    for (const auto &partner : opponent->GetCollisionPartners())
    {
        if (partner.first == ObjectTypeOSI::Object)
        {
            agent->UpdateCollision({ObjectTypeOSI::Object, partner.second});
        }
        else
        {
            UpdateCollision(world->GetAgent(partner.second), agent);
        }
    }
}

EventContainer CollisionManipulator::GetEvents()
{
    return eventNetwork->GetEvents(EventDefinitions::EventCategory::OpenPASS);
}
