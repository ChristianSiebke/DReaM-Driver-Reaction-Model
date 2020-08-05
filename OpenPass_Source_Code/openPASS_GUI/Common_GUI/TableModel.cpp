/*********************************************************************
* Copyright (c) 2019 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "TableModel.h"


TableModel::TableModel(QObject *parent, const QString &fileName)
    : QAbstractTableModel(parent)
{
    ReadCsv(fileName);
}

TableModel::~TableModel()
{
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return dataCsv.at(0).size();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return dataCsv.size();
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        if (rowCount() > index.row()
                && columnCount() > index.column()) {
            return dataCsv.at(index.column()).at(index.row());
        }
    }
    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
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

QStringList TableModel::GetHeader() const
{
    return headerDataCsv;
}


void TableModel::Clear()
{
    for (auto row : dataCsv) {
        row.clear();
    }
    dataCsv.clear();
    headerDataCsv.clear();
}

bool TableModel::ReadCsv(const QString &fileName)
{
    Clear();

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    QTextStream csvStream(&file);
    headerDataCsv = csvStream.readLine().split(QRegExp(",|;"));

    std::vector<double> column;
    dataCsv.assign(headerDataCsv.size(), column);

    FillInData(&csvStream);
    return true;
}

bool TableModel::AppendCsv(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    QTextStream csvStream(&file);
    QStringList headerAppend = csvStream.readLine().split(QRegExp(",|;"));

    if (headerAppend != headerDataCsv) {
        return false;
    }

    FillInData(&csvStream);
    return true;
}

void TableModel::FillInData(QTextStream *csvStream)
{
    int col = 0;
    while (!csvStream->atEnd()) {
        QStringList row;
        row << csvStream->readLine().split(QRegExp(",|;"));

        if (row.size() == headerDataCsv.size()) {
            col = 0;
            for (QString item : row) {
                dataCsv.at(col).push_back(item.toDouble());
                col++;
            }
        }
    }
}

const std::vector<double> *TableModel::GetColumnVector(int section) const
{
    if (section < columnCount()) {
        return &dataCsv.at(section);
    } else {
        return nullptr;
    }
}
