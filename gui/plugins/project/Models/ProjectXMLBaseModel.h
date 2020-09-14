/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/

#ifndef PROJECTXMLBASEMODEL_H
#define PROJECTXMLBASEMODEL_H

#include <QList>
#include <QString>

class ProjectXMLBaseModel
{
protected:
    static QString const KeyMasterConfig;
    static QString const KeySlaveConfigs;
    static QString const KeySlaveConfig;
    static QString const KeyLibraryPath;
    static QString const KeyResultPath;
    static QString const KeyConfigPath;
    static QString const KeyLogFileMaster;
    static QString const KeyLogFileSlave;
    static QString const KeyLogLevel;
    static QString const KeySlavePath;

protected:
    static QList<QString> const KeyListMasterConfig;
    static QList<QString> const KeyListSlaveConfig;
};

#endif // PROJECTXMLBASEMODEL_H
