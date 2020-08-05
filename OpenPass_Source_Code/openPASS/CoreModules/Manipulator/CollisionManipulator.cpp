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
            CalculateCrash(collisionAgent, collisionOpponent, event);
        }
        else
        {
            auto pair = std::make_pair(ObjectTypeOSI::Object, event->collisionOpponentId);
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

void CollisionManipulator::CalculateCrash(AgentInterface *agent, AgentInterface *opponent, std::shared_ptr<CollisionEvent> event)
{
    //! Stores calculated dynamics for ego/host agent
    PostCrashDynamic postCrashDynamic1;
    //! Stores calculated dynamics for opponent agent
    PostCrashDynamic postCrashDynamic2;

    int timeOfFirstContact = 0;
    if (!collisionPostCrash.CreatePostCrashDynamics(agent,
                                                    opponent,
                                                    &postCrashDynamic1,
                                                    &postCrashDynamic2,
                                                    timeOfFirstContact)) {
        std::cout << "PostCrash Calculation failed" << std::endl;
        agent->SetPostCrashVelocity({false, 0.0, 0.0, 0.0});
        opponent->SetPostCrashVelocity({false, 0.0, 0.0, 0.0});
        return;
    }

    agent->SetPostCrashVelocity({true,
                                postCrashDynamic1.GetVelocity(),
                                postCrashDynamic1.GetVelocityDirection(),
                                postCrashDynamic1.GetYawVelocity()});
    opponent->SetPostCrashVelocity({true,
                                    postCrashDynamic2.GetVelocity(),
                                    postCrashDynamic2.GetVelocityDirection(),
                                    postCrashDynamic2.GetYawVelocity()});
    event->AddParameters(collisionPostCrash.GetCollisionAngles(), postCrashDynamic1, postCrashDynamic2);
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
    std::vector<std::pair<ObjectTypeOSI, int>> collisionPartners = agent->GetCollisionPartners();
    for (const auto& partner : collisionPartners)
    {
        if (partner.second == opponent->GetId())
        {
            return;
        }
    }

    //Update the collision partners
    agent->UpdateCollision(std::make_pair(opponent->GetType(), opponent->GetId()));
    opponent->UpdateCollision(std::make_pair(agent->GetType(), agent->GetId()));

    //Recursion for agent
    collisionPartners = agent->GetCollisionPartners();
    for (const auto& partner : collisionPartners)
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
    collisionPartners = opponent->GetCollisionPartners();
    for (const auto& partner : collisionPartners)
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
