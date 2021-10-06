/********************************************************************************
 * Copyright (c) 2017 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "ProjectXMLLoadModel.h"

#include <QFile>
#include <QIODevice>
#include <QXmlStreamWriter>

bool ProjectXMLLoadModel::load(QString const &filepath, ProjectModel *const project)
{
    // Does the file exist and can we open it in read only mode?
    QFile file(filepath);
    if ((file.exists()) && (file.open(QIODevice::ReadOnly | QIODevice::Text)))
    {
        // Load project to file
        return load(&file, project);
    }
    // Return failure
    return false;
}

bool ProjectXMLLoadModel::load(QIODevice *const device, ProjectModel *const project)
{
    // Initialize xml stream reader
    QXmlStreamReader xml(device);

    // Verify xml header
    if ((xml.readNext() == QXmlStreamReader::TokenType::StartDocument) &&
        (xml.readNext() == QXmlStreamReader::TokenType::StartElement) &&
        (xml.name() == KeyOpSimulationManager))
    {
        // Load project from xml stream
        return loadProject(xml, project);
    }
    return false;
}

bool ProjectXMLLoadModel::loadProject(QXmlStreamReader &xml,
                                      ProjectModel *const project)
{
    // Import the elements
    bool success = true;
    QList<QString> keys = KeyListOpSimulationManager;
    while (xml.readNextStartElement())
    {
        keys.removeAll(xml.name().toString());
        if (xml.name() == KeyLibraryPath)
            project->setLibraryPath(xml.readElementText());
        else if (xml.name() == KeyLogFileOpSimulationManager)
            project->setLogOpSimulationManager(xml.readElementText());
        else if (xml.name() == KeyLogLevel)
            project->setLogLevel(xml.readElementText().toUInt());
        else if (xml.name() == KeySimulationPath)
            project->setSimulationExe(xml.readElementText());
        else if (xml.name() == KeySimulationConfigs)
            success = loadSimulationConfig(xml, project) && success;
        else
            xml.skipCurrentElement();
    }
    return success && keys.isEmpty();
}

bool ProjectXMLLoadModel::loadSimulationConfig(QXmlStreamReader &xml, ProjectModel *const project)
{
    bool success = true;
    QList<QString> keys = KeyListSimulationConfig;

    xml.readNextStartElement();
    if (xml.name() == KeySimulationConfig)
    {
        while (xml.readNextStartElement())
        {
            keys.removeAll(xml.name().toString());

            if (xml.name() == KeyLogFileSimulation)
                project->setLogSimulation(xml.readElementText());
            else if (xml.name() == KeyConfigPath)
                project->setConfigPath(xml.readElementText());
            else if (xml.name() == KeyResultPath)
                project->setResultPath(xml.readElementText());
            else
                xml.skipCurrentElement();
        }

        return success && keys.isEmpty();
    }

    return false;
}
