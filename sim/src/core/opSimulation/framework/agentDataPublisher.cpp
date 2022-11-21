/********************************************************************************
 * Copyright (c) 2020-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "agentDataPublisher.h"

namespace openpass::publisher {

void AgentDataPublisher::Publish(const openpass::databuffer::Key &key, const openpass::databuffer::Value &value)
{
    dataBuffer->PutCyclic(agentId, key, value);
}

void AgentDataPublisher::Publish(const openpass::databuffer::Key &key, const openpass::databuffer::ComponentEvent &event)
{
    dataBuffer->PutAcyclic(agentId, key, Acyclic(key, agentId, event.parameter));
}

} // namespace openpass::publisher
