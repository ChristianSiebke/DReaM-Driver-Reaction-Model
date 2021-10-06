/********************************************************************************
 * Copyright (c) 2017 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef PROJECTXMLBASEMODEL_H
#define PROJECTXMLBASEMODEL_H

#include <QList>
#include <QString>

class ProjectXMLBaseModel
{
protected:
    static QString const KeyOpSimulationManager;
    static QString const KeySimulationConfigs;
    static QString const KeySimulationConfig;
    static QString const KeyLibraryPath;
    static QString const KeyResultPath;
    static QString const KeyConfigPath;
    static QString const KeyLogFileOpSimulationManager;
    static QString const KeyLogFileSimulation;
    static QString const KeyLogLevel;
    static QString const KeySimulationPath;

protected:
    static QList<QString> const KeyListOpSimulationManager;
    static QList<QString> const KeyListSimulationConfig;
};

#endif // PROJECTXMLBASEMODEL_H
