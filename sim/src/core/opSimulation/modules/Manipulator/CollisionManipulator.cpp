/********************************************************************************
 * Copyright (c) 2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** \file  CollisionManipulator.cpp */
//-----------------------------------------------------------------------------

#include "CollisionManipulator.h"

#include <iostream>

#include "common/events/collisionEvent.h"

CollisionManipulator::CollisionManipulator(WorldInterface *world,
                                           core::EventNetworkInterface *eventNetwork,
                                           const CallbackInterface *callbacks,
                                           openpass::publisher::CoreDataPublisher *publisher) :
    ManipulatorCommonBase(world,
                          eventNetwork,
                          callbacks),
    publisher{publisher}
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

                const auto crashInfo = CalculateCrash(collisionAgent, collisionOpponent);
                PublishCrash(event, crashInfo);
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

CollisionManipulator::CrashInfo CollisionManipulator::CalculateCrash(AgentInterface *agent, AgentInterface *opponent)
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
                                                    timeOfFirstContact))
    {
        std::cout << "PostCrash Calculation failed" << std::endl;
        const std::string msg = "PostCrash Calculation failed ";
        LOG(CbkLogLevel::Error, msg);
    }

    agent->SetPostCrashVelocity({true,
                                 postCrashDynamic1.GetVelocity(),
                                 postCrashDynamic1.GetVelocityDirection(),
                                 postCrashDynamic1.GetYawVelocity()});
    opponent->SetPostCrashVelocity({true,
                                    postCrashDynamic2.GetVelocity(),
                                    postCrashDynamic2.GetVelocityDirection(),
                                    postCrashDynamic2.GetYawVelocity()});

    return {collisionPostCrash.GetCollisionAngles(), postCrashDynamic1, postCrashDynamic2};
}

void CollisionManipulator::PublishCrash(const std::shared_ptr<openpass::events::CollisionEvent>& event, const CrashInfo& crashInfo)
{
    auto logEntry = openpass::publisher::LogEntry::FromEvent(event);
    logEntry.name = "ExtendedCollisionInformation";
    logEntry.parameter.insert({{"Velocity", crashInfo.postCrashDynamic1.GetVelocity()},
                               {"VelocityChange", crashInfo.postCrashDynamic1.GetVelocityChange()},
                               {"VelocityDirection", crashInfo.postCrashDynamic1.GetVelocityDirection()},
                               {"YawVelocity", crashInfo.postCrashDynamic1.GetYawVelocity()},
                               {"PointOfContactLocalX", crashInfo.postCrashDynamic1.GetPointOfContactLocal().x},
                               {"PointOfContactLocalY", crashInfo.postCrashDynamic1.GetPointOfContactLocal().y},
                               {"CollisionVelocity", crashInfo.postCrashDynamic1.GetCollisionVelocity()},
                               {"Sliding", crashInfo.postCrashDynamic1.GetSliding()},
                               {"OpponentVelocity", crashInfo.postCrashDynamic2.GetVelocity()},
                               {"OpponentVelocityChange", crashInfo.postCrashDynamic2.GetVelocityChange()},
                               {"OpponentVelocityDirection", crashInfo.postCrashDynamic2.GetVelocityDirection()},
                               {"OpponentYawVelocity", crashInfo.postCrashDynamic2.GetYawVelocity()},
                               {"OpponentPointOfContactLocalX", crashInfo.postCrashDynamic2.GetPointOfContactLocal().x},
                               {"OpponentPointOfContactLocalY", crashInfo.postCrashDynamic2.GetPointOfContactLocal().y},
                               {"OpponentCollisionVelocity", crashInfo.postCrashDynamic2.GetCollisionVelocity()},
                               {"OpponentSliding", crashInfo.postCrashDynamic2.GetSliding()},
                               {"OYA", crashInfo.angles.OYA},
                               {"HCPAo", crashInfo.angles.HCPAo},
                               {"OCPAo", crashInfo.angles.OCPAo},
                               {"HCPA", crashInfo.angles.HCPA},
                               {"OCPA", crashInfo.angles.OCPA}});
    publisher->Publish(EventDefinitions::utils::GetAsString(event->GetCategory()), logEntry);
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
