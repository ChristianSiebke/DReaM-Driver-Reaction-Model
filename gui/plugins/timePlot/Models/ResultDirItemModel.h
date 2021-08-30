/*********************************************************************
* Copyright (c) 2019, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef RESULTDIRITEMMODEL_H
#define RESULTDIRITEMMODEL_H

#include <QStandardItem>
#include <QStandardItemModel>

#include "TableModelPlot.h"

class ResultDirItemModel : public QStandardItemModel
{
public:
    ResultDirItemModel(QObject *parent = nullptr);
    ~ResultDirItemModel();

    void setRootResultDir(QString resultDir);

private:
    QStandardItem *rootItem;
    QIcon dirIcon;
    QIcon fileIcon;
    QStringList firstHeader;

    void createDirectoryItem(QString dirPath, QStandardItem *parentItem, int dirDepth);
    void SetTracks(const QString &fileName, QStandardItem *parentItem);
};

#endif // RESULTDIRITEMMODEL_H
