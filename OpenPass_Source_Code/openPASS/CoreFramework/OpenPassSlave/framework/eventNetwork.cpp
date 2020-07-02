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

Events *EventNetwork::GetActiveEvents()
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
    if (iterator == activeEvents.end())
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
    for (Events::iterator iterator = archivedEvents.begin(); iterator != archivedEvents.end(); iterator++)
    {
        while ((*iterator).second.front()->GetEventTime() < time)
        {
            (*iterator).second.pop_front();
        }
    }
}

// This glue logic is only an intermediate state, as we intend to remove EventInterface in the future
void EventNetwork::Log(const std::shared_ptr<EventInterface> &event)
{
    openpass::narrator::Event narratorEvent(event->GetName());

    // TODO: Named type issue (assigment missing)
    for (auto agent : event->GetTriggeringAgents())
    {
        narratorEvent.triggeringEntities.entities.push_back(agent);
    }

    // TODO: Named type issue (assigment missing)
    for (auto agent : event->GetActingAgents())
    {
        narratorEvent.affectedEntities.entities.push_back(agent);
    }

    narratorEvent.parameter = event->GetParameter();

    publisher.Publish(EventDefinitions::helper::GetAsString(event->GetCategory()), narratorEvent);
}

void EventNetwork::InsertEvent(std::shared_ptr<EventInterface> event)
{
    event->SetId(eventId);
    eventId++;

    activeEvents[event->GetCategory()].push_back(event);

    // This filter is currently necessary, as manipulators fire events too (which shall not be logged anymore)
    if (event->GetCategory() == EventDefinitions::EventCategory::OpenSCENARIO ||
        event->GetCategory() == EventDefinitions::EventCategory::OpenPASS)
    {
        Log(event);
    }
}

void EventNetwork::ClearActiveEvents()
{
    for (std::pair<EventCategory, std::list<std::shared_ptr<EventInterface>>> eventMapEntry : activeEvents)
    {
        if (archivedEvents.find(eventMapEntry.first) != archivedEvents.end())
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

    runResult = nullptr;
}

void EventNetwork::AddCollision(const int agentId)
{
    if (runResult != nullptr)
    {
        runResult->AddCollisionId(agentId);
    }
}

void EventNetwork::Initialize(RunResultInterface *runResult)
{
    this->runResult = runResult;
}
} //namespace SimulationSlave
