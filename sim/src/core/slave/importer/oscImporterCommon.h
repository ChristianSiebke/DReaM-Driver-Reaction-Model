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

#include <QDomDocument>
#include <optional>

#include "common/xmlParser.h"
#include "common/log.h"
#include "common/openScenarioDefinitions.h"

namespace Importer
{

//! Parse an XML attribute from a .xosc file, which is either given by value or by referencing a parameter declared before
//!
//! \param element              xml element
//! \param attributeName        name of the attribute to parse
//! \param parameters           parameters declared by the ParameterDeclarations element
//! \param assignedParameters   if the element is from a referenced catalog, these are the parameters declared by the ParameterAssignments element
template<typename T>
T ParseAttribute(const QDomElement& element, const char attributeName[], openScenario::Parameters& parameters, const openScenario::Parameters& assignedParameters = {})
{
    std::string valueString;
    ThrowIfFalse(SimulationCommon::ParseAttribute(element, attributeName, valueString), element,
                 "Attribute " + std::string(attributeName) + " is missing");
    if constexpr (!std::is_same_v<T, std::string>)
    {
        ThrowIfFalse(!valueString.empty(), element,
                     "Attribute " + std::string(attributeName) + " is empty");
    }
    if (!valueString.empty() && valueString[0] == '$')
    {
        auto foundAssignedValue = assignedParameters.find(valueString.substr(1));
        if (foundAssignedValue != assignedParameters.cend())
        {
            ThrowIfFalse(std::holds_alternative<T>(foundAssignedValue->second), element,
                         "Parameter " + valueString + " has wrong type.");
            return std::get<T>(foundAssignedValue->second);
        }
        auto foundValue = parameters.find(valueString.substr(1));
        ThrowIfFalse(foundValue != parameters.end(), element,
                     "No parameter " + valueString + " defined.");
        ThrowIfFalse(std::holds_alternative<T>(foundValue->second), element,
                     "Parameter " + valueString + " has wrong type.");
        return std::get<T>(foundValue->second);
    }
    else
    {
        T value;
        SimulationCommon::ParseAttribute(element, attributeName, value);
        return value;
    }
}

//! Parse an optional XML attribute from a .xosc file, which is either given by value or by referencing a parameter declared before
//!
//! \param element              xml element
//! \param attributeName        name of the attribute to parse
//! \param parameters           parameters declared by the ParameterDeclarations element
//! \param assignedParameters   if the element is from a referenced catalog, these are the parameters declared by the ParameterAssignments element
template<typename T>
std::optional<T> ParseOptionalAttribute(const QDomElement& element, const char attributeName[], openScenario::Parameters& parameters, const openScenario::Parameters& assignedParameters = {})
{
    if(SimulationCommon::HasAttribute(element, attributeName))
    {
        return ParseAttribute<T>(element, attributeName, parameters);
    }

    return std::nullopt;
}

//! Parse an XML attribute from a .xosc file, which is either given by value or by reference a parameter declared before.
//! This variant is for attributes in catalog that may later be overruled by parameter assignments
//!
//! \param element              xml element
//! \param attributeName        name of the attribute to parse
//! \param parameters           parameters declared by the ParameterDeclarations element
template<typename T>
openScenario::ParameterizedAttribute<T> ParseParametrizedAttribute(const QDomElement& element, const char attributeName[], const openScenario::Parameters& defaultParameters)
{
    std::string valueString;
    ThrowIfFalse(SimulationCommon::ParseAttribute(element, attributeName, valueString), element,
                 "Attribute " + std::string(attributeName) + " is missing");
    if constexpr (!std::is_same_v<T, std::string>)
    {
        ThrowIfFalse(!valueString.empty(), element,
                     "Attribute " + std::string(attributeName) + " is empty");
    }
    if (!valueString.empty() && valueString.at(0) == '$')
    {
        auto foundDefaultValue = defaultParameters.find(valueString.substr(1));
        ThrowIfFalse(foundDefaultValue != defaultParameters.end(), element,
                     "No parameter " + valueString + " defined.");
        ThrowIfFalse(std::holds_alternative<T>(foundDefaultValue->second), element,
                     "Parameter " + valueString + " has wrong type.");
        return {valueString.substr(1), std::get<T>(foundDefaultValue->second)};
    }
    else
    {
        T value;
        SimulationCommon::ParseAttribute(element, attributeName, value);
        return value;
    }
}

//! Import a ParameterElement from an .xosc file
//!
//! \param parameterElement     element to parse
//! \param parameters           map where result is stored
void ImportParameterElement(QDomElement& parameterElement, openScenario::Parameters& parameters);


//! Import a ParameterDeclarationElement from an .xosc file
//!
//! \param parameterDeclarationElement  element to parse
//! \param parameters                   map where result is stored
void ImportParameterDeclarationElement(QDomElement& parameterDeclarationElement, openScenario::Parameters& parameters);
}
