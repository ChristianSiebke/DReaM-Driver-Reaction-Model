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
#include "CoreFramework/CoreShare/log.h"


namespace TAG = openpass::importer::xml::parameterImporter::tag;
namespace ATTRIBUTE = openpass::importer::xml::parameterImporter::attribute;
using namespace SimulationCommon;

void ParameterImporter::ImportBoolParameters(QDomElement componentParameterSetElement,
                                             ParameterInterface& componentParameterSet)
{
    QDomElement boolParameterElement;
    if (SimulationCommon::GetFirstChildElement(componentParameterSetElement, TAG::Bool, boolParameterElement))
    {
        //Iterates over all int parameters
        while (!boolParameterElement.isNull())
        {
            std::string boolParameterName;
            bool boolParameterValue;

            ThrowIfFalse(SimulationCommon::ParseAttributeString(boolParameterElement, ATTRIBUTE::key, boolParameterName)
                            && SimulationCommon::ParseAttributeBool(boolParameterElement, ATTRIBUTE::value, boolParameterValue)
                            && componentParameterSet.AddParameterBool(boolParameterName, boolParameterValue),
                         "Could not import bool parameter.");

            boolParameterElement = boolParameterElement.nextSiblingElement(TAG::Bool);
        }
    }
}

void ParameterImporter::ImportIntParameters(QDomElement componentParameterSetElement,
                                            ParameterInterface& componentParameterSet)
{
    QDomElement intParameterElement;
    if (SimulationCommon::GetFirstChildElement(componentParameterSetElement, TAG::Int, intParameterElement))
    {
        //Iterates over all int parameters
        while (!intParameterElement.isNull())
        {
            std::string intParameterName;
            int intParameterValue;

            ThrowIfFalse(SimulationCommon::ParseAttributeString(intParameterElement, ATTRIBUTE::key, intParameterName)
                            && SimulationCommon::ParseAttributeInt(intParameterElement, ATTRIBUTE::value, intParameterValue)
                            && componentParameterSet.AddParameterInt(intParameterName, intParameterValue),
                         "Could not import int parameter.");

            intParameterElement = intParameterElement.nextSiblingElement(TAG::Int);
        }
    }
}

void ParameterImporter::ImportIntVectorParameters(QDomElement componentParameterSetElement,
                                                  ParameterInterface& componentParameterSet)
{
    QDomElement intVectorParameterElement;
    if (SimulationCommon::GetFirstChildElement(componentParameterSetElement, TAG::intVector, intVectorParameterElement))
    {
        //Iterates over all int vector parameters
        while (!intVectorParameterElement.isNull())
        {
            std::string intVectorParameterName;
            std::vector<int> intVectorParameterValue;

            ThrowIfFalse(SimulationCommon::ParseAttributeString(intVectorParameterElement, ATTRIBUTE::key, intVectorParameterName)
                            && SimulationCommon::ParseAttributeIntVector(intVectorParameterElement, ATTRIBUTE::value, &intVectorParameterValue)
                            && componentParameterSet.AddParameterIntVector(intVectorParameterName, intVectorParameterValue),
                         "Could not import int vector parameter.");

            intVectorParameterElement = intVectorParameterElement.nextSiblingElement(TAG::intVector);
        }
    }
}

void ParameterImporter::ImportDoubleParameters(QDomElement componentParameterSetElement,
                                               ParameterInterface& componentParameterSet)
{
    QDomElement doubleParameterElement;
    if (SimulationCommon::GetFirstChildElement(componentParameterSetElement, TAG::Double, doubleParameterElement))
    {
        //Iterates over all double parameters
        while (!doubleParameterElement.isNull())
        {
            std::string doubleParameterName;
            double doubleParameterValue;

            ThrowIfFalse(SimulationCommon::ParseAttributeString(doubleParameterElement, ATTRIBUTE::key, doubleParameterName)
                            && SimulationCommon::ParseAttributeDouble(doubleParameterElement, ATTRIBUTE::value, doubleParameterValue)
                            && componentParameterSet.AddParameterDouble(doubleParameterName, doubleParameterValue),
                         "Could not import double parameter.");

            doubleParameterElement = doubleParameterElement.nextSiblingElement(TAG::Double);
        }
    }
}

void ParameterImporter::ImportDoubleVectorParameters(QDomElement componentParameterSetElement,
                                                     ParameterInterface& componentParameterSet)
{
    QDomElement doubleVectorParameterElement;
    if (SimulationCommon::GetFirstChildElement(componentParameterSetElement, TAG::doubleVector, doubleVectorParameterElement))
    {
        //Iterates over all double vector parameters
        while (!doubleVectorParameterElement.isNull())
        {
            std::string doubleVectorParameterName;
            std::vector<double> doubleVectorParameterValue;

            ThrowIfFalse(SimulationCommon::ParseAttributeString(doubleVectorParameterElement, ATTRIBUTE::key, doubleVectorParameterName)
                            && SimulationCommon::ParseAttributeDoubleVector(doubleVectorParameterElement, ATTRIBUTE::value, &doubleVectorParameterValue)
                            && componentParameterSet.AddParameterDoubleVector(doubleVectorParameterName, doubleVectorParameterValue),
                         "Could not import double vector parameter.");

            doubleVectorParameterElement = doubleVectorParameterElement.nextSiblingElement(TAG::doubleVector);
        }
    }
}

void ParameterImporter::ImportStringParameters(QDomElement componentParameterSetElement,
                                               ParameterInterface& componentParameterSet)
{
    QDomElement stringParameterElement;
    if (SimulationCommon::GetFirstChildElement(componentParameterSetElement, TAG::string, stringParameterElement))
    {
        //Iterates over all string parameters
        while (!stringParameterElement.isNull())
        {
            std::string stringParameterName;
            std::string stringParameterValue;

            ThrowIfFalse(SimulationCommon::ParseAttributeString(stringParameterElement, ATTRIBUTE::key, stringParameterName)
                            && SimulationCommon::ParseAttributeString(stringParameterElement, ATTRIBUTE::value, stringParameterValue)
                            && componentParameterSet.AddParameterString(stringParameterName, stringParameterValue),
                         "Could not import string parameter.");

            stringParameterElement = stringParameterElement.nextSiblingElement(TAG::string);
        }
    }
}

void ParameterImporter::ImportNormalDistributionParameters(QDomElement componentParameterSetElement,
                                                           ParameterInterface& componentParameterSet)
{
    QDomElement normalDistributionParameterElement;
    if (SimulationCommon::GetFirstChildElement(componentParameterSetElement, TAG::normalDistribution,
            normalDistributionParameterElement))
    {
        //Iterates over all string parameters
        while (!normalDistributionParameterElement.isNull())
        {
            std::string normalDistributionParameterName;
            StochasticDefintions::NormalDistributionParameter normalDistribution;

            ThrowIfFalse(SimulationCommon::ParseAttributeString(normalDistributionParameterElement, ATTRIBUTE::key, normalDistributionParameterName)
                            && SimulationCommon::ParseAttributeDouble(normalDistributionParameterElement, ATTRIBUTE::mean, normalDistribution.mean)
                            && SimulationCommon::ParseAttributeDouble(normalDistributionParameterElement, ATTRIBUTE::sd, normalDistribution.standardDeviation)
                            && SimulationCommon::ParseAttributeDouble(normalDistributionParameterElement, ATTRIBUTE::min, normalDistribution.min)
                            && SimulationCommon::ParseAttributeDouble(normalDistributionParameterElement, ATTRIBUTE::max, normalDistribution.max)
                            && componentParameterSet.AddParameterNormalDistribution(normalDistributionParameterName, normalDistribution),
                         "Could not import normal distribution parameter.");

            normalDistributionParameterElement = normalDistributionParameterElement.nextSiblingElement(TAG::normalDistribution);
        }
    }
}

void ParameterImporter::ImportParameterLists(QDomElement componentParameterSetElement,
                                             ParameterInterface& componentParameterSet)
{
    QDomElement parameterListsElement;
    if (SimulationCommon::GetFirstChildElement(componentParameterSetElement, TAG::list, parameterListsElement))
    {
        //Iterates over all parameter lists
        while (!parameterListsElement.isNull())
        {
            std::string parameterListName;

            ThrowIfFalse(SimulationCommon::ParseAttributeString(parameterListsElement, ATTRIBUTE::name, parameterListName), "Could not import string parameter.");

            // Parse all list items
            QDomElement parameterListElement;
            if (SimulationCommon::GetFirstChildElement(parameterListsElement, TAG::listItem, parameterListElement))
            {
                while (!parameterListElement.isNull())
                {
                    auto& listItem = componentParameterSet.InitializeListItem(parameterListName);

                    try
                    {
                        ImportParameters(parameterListElement, listItem);
                    }
                    catch(std::runtime_error error)
                    {
                        LogErrorAndThrow("Could not import parameter list." + std::string(error.what()));
                    }

                    parameterListElement = parameterListElement.nextSiblingElement(TAG::listItem);
                }
            }

            parameterListsElement = parameterListsElement.nextSiblingElement(TAG::list);
        }
    }
}

void ParameterImporter::ImportParameters(QDomElement componentParameterSetElement,
                                         ParameterInterface& componentParameterSet)
{
    //Parse Bool Parameters
    ImportBoolParameters(componentParameterSetElement, componentParameterSet);
    ImportIntParameters(componentParameterSetElement, componentParameterSet);
    ImportIntVectorParameters(componentParameterSetElement, componentParameterSet);
    ImportDoubleParameters(componentParameterSetElement, componentParameterSet);
    ImportDoubleVectorParameters(componentParameterSetElement, componentParameterSet);
    ImportNormalDistributionParameters(componentParameterSetElement, componentParameterSet);
    ImportStringParameters(componentParameterSetElement, componentParameterSet);
    ImportParameterLists(componentParameterSetElement, componentParameterSet);
}
