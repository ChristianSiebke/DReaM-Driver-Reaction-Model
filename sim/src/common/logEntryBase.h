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

#include "common/openPassTypes.h"
#include "include/dataBufferInterface.h"

namespace openpass::publisher {

/**
 * @brief Base class for logging of acyclic events in the dataBuffer
 * @see   openpass::publisher::coreDataPublisher
 * @see   openpass::publisher::LogEntry
 */
class LogEntryBase
{
public:
    LogEntryBase() = default;
    explicit LogEntryBase(std::string name) :
        name{std::move(name)}
    {
    }

    LogEntryBase(std::string name, openpass::type::TriggeringEntities triggeringEntities, openpass::type::AffectedEntities affectedEntities) :
        name{std::move(name)},
        triggeringEntities{std::move(triggeringEntities)},
        affectedEntities{std::move(affectedEntities)}
    {
    }

    std::string name;
    openpass::type::TriggeringEntities triggeringEntities;
    openpass::type::AffectedEntities affectedEntities;

    virtual operator openpass::databuffer::Acyclic() const = 0;

    // We do not allow copy to force move semantics
    LogEntryBase(const LogEntryBase &) = delete;
    LogEntryBase &operator=(const LogEntryBase &) = delete;

    // Explicitly allow move
    LogEntryBase(LogEntryBase &&) = default;
    LogEntryBase &operator=(LogEntryBase &&) = default;

    // Neccesary evil
    virtual ~LogEntryBase() = default;
};

} // namespace openpass::publisher
