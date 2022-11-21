/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "ParameterMapPresenter.h"

ParameterMapPresenter::ParameterMapPresenter(ParameterMapInterface *parameters,
                                             QObject *parent) :
    QAbstractTableModel(parent), parameters(parameters)
{
    _columnHeaders << "Type"
                   << "Key"
                   << "Unit"
                   << "Value";
}

QVariant ParameterMapPresenter::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
        return _columnHeaders.value(section);

    if ((role == Qt::DisplayRole) && (orientation == Qt::Vertical))
        return section + 1;

    return QVariant();
}

int ParameterMapPresenter::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if (parameters)
        return parameters->count();
    else
        return 0;
}

int ParameterMapPresenter::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return _columnHeaders.count();
}

QVariant ParameterMapPresenter::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();
    ParameterItemInterface *const param = parameters->getItem(RowToID[row]);

    if (!param && row >= parameters->count())
        return QVariant::Invalid;

    if (role == Qt::TextAlignmentRole)
        return QVariant(Qt::AlignCenter);

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (col == 0)
            return QVariant(param->getType());
        else if (col == 1)
            return QVariant(param->getKey());
        else if (col == 2)
            return QVariant(param->getUnit());
        else if (col == 3)
            return QVariant(param->getValue());
    }

    return QVariant();
}

bool ParameterMapPresenter::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int row = index.row();
    int col = index.column();
    ParameterItemInterface *const param = parameters->getItem(RowToID[row]);

    if (!param && row >= parameters->count())
        return false;

    if (role == Qt::EditRole)
    {
        if (col == 3)
        {
            param->setValue(value.toString());
            Q_EMIT dataChanged(index, index);
            return true;
        }
    }

    return false;
}

Qt::ItemFlags ParameterMapPresenter::flags(const QModelIndex &index) const
{
    if (index.column() == 3)
        return Qt::ItemIsEditable | QAbstractTableModel::flags(index);

    return QAbstractTableModel::flags(index);
}

void ParameterMapPresenter::addRow(ParameterItemInterface::Type _type,
                                   ParameterItemInterface::Key _key,
                                   ParameterItemInterface::Unit _unit,
                                   ParameterItemInterface::Value _value)
{
    int row = rowCount();

    if (parameters)
    {
        beginInsertRows(QModelIndex(), row, row);

        ParameterMapInterface::ID id = parameters->generateID();
        parameters->add(id);
        RowToID.insert(row, id);

        ParameterItemInterface *const param = parameters->getItem(id);
        param->setType(_type);
        param->setKey(_key);
        param->setUnit(_unit);
        param->setValue(_value);

        endInsertRows();
    }
}

bool ParameterMapPresenter::removeRow(const int &row)
{
    if (parameters && RowToID.contains(row) && parameters->contains(RowToID[row]))
    {
        beginRemoveRows(QModelIndex(), row, row);

        parameters->remove(RowToID[row]);
        RowToID.remove(row);

        for (int i = row + 1; i <= RowToID.count(); i++)
            RowToID.insert(i - 1, RowToID.take(i));

        endRemoveRows();
        return true;
    }

    return false;
}

void ParameterMapPresenter::setParameters(ParameterMapInterface *_parameters)
{
    parameters = _parameters;
    update();
}

void ParameterMapPresenter::update()
{
    int rows = 0;
    beginResetModel();
    endResetModel();
    RowToID.clear();

    if (parameters)
    {
        for (auto id : parameters->keys())
        {
            RowToID.insert(rows, id);
            rows++;
        }
    }
}
