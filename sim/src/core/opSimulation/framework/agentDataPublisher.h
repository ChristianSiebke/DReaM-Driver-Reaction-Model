/********************************************************************************
 * Copyright (c) 2020-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "include/dataBufferInterface.h"
#include "include/publisherInterface.h"

namespace openpass::publisher {

///! Interface which has to be provided by observation modules
class AgentDataPublisher : public PublisherInterface
{
public:
    AgentDataPublisher(DataBufferWriteInterface *const dataBuffer, const int agentId) :
        PublisherInterface{dataBuffer},
        agentId{agentId}
    {
    }

    void Publish(const openpass::databuffer::Key &key, const openpass::databuffer::Value &value) override;

    void Publish(const openpass::databuffer::Key &key, const openpass::databuffer::ComponentEvent &event) override;

private:
    const int agentId;
};

} // namespace openpass::publisher
