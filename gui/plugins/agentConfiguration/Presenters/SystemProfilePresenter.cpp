/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "SystemProfilePresenter.h"

SystemProfilePresenter::SystemProfilePresenter(VehicleProfileItemInterface::Systems *const systems,
                                               QObject *parent) :
    QAbstractTableModel(parent), systems(systems)
{
    _columnHeaders << "ID"
                   << "Title"
                   << "Probability";
}

QVariant SystemProfilePresenter::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
        return _columnHeaders.value(section);

    if ((role == Qt::DisplayRole) && (orientation == Qt::Vertical))
        return section + 1;

    return QVariant();
}

int SystemProfilePresenter::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return systems->count();
}

int SystemProfilePresenter::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _columnHeaders.count();
}

QVariant SystemProfilePresenter::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();

    if (row >= systems->count())
        return QVariant::Invalid;

    if (role == Qt::TextAlignmentRole)
        return QVariant(Qt::AlignCenter);

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (col == 0)
            return QVariant(RowToSystem[row].first);

        else if (col == 1)
            return QVariant(RowToSystem[row].second);

        else if (col == 2)
            return QVariant(systems->value(RowToSystem[row]));
    }

    return QVariant();
}

bool SystemProfilePresenter::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int row = index.row();
    int col = index.column();

    if (role == Qt::EditRole)
    {
        if (col == 2)
        {
            (*systems)[RowToSystem[row]] = value.toDouble();
            Q_EMIT dataChanged(index, index);
            return true;
        }
    }

    return false;
}

Qt::ItemFlags SystemProfilePresenter::flags(const QModelIndex &index) const
{
    if (index.column() == 2)
        return Qt::ItemIsEditable | QAbstractTableModel::flags(index);

    return QAbstractTableModel::flags(index);
}

void SystemProfilePresenter::addRow(const int &row,
                                    VehicleProfileItemInterface::System system,
                                    VehicleProfileItemInterface::Probability probability)
{
    if (!systems->contains(system))
    {
        for (auto _row = RowToSystem.size() - 1; _row >= row; _row--)
            RowToSystem.insert(_row + 1, RowToSystem.take(_row));

        RowToSystem.insert(row, system);

        beginInsertRows(QModelIndex(), row, row);
        systems->insert(system, probability);
        endInsertRows();
    }
}

bool SystemProfilePresenter::removeRow(const int &row)
{
    if (RowToSystem.contains(row) && systems->contains(RowToSystem[row]))
    {
        beginRemoveRows(QModelIndex(), row, row);

        systems->remove(RowToSystem[row]);
        RowToSystem.remove(row);

        for (int i = row + 1; i <= RowToSystem.count(); i++)
            RowToSystem.insert(i - 1, RowToSystem.take(i));

        endRemoveRows();
        return true;
    }

    return false;
}

void SystemProfilePresenter::setSystems(VehicleProfileItemInterface::Systems *const _systems)
{
    systems = _systems;

    if (systems)
        update();
}

void SystemProfilePresenter::update()
{
    int rows = 0;
    beginResetModel();
    endResetModel();
    RowToSystem.clear();

    for (auto system : systems->keys())
    {
        RowToSystem.insert(rows, system);
        rows++;
    }
}
