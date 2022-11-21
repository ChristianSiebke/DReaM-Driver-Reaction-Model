/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "TrafficItemMapPresenter.h"

TrafficItemMapPresenter::TrafficItemMapPresenter(TrafficItemMapInterface *const trafficItemMap,
                                                 QObject *parent) :
    QAbstractTableModel(parent), trafficItemMap(trafficItemMap)
{
    _columnHeaders << "Value"
                   << "Probability";
}

QVariant TrafficItemMapPresenter::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
        return _columnHeaders.value(section);

    if ((role == Qt::DisplayRole) && (orientation == Qt::Vertical))
        return section + 1;

    return QVariant();
}

int TrafficItemMapPresenter::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return trafficItemMap->count();
}

int TrafficItemMapPresenter::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _columnHeaders.count();
}

QVariant TrafficItemMapPresenter::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();
    TrafficItemMapInterface::Item *item = trafficItemMap->getItem(RowToID[row]);

    if (row >= trafficItemMap->count())
        return QVariant::Invalid;

    if (role == Qt::TextAlignmentRole)
        return QVariant(Qt::AlignCenter);

    if (item && (role == Qt::DisplayRole || role == Qt::EditRole))
    {
        if (col == 0)
            return QVariant(item->getValue());

        else if (col == 1)
            return QVariant(item->getProbability());
    }

    return QVariant();
}

bool TrafficItemMapPresenter::setData(const QModelIndex &index,
                                      const QVariant &value,
                                      int role)
{
    int row = index.row();
    int col = index.column();
    TrafficItemMapInterface::Item *item = trafficItemMap->getItem(RowToID[row]);

    if (role == Qt::EditRole)
    {
        if (col == 0)
        {
            item->setValue(value.toString());
            Q_EMIT dataChanged(index, index);
            Q_EMIT trafficItemMap->edited();
            return true;
        }
        else if (col == 1)
        {
            item->setProbability(value.toDouble());
            Q_EMIT dataChanged(index, index);
            Q_EMIT trafficItemMap->edited();
            return true;
        }
    }

    return false;
}

Qt::ItemFlags TrafficItemMapPresenter::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

void TrafficItemMapPresenter::addRow(const int &row,
                                     TrafficItemInterface::Value const &value,
                                     TrafficItemInterface::Probability const &probability)
{
    TrafficItemMapInterface::ID Id = trafficItemMap->generateID();

    // shift all row indices, starting from given row, up
    for (auto _row = RowToID.size() - 1; _row >= row; _row--)
        RowToID.insert(_row + 1, RowToID.take(_row));

    RowToID.insert(row, Id);

    beginInsertRows(QModelIndex(), row, row);
    trafficItemMap->add(Id, probability, value);
    endInsertRows();
}

bool TrafficItemMapPresenter::removeRow(const int &row)
{
    if (RowToID.contains(row))
    {
        beginRemoveRows(QModelIndex(), row, row);
        trafficItemMap->remove(RowToID[row]);
        RowToID.take(row);

        // shift the ID to row map for everything below this Item
        for (int i = row + 1; i <= RowToID.count(); i++)
            RowToID.insert(i - 1, RowToID.take(i));

        endRemoveRows();

        return true;
    }
    else
        return false;
}

void TrafficItemMapPresenter::update()
{
    int rows = 0;

    beginResetModel();
    endResetModel();
    RowToID.clear();

    for (auto item : trafficItemMap->values())
    {
        RowToID.insert(rows, trafficItemMap->getID(item));
        rows++;
    }
}
