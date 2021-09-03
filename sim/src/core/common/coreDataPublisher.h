/*******************************************************************************
* Copyright (c) 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "common/logEntryBase.h"
#include "include/publisherInterface.h"

namespace openpass::publisher {

static inline constexpr int CORE_ID = 67798269; // ASCII: C O R E

class LogEntry : public LogEntryBase
{
public:
    LogEntry(std::string name) :
        LogEntryBase(std::move(name))
    {
    }

    template <typename BasicEvent>
    static inline LogEntry FromEvent(const std::shared_ptr<BasicEvent> &event)
    {
        LogEntry logEntry(event->GetName());
        logEntry.triggeringEntities = event->triggeringAgents;
        logEntry.affectedEntities = event->actingAgents;
        logEntry.parameter = event->GetParameter();
        return logEntry;
    }

    operator Acyclic() const noexcept override
    {
        return Acyclic(name, triggeringEntities, affectedEntities, parameter);
    }

    openpass::databuffer::Parameter parameter;
};

///! Interface which has to be provided by observation modules
class CoreDataPublisher : public PublisherInterface
{
public:
    CoreDataPublisher(DataBufferWriteInterface *const dataBuffer) :
        PublisherInterface(dataBuffer)
    {
    }

    void Publish(const openpass::databuffer::Key &key, const openpass::publisher::LogEntryBase &event) override;
};

} // namespace openpass::publisher
