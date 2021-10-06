/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  SystemProfilePresenter.h
//! @ingroup agentConfigurationPlugin
//! @brief This class constitutes the re-implementation of QAbstractTableModel
//!        for the table view of systems with a given algorithm type. The
//!        corresponding model data object is referred to as "system profile".
//!        In fact, this class implements a wrapper for these system profiles
//!        to be editable by QTableView objects within Qt's Model/View framework.
//-----------------------------------------------------------------------------

#ifndef SYSTEMPROFILEPRESENTER_H
#define SYSTEMPROFILEPRESENTER_H

#include <QAbstractTableModel>

#include "openPASS-AgentConfiguration/VehicleProfileItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class constitutes the re-implementation of QAbstractTableModel
//!        for the table view of systems with a given algorithm type. The
//!        corresponding model data object is referred to as "system profile".
//!        In fact, this class implements a wrapper for these system profiles
//!        to be editable by QTableView objects within Qt's Model/View framework.
//-----------------------------------------------------------------------------
class SystemProfilePresenter : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit SystemProfilePresenter(VehicleProfileItemInterface::Systems *const systems,
                                    QObject *parent = nullptr);

    ~SystemProfilePresenter() override = default;

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
    //! rows in the table which is, in fact, the number systems in the map.
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
    Qt::ItemFlags flags(const QModelIndex &index) const override;

public:
    //-----------------------------------------------------------------------------
    //! Method that adds a new row, i.e. a system with the given probability.
    //! @param[in]      row         the index of the row to be added
    //! @param[in]      system      the name of the system to be added
    //! @param[in]      probability the probability of the system
    //-----------------------------------------------------------------------------
    virtual void addRow(int const &row,
                        VehicleProfileItemInterface::System system,
                        VehicleProfileItemInterface::Probability probability);

    //-----------------------------------------------------------------------------
    //! Method that removes the row specified by the given row-index, if this is in
    //! a meaningful interval.
    //!
    //! @param[in]      row     the row-index
    //!
    //! @return                 "True", if row is between 0 and the number of systems,
    //!                         "false", otherwise
    //-----------------------------------------------------------------------------
    virtual bool removeRow(const int &row);

    //-----------------------------------------------------------------------------
    //! Method which sets the currently processed systems map to the given
    //! argument.
    //!
    //! @param[in]      _systems        the systems map to be processed
    //-----------------------------------------------------------------------------
    void setSystems(VehicleProfileItemInterface::Systems *const _systems);

    //-----------------------------------------------------------------------------
    //! Method which updates the table according to the current entries of
    //! #systems
    //-----------------------------------------------------------------------------
    void update();

private:
    VehicleProfileItemInterface::Systems *systems; //!< the map of systems to be edited by this presenter

private:
    QStringList _columnHeaders;                                 //!< the string list of header data for columns
    QMap<int, VehicleProfileItemInterface::System> RowToSystem; //!< the mapping of row indices to systems
};

#endif // SYSTEMPROFILEPRESENTER_H
