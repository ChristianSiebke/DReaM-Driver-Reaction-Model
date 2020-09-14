/*********************************************************************
* Copyright (c) 2019 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "ModelTimePlot.h"

ModelTimePlot::ModelTimePlot(QObject *parent) :
    QObject(parent)
{
    treeModelHighD = new ResultDirItemModel(parent);
    firstHeader = QStringList({});
}

ModelTimePlot::~ModelTimePlot()
{
    for (auto tableModel : tableMap)
    {
        delete tableModel.second;
    }

    if (treeModelHighD != nullptr)
    {
        delete treeModelHighD;
    }
}

void ModelTimePlot::LoadPathData(const QString &pathName)
{
    firstHeader = QStringList({});
    trackIds = {};

    rootPath = pathName;

    Q_EMIT Clear();
    selectedIndexList.clear();

    qDebug("LoadPathData %s", qPrintable(rootPath));
    treeModelHighD->setRootResultDir(rootPath);
}

void ModelTimePlot::OnFolderSelectionChanged(const QItemSelection &selected,
                                             const QItemSelection &deselected)
{
    Q_UNUSED(deselected);

    Q_EMIT Clear();

    for (QModelIndex index : selected.indexes())
    {
        selectedIndexList.append(index);
    }
    for (QModelIndex index : deselected.indexes())
    {
        selectedIndexList.removeOne(index);
    }

    for (QModelIndex index : selectedIndexList)
    {
        if (!index.child(0, 0).isValid())
        { // selected index is leaf item
            QString simulationRun = index.parent().data().toString();
            QString indexData = index.data().toString();
            int trackId = indexData.split("_").last().toInt();

            QDir resultDir(simulationRun);
            QStringList resultsFileList = resultDir.entryList(QDir::Files | QDir::NoSymLinks);
            for (QString file : resultsFileList)
            {
                QString resultFile = simulationRun + "/" + file;
                LoadFileData(resultFile, trackId, QFileInfo(simulationRun).baseName());
            }
        }
        else if (!index.child(0, 0).child(0,
                                          0)
                      .isValid())
        { // selected index is directory above leaf item
            QString simulationRun = index.data().toString();

            QDir resultDir(simulationRun);
            QStringList resultsFileList = resultDir.entryList(QDir::Files | QDir::NoSymLinks);
            for (QString file : resultsFileList)
            {
                QString resultFile = simulationRun + "/" + file;
                LoadFileData(resultFile, QFileInfo(simulationRun).baseName());
            }
        }
    }
}

QAbstractItemModel *ModelTimePlot::GetItemModelHighD() const
{
    return treeModelHighD;
}

void ModelTimePlot::SetSelectionModelHighD(QItemSelectionModel *selectionModel)
{
    selectionModelHighD = selectionModel;
    connect(selectionModelHighD, &QItemSelectionModel::selectionChanged,
            this, &ModelTimePlot::OnFolderSelectionChanged);
}

void ModelTimePlot::LoadFileData(const QString &fileName, int trackId, QString simulationRun)
{
    QStringList stringList = fileName.split(".");
    if (stringList.last() == "csv")
    {
        if (ContainsHighDFormat(fileName))
        {
            if (trackIds.find(trackId) != trackIds.end())
            {
                CreateTabTables(fileName, trackId, simulationRun);
            }
        }
    }
}

void ModelTimePlot::LoadFileData(const QString &fileName, QString simulationRun)
{
    QStringList stringList = fileName.split(".");
    if (stringList.last() == "csv")
    {
        if (ContainsHighDFormat(fileName))
        {
            for (int trackId : trackIds)
            {
                CreateTabTables(fileName, trackId, simulationRun);
            }
        }
    }
}

void ModelTimePlot::CreateTabTables(const QString &fileName, int trackId, QString simulationRun)
{
    TableModelPlot *tabTableModel = new TableModelPlot(this, fileName, trackId);

    QString track = simulationRun + "_track_" + QString::number(trackId);
    tableMap.emplace(track.toStdString(), tabTableModel);

    Q_EMIT TableModelCreated(&tableMap, track);
}

bool ModelTimePlot::ContainsHighDFormat(const QString &fileName)
{
    TableModelPlot tableModel(this, fileName);
    if (tableModel.headerData(0, Qt::Horizontal, Qt::DisplayRole).toString() == "frame" &&
        tableModel.headerData(1, Qt::Horizontal, Qt::DisplayRole).toString() == "id")
    {
        if (!firstHeader.isEmpty() && firstHeader != tableModel.GetHeaderData())
        {
            return false;
        }
        if (firstHeader.isEmpty())
        {
            firstHeader = tableModel.GetHeaderData();
        }
        trackIds = tableModel.GetTrackIds();
        return true;
    }
    return false;
}

void ModelTimePlot::FindTableModel(const QString &tableName)
{
    auto tableModel = tableMap.find(tableName.toStdString());
    Q_EMIT SendTableModel(tableModel->second);
}
