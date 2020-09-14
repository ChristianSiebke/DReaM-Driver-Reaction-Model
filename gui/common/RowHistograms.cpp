/*********************************************************************
* Copyright (c) 2019 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "RowHistograms.h"

RowHistograms::RowHistograms(QObject *parent,
                             const TableModel &tableModel, int numberOfBins, const std::vector<double> &minVec,
                             const std::vector<double> &maxVec) :
    QAbstractTableModel(parent)
{
    for (int i = 0; i < tableModel.columnCount(); ++i)
    {
        QString headerItem = tableModel.headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
        headerHisto.append(headerItem);

        Histogram histo = Histogram(numberOfBins, *tableModel.GetColumnVector(i), minVec.at(i),
                                    maxVec.at(i));

        dataHisto.push_back(histo);
    }
}

int RowHistograms::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return dataHisto.at(0).GetData().size();
}

int RowHistograms::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return dataHisto.size();
}

QVariant RowHistograms::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (rowCount() > index.row() && columnCount() > index.column())
        {
            return dataHisto.at(index.column()).GetData().at(index.row());
        }
    }
    return QVariant();
}

QVariant RowHistograms::headerData(int section, Qt::Orientation orientation,
                                   int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            if (headerHisto.size() > section)
            {
                return headerHisto.at(section);
            }
        }
    }
    return QVariant();
}

void RowHistograms::Clear()
{
    dataHisto.clear();
    headerHisto.clear();
}

const Histogram *RowHistograms::GetHistogram(int section) const
{
    if (section < columnCount())
    {
        return &dataHisto.at(section);
    }
    else
    {
        return nullptr;
    }
}
