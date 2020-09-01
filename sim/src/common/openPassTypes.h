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

#include <functional>
#include <string>
#include <variant>

#include "namedType.h"

namespace openpass::type {

// TODO: MOVE TO CORE ONLY!
static constexpr int CoreId = 67798269; // ASCII: C O R E

using AgentId = int;
using EntityId = NamedType<AgentId, struct EntityIdType>;
using Timestamp = NamedType<int, struct TimestampType>;
using EntityIds = std::vector<EntityId>;

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
