/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "LoggingGroupsPresenter.h"

LoggingGroupsPresenter::LoggingGroupsPresenter(ExperimentInterface::LoggingGroups * const loggingGroups,
                                               QObject *parent)
    : QAbstractListModel(parent)
    , loggingGroups(loggingGroups)
{
}

int LoggingGroupsPresenter::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return loggingGroups->count();
}

QVariant LoggingGroupsPresenter::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant::Invalid;

    if(role == Qt::TextAlignmentRole)
        return QVariant(Qt::AlignCenter);

    if( role == Qt::DisplayRole || role == Qt::EditRole )
        return QVariant(loggingGroups->at(index.row()));

    return QVariant();
}

bool LoggingGroupsPresenter::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value && role == Qt::EditRole)
    {
        if(!loggingGroups->contains(value.toString()))
            (*loggingGroups)[index.row()] = value.toString();

        Q_EMIT dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags LoggingGroupsPresenter::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractListModel::flags(index);
}

void LoggingGroupsPresenter::insertRow(int row, QString item, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row );

    loggingGroups->insert(row, item);

    endInsertRows();
}

void LoggingGroupsPresenter::removeRow(int row, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row);

    loggingGroups->removeAt(row);

    endRemoveRows();
}


void LoggingGroupsPresenter::update()
{
    beginResetModel();
    endResetModel();
}
