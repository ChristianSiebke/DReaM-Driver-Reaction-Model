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
/** \file  EventNetwork.cpp */
//-----------------------------------------------------------------------------

#include "eventNetwork.h"

#include <algorithm>

using namespace EventDefinitions;

namespace core {

EventContainer EventNetwork::GetEvents(const EventCategory eventCategory)
{
    if (auto iter = events.find(eventCategory); iter != events.end())
    {
        return iter->second;
    }
    return {};
}

std::vector<EventInterface const *> EventNetwork::GetTrigger(const std::string &topic) const
{
    std::vector<EventInterface const *> matching_trigger;

    if (auto iter = trigger.find(topic); iter != trigger.end())
    {
        const auto &found_trigger = iter->second;
        matching_trigger.reserve(found_trigger.size());
        std::transform(found_trigger.begin(), found_trigger.end(),
                       std::back_inserter(matching_trigger), [](const auto &t) { return t.get(); });
    }
    return matching_trigger;
}

void EventNetwork::InsertTrigger(const std::string &identifier, std::unique_ptr<EventInterface> event)
{
    trigger[identifier].push_back(std::move(event));
}

void EventNetwork::InsertEvent(std::shared_ptr<EventInterface> event)
{
    events[event->GetCategory()].push_back(event);
    publisher.Publish(EventDefinitions::utils::GetAsString(event->GetCategory()),
                      openpass::publisher::LogEntry::FromEvent(event));
}

void EventNetwork::Clear()
{
    trigger.clear();
    events.clear();
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
} //namespace core
