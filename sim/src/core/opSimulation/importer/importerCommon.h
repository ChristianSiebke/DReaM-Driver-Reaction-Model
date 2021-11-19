/********************************************************************************
 * Copyright (c) 2021 in-tech GmbH
 *               2021 Arrk Engineering
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "common/log.h"
#include "common/xmlParser.h"
#include <QUrl>
#include <QtXmlPatterns>
#include <QSourceLocation>
#include <QAbstractMessageHandler>
/*!
    * \brief
    * \details validates a xml with a xml shema file which is located at the same location
    * logs warnings
    * @param[in]     xmlFile                Name of the Xml file to be validated
    * @return	     true, if validation successful
    */

class XMLMessageHandler : public QAbstractMessageHandler
{
public:
    ~XMLMessageHandler() override = default;
protected:
    void handleMessage(QtMsgType type, const QString &description,
                       const QUrl &identifier, const QSourceLocation &sourceLocation) override;
};

class ImporterCommon
{
public:
    static bool validateProfilesCatalogXMLSchema(const QString& xmlFileName, const QByteArray& xmlData);

private:
    static const inline std::string profilesCatalogSchemaFilename = "ProfilesCatalog.xsd";
    static bool validateXMLSchema(const QString& xmlFileName, const std::string& xmlSchemaFilename, const QByteArray& xmlData);
};

//! If the first parameter is false writes a message into the log including the line and column number of the erronous xml element
//!
//! \param success      writes message if success is false
//! \param element      erronous xml element
//! \param message      message describing error
//! \param logFunction  function to use for logging
[[maybe_unused]] static void ThrowIfFalse(bool success, const QDomElement element, const std::string &message)
{
    if (!success)
    {
        LogErrorAndThrow("Could not import element " + element.tagName().toStdString() +
                         " (line " + std::to_string(element.lineNumber()) +
                         ", column " + std::to_string(element.columnNumber()) + "): " +
                         message);
    }
}
