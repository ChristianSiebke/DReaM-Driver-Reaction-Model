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
/** \file  CollisionManipulator.cpp */
//-----------------------------------------------------------------------------

#include "CollisionManipulator.h"

#include <QtGlobal>

CollisionManipulator::CollisionManipulator(WorldInterface* world,
        SimulationSlave::EventNetworkInterface* eventNetwork,
        const CallbackInterface* callbacks):
    ManipulatorCommonBase(world,
                          eventNetwork,
                          callbacks)
{
    cycleTime = 100;
}

void CollisionManipulator::Trigger(int time)
{
    Q_UNUSED(time);

    for (std::shared_ptr<EventInterface> eventInterface : GetEvents())
    {
        std::shared_ptr<CollisionEvent> event = std::dynamic_pointer_cast<CollisionEvent>(eventInterface);
        AgentInterface* collisionAgent = world->GetAgent(event->collisionAgentId);

        eventNetwork->AddCollision(event->collisionAgentId);

        if (event->collisionWithAgent)
        {
            AgentInterface* collisionOpponent = world->GetAgent(event->collisionOpponentId);
            try
            {
                UpdateCollision(collisionAgent, collisionOpponent);
            }
            catch (const std::bad_alloc&)
            {
                std::cout << "BAD ALLOC";
            }
            eventNetwork->AddCollision(event->collisionOpponentId);
        }
        else
        {
            const auto pair = std::make_pair(ObjectTypeOSI::Object, event->collisionOpponentId);
            for (auto partner : collisionAgent->GetCollisionPartners())
            {
                if (partner.first == ObjectTypeOSI::Vehicle)
                {
                    AgentInterface* partnerAgent = world->GetAgent(partner.second);
                    partnerAgent->UpdateCollision(pair);
                }
            }

            collisionAgent->UpdateCollision(pair);
        }
    }
}

void CollisionManipulator::UpdateCollision(AgentInterface* agent, AgentInterface* opponent)
{
    //Stop conditions for the recursion
    if (agent == nullptr || opponent == nullptr)
    {
        return ;
    }

    if (agent->GetId() == opponent->GetId())
    {
        return;
    }

    for (const auto& partner : agent->GetCollisionPartners())
    {
        if (partner.second == opponent->GetId() && partner.first == ObjectTypeOSI::Vehicle)
        {
            return;
        }
    }

    //Update the collision partners
    agent->UpdateCollision(std::make_pair(opponent->GetType(), opponent->GetId()));
    opponent->UpdateCollision(std::make_pair(agent->GetType(), agent->GetId()));

    //Recursion for agent
    for (const auto& partner : agent->GetCollisionPartners())
    {
        if (partner.first == ObjectTypeOSI::Object)
        {
            opponent->UpdateCollision(std::make_pair(ObjectTypeOSI::Object, partner.second));
        }
        else
        {
            UpdateCollision(world->GetAgent(partner.second), opponent);
        }
    }

    //Recursion for opponent
    for (const auto& partner : opponent->GetCollisionPartners())
    {
        if (partner.first == ObjectTypeOSI::Object)
        {
            agent->UpdateCollision(std::make_pair(ObjectTypeOSI::Object, partner.second));
        }
        else
        {
            UpdateCollision(world->GetAgent(partner.second), agent);
        }
    }
}


EventContainer CollisionManipulator::GetEvents()
{
    return eventNetwork->GetActiveEventCategory(EventDefinitions::EventCategory::Collision);
}
