/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "LibrariesPresenter.h"

#include <QBrush>
#include <QFileInfo>

LibrariesPresenter::LibrariesPresenter(ExperimentInterface::Libraries *const libraries,
                                       ProjectInterface *const project,
                                       QObject *parent) :
    QAbstractTableModel(parent), libraries(libraries), project(project)
{
    _columnHeaders << "Library Type"
                   << "Library Name";
}

int LibrariesPresenter::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return libraries->count();
}

int LibrariesPresenter::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _columnHeaders.count();
}

QVariant LibrariesPresenter::headerData(int section,
                                        Qt::Orientation orientation,
                                        int role) const
{
    if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
        return _columnHeaders.value(section);

    if ((role == Qt::DisplayRole) && (orientation == Qt::Vertical))
        return section + 1;

    return QVariant();
}

QVariant LibrariesPresenter::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();

    if (row >= libraries->count())
        return QVariant::Invalid;

    if (role == Qt::TextAlignmentRole)
        return QVariant(Qt::AlignCenter);

    if (role == Qt::TextColorRole)
    {
        if (col == 1)
        {
            QString lib = libraries->value(RowToLibType[row]);

            if (!QFileInfo::exists(project->getLibrary().absoluteFilePath(lib + QString(".dll"))) && !lib.isEmpty())
                return QBrush(Qt::red);
            else
            {
                return QBrush(Qt::black);
            }
        }
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (col == 0)
            return QVariant(RowToLibType.value(row));
        else if (col == 1)
        {
            QString lib = libraries->value(RowToLibType[row]);

            if (!QFileInfo::exists(project->getLibrary().absoluteFilePath(lib + QString(".dll"))) && !lib.isEmpty())
                return QString("%1 (not existing!)").arg(lib);

            else
                return lib;
        }
    }

    return QVariant();
}

bool LibrariesPresenter::setData(const QModelIndex &index,
                                 const QVariant &value,
                                 int role)
{
    int row = index.row();
    int col = index.column();

    if (role == Qt::EditRole)
    {
        if (col == 0 && !libraries->contains(value.toString()))
        {
            libraries->insert(value.toString(), libraries->take(RowToLibType[row]));
            RowToLibType[row] = value.toString();
            Q_EMIT dataChanged(index, index);
            return true;
        }
        else if (col == 1)
        {
            (*libraries)[RowToLibType[row]] = value.toString();
            Q_EMIT dataChanged(index, index);
            return true;
        }
    }

    return false;
}

Qt::ItemFlags LibrariesPresenter::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

void LibrariesPresenter::addRow(const int &row, QString libType)
{
    // shift all row indices,starting from given row, up
    for (auto _row = RowToLibType.size() - 1; _row >= row; _row--)
        RowToLibType.insert(_row + 1, RowToLibType.take(_row));

    beginInsertRows(QModelIndex(), row, row);

    RowToLibType.insert(row, libType);
    libraries->insert(libType, "");

    endInsertRows();
}

bool LibrariesPresenter::removeRow(const int &row)
{
    if (RowToLibType.contains(row))
    {
        beginRemoveRows(QModelIndex(), row, row);
        libraries->take(RowToLibType[row]);
        RowToLibType.take(row);

        for (int i = row + 1; i <= RowToLibType.count(); i++)
            RowToLibType.insert(i - 1, RowToLibType.take(i));

        endRemoveRows();
        return true;
    }

    return false;
}

void LibrariesPresenter::update()
{
    int rows = 0;

    beginResetModel();
    endResetModel();
    RowToLibType.clear();

    for (auto key : libraries->keys())
    {
        RowToLibType.insert(rows, key);
        rows++;
    }
}
