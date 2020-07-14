/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
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
#include <cmath>

namespace TAG = openpass::importer::xml::parameterImporter::tag;
namespace ATTRIBUTE = openpass::importer::xml::parameterImporter::attribute;

namespace openpass::parameter::internal {

using namespace SimulationCommon;

template <typename T>
ParameterSetLevel3 ImportParameter(const QDomElement& domElement, const std::string& elementName)
{
    ParameterSetLevel3 param;

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
ParameterSetLevel3 ImportParameter<NormalDistribution>(const QDomElement& domElement, const std::string& elementName)
{
    ParameterSetLevel3 param;

    QDomElement parameterElement;
    if (SimulationCommon::GetFirstChildElement(domElement, elementName, parameterElement))
    {
        while (!parameterElement.isNull())
        {
            std::string parameterName;
            double min, max, mean, sd;

            ThrowIfFalse(ParseAttributeString(parameterElement, ATTRIBUTE::key, parameterName),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::key) + " is missing.");
            ThrowIfFalse(ParseAttribute(parameterElement, ATTRIBUTE::min, min),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::min) + " is missing or of wrong type");
            ThrowIfFalse(ParseAttribute(parameterElement, ATTRIBUTE::max, max),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::max) + " is missing or of wrong type");

            if (!ParseAttribute(parameterElement, ATTRIBUTE::mean, mean)
                || !ParseAttribute(parameterElement, ATTRIBUTE::sd, sd))
            {
                ThrowIfFalse(ParseAttribute(parameterElement, ATTRIBUTE::mu, mean)
                             && ParseAttribute(parameterElement, ATTRIBUTE::sigma,sd),
                             parameterElement, std::string("Either attribute ") + ATTRIBUTE::mu + " and " + ATTRIBUTE::sigma + " or "
                             + ATTRIBUTE::mean + " and " + ATTRIBUTE::sd + " required for NormalDistribution");
            }

            param.emplace_back(parameterName, NormalDistribution{mean, sd, min, max});
            parameterElement = parameterElement.nextSiblingElement(QString::fromStdString(elementName));
        }
    }

    return param;
}

template <>
ParameterSetLevel3 ImportParameter<LogNormalDistribution>(const QDomElement& domElement, const std::string& elementName)
{
    ParameterSetLevel3 param;

    QDomElement parameterElement;
    if (SimulationCommon::GetFirstChildElement(domElement, elementName, parameterElement))
    {
        while (!parameterElement.isNull())
        {
            std::string parameterName;
            double min, max, mean, sd{}, mu, sigma{};

            ThrowIfFalse(ParseAttributeString(parameterElement, ATTRIBUTE::key, parameterName),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::key) + " is missing.");
            ThrowIfFalse(ParseAttribute(parameterElement, ATTRIBUTE::min, min),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::min) + " is missing or of wrong type");
            ThrowIfFalse(ParseAttribute(parameterElement, ATTRIBUTE::max, max),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::max) + " is missing or of wrong type");
            if (ParseAttribute(parameterElement, ATTRIBUTE::mu, mu)
                && ParseAttribute(parameterElement, ATTRIBUTE::sigma, sigma))
            {
                param.emplace_back(parameterName, LogNormalDistribution{mu, sigma, min, max});
            }
           else {
                ThrowIfFalse(ParseAttribute(parameterElement, ATTRIBUTE::mean, mean)
                             && ParseAttribute(parameterElement, ATTRIBUTE::sd, sd),
                             parameterElement, "Either attribute " + std::string(ATTRIBUTE::mu) + " and " + std::string(ATTRIBUTE::sigma) + " or "
                             + std::string(ATTRIBUTE::mean) + " and " + std::string(ATTRIBUTE::sd) + " required for LogNormalDistribution");

                param.emplace_back(parameterName, LogNormalDistribution::CreateWithMeanSd(mean, sd, min, max));

            }

            parameterElement = parameterElement.nextSiblingElement(QString::fromStdString(elementName));
        }
    }

    return param;
}

template <>
ParameterSetLevel3 ImportParameter<UniformDistribution>(const QDomElement& domElement, const std::string& elementName)
{
    ParameterSetLevel3 param;

    QDomElement parameterElement;
    if (SimulationCommon::GetFirstChildElement(domElement, elementName, parameterElement))
    {
        while (!parameterElement.isNull())
        {
            std::string parameterName;
            double min, max;

            ThrowIfFalse(ParseAttributeString(parameterElement, ATTRIBUTE::key, parameterName),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::key) + " is missing.");
            ThrowIfFalse(ParseAttribute(parameterElement, ATTRIBUTE::min, min),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::min) + " is missing or of wrong type");
            ThrowIfFalse(ParseAttribute(parameterElement, ATTRIBUTE::max, max),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::max) + " is missing or of wrong type");

            param.emplace_back(parameterName, UniformDistribution{min, max});
            parameterElement = parameterElement.nextSiblingElement(QString::fromStdString(elementName));
        }
    }

    return param;
}

template <>
ParameterSetLevel3 ImportParameter<ExponentialDistribution>(const QDomElement& domElement, const std::string& elementName)
{
    ParameterSetLevel3 param;

    QDomElement parameterElement;
    if (SimulationCommon::GetFirstChildElement(domElement, elementName, parameterElement))
    {
        while (!parameterElement.isNull())
        {
            std::string parameterName;
            double min, max, lambda, mean{};

            ThrowIfFalse(ParseAttributeString(parameterElement, ATTRIBUTE::key, parameterName),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::key) + " is missing.");
            ThrowIfFalse(ParseAttribute(parameterElement, ATTRIBUTE::min, min),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::min) + " is missing or of wrong type");
            ThrowIfFalse(ParseAttribute(parameterElement, ATTRIBUTE::max, max),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::max) + " is missing or of wrong type");

            if (ParseAttribute(parameterElement, ATTRIBUTE::lambda, lambda))
            {
                param.emplace_back(parameterName, ExponentialDistribution{lambda, min, max});
            }
            else
            {
                ThrowIfFalse(ParseAttribute(parameterElement, ATTRIBUTE::mean, mean),
                             parameterElement, "Either attribute " + std::string(ATTRIBUTE::lambda) + " or "
                             + std::string(ATTRIBUTE::mean) + " required for ExponentialDistribution");
                param.emplace_back(parameterName, ExponentialDistribution{1 / mean, min, max});
            }

            parameterElement = parameterElement.nextSiblingElement(QString::fromStdString(elementName));
        }
    }

    return param;
}

template <>
ParameterSetLevel3 ImportParameter<GammaDistribution>(const QDomElement& domElement, const std::string& elementName)
{
    ParameterSetLevel3 param;

    QDomElement parameterElement;
    if (SimulationCommon::GetFirstChildElement(domElement, elementName, parameterElement))
    {
        while (!parameterElement.isNull())
        {
            std::string parameterName;
            double min, max, mean, sd{}, shape, scale{};

            ThrowIfFalse(ParseAttributeString(parameterElement, ATTRIBUTE::key, parameterName),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::key) + " is missing.");
            ThrowIfFalse(ParseAttribute(parameterElement, ATTRIBUTE::min, min),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::min) + " is missing or of wrong type");
            ThrowIfFalse(ParseAttribute(parameterElement, ATTRIBUTE::max, max),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::max) + " is missing or of wrong type");
            if (ParseAttribute(parameterElement, ATTRIBUTE::shape, shape)
                && ParseAttribute(parameterElement, ATTRIBUTE::scale, scale))
            {
                param.emplace_back(parameterName, GammaDistribution{shape, scale, min, max});
            }
           else {
                ThrowIfFalse(ParseAttribute(parameterElement, ATTRIBUTE::mean, mean)
                             && ParseAttribute(parameterElement, ATTRIBUTE::sd, sd),
                             parameterElement, "Either attribute " + std::string(ATTRIBUTE::mu) + " and " + std::string(ATTRIBUTE::sigma) + " or "
                             + std::string(ATTRIBUTE::mean) + " and " + std::string(ATTRIBUTE::sd) + " required for LogNormalDistribution");

                param.emplace_back(parameterName, GammaDistribution::CreateWithMeanSd(mean, sd, min, max));

            }

            parameterElement = parameterElement.nextSiblingElement(QString::fromStdString(elementName));
        }
    }

    return param;
}

std::vector<QDomElement> ImportReference(const QDomElement& domElement, const QDomElement& profilesElement)
{
    std::vector<QDomElement> references;

    QDomElement parameterElement;
    if (GetFirstChildElement(domElement, TAG::Reference, parameterElement))
    {
        while (!parameterElement.isNull())
        {
            std::string type;
            std::string name;

            ThrowIfFalse(ParseAttributeString(parameterElement, ATTRIBUTE::type, type),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::type) + " is missing.");
            ThrowIfFalse(ParseAttributeString(parameterElement, ATTRIBUTE::name, name),
                         parameterElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

            QDomElement profileGroupElement;
            GetFirstChildElement(profilesElement, openpass::importer::xml::profilesImporter::tag::profileGroup, profileGroupElement);

            while (!profileGroupElement.isNull())
            {
                std::string groupType;
                ThrowIfFalse(ParseAttributeString(profileGroupElement, ATTRIBUTE::type, groupType),
                             profileGroupElement, "Attribute " + std::string(ATTRIBUTE::type) + " is missing.");
                if (type == groupType)
                {
                    QDomElement profileElement;
                    GetFirstChildElement(profileGroupElement, openpass::importer::xml::profilesImporter::tag::profile, profileElement);

                    while (!profileElement.isNull())
                    {
                        std::string profileName;
                        ThrowIfFalse(ParseAttributeString(profileElement, ATTRIBUTE::name, profileName),
                                     profileElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");
                        if (name == profileName)
                        {
                            QDomElement nestedReference;
                            GetFirstChildElement(profileElement, TAG::Reference, nestedReference);
                            ThrowIfFalse(nestedReference.isNull(), nestedReference, "Referenced profiles may not contain another reference");
                            references.push_back(profileElement);
                        }

                        profileElement = profileElement.nextSiblingElement(openpass::importer::xml::profilesImporter::tag::profile);
                    }
                }

                profileGroupElement = profileGroupElement.nextSiblingElement(openpass::importer::xml::profilesImporter::tag::profileGroup);
            }

            parameterElement = parameterElement.nextSiblingElement(QString::fromStdString(TAG::Reference));
        }
    }

    return references;
}

static ParameterSetLevel3 ImportParameterLevel3WithoutReferences(const QDomElement& element)
{
    ParameterSetLevel3 param;

    auto boolParams = ImportParameter<bool>(element, TAG::Bool);
    auto intParams = ImportParameter<int>(element, TAG::Int);
    auto intVectorParams = ImportParameter<std::vector<int>>(element, TAG::IntVector);
    auto doubleParams = ImportParameter<double>(element, TAG::Double);
    auto doubleVectorParams = ImportParameter<std::vector<double>>(element, TAG::DoubleVector);
    auto stringParams = ImportParameter<std::string>(element, TAG::String);
    auto stringVectorParams = ImportParameter<std::vector<std::string>>(element, TAG::StringVector);
    auto normalDistributionParams = ImportParameter<openpass::parameter::NormalDistribution>(element, TAG::NormalDistribution);
    auto lognormalDistributionParams = ImportParameter<openpass::parameter::LogNormalDistribution>(element, TAG::LogNormalDistribution);
    auto uniformDistributionParams = ImportParameter<openpass::parameter::UniformDistribution>(element, TAG::UniformDistribution);
    auto exponentialDistributionParams = ImportParameter<openpass::parameter::ExponentialDistribution>(element, TAG::ExponentialDistribution);
    auto gammaDistributionParams = ImportParameter<openpass::parameter::GammaDistribution>(element, TAG::GammaDistribution);

    param.insert(param.end(), boolParams.begin(), boolParams.end());
    param.insert(param.end(), intParams.begin(), intParams.end());
    param.insert(param.end(), intVectorParams.begin(), intVectorParams.end());
    param.insert(param.end(), doubleParams.begin(), doubleParams.end());
    param.insert(param.end(), doubleVectorParams.begin(), doubleVectorParams.end());
    param.insert(param.end(), stringParams.begin(), stringParams.end());
    param.insert(param.end(), stringVectorParams.begin(), stringVectorParams.end());
    param.insert(param.end(), normalDistributionParams.begin(), normalDistributionParams.end());
    param.insert(param.end(), lognormalDistributionParams.begin(), lognormalDistributionParams.end());
    param.insert(param.end(), uniformDistributionParams.begin(), uniformDistributionParams.end());
    param.insert(param.end(), exponentialDistributionParams.begin(), exponentialDistributionParams.end());
    param.insert(param.end(), gammaDistributionParams.begin(), gammaDistributionParams.end());

    return param;
}

static ParameterSetLevel3 ImportParameterLevel3(const QDomElement& domElement, const QDomElement& profilesElement)
{
    ParameterSetLevel3 param;
    auto elementsToCombine = ImportReference(domElement, profilesElement);
    elementsToCombine.push_back(domElement);

    for (auto& element : elementsToCombine)
    {
        auto partialParameters = ImportParameterLevel3WithoutReferences(element);
        param.insert(param.end(), partialParameters.begin(), partialParameters.end());
    }

    return param;
}

static ParameterSetLevel2 ImportParameterListsLevel2(const QDomElement& parameterElement, const QDomElement& profilesElement)
{
    QDomElement lists;

    ParameterSetLevel2 param;

    if (SimulationCommon::GetFirstChildElement(parameterElement, TAG::List, lists))
    {
        while (!lists.isNull())
        {
            std::string name;
            ThrowIfFalse(ParseAttributeString(lists, ATTRIBUTE::name, name),
                         lists, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

            QDomElement listItem;
            if (SimulationCommon::GetFirstChildElement(lists, TAG::ListItem, listItem))
            {
                ParameterListLevel2 parameterList;
                while (!listItem.isNull())
                {
                    auto temporaryContainer = ImportParameterLevel3(listItem, profilesElement);
                    parameterList.emplace_back(temporaryContainer);

                    listItem = listItem.nextSiblingElement(TAG::ListItem);
                }
                param.emplace_back(name, parameterList);
            }

            lists = lists.nextSiblingElement(TAG::List);
        }
    }

    return param;
}

static ParameterSetLevel2 ImportParameterSetLevel2(const QDomElement& parameterElement, const QDomElement& profilesElement)
{
    ParameterSetLevel2 container;

    auto domElements = ImportReference(parameterElement, profilesElement);
    domElements.push_back(parameterElement);

    for (auto element : domElements)
    {
        auto parameterSet = ImportParameterLevel3WithoutReferences(element);
        container.insert(container.end(), parameterSet.begin(), parameterSet.end());

        auto parameterList = ImportParameterListsLevel2(element, profilesElement);
        container.insert(container.end(), parameterList.begin(), parameterList.end());
    }

    return container;
}

static ParameterSetLevel1 ImportParameterListsLevel1(const QDomElement& parameterElement, const QDomElement& profilesElement)
{
    QDomElement lists;

    ParameterSetLevel1 param;

    if (SimulationCommon::GetFirstChildElement(parameterElement, TAG::List, lists))
    {
        while (!lists.isNull())
        {
            std::string name;
            ThrowIfFalse(ParseAttributeString(lists, ATTRIBUTE::name, name),
                         lists, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

            QDomElement listItem;
            if (SimulationCommon::GetFirstChildElement(lists, TAG::ListItem, listItem))
            {
                ParameterListLevel1 parameterList;
                while (!listItem.isNull())
                {
                    auto temporaryParameterSet = ImportParameterSetLevel2(listItem, profilesElement);

                    parameterList.emplace_back(temporaryParameterSet);

                    listItem = listItem.nextSiblingElement(TAG::ListItem);
                }
                param.emplace_back(name, parameterList);
            }

            lists = lists.nextSiblingElement(TAG::List);
        }
    }

    return param;
}

} // namespace openpass::parameter::internal

namespace openpass::parameter {

ParameterSetLevel1 Import(const QDomElement& parameterElement, const QDomElement& profilesElement)
{
    openpass::parameter::ParameterSetLevel1 tempContainer;

    auto referenceDomElements = openpass::parameter::internal::ImportReference(parameterElement, profilesElement);
    for (auto reference : referenceDomElements)
    {
        auto refernceParameterSet = openpass::parameter::internal::ImportParameterLevel3WithoutReferences(reference);
        tempContainer.insert(tempContainer.end(), refernceParameterSet.begin(), refernceParameterSet.end());

        auto referenceParameterLists = openpass::parameter::internal::ImportParameterListsLevel1(reference, profilesElement);
        tempContainer.insert(tempContainer.end(), referenceParameterLists.begin(), referenceParameterLists.end());
    }

    auto tempParameterSet = openpass::parameter::internal::ImportParameterLevel3WithoutReferences(parameterElement);
    auto tempParameterLists = openpass::parameter::internal::ImportParameterListsLevel1(parameterElement, profilesElement);

    tempContainer.insert(tempContainer.end(), tempParameterSet.begin(), tempParameterSet.end());
    tempContainer.insert(tempContainer.end(), tempParameterLists.begin(), tempParameterLists.end());

    return tempContainer;
}

} // openpass::parameter
