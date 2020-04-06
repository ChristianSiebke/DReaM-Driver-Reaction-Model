/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "agentDataPublisher.h"

#include "../scheduler/timeKeeper.h" //TODO René/Reinhard: this is troublesome

namespace openpass::publisher {

void AgentDataPublisher::Publish(const openpass::datastore::Key &key, const openpass::datastore::Value &value)
{
    dataStore->PutCyclic(openpass::scheduling::TimeKeeper::Now(), agentId, key, value);
}

void AgentDataPublisher::Publish(const openpass::datastore::Key &key, const openpass::datastore::ComponentEvent &event)
{
    dataStore->PutAcyclic(openpass::scheduling::TimeKeeper::Now(), agentId, key, Acyclic(key, agentId, event.parameter));
}

} // namespace openpass::publisher
