/*********************************************************************************
* Copyright (c) 2021 Arrk Engineering
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************************/

#include "importerCommon.h"
#include "QCoreApplication"
#include <QDir>

bool ImporterCommon::validateProfilesCatalogXMLSchema(const QString &xmlFileName, const QByteArray &xmlData) {
   return validateXMLSchema(xmlFileName, profilesCatalogSchemaFilename, xmlData );
}

bool ImporterCommon::validateXMLSchema(const QString &xmlFileName, const std::string& xmlSchemaFilename,
                                       const QByteArray &xmlData) {
    QString applicationPath = QCoreApplication::applicationDirPath();
    QDir application_directory (applicationPath);
    if(!application_directory.cd("schemas"))
    {
        if(!application_directory.cdUp() || !application_directory.cd("schemas"))
        {
            LOG_INTERN(LogLevel::Warning) << "schemas folder is missing in installation directory";
            return false;
        }
    }
    std::string schemaDirectory = application_directory.path().toStdString();
    std::string xmlSchemaFileLocation = schemaDirectory + application_directory.separator().toLatin1();
    xmlSchemaFileLocation = xmlSchemaFileLocation.append(xmlSchemaFilename);
    QFile xmlSchemaFile(xmlSchemaFileLocation.c_str()); // automatic object will be closed on destruction
    if (!(xmlSchemaFile.open(QIODevice::ReadOnly))) {
        LOG_INTERN(LogLevel::Warning) << "Could not open XML Schema from" + xmlSchemaFileLocation + "";
        return false;
    }

    XMLMessageHandler messageHandler;
    QXmlSchema schema;
    if (!schema.load(&xmlSchemaFile, QUrl::fromLocalFile(xmlSchemaFile.fileName()))) {
        LOG_INTERN(LogLevel::Warning) << "XML Schema file: " + xmlSchemaFileLocation + " is not a Schema file";
        return false;
    };
    if (!schema.isValid()) {
        LOG_INTERN(LogLevel::Warning) << "XML Schema file: " + xmlSchemaFileLocation + " is not a valid Schema file";
        return false;
    }

    QXmlSchemaValidator xmlValidator(schema);
    xmlValidator.setMessageHandler(&messageHandler);
    if (!xmlValidator.validate(xmlData)) {
        LOG_INTERN(LogLevel::Warning)
            << "Schema file: " + xmlSchemaFileLocation + "does not meet the XML Schema in " + xmlFileName.toStdString();
        return false;
    }
    return true;
}

void XMLMessageHandler::handleMessage(QtMsgType type, const QString &description,
                   const QUrl &identifier, const QSourceLocation &sourceLocation)
{
    LOG_INTERN(LogLevel::Warning)
        << "XML Schema Validator message :" + description.toStdString() + " at line " + std::to_string(sourceLocation.line()) ;
}
