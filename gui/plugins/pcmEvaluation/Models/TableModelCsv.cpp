/*********************************************************************
* Copyright (c) 2017, 2018, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "TableModelCsv.h"

TableModelCsv::TableModelCsv(QObject *parent, const QString &fileName, unsigned indexAgent) :
    QAbstractTableModel(parent)
{
    ReadCsv(fileName, indexAgent);
}

TableModelCsv::~TableModelCsv()
{
    Clear();
}

int TableModelCsv::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return dataCsv.size();
}

int TableModelCsv::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return dataCsv.at(0).size();
}

QVariant TableModelCsv::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (rowCount() > index.row() && columnCount() > index.column())
        {
            return dataCsv.at(index.row()).at(index.column());
        }
    }
    return QVariant();
}

QVariant TableModelCsv::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            if (headerDataCsv.size() > section)
            {
                return headerDataCsv.at(section);
            }
        }
    }
    return QVariant();
}

void TableModelCsv::Clear()
{
    for (QStringList row : dataCsv)
    {
        row.clear();
    }
    dataCsv.clear();
    headerDataCsv.clear();
}

bool TableModelCsv::ReadCsv(const QString &fileName, unsigned indexAgent)
{
    Clear();

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QTextStream csvStream(&file);

    headerDataCsv = csvStream.readLine().split(QRegExp(",|;"));

    while (!csvStream.atEnd())
    {
        QStringList row;
        row << csvStream.readLine().split(QRegExp(",|;"));
        if (row.size() == headerDataCsv.size() && row.at(1) == QString::number(indexAgent))
        {
            dataCsv.append(row);
        }
    }

    CreateTrajectory();

    return true;
}

QVector<QPointF> *TableModelCsv::GetTrajectoryData()
{
    if (trajectoryPoints.isEmpty())
    {
        return nullptr;
    }

    return &trajectoryPoints;
}

void TableModelCsv::CreateTrajectory()
{
    int iX = -1;
    int iY = -1;
    int iH = 0;

    // find X and Y columns
    for (auto head : headerDataCsv)
    {
        if (head == "XPos" || head == "XPosition")
        {
            iX = iH;
        }
        else if (head == "YPos" || head == "YPosition")
        {
            iY = iH;
        }
        iH++;
    }

    if (iX > -1 && iY > -1)
    {
        for (QStringList row : dataCsv)
        {
            AddPoint(row.at(iX), row.at(iY));
        }
    }
}

void TableModelCsv::AddPoint(QString xPos, QString yPos)
{
    trajectoryPoints.push_back(QPointF(xPos.toDouble(), yPos.toDouble()));
}
