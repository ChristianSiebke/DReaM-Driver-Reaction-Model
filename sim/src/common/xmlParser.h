/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  XmlParser.h
//! @brief This file contains helper functions to parse the configuration.
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <unordered_map>

#include <QDomDocument>
#include <QFile>

//! Writes a message into the log including the line and column number of the erronous xml element
//!
//! \param element      erronous xml element
//! \param message      message describing error
//! \param logFunction  function to use for logging
static void LogMessage(const QDomElement element, const std::string &message,
                       std::function<void(const std::string&)> logFunction)
{
    logFunction("Could not import element " + element.tagName().toStdString() +
                " (line " + std::to_string(element.lineNumber()) +
                ", column " + std::to_string(element.columnNumber()) + "): " +
                message);
}

namespace SimulationCommon {

extern bool GetFirstChildElement(QDomElement rootElement, const std::string &tag, QDomElement &result);

extern bool GetFirstChild(QDomElement rootElement, QDomElement &result);

extern bool GetLastChildElement(QDomElement rootElement, const std::string &tag, QDomElement &result);

extern bool ParseCurrentInt(QDomElement currentElement, int &result);

/// Tempalted wrapper for Parse* functions
template <typename T>
bool Parse(QDomElement rootElement, const std::string &tag, T &result);

extern bool ParseString(QDomElement rootElement, const std::string &tag, std::string &result);

extern bool ParseDouble(QDomElement rootElement, const std::string &tag, double &result);

extern bool ParseDoubleVector(QDomElement rootElement, const std::string &tag, std::vector<double> &result);

extern bool ParseInt(QDomElement rootElement, const std::string &tag, int &result);

extern bool ParseULong(QDomElement rootElement, const std::string &tag, unsigned long &result);

extern bool ParseBool(QDomElement rootElement, const std::string &tag, bool &result);

extern bool ParseAttributeString(QDomElement element, const std::string &attributeName, std::string &result, std::optional<std::string> defaultValue = std::nullopt);

extern bool ParseAttributeDouble(QDomElement element, const std::string &attributeName, double &result, std::optional<double> defaultValue = std::nullopt);

extern bool ParseAttributeInt(QDomElement element, const std::string &attributeName, int &result);

extern bool ParseAttributeBool(QDomElement element, const std::string &attributeName, bool &result);

extern bool ParseAttributeStringVector(QDomElement element, const std::string &attributeName, std::vector<std::string> *result);

extern bool ParseAttributeDoubleVector(QDomElement element, const std::string &attributeName, std::vector<double> *result);

extern bool ParseAttributeIntVector(QDomElement element, const std::string &attributeName, std::vector<int> *result);

extern bool ParseAttributeBoolVector(QDomElement element, const std::string &attributeName, std::vector<bool> *result);

/*!
 *  \brief Parses the value of an XML attribute into the provided container.
 *
 *  Depending on the datatype of the result parameter, different parsing techniques are applied.
 *
 *  \param[in]  element         XML element holding the attribute
 *  \param[in]  attributeName   Name of the attribute to parse the value from
 *  \param[out] result          Container holding the result
 *
 *  \return     True on successful parsing, false otherwise.
 */
template <typename T>
bool ParseAttribute(QDomElement element, const std::string &attributeName, T &result);

//! Returns true if the given element has an attribute of the given name
bool HasAttribute(QDomElement element, const std::string &attributeName);

/*!
* \brief Imports a probability map
*
* \details This method imports a probability map for some value of type T. T can either be int, double or string
*
*
* @param[in]     parentElement          Element containing the information
* @param[in]     key                    Name how the value is specified in the xml file
* @param[in]     tag                    Name of the tags that should be imported
* @param[out]    probabilities          Map where the probabilities get saved
* @param[in]     mustAddUpToOne         flag which specifies wether the sum of all probalities must be 1
* @return	     true, if successful
*/
template <typename T>
bool ImportProbabilityMap(QDomElement parentElement,
                          const std::string key,
                          const QString tag,
                          std::vector<std::pair<T, double>> &probabilities,
                          std::function<void(const std::string&)> logFunction,
                          bool mustAddUpToOne = true)
{
    double probabilitySum = 0.0;

    QDomElement childElement;
    if (!GetFirstChildElement(parentElement, tag.toStdString(), childElement))
    {
        LogMessage(parentElement, "At least one element is required.", logFunction);
        return false;
    }

    while (!childElement.isNull())
    {
        T keyValue;
        double probability;

        if (!ParseAttribute<T>(childElement, key, keyValue))
        {
            LogMessage(childElement, "Key is invalid.", logFunction);
            return false;
        }

        if (!ParseAttributeDouble(childElement, "Probability", probability))
        {
            LogMessage(childElement, "Probability is invalid.", logFunction);
            return false;
        }

        probabilities.push_back({keyValue, probability});

        probabilitySum += probability;

        childElement = childElement.nextSiblingElement(tag);
    }

    //Checks probabilities
    if (mustAddUpToOne && std::abs(probabilitySum - 1.0) > 1e-6)
    {
        LogMessage(parentElement, "Probabilities do not add up to 1.0.", logFunction);
        return false;
    }

    if (probabilitySum > 1.0 + 1e-6)
    {
        LogMessage(parentElement, "Probabilities add up to more than 1.0.", logFunction);
        return false;
    }

    return true;
}

} // namespace SimulationCommon
