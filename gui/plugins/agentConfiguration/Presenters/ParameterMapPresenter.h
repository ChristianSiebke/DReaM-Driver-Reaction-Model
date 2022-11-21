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
//! @file  ParameterMapPresenter.h
//! @ingroup agentConfigurationPlugin
//! @brief This class constitutes the re-implementation of QAbstractTableModel
//!        for the table view of parameters. In fact, this class implements
//!        a wrapper for the map of parameters (ParameterMapInterface)
//!        to be editable by QTableView objects within Qt's Model/View framework.
//-----------------------------------------------------------------------------

#ifndef PARAMETERMAPPRESENTER_H
#define PARAMETERMAPPRESENTER_H

#include <QAbstractTableModel>

#include "openPASS-AgentConfiguration/ParameterMapInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class constitutes the re-implementation of QAbstractTableModel
//!        for the table view of parameters. In fact, this class implements
//!        a wrapper for the map of parameters (ParameterMapInterface)
//!        to be editable by QTableView objects within Qt's Model/View framework.
//-----------------------------------------------------------------------------
class ParameterMapPresenter : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ParameterMapPresenter(ParameterMapInterface *parameters = nullptr,
                                   QObject *parent = nullptr);

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
    //! rows in the table which is, in fact, the number of parameters.
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
    //! Method that adds a new row, i.e. a new parameter with the given attributes.
    //! @param[in]      _type       the datatype of the new parameter
    //! @param[in]      _key        the key (name) of the new parameter
    //! @param[in]      _unit       the unit of the new parameter
    //! @param[in]      _value      the value of the new parameter
    //-----------------------------------------------------------------------------
    virtual void addRow(ParameterItemInterface::Type _type,
                        ParameterItemInterface::Key _key,
                        ParameterItemInterface::Unit _unit,
                        ParameterItemInterface::Value _value);

    //-----------------------------------------------------------------------------
    //! Method that removes the row specified by the given row-index, if this is in
    //! a meaningful interval.
    //!
    //! @param[in]      row     the row-index
    //!
    //! @return                 "True", if row is between 0 and the number of parameters,
    //!                         "false", otherwise
    //-----------------------------------------------------------------------------
    virtual bool removeRow(const int &row);

    //-----------------------------------------------------------------------------
    //! Method which sets the currently processed parameter map to the given
    //! argument.
    //!
    //! @param[in]      _parameters        the parameter map to be processed
    //-----------------------------------------------------------------------------
    void setParameters(ParameterMapInterface *_parameters);

    //-----------------------------------------------------------------------------
    //! Method which updates the table according to the current entries of
    //! #parameters
    //-----------------------------------------------------------------------------
    void update();

private:
    QMap<int, ParameterMapInterface::ID> RowToID; //!< a map of row indices to parameter Ids
    QStringList _columnHeaders;                   //!< a string list containing the column header data of the table
    ParameterMapInterface *parameters;            //!< the pointer to the map of parameters edited by this table wrapper
};

#endif // PARAMETERMAPPRESENTER_H
