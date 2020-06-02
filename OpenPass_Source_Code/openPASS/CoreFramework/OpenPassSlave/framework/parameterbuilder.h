/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "Common/runtimeInformation.h"
#include "Interfaces/parameterInterface.h"
#include "Common/parameter.h"

#include <memory>

namespace openpass::parameter::internal {

/// transforms raw parameters into the corresponding paramter interface add method
static void TransformParameterValue(ParameterInterface* param, const std::string& key, const ParameterValue& value)
{
    if (std::holds_alternative<bool>(value)) { param->AddParameterBool(key, std::get<bool>(value)); }
    else if (std::holds_alternative<int>(value)) { param->AddParameterInt(key, std::get<int>(value)); }
    else if (std::holds_alternative<double>(value)) { param->AddParameterDouble(key, std::get<double>(value)); }
    else if (std::holds_alternative<std::string>(value)) { param->AddParameterString(key, std::get<std::string>(value)); }
    else if (std::holds_alternative<std::vector<bool>>(value)) { param->AddParameterBoolVector(key, std::get<std::vector<bool>>(value)); }
    else if (std::holds_alternative<std::vector<int>>(value)) { param->AddParameterIntVector(key, std::get<std::vector<int>>(value)); }
    else if (std::holds_alternative<std::vector<double>>(value)) { param->AddParameterDoubleVector(key, std::get<std::vector<double>>(value)); }
    else if (std::holds_alternative<std::vector<std::string>>(value)) { param->AddParameterStringVector(key, std::get<std::vector<std::string>>(value)); }
    else if (std::holds_alternative<NormalDistribution>(value)) { param->AddParameterNormalDistribution(key, std::get<NormalDistribution>(value)); }
    else throw std::runtime_error("unable to transform parameter type");

}

/// switches between flat parameters and parameter lists
struct VisitParameterElement
{
    const ParameterKey key;
    ParameterInterface* param;

    VisitParameterElement(const ParameterKey key, ParameterInterface* param) : key{key}, param{param}
    {}

    /// execute if visited variant holds a ParameterValue
    void operator()(const ParameterValue& value)
    {
        TransformParameterValue(param, key, value);
    }

    /// execute if visited variant holds a ParameterList
    void operator()(const ParameterList& list)
    {
        for (const auto& parameterSet : list)
        {
            auto& sub_param = param->InitializeListItem(key);
            for (const auto& [sub_key, sub_value] : parameterSet)
            {
                TransformParameterValue(&sub_param, sub_key, sub_value);
            }
        }
    }
};

} // namespace openpass::parameter::internal

namespace openpass::parameter {

//! @brief Takes a parameter set and transforms it into an unqiue parameter interface
//! @param runtimeInformation    Common runtimeInformation
//! @param container             The parameter set
//! @returns Unique pointer to a new parameter interface
template <typename T>
std::unique_ptr<T> make(const openpass::common::RuntimeInformation& runtimeInformation, const Container& container)
{
    auto parameter = std::make_unique<T>(runtimeInformation);

    for (const auto& [key, value] : container)
    {
        internal::VisitParameterElement visitor(key, parameter.get());
        std::visit(visitor, value);
    }

    return std::move(parameter);
}

} // namespace openpass::parameter
