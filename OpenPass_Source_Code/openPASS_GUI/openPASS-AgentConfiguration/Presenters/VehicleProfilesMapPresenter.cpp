/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "VehicleProfilesMapPresenter.h"

VehicleProfilesMapPresenter::VehicleProfilesMapPresenter(AgentItemInterface::VehicleProfiles *vehicleProfiles,
                                                         QObject *parent)
    : QAbstractTableModel(parent)
    , vehicleProfiles(vehicleProfiles)
{
    _columnHeaders << "Vehicle Profile" << "Probability";
}

QVariant VehicleProfilesMapPresenter::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( (role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
        return _columnHeaders.value(section);

    if((role == Qt::DisplayRole) && (orientation == Qt::Vertical))
        return section + 1;

    return QVariant();
}

int VehicleProfilesMapPresenter::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if(vehicleProfiles)
        return vehicleProfiles->count();
    else
        return 0;
}

int VehicleProfilesMapPresenter::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return _columnHeaders.count();
}

QVariant VehicleProfilesMapPresenter::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();

    if(!vehicleProfiles && row >= vehicleProfiles->count())
        return QVariant::Invalid;

    if( role == Qt::TextAlignmentRole)
        return QVariant( Qt::AlignCenter );

    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if(col == 0)
            return QVariant(RowToProfile[row]);
        else if(col == 1)
            return QVariant(vehicleProfiles->value( RowToProfile[row] ));
    }

    return QVariant();
}

bool VehicleProfilesMapPresenter::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int row = index.row();
    int col = index.column();

    if(vehicleProfiles && role == Qt::EditRole)
    {
        if(col == 0 && !vehicleProfiles->keys().contains(value.toString()))
        {
            vehicleProfiles->insert( value.toString(), vehicleProfiles->take(RowToProfile[row]) );
            RowToProfile.take( row );
            RowToProfile.insert( row, value.toString() );
            Q_EMIT dataChanged(index, index);
            return true;
        }
        else if ( col ==1 && vehicleProfiles->contains(RowToProfile[row]))
        {
            (*vehicleProfiles)[RowToProfile[row]] = value.toDouble();
            Q_EMIT dataChanged(index, index);
            Q_EMIT checkProbabilities();
            return true;
        }
    }

    return false;
}

Qt::ItemFlags VehicleProfilesMapPresenter::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

void VehicleProfilesMapPresenter::addRow(const int &row,
                                         const AgentItemInterface::VehicleProfile &profile,
                                         const AgentItemInterface::Probability &probability)
{
    if(vehicleProfiles)
    {
        for(auto _row= RowToProfile.size()-1; _row>=row; _row--)
            RowToProfile.insert(_row + 1, RowToProfile.take(_row));

        RowToProfile.insert( row, profile );

        beginInsertRows(QModelIndex(), row, row );
        vehicleProfiles->insert(profile, probability);
        endInsertRows();
        Q_EMIT checkProbabilities();
    }

}

bool VehicleProfilesMapPresenter::removeRow(const int &row)
{
    if(vehicleProfiles && RowToProfile.contains(row) && vehicleProfiles->contains(RowToProfile[row]))
    {
        beginRemoveRows(QModelIndex(), row, row);

        vehicleProfiles->take( RowToProfile[row] );
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

void VehicleProfilesMapPresenter::setVehicleProfiles(AgentItemInterface::VehicleProfiles * const _vehicleProfiles)
{
    vehicleProfiles=_vehicleProfiles;
    update();
}


void VehicleProfilesMapPresenter::update()
{
    int rows = 0;
    beginResetModel();
    endResetModel();
    RowToProfile.clear();

    if(vehicleProfiles)
    {
        for( auto profile : vehicleProfiles->keys())
        {
            RowToProfile.insert(rows, profile);
            rows++;
        }
    }
    Q_EMIT checkProbabilities();
}

void VehicleProfilesMapPresenter::editName(const AgentItemInterface::VehicleProfile &oldName,
                                           const AgentItemInterface::VehicleProfile &newName)
{
    if(vehicleProfiles && vehicleProfiles->contains(oldName))
    {
        vehicleProfiles->insert( newName, vehicleProfiles->take(oldName));

        int row = RowToProfile.key(oldName);
        RowToProfile.insert(row, newName);
    }
}

void VehicleProfilesMapPresenter::remove(const AgentItemInterface::VehicleProfile &profile)
{
    removeRow(RowToProfile.key(profile));
}
