/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "parameterImporter.h"
#include "importerLoggingHelper.h"
#include "CoreFramework/CoreShare/log.h"
#include "CoreFramework/CoreShare/xmlParser.h"

namespace TAG = openpass::importer::xml::parameterImporter::tag;
namespace ATTRIBUTE = openpass::importer::xml::parameterImporter::attribute;

namespace openpass::parameter::internal {

using namespace SimulationCommon;

template <typename T>
ParameterSet ImportParameter(QDomElement domElement, const std::string& elementName)
{
    ParameterSet param;

    QDomElement parameterElement;
    if (GetFirstChildElement(domElement, elementName, parameterElement))
    {
        while (!parameterElement.isNull())
        {
            std::string parameterName;
            T parameterValue;

            ThrowIfFalse(ParseAttributeString(parameterElement, ATTRIBUTE::key, parameterName),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::key) + " is missing.");
            ThrowIfFalse(ParseAttribute<T>(parameterElement, ATTRIBUTE::value, parameterValue),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::value) + " is missing or of wrong type");

            param.emplace_back(parameterName, parameterValue);
            parameterElement = parameterElement.nextSiblingElement(QString::fromStdString(elementName));
        }
    }

    return param;
}

template <>
ParameterSet ImportParameter<NormalDistribution>(QDomElement domElement, const std::string& elementName)
{
    ParameterSet param;

    QDomElement parameterElement;
    if (SimulationCommon::GetFirstChildElement(domElement, elementName, parameterElement))
    {
        while (!parameterElement.isNull())
        {
            std::string parameterName;
            NormalDistribution parameterValue;

            ThrowIfFalse(ParseAttributeString(parameterElement, ATTRIBUTE::key, parameterName),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::key) + " is missing.");
            ThrowIfFalse(ParseAttribute(parameterElement, ATTRIBUTE::mean, parameterValue.mean),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::mean) + " is missing or of wrong type");
            ThrowIfFalse(ParseAttribute(parameterElement, ATTRIBUTE::sd, parameterValue.standardDeviation),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::sd) + " is missing or of wrong type");
            ThrowIfFalse(ParseAttribute(parameterElement, ATTRIBUTE::min, parameterValue.min),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::min) + " is missing or of wrong type");
            ThrowIfFalse(ParseAttribute(parameterElement, ATTRIBUTE::max, parameterValue.max),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::max) + " is missing or of wrong type");

            param.emplace_back(parameterName, parameterValue);
            parameterElement = parameterElement.nextSiblingElement(QString::fromStdString(elementName));
        }
    }

    return param;
}

static ParameterSet ImportParameterFlat(QDomElement domElement)
{
    ParameterSet param;

    auto boolParams = ImportParameter<bool>(domElement, "Bool");
    auto intParams = ImportParameter<int>(domElement, "Int");
    auto intVectorParams = ImportParameter<std::vector<int>>(domElement, "IntVector");
    auto doubleParams = ImportParameter<double>(domElement, "Double");
    auto doubleVectorParams = ImportParameter<std::vector<double>>(domElement, "DoubleVector");
    auto stringParams = ImportParameter<std::string>(domElement, "String");
    auto normalDistributionParams = ImportParameter<openpass::parameter::NormalDistribution>(domElement, "NormalDistribution");

    param.insert(param.end(), boolParams.begin(), boolParams.end());
    param.insert(param.end(), intParams.begin(), intParams.end());
    param.insert(param.end(), intVectorParams.begin(), intVectorParams.end());
    param.insert(param.end(), doubleParams.begin(), doubleParams.end());
    param.insert(param.end(), doubleVectorParams.begin(), doubleVectorParams.end());
    param.insert(param.end(), stringParams.begin(), stringParams.end());
    param.insert(param.end(), normalDistributionParams.begin(), normalDistributionParams.end());

    return param;
}

static Container ImportParameterLists(QDomElement parameterElement)
{
    QDomElement lists;

    Container param;

    if (SimulationCommon::GetFirstChildElement(parameterElement, "List", lists))
    {
        while (!lists.isNull())
        {
            std::string name;
            ThrowIfFalse(ParseAttributeString(lists, ATTRIBUTE::name, name),
                         lists, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

            QDomElement listItem;
            if (SimulationCommon::GetFirstChildElement(lists, "ListItem", listItem))
            {
                ParameterList parameterList;
                while (!listItem.isNull())
                {
                    parameterList.emplace_back(ImportParameterFlat(listItem));
                    listItem = listItem.nextSiblingElement("ListItem");
                }
                param.emplace_back(name, parameterList);
            }

            lists = lists.nextSiblingElement("List");
        }
    }

    return param;
}

} // namespace openpass::parameter::internal

namespace openpass::parameter {

openpass::parameter::Container Import(QDomElement parameterElement)
{
    auto parameterSet = openpass::parameter::internal::ImportParameterFlat(parameterElement);
    auto parameterLists = openpass::parameter::internal::ImportParameterLists(parameterElement);

    openpass::parameter::Container param;
    param.insert(param.end(), parameterSet.begin(), parameterSet.end());
    param.insert(param.end(), parameterLists.begin(), parameterLists.end());

    return param;
}

} // openpass::parameter
