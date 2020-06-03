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

    // Events shall have it's own specialized members (e.g. TrajectoryFile for TrajectoryEvents)
    // Once we need it as acyclic, we convert it properly
    // Right now, this is not possible, as we internally use the "old" EventInterface
    [[deprecated("will be replaced by specializations of the type")]] openpass::datastore::Parameter parameter;
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
