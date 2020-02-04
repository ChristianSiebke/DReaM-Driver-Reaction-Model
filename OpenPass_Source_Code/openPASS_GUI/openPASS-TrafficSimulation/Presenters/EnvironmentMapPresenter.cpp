/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "EnvironmentMapPresenter.h"

EnvironmentMapPresenter::EnvironmentMapPresenter(EnvironmentInterface * const environment,
                                                 QMap<ID, Item*> * environmentMap,
                                                 QUndoStack * const undoStack,
                                                 QObject *parent)
    : QAbstractTableModel(parent)
    , environment(environment)
    , environmentMap(environmentMap)
    , undoStack(undoStack)
{
    _columnHeaders << "Value" << "Probability";
}


int EnvironmentMapPresenter::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _columnHeaders.count();
}

int EnvironmentMapPresenter::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return environmentMap->count();
}

QVariant EnvironmentMapPresenter::headerData(int section,
                                             Qt::Orientation orientation,
                                             int role) const
{
    if( (role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
        return _columnHeaders.value(section);

    if((role == Qt::DisplayRole) && (orientation == Qt::Vertical))
        return section + 1;

    return QVariant();
}

QVariant EnvironmentMapPresenter::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();
    EnvironmentInterface::Item * item = environmentMap->value(RowToID[row]);

    if(row >= environmentMap->count())
        return QVariant::Invalid;

    if(role == Qt::TextAlignmentRole)
        return QVariant(Qt::AlignCenter);

    if(item && (role == Qt::DisplayRole || role == Qt::EditRole))
    {
       if( col == 0)
            return QVariant(item->getValue());
        else if(col == 1)
            return QVariant(item->getProbability());
    }

    return QVariant();
}

bool EnvironmentMapPresenter::setData(const QModelIndex &index,
                                      const QVariant &value,
                                      int role)
{
    int row = index.row();
    int col = index.column();
    EnvironmentInterface::Item * item = environmentMap->value(RowToID[row]);

    if(role == Qt::EditRole)
    {
        if(col == 0)
        {
            item->setValue(value.toString());
            Q_EMIT dataChanged(index, index);
            return true;
        }
        else if(col == 1)
        {
            item->setProbability(value.toDouble());
            Q_EMIT dataChanged(index, index);
            return true;
        }
    }

    return false;
}

Qt::ItemFlags EnvironmentMapPresenter::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

void EnvironmentMapPresenter::addRow(const int &row, QString value, double probability)
{
    EnvironmentInterface::ID Id = (environmentMap->isEmpty() ? 1 : environmentMap->lastKey() +1 );

    // shift all row indices,starting from given row, up
    for(auto _row= RowToID.size()-1; _row>=row; _row--)
        RowToID.insert(_row + 1, RowToID.take(_row));

    // insert row with new index
    RowToID.insert(row , Id);

    beginInsertRows(QModelIndex(), row, row);

    EnvironmentInterface::Item * item = environment->createItem(value, probability);
    environmentMap->insert(Id, item);
    Q_EMIT environment->edited();

    endInsertRows();
}

bool EnvironmentMapPresenter::removeRow(const int &row)
{
    if( RowToID.contains(row) )
    {
        beginRemoveRows(QModelIndex(), row, row);
        delete environmentMap->take(RowToID[row]);
        RowToID.take(row);

        // shift the ID to row map for everything below this Item
        for( int i = row + 1; i<=RowToID.count() ; i++)
            RowToID.insert( i-1, RowToID.take(i) );

        Q_EMIT environment->edited();
        endRemoveRows();

        return true;
    }
    else
        return false;
}


void EnvironmentMapPresenter::update()
{
    int rows = 0;

    beginResetModel();
    endResetModel();
    RowToID.clear();

    for(auto key : environmentMap->keys())
    {
        RowToID.insert(rows, key);
        rows++;
    }

    Q_EMIT environment->edited();
}
