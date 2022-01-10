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
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

template <class... Ts>
struct overload : Ts...
{
    using Ts::operator()...;
};

template <class... Ts>
overload(Ts...)->overload<Ts...>;

namespace openpass::utils {

namespace vector {

/// \brief  convenience function to convert a vector into a string using a constom delimiter
/// \remark Please refer to the standard, how decimals are converted
/// \param  values     vector of strings
/// \param  delimiter  string in between the individual values
/// \return delimiter seperated list as string
template <typename T>
std::string to_string(const std::vector<T> &values, const std::string &delimiter = ",")
{
    if (values.empty())
    {
        return "";
    }

    std::ostringstream oss;
    std::copy(values.begin(), values.end(), std::ostream_iterator<T>(oss, delimiter.c_str()));

    return {oss.str(), 0, oss.str().length() - delimiter.size()};
}

} // namespace vector

namespace FlatParameter {

/// \brief visitor for parameter values of events
/// Converts the visitor values into strings and forwards the result to writer callable
/// Values in vectors are converted and seperated by a delimiter.
/// If value is 12.34 => writer("12.34");
/// If value is {12.34, 23.45} => writer("12.34,23.45");
/// \see openpass::type::FlatParameterValue
/// \see openpass::utils::vector::to_string
/// \param[in] writer  callable collecting the converted value
/// \param[in] delimiter used for vector type (default = ",")
[[maybe_unused]] static auto to_string(std::function<void(std::string)> writer, const std::string &delimiter = ",")
{
    return overload {
        [&](const std::string &value) { writer(value); },
        [&](auto &value) {
            if constexpr (std::is_arithmetic_v<std::decay_t<decltype(value)>>)
            {
                writer(std::to_string(value));
            }
            else // path for std::vector types
            {
                if (!value.empty())
                {
                    writer(vector::to_string(value, delimiter));
                }
            }
        }
    };
}

/// \brief visitor for parameter values of events
/// Converts the visitor values into strings and returns the result.
/// Values in vectors are converted and seperated by a delimiter.
/// If value is 12.34 => writer("12.34");
/// If value is {12.34, 23.45} => return value is "12.34,23.45"
/// \see openpass::type::FlatParameterValue
/// \see openpass::utils::vector::to_string
/// \param[in] delimiter used for vector type (default = ",")
[[maybe_unused]] static auto to_string(const std::string &delimiter = ",")
{
    return overload {
        [&](const std::string &value) { return value; },
        [&](auto &value) {
            if constexpr (std::is_arithmetic_v<std::decay_t<decltype(value)>>)
            {
                return std::to_string(value);
            }
            else // path for std::vector types
            {
                return value.empty() ? "" : vector::to_string(value, delimiter);
            }
        }
    };
}


} // namespace FlatParameter

} // namespace openpass::utils
