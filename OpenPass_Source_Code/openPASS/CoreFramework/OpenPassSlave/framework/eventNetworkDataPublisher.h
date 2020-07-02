/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "Interfaces/publisherInterface.h"

namespace openpass::narrator {

class Event : public EventBase
{
public:
    Event(std::string name) :
        EventBase(std::move(name))
    {
    }

    explicit operator Acyclic() const noexcept override
    {
        return Acyclic(name, triggeringEntities, affectedEntities, parameter);
    }

    openpass::datastore::Parameter parameter;
};

} // namespace openpass::narrator

namespace openpass::publisher {

///! Interface which has to be provided by observation modules
class EventNetworkDataPublisher : public PublisherInterface
{
public:
    EventNetworkDataPublisher(DataStoreWriteInterface *const dataStore) :
        PublisherInterface(dataStore)
    {
    }

    void Publish(const openpass::datastore::Key &key, const openpass::narrator::EventBase &event) override;
};

} // namespace openpass::publisher
