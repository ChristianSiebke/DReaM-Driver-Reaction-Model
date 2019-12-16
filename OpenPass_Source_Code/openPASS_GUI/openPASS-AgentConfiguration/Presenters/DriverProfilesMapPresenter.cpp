/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "DriverProfilesMapPresenter.h"

DriverProfilesMapPresenter::DriverProfilesMapPresenter(AgentItemInterface::DriverProfiles * driverProfiles,
                                                       QObject *parent)
    : QAbstractTableModel(parent)
    , driverProfiles(driverProfiles)
{
    _columnHeaders << "Driver Profile" << "Probability";
}

QVariant DriverProfilesMapPresenter::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( (role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
        return _columnHeaders.value(section);

    if((role == Qt::DisplayRole) && (orientation == Qt::Vertical))
        return section + 1;

    return QVariant();
}

int DriverProfilesMapPresenter::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if(driverProfiles)
        return driverProfiles->count();
    else
        return 0;
}

int DriverProfilesMapPresenter::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return _columnHeaders.count();
}

QVariant DriverProfilesMapPresenter::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();

    if(!driverProfiles && row >= driverProfiles->count())
        return QVariant::Invalid;

    if( role == Qt::TextAlignmentRole)
        return QVariant( Qt::AlignCenter );

    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if(col == 0)
            return QVariant(RowToProfile[row]);
        else if(col == 1)
            return QVariant(driverProfiles->value( RowToProfile[row] ));
    }

    return QVariant();
}

bool DriverProfilesMapPresenter::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int row = index.row();
    int col = index.column();

    if(driverProfiles && role == Qt::EditRole)
    {
        if(col == 0 && !driverProfiles->keys().contains(value.toString()))
        {
            driverProfiles->insert( value.toString(), driverProfiles->take(RowToProfile[row]) );
            RowToProfile.take( row );
            RowToProfile.insert( row, value.toString() );
            Q_EMIT dataChanged(index, index);
            return true;
        }
        else if ( col ==1 && driverProfiles->contains(RowToProfile[row]))
        {
            (*driverProfiles)[RowToProfile[row]] = value.toDouble();
            Q_EMIT dataChanged(index, index);
            Q_EMIT checkProbabilities();
            return true;
        }
    }

    return false;
}

Qt::ItemFlags DriverProfilesMapPresenter::flags(const QModelIndex &index) const
{

    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

void DriverProfilesMapPresenter::addRow(int const &row,
                                        const AgentItemInterface::DriverProfile &profile,
                                        const AgentItemInterface::Probability &probability)
{
    // shift all row indices up
    for(auto _row = RowToProfile.size()-1 ; _row>= row; _row--)
        RowToProfile.insert(_row + 1, RowToProfile.take(_row));

    RowToProfile.insert( row, profile );

    if(driverProfiles)
    {
        beginInsertRows(QModelIndex(), row, row );
        driverProfiles->insert(profile, probability);
        endInsertRows();
        Q_EMIT checkProbabilities();
    }

}

bool DriverProfilesMapPresenter::removeRow(const int &row)
{
    if(driverProfiles && RowToProfile.contains(row) && driverProfiles->contains(RowToProfile[row]))
    {
        beginRemoveRows(QModelIndex(), row, row);

        driverProfiles->take( RowToProfile[row] );
        RowToProfile.take(row);

        for( int i = row + 1; i<=RowToProfile.count() ; i++ )
            RowToProfile.insert( i-1, RowToProfile.take(i));

        endRemoveRows();
        Q_EMIT checkProbabilities();
        return true;
    }
    else
        return false;
}

void DriverProfilesMapPresenter::setDriverProfiles(AgentItemInterface::DriverProfiles * const _driverProfiles)
{
    driverProfiles=_driverProfiles;
    update();
}


void DriverProfilesMapPresenter::update()
{
    int rows = 0;
    beginResetModel();
    endResetModel();
    RowToProfile.clear();

    if(driverProfiles)
    {
        for( auto profile : driverProfiles->keys())
        {
            RowToProfile.insert(rows, profile);
            rows++;
        }
    }
    Q_EMIT checkProbabilities();
}


void DriverProfilesMapPresenter::editName(const AgentItemInterface::DriverProfile &oldName,
                                          const AgentItemInterface::DriverProfile &newName)
{
    if(driverProfiles && driverProfiles->contains(oldName))
    {
        driverProfiles->insert( newName, driverProfiles->take(oldName));

        int row = RowToProfile.key(oldName);
        RowToProfile.insert(row, newName);
    }
}

void DriverProfilesMapPresenter::remove(const AgentItemInterface::DriverProfile &profile)
{
    removeRow(RowToProfile.key(profile));
}
