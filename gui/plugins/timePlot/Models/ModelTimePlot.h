/*********************************************************************
* Copyright (c) 2019, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef MODELTIMEPLOT_H
#define MODELTIMEPLOT_H

#include <unordered_map>
#include <utility>

#include <QDir>
#include <QFileInfo>
#include <QItemSelectionModel>
#include <QMap>
#include <QObject>

#include "ResultDirItemModel.h"

class ModelTimePlot : public QObject
{
    Q_OBJECT

public:
    explicit ModelTimePlot(QObject *parent = nullptr);
    virtual ~ModelTimePlot();

    QAbstractItemModel *GetItemModelHighD() const;
    void SetSelectionModelHighD(QItemSelectionModel *selectionModel);

public:
Q_SIGNALS:
    void TableModelCreated(std::unordered_map<std::string, QAbstractTableModel *> *newModel, const QString &tableName) const;
    void SendTableModel(QAbstractItemModel *tableModel);

    void Clear();

public Q_SLOTS:
    void LoadFileData(const QString &fileName, QString simulationRun);
    void LoadFileData(const QString &fileName, int trackId, QString simulationRun);
    void LoadPathData(const QString &pathName);
    void FindTableModel(const QString &tableName);

private Q_SLOTS:
    void OnFolderSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    bool ContainsHighDFormat(const QString &fileName);
    void CreateTabTables(const QString &fileName, int trackId, QString simulationRun);

    std::unordered_map<std::string, QAbstractTableModel *> tableMap;

    ResultDirItemModel *treeModelHighD = nullptr;
    QItemSelectionModel *selectionModelHighD = nullptr;
    QModelIndexList selectedIndexList;

    QString rootPath = "";

    std::set<int> trackIds = {};
    QStringList firstHeader;
};

#endif // MODELTIMEPLOT_H
