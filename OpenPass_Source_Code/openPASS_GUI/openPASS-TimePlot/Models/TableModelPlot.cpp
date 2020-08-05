/*********************************************************************
* Copyright (c) 2019 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "TableModelPlot.h"


TableModelPlot::TableModelPlot(QObject *parent, const QString &fileName)
    : QAbstractTableModel(parent)
{
    ReadCsv(fileName);
}

TableModelPlot::TableModelPlot(QObject *parent, const QString &fileName, int trackId)
    : QAbstractTableModel(parent)
{
    ReadTrack(fileName, trackId);
}

TableModelPlot::~TableModelPlot()
{
    Clear();
}

int TableModelPlot::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return dataCsv.size();
}

int TableModelPlot::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return dataCsv.at(0).size();
}

QVariant TableModelPlot::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        if (rowCount() > index.row()
                && columnCount() > index.column()) {
            return dataCsv.at(index.row()).at(index.column());
        }
    }
    return QVariant();
}

QVariant TableModelPlot::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            if (headerDataCsv.size() > section) {
                return headerDataCsv.at(section);
            }
        }
    }
    return QVariant();
}

QStringList TableModelPlot::GetHeaderData() const
{
    return headerDataCsv;
}

void TableModelPlot::Clear()
{
    for (QStringList row : dataCsv) {
        row.clear();
    }
    dataCsv.clear();
    headerDataCsv.clear();
}

bool TableModelPlot::ReadCsv(const QString &fileName)
{
    Clear();

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QTextStream csvStream(&file);

    headerDataCsv = csvStream.readLine().split(QRegExp(",|;"));

    while (!csvStream.atEnd()) {
        QStringList row;
        row << csvStream.readLine().split(QRegExp(",|;"));
        if (row.size() == headerDataCsv.size()) {
            dataCsv.append(row);
            trackIds.insert(row.at(1).toInt());
        }
    }

    return true;
}

bool TableModelPlot::ReadTrack(const QString &fileName, int trackId)
{
    Clear();

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QTextStream csvStream(&file);

    headerDataCsv = csvStream.readLine().split(QRegExp(",|;"));

    while (!csvStream.atEnd()) {
        QStringList row;
        row << csvStream.readLine().split(QRegExp(",|;"));
        if (row.size() == headerDataCsv.size() && row.at(1).toInt() == trackId) {
            dataCsv.append(row);
        }
    }

    return true;
}

std::set<int> TableModelPlot::GetTrackIds()
{
    return trackIds;
}
