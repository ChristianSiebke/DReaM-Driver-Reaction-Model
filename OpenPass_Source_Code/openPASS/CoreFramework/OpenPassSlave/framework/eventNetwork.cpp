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
/** \file  EventNetwork.cpp */
//-----------------------------------------------------------------------------

#include "eventNetwork.h"

using namespace EventDefinitions;

namespace SimulationSlave {

EventNetwork::EventNetwork()
{
}

EventNetwork::~EventNetwork()
{
    activeEvents.clear();
    archivedEvents.clear();
}

Events* EventNetwork::GetActiveEvents()
{
    return &activeEvents;
}

Events *EventNetwork::GetArchivedEvents()
{
    return &archivedEvents;
}

EventContainer EventNetwork::GetActiveEventCategory(const EventCategory eventCategory)
{
    auto iterator = activeEvents.find(eventCategory);
    if(iterator == activeEvents.end())
    {
        return {};
    }
    else
    {
        return iterator->second;
    }
}

void EventNetwork::RemoveOldEvents(int time)
{
    for(Events::iterator iterator = archivedEvents.begin(); iterator != archivedEvents.end(); iterator++)
    {
        while((*iterator).second.front()->GetEventTime() < time)
        {
            (*iterator).second.pop_front();
        }
    }
}

void EventNetwork::InsertEvent(std::shared_ptr<EventInterface> event)
{
    event->SetId(eventId);
    eventId++;

    activeEvents[event->GetCategory()].push_back(event);
}

void EventNetwork::ClearActiveEvents()
{
    for(std::pair<EventCategory, std::list<std::shared_ptr<EventInterface>>> eventMapEntry : activeEvents)
    {
        if(archivedEvents.find(eventMapEntry.first) != archivedEvents.end())
        {
            std::list<std::shared_ptr<EventInterface>> *eventList = &(archivedEvents.at(eventMapEntry.first));
            eventList->insert(eventList->end(), eventMapEntry.second.begin(), eventMapEntry.second.end());
        }
        else
        {
            archivedEvents.insert(eventMapEntry);
        }
    }

    activeEvents.clear();
}

void EventNetwork::Clear()
{
    eventId = 0;

    activeEvents.clear();
    archivedEvents.clear();

    observer = nullptr;
    runResult = nullptr;
}

void EventNetwork::AddCollision(const int agentId)
{
    if(runResult != nullptr)
    {
        runResult->AddCollisionId(agentId);
    }
}

void EventNetwork::Initialize(RunResultInterface *runResult,
                              ObservationInterface* observer)
{
    this->observer = observer;
    this->runResult = runResult;
}
} //namespace SimulationSlave
