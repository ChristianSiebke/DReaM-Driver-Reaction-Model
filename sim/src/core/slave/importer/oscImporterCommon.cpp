/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "oscImporterCommon.h"
#include "importerLoggingHelper.h"

namespace TAG = openpass::importer::xml::scenarioImporter::tag;
namespace ATTRIBUTE = openpass::importer::xml::scenarioImporter::attribute;

void Importer::ImportParameterElement(QDomElement& parameterElement, openScenario::Parameters& parameters)
{
    std::string parameterName;
    ThrowIfFalse(SimulationCommon::ParseAttribute(parameterElement, ATTRIBUTE::name, parameterName),
                 parameterElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

    std::string parameterType;
    ThrowIfFalse(SimulationCommon::ParseAttribute(parameterElement, ATTRIBUTE::parameterType, parameterType),
                 parameterElement, "Attribute " + std::string(ATTRIBUTE::parameterType) + " is missing.");

    if (parameterType == "bool")
    {
        bool parameterValueBool;
        ThrowIfFalse(SimulationCommon::ParseAttribute(parameterElement, ATTRIBUTE::value, parameterValueBool),
                     parameterElement, "Attribute " + std::string(ATTRIBUTE::value) + " is missing.");
        parameters.insert({parameterName, parameterValueBool});
    }
    else if (parameterType == "integer")
    {
        int parameterValueInt;
        ThrowIfFalse(SimulationCommon::ParseAttribute(parameterElement, ATTRIBUTE::value, parameterValueInt),
                     parameterElement, "Attribute " + std::string(ATTRIBUTE::value) + " is missing.");
        parameters.insert({parameterName, parameterValueInt});
    }
    else if (parameterType == "double")
    {
        double parameterValueDouble;
        ThrowIfFalse(SimulationCommon::ParseAttribute(parameterElement, ATTRIBUTE::value, parameterValueDouble),
                     parameterElement, "Attribute " + std::string(ATTRIBUTE::value) + " is missing.");
        parameters.insert({parameterName, parameterValueDouble});
    }
    else if (parameterType == "string")
    {
        std::string parameterValueString;
        ThrowIfFalse(SimulationCommon::ParseAttribute(parameterElement, ATTRIBUTE::value, parameterValueString),
                     parameterElement, "Attribute " + std::string(ATTRIBUTE::value) + " is missing.");
        parameters.insert({parameterName, parameterValueString});
    }
    else
    {
        ThrowIfFalse(false, parameterElement, "Unknown parameter type "+ parameterType);
    }
}

void Importer::ImportParameterDeclarationElement(QDomElement& parameterDeclarationElement, openScenario::Parameters& parameters)
{
    QDomElement parameterElement;
    if (SimulationCommon::GetFirstChildElement(parameterDeclarationElement, TAG::parameterDeclaration, parameterElement))
    {
        while (!parameterElement.isNull())
        {
            ImportParameterElement(parameterElement, parameters);
            parameterElement = parameterElement.nextSiblingElement(TAG::parameterDeclaration);
        }
    }
}
