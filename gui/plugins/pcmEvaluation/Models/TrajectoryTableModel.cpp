/********************************************************************************
 * Copyright (c) 2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "TrajectoryTableModel.h"

TrajectoryTableModel::TrajectoryTableModel(QObject *parent, const QString &fileName, unsigned indexAgent) :
    QAbstractTableModel(parent)
{
    ReadCsv(fileName, indexAgent);
}

TrajectoryTableModel::~TrajectoryTableModel()
{
    Clear();
}

int TrajectoryTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return dataCsv.size();
}

int TrajectoryTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return dataCsv.at(0).size();
}

QVariant TrajectoryTableModel::data(const QModelIndex &index, int role) const
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

QVariant TrajectoryTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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

void TrajectoryTableModel::Clear()
{
    dataCsv.clear();
    headerDataCsv.clear();
}

bool TrajectoryTableModel::ReadCsv(const QString &fileName, unsigned indexAgent)
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

QVector<QPointF> *TrajectoryTableModel::GetTrajectoryData()
{
    if (trajectoryPoints.isEmpty())
    {
        return nullptr;
    }

    return &trajectoryPoints;
}

void TrajectoryTableModel::CreateTrajectory()
{
    int x = -1;
    int y = -1;
    int h = 0;

    // find X and Y columns
    for (auto head : headerDataCsv)
    {
        if (head == "XPos" || head == "XPosition")
        {
            x = h;
        }
        else if (head == "YPos" || head == "YPosition")
        {
            y = h;
        }
        h++;
    }

    if (x > -1 && y > -1)
    {
        for (QStringList row : dataCsv)
        {
            AddPoint(row.at(x), row.at(y));
        }
    }
}

void TrajectoryTableModel::AddPoint(QString xPos, QString yPos)
{
    trajectoryPoints.push_back(QPointF(xPos.toDouble(), yPos.toDouble()));
}
