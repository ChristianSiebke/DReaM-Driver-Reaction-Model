/*********************************************************************
* Copyright (c) 2019, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "ModelStatistics.h"

ModelStatistics::ModelStatistics(QObject *parent) :
    QObject(parent)
{
    listFolderModel = new QStandardItemModel(parent);
}

ModelStatistics::~ModelStatistics()
{
    Q_EMIT Clear();
    for (auto folderModel : folderTables)
    {
        delete folderModel.second;
    }

    for (auto histoModel : histoTables)
    {
        delete histoModel.second;
    }

    if (listFolderModel != nullptr)
    {
        delete listFolderModel;
    }
}

void ModelStatistics::LoadPathData(const QString &pathName)
{
    Q_EMIT Clear();
    rootPath = pathName;
    selectedIndexList.clear();

    QIcon dirIcon = QApplication::style()->standardIcon(QStyle::SP_DirIcon);
    QString fileName = QFileInfo(pathName).fileName();
    QStandardItem *item = new QStandardItem(dirIcon, fileName);
    listFolderModel->appendRow(item);

    QStringList folderPath = rootPath.split("/");
    QString folderName = folderPath.last();
    QDir resultDir(rootPath);
    QDirIterator it(resultDir, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        AddTableModel(it.next(), folderName);
    }
}

// (re)calculate all histograms from folderTable entries with selected names
void ModelStatistics::OnFolderSelectionChanged(const QItemSelection &selected,
                                               const QItemSelection &deselected)
{
    Q_EMIT Clear();
    for (auto histoModel : histoTables)
    {
        delete histoModel.second;
    }
    histoTables.clear();
    minVec.clear();
    maxVec.clear();
    yMaxVec.clear();
    headers.clear();

    for (QModelIndex index : selected.indexes())
    {
        selectedIndexList.append(index);
    }
    for (QModelIndex index : deselected.indexes())
    {
        selectedIndexList.removeOne(index);
    }
    // from all selected folders, extract extrema over columns in all tables
    for (QModelIndex index : selectedIndexList)
    {
        QString folderName = index.data().toString();
        std::map<QString, TableModel *>::iterator tableIt = folderTables.find(folderName);
        if (tableIt != folderTables.end())
        {
            TableModel *folderTable = tableIt->second;
            if (folderTable->GetHeader() == headers || headers.empty())
            {
                headers = folderTable->GetHeader();
                FillMinMax(*folderTable);
            }
            else
            {
                Q_EMIT ShowMessage("ERROR", "Folder contains incompatible csv files with different headers.");
            }
        }
    }
    // for all selected folders, calculate all histograms
    for (QModelIndex index : selectedIndexList)
    {
        QString folderName = index.data().toString();
        std::map<QString, TableModel *>::iterator tableIt = folderTables.find(folderName);
        if (tableIt != folderTables.end())
        {
            TableModel *folderTable = tableIt->second;
            if (folderTable->GetHeader() == headers || headers.empty())
            {
                CalculateHistograms(folderName, *folderTable);
            }
        }
    }
    int row = 0;
    std::map<QLabel *, RowHistograms *>::iterator rowIt;
    for (rowIt = histoTables.begin(); rowIt != histoTables.end(); rowIt++)
    {
        HistogramAdded(rowIt->second, row, rowIt->first, &yMaxVec);
        row++;
    }
}

void ModelStatistics::FillMinMax(const TableModel &folderTable)
{
    if (minVec.empty())
    {
        for (int i = 0; i < folderTable.columnCount(); i++)
        {
            auto tableColumn = folderTable.GetColumnVector(i);
            auto result = std::minmax_element(std::begin(*tableColumn), std::end(*tableColumn));
            double min = *result.first;
            double max = *result.second;
            minVec.push_back(min);
            maxVec.push_back(max);
        }
    }
    else
    {
        for (int i = 0; i < folderTable.columnCount(); i++)
        {
            auto tableColumn = folderTable.GetColumnVector(i);
            auto result = std::minmax_element(std::begin(*tableColumn), std::end(*tableColumn));
            minVec.at(i) = std::min(minVec.at(i), *result.first);
            maxVec.at(i) = std::max(maxVec.at(i), *result.second);
        }
    }
}

QAbstractItemModel *ModelStatistics::GetItemModelHistograms() const
{
    return listFolderModel;
}

void ModelStatistics::SetSelectionModelHistograms(QItemSelectionModel *selectionModel)
{
    selectionFolderModel = selectionModel;
    connect(selectionFolderModel, &QItemSelectionModel::selectionChanged,
            this, &ModelStatistics::OnFolderSelectionChanged);
}

bool ModelStatistics::AddTableModel(const QString &resultPath, const QString &folderName)
{
    QStringList stringList = resultPath.split(".");
    if (stringList.last() == "csv")
    {
        if (ContainsHighDFormat(resultPath))
        {
            std::map<QString, TableModel *>::iterator it = folderTables.find(folderName);
            if (it != folderTables.end())
            {
                if (!it->second->AppendCsv(resultPath))
                {
                    Q_EMIT ShowMessage("ERROR", "Found incompatible csv files with different headers.");
                }
            }
            else
            {
                TableModel *tableModel = new TableModel(this, resultPath);
                folderTables.insert(std::pair<QString, TableModel *>(folderName, tableModel));
            }
            return true;
        }
    }
    return false;
}

void ModelStatistics::CalculateHistograms(QString folderName, const TableModel &tableModel)
{
    RowHistograms *histograms = new RowHistograms(this, tableModel, numberOfBins, minVec, maxVec);
    QLabel *folderLabel = new QLabel(folderName);
    histoTables.insert(std::pair<QLabel *, RowHistograms *>(folderLabel, histograms));

    if (yMaxVec.empty())
    {
        for (int i = 0; i < histograms->columnCount(); ++i)
        {
            yMaxVec.push_back(histograms->GetHistogram(i)->GetYScaleMax());
        }
    }
    else
    {
        for (int i = 0; i < histograms->columnCount(); ++i)
        {
            yMaxVec.at(i) = std::max(histograms->GetHistogram(i)->GetYScaleMax(), yMaxVec.at(i));
        }
    }
}

bool ModelStatistics::ContainsHighDFormat(const QString &fileName)
{
    TableModel tableModel(this, fileName);
    if (tableModel.headerData(0, Qt::Horizontal, Qt::DisplayRole).toString() == "frame" &&
        tableModel.headerData(1, Qt::Horizontal, Qt::DisplayRole).toString() == "id")
    {
        return true;
    }
    return false;
}
