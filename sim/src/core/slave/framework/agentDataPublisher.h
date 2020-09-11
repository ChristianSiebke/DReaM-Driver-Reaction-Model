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

#include "include/dataStoreInterface.h"
#include "include/publisherInterface.h"

namespace openpass::publisher {

///! Interface which has to be provided by observation modules
class AgentDataPublisher : public PublisherInterface
{
public:
    AgentDataPublisher(DataStoreWriteInterface *const dataStore, const int agentId) :
        PublisherInterface{dataStore},
        agentId{agentId}
    {
    }

    void Publish(const openpass::datastore::Key &key, const openpass::datastore::Value &value) override;

    void Publish(const openpass::datastore::Key &key, const openpass::datastore::ComponentEvent &event) override;

private:
    const int agentId;
};

} // namespace openpass::publisher
