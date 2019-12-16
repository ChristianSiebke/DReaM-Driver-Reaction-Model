/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef VEHICLEPROFILESMAPPRESENTER_H
#define VEHICLEPROFILESMAPPRESENTER_H

#include "openPASS-AgentConfiguration/AgentItemInterface.h"

#include <QAbstractTableModel>

class VehicleProfilesMapPresenter : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit VehicleProfilesMapPresenter( AgentItemInterface::VehicleProfiles * vehicleProfiles = nullptr,
                                          QObject *parent = nullptr);

Q_SIGNALS:
    void checkProbabilities();

public:
    //-----------------------------------------------------------------------------
    //! Reimplementation of the headerData method. Returns the data of the table's header
    //! in the table section passed as a first argument. Further, the given role needs to be
    //! Qt::DisplayRole and the given orientation needs to be horizontal.
    //! The header data (stored in #_columnHeaders) is returned in the QVariant data format.
    //!
    //! @param[in]      section     A table section
    //! @param[in]      role        A Qt role
    //! @param [in]     orientation A table orientation
    //!
    //! @return                     The content of the table's header at the given section
    //-----------------------------------------------------------------------------
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public:
    //-----------------------------------------------------------------------------
    //! Reimplementation of the rowCount method. Returns the number of
    //! rows in the table which is, in fact, the number of environment items.
    //!
    //! @param[in]      parent      This variable is unused (but required according to
    //!                             the signature of the method)
    //!
    //! @return                     The number of table rows
    //-----------------------------------------------------------------------------
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    //-----------------------------------------------------------------------------
    //! Reimplementation of the columnCount method. Returns the number of
    //! columns, in fact the number of list entries  of #_columnHeaders.
    //!
    //! @param[in]      parent      This variable is unused (but required according to
    //!                             the signature of the method)
    //!
    //! @return                     The number of table columns
    //-----------------------------------------------------------------------------
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

public:
    //-----------------------------------------------------------------------------
    //! Reimplementation of the data method. Returns the data stored in the table cell
    //! referenced by the given index (QModelIndex).
    //!
    //! @param[in]      index       An index identifying the cell of the table
    //! @param[in]      role        A Qt role (needs to be in Qt::DisplayRole or Qt::EditRole)
    //!
    //! @return                     The data stored in the table cell (as a QVariant)
    //-----------------------------------------------------------------------------
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    //-----------------------------------------------------------------------------
    //! Reimplementation of the setData method. Sets the data stored in the table cell
    //! referenced by the given index (QModelIndex) to the given value.
    //!
    //! @param[in]      index       An index identifying the cell of the table
    //! @param[in]      role        A Qt role (needs to be in Qt::EditRole)
    //!
    //! @return                     "True" if the index corresponds to an existing cell in the table,
    //!                             otherwise "false"
    //-----------------------------------------------------------------------------
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    //-----------------------------------------------------------------------------
    //! Reimplementation of the flags method. This will make the items of the model
    //! editable!
    //!
    //! @param[in]      index       An index identifying the cell of the table
    //!
    //! @return                     An item flag
    //-----------------------------------------------------------------------------
    Qt::ItemFlags flags(const QModelIndex& index) const override;

public:
    //-----------------------------------------------------------------------------
    //! Method that adds a new row, i.e. a new environment item to the edited
    //! component.
    //!
    //-----------------------------------------------------------------------------
    virtual void addRow(int const &row,
                        AgentItemInterface::VehicleProfile const & profile,
                        AgentItemInterface::Probability const & probability);

    //-----------------------------------------------------------------------------
    //! Method that removes the row specified by the given row-index, if this is in
    //! a meaningful interval.
    //!
    //! @param[in]      row     the row-index
    //!
    //! @return                 "True", if row is between 0 and the number of inputs,
    //!                         "false", otherwise
    //-----------------------------------------------------------------------------
    virtual bool removeRow(const int &row);

    //-----------------------------------------------------------------------------
    //! Method which sets the currently processed vehicle profiles map to the given
    //! argument.
    //!
    //! @param[in]      _vehicleProfiles        the vehicle profiles map to be processed
    //-----------------------------------------------------------------------------
    void setVehicleProfiles(AgentItemInterface::VehicleProfiles * const _vehicleProfiles);

    //-----------------------------------------------------------------------------
    //! Method which updates the table according to the current entries of
    //! #vehicleProfiles
    //-----------------------------------------------------------------------------
    void update();

public Q_SLOTS:
    void editName(AgentItemInterface::VehicleProfile const & oldName,
                  AgentItemInterface::VehicleProfile const & newName);

    void remove(AgentItemInterface::VehicleProfile const & profile);


private:
    AgentItemInterface::VehicleProfiles * vehicleProfiles;

private:
    QStringList _columnHeaders;
    QMap<int, AgentItemInterface::VehicleProfile> RowToProfile;
};

#endif // VEHICLEPROFILESMAPPRESENTER_H
