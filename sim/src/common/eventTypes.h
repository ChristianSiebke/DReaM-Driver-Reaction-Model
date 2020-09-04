/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2020 BMW AG
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <map>
#include <string>

#include "common/openPassTypes.h"
#include "include/dataStoreInterface.h"

namespace openpass::narrator {

class EventBase
{
public:
    EventBase() = default;
    explicit EventBase(std::string name) :
        name{std::move(name)}
    {
    }
    EventBase(std::string name, TriggeringEntities triggeringEntities, AffectedEntities affectedEntities) :
        name{std::move(name)},
        triggeringEntities{std::move(triggeringEntities)},
        affectedEntities{std::move(affectedEntities)}
    {
    }

    std::string name;
    TriggeringEntities triggeringEntities;
    AffectedEntities affectedEntities;

    virtual operator Acyclic() const = 0;

    // We do not allow copy to force move semantics
    EventBase(const EventBase &) = delete;
    EventBase &operator=(const EventBase &) = delete;

    // Explicitly allow move
    EventBase(EventBase &&) = default;
    EventBase &operator=(EventBase &&) = default;

    // Neccesary evil
    virtual ~EventBase() = default;
};

} // namespace openpass::narrator

namespace EventDefinitions {

enum struct EventCategory
{
    OpenPASS,
    OpenSCENARIO
};

namespace utils {

static constexpr std::array<const char *, 2> EventCategoryMapping{
    "OpenPASS",
    "OpenSCENARIO"};

constexpr auto GetAsString(EventCategory category)
{
    return EventCategoryMapping[static_cast<size_t>(category)];
}

} // namespace utils

} //namespace EventDefinitions
