/********************************************************************************
 * Copyright (c) 2017 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "ProjectXMLSaveModel.h"

#include <QFile>
#include <QIODevice>
#include <QXmlStreamWriter>

bool ProjectXMLSaveModel::save(QString const &filepath, ProjectModel const *const project)
{
    // Can we open the file in write only mode?
    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        // Save project to file
        return save(&file, project);
    }
    // Return failure
    return false;
}

bool ProjectXMLSaveModel::save(QIODevice *const device, const ProjectModel *const project)
{
    // Initialize xml stream writer
    QXmlStreamWriter xml(device);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);

    // Save project to xml stream
    xml.writeStartDocument();
    saveProject(xml, project);
    xml.writeEndDocument();
    return true;
}

void ProjectXMLSaveModel::saveProject(QXmlStreamWriter &xml, const ProjectModel *const project)
{
    // Write the elements to the xml stream
    xml.writeStartElement(KeyOpSimulationManager);
    xml.writeTextElement(KeyLogLevel, QString::number(project->getLogLevel()));
    xml.writeTextElement(KeyLogFileOpSimulationManager, project->getLogOpSimulationManager());
    xml.writeTextElement(KeySimulationPath, project->getSimulationExe());
    xml.writeTextElement(KeyLibraryPath, project->getLibraryPath());

    xml.writeStartElement(KeySimulationConfigs);
    xml.writeStartElement(KeySimulationConfig);
    xml.writeTextElement(KeyLogFileSimulation, project->getLogSimulation());
    xml.writeTextElement(KeyConfigPath, project->getConfigPath());
    xml.writeTextElement(KeyResultPath, project->getResultPath());

    xml.writeEndElement();
    xml.writeEndElement();
    xml.writeEndElement();
}
