/*********************************************************************
* Copyright (c) 2019, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "ResultDirItemModel.h"

#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QStyle>

ResultDirItemModel::ResultDirItemModel(QObject *parent) :
    QStandardItemModel(parent)
{
    rootItem = this->invisibleRootItem();
    dirIcon = QApplication::style()->standardIcon(QStyle::SP_DirIcon);   //icon for directories
    fileIcon = QApplication::style()->standardIcon(QStyle::SP_FileIcon); //icon for files
    firstHeader = QStringList({});
}

ResultDirItemModel::~ResultDirItemModel()
{
}

void ResultDirItemModel::setRootResultDir(QString resultDir)
{
    if (!resultDir.isEmpty())
    {
        firstHeader.clear();
        // clear all existing data (except for the invisible root) in the tree model
        rootItem->removeRows(0, rootItem->rowCount());

        QString dirName = QFileInfo(resultDir).absoluteFilePath();
        QStandardItem *parent = new QStandardItem(dirIcon, dirName); //create the parent directory item
        parent->setAccessibleDescription(dirName);                   //set actual path to item
        rootItem->appendRow(parent);                                 //add the parent item to root item
        createDirectoryItem(resultDir, parent, 0);                   //Iterate and populate the contents
    }
}

/*
  Method to populate the contents of the given directory recursively.
*/
void ResultDirItemModel::createDirectoryItem(QString dirPath, QStandardItem *parentItem,
                                             int dirDepth)
{
    QString subDirName;
    QStandardItem *child;

    QDirIterator it(dirPath, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    while (it.hasNext())
    {
        subDirName = QFileInfo(it.next()).absoluteFilePath();
        child = new QStandardItem(dirIcon, subDirName);
        child->setAccessibleDescription(subDirName);
        parentItem->appendRow(child);
        createDirectoryItem(subDirName, child, dirDepth + 1);
    }
    QDir resultDir(dirPath);
    QStringList resultsFileList = resultDir.entryList(QDir::Files | QDir::NoSymLinks);
    for (QString file : resultsFileList)
    {
        QString resultFile = dirPath + "/" + file;
        SetTracks(resultFile, parentItem);
    }
}

void ResultDirItemModel::SetTracks(const QString &fileName, QStandardItem *parentItem)
{
    QStringList stringList = fileName.split(".");
    if (stringList.last() == "csv")
    {
        TableModelPlot tableModel(this, fileName);

        if (tableModel.headerData(0, Qt::Horizontal, Qt::DisplayRole).toString() == "TimeStep" &&
            tableModel.headerData(1, Qt::Horizontal, Qt::DisplayRole).toString() == "AgentId")
        {
            if (!firstHeader.isEmpty() && firstHeader != tableModel.GetHeaderData())
            {
                return;
            }
            if (firstHeader.isEmpty())
            {
                firstHeader = tableModel.GetHeaderData();
            }
            std::set<int> trackIds = tableModel.GetTrackIds();
            QString track_n;
            QStandardItem *child;
            for (int trackId : trackIds)
            {
                track_n = "track_" + QString::number(trackId);
                child = new QStandardItem(fileIcon, track_n);
                child->setAccessibleDescription(track_n);
                parentItem->appendRow(child);
            }
        }
    }
}
