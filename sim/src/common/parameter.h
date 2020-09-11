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

#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "common/stochasticDefinitions.h"

namespace openpass::parameter {

namespace internal {

using ParameterKey = std::string;
using ParameterValue = std::variant<
                    bool,
                    std::vector<bool>,
                    int,
                    std::vector<int>,
                    double,
                    std::vector<double>,
                    std::string,
                    std::vector<std::string>,
                    StochasticDistribution>;

/// @brief the elementary type of a container, in a sense such as std::vector<std::pair<key, value>>
using ParameterElementLevel3 = std::pair<internal::ParameterKey, internal::ParameterValue>;
/// @brief set of parameter level 3 elements
using ParameterSetLevel3 = std::vector<internal::ParameterElementLevel3>;

/// @brief list of parameter sets level 3
using ParameterListLevel2 = std::vector<internal::ParameterSetLevel3>;

/// @brief level 2 parameter elements are key value pairs, where the value can either be a ParameterValue or a ParameterList level 2
using ParameterElementLevel2 = std::pair<internal::ParameterKey, std::variant<internal::ParameterValue, internal::ParameterListLevel2>>;

/// @brief set of parameter level 2 elements
using ParameterSetLevel2 = std::vector<ParameterElementLevel2>;

/// @brief list of parameter sets level 2
using ParameterListLevel1 = std::vector<internal::ParameterSetLevel2>;

} // namespace internal

/// @brief use to collect ParameterValues or ParameterLists (only one layer of lists is allowed)
using ParameterSetLevel1 = std::vector<std::pair<internal::ParameterKey, std::variant<internal::ParameterValue, internal::ParameterListLevel2, internal::ParameterListLevel1>>>;

//! Query the internal parameter set of a parameter list for a specific token
//! @param param       the internal parameter set
//! @param searchToken the search token
//! @return value or std::nullopt
template <typename T>
static std::optional<T> Get(const internal::ParameterSetLevel3& param, const internal::ParameterKey& searchToken)
{
    for (const auto& [key, value] : param)
    {
        if (searchToken == key && std::holds_alternative<T>(value))
        {
            return std::get<T>(value);
        }
    }

    return std::nullopt;
}

template <typename T>
static std::optional<T> Get(const internal::ParameterSetLevel2& container, const internal::ParameterKey& searchToken)
{
    for (const auto& [key, value] : container)
    {
        if (searchToken == key)
        {
            if constexpr (std::is_same_v<T, internal::ParameterListLevel2>)
            {
                if (std::holds_alternative<T>(value))
                {
                    return std::get<T>(value);
                }
            }
            else if (std::holds_alternative<T>(std::get<internal::ParameterValue>(value)))
            {
                    return std::get<T>(std::get<internal::ParameterValue>(value));
            }
        }
    }

    return std::nullopt;
}

//! Query a parameter container for a specific token
//! @param container       the parameter container
//! @param searchToken the search token
//! @return value or std::nullopt
template <typename T>
static std::optional<T> Get(const ParameterSetLevel1& container, const internal::ParameterKey& searchToken)
{
    for (const auto& [key, value] : container)
    {
        if (searchToken == key)
        {
            if constexpr (std::is_same_v<T, internal::ParameterListLevel1> || std::is_same_v<T, internal::ParameterListLevel2>)
            {
                if (std::holds_alternative<T>(value))
                {
                    return std::get<T>(value);
                }
            }
            else if (std::holds_alternative<T>(std::get<internal::ParameterValue>(value)))
            {
                    return std::get<T>(std::get<internal::ParameterValue>(value));
            }
        }
    }

    return std::nullopt;
}

} // namespace openpass::parameter
