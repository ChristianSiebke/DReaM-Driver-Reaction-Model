/********************************************************************************
 * Copyright (c) 2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <functional>
#include <string>
#include <variant>

#include "namedType.h"

namespace openpass::type {

using AgentId = int;
using EntityId = NamedType<AgentId, struct EntityIdType>;
using Timestamp = NamedType<int, struct TimestampType>;
using EntityIds = std::vector<EntityId>;

/*!
 * \brief Contains the triggering entities of an acyclic
 *
 *  Might be enhanced in the future to hold additional information about the affected entities
 */
struct TriggeringEntities
{
    type::EntityIds entities;
};

/*!
 * \brief Contains the affected entities of an acyclic
 *
 *  Might be enhanced in the future to hold additional information about the affected entities
 */
struct AffectedEntities
{
    type::EntityIds entities;
};


using FlatParameterKey = std::string;
using FlatParameterValue = std::variant<
    bool, std::vector<bool>,
    char, std::vector<char>,
    int, std::vector<int>,
    size_t, std::vector<size_t>,
    float, std::vector<float>,
    double, std::vector<double>,
    std::string, std::vector<std::string>>;

using FlatParameter = std::unordered_map<FlatParameterKey, FlatParameterValue>;

} // namespace openpass::type
