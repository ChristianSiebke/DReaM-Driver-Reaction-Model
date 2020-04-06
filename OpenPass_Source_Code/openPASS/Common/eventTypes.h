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

#include "Common/openPassTypes.h"
#include "Interfaces/dataStoreInterface.h" // TODO: RP: make datastore types (Acyclic, TriggeringEntites,...) explicitly available

namespace openpass::narrator {

class EventBase
{
public:
    EventBase() = default;
    EventBase(std::string name) :
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

    virtual explicit operator Acyclic() const = 0;

    // We do not allow copy to force move semantics
    EventBase(const EventBase &) = delete;
    EventBase &operator=(const EventBase &) = delete;

    // Explicitly allow move
    EventBase(EventBase &&) = default;
    EventBase &operator=(EventBase &&) = default;

    // Neccesary evil
    virtual ~EventBase() = default;
};

}

namespace EventDefinitions {

enum struct EventCategory
{
    Basic,                // deprecated
    Conditional,          // deprecated
    Collision,            // deprecated
    ComponentStateChange, // deprecated
    LaneChange,           // deprecated
    CustomLaneChange,     // deprecated ?
    SetTrajectory,        // deprecated ?
    SetGazeFollower,      // deprecated ?
    VehicleComponent,     // deprecated
    OpenPASS,
    OpenSCENARIO
};

namespace helper {

static constexpr std::array<const char *, 11> EventCategoryMapping{
    "Basic",
    "Conditional",
    "Collision",
    "ComponentStateChange",
    "LaneChange",
    "CustomLaneChange",
    "SetTrajectory",
    "SetGazeFollower",
    "VehicleComponent",
    "OpenPASS",
    "OpenSCENARIO"};

constexpr auto GetAsString(EventCategory category)
{
    return EventCategoryMapping[static_cast<size_t>(category)];
}

} // namespace helper

} //namespace EventDefinitions
