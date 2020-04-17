/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  LibrariesPresenter.h
//! @ingroup trafficSimulationPlugin
//! @brief This class describes the presenter object of the list of logging
//!        groups.
//!
//!        In the spirit of Qt's Model/View framework it constitutes the wrapper
//!        object connecting the model data (the logging groups list defined in
//!        ExperimentInterface) with the interface defined in QAbstractListModel.
//!        This interface can be then utilized by an instance Qt's QListView to
//!        display the list.
//!
//!        See https://doc.qt.io/qt-5/model-view-programming.html for Qt's
//!        model/view framework.
//-----------------------------------------------------------------------------

#ifndef LOGGINGGROUPSPRESENTER_H
#define LOGGINGGROUPSPRESENTER_H

#include "interfaces/openPASS-TrafficSimulation/ExperimentInterface.h"

#include <QAbstractListModel>

//-----------------------------------------------------------------------------
//! @brief This class describes the presenter object of the list of logging
//!        groups.
//!
//!        In the spirit of Qt's Model/View framework it constitutes the wrapper
//!        object connecting the model data (the logging groups list defined in
//!        ExperimentInterface) with the interface defined in QAbstractListModel.
//!        This interface can be then utilized by an instance Qt's QListView to
//!        display the list.
//!
//!        See https://doc.qt.io/qt-5/model-view-programming.html for Qt's
//!        model/view framework.
//-----------------------------------------------------------------------------
class LoggingGroupsPresenter : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit LoggingGroupsPresenter(ExperimentInterface::LoggingGroups * const loggingGroups,
                                    QObject *parent = 0);

public:
    //-----------------------------------------------------------------------------
    //! Reimplementation of the rowCount method. Returns the number of
    //! items in the logging groups list.
    //!
    //! @param[in]      parent      This variable is unused (but required according to
    //!                             the signature of the method)
    //!
    //! @return                     The number of logging groups
    //-----------------------------------------------------------------------------
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

public:
    //-----------------------------------------------------------------------------
    //! Reimplementation of the data method. Returns the data stored in the model
    //! referenced by the given index (QModelIndex).
    //!
    //! @param[in]      index       A model index containing the row index
    //! @param[in]      role        A Qt role (needs to be in Qt::DisplayRole or Qt::EditRole)
    //!
    //! @return                     The data stored in the list (as a QVariant)
    //-----------------------------------------------------------------------------
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    //-----------------------------------------------------------------------------
    //! Reimplementation of the setData method. Sets the data stored in the row
    //! referenced by the given index (QModelIndex) to the given value.
    //!
    //! @param[in]      index       A model index containing the row index
    //! @param[in]      role        A Qt role (needs to be in Qt::EditRole)
    //!
    //! @return                     "True" if the index is valid,
    //!                             otherwise "false"
    //-----------------------------------------------------------------------------
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    //-----------------------------------------------------------------------------
    //! Reimplementation of the flags method. This will make the items of the model
    //! editable.
    //!
    //! @param[in]      index       A model index containing the row index
    //!
    //! @return                     An item flag
    //-----------------------------------------------------------------------------
    Qt::ItemFlags flags(const QModelIndex& index) const override;

public:
    //-----------------------------------------------------------------------------
    //! Method that adds a new logging group to the list at the given row index.
    //!
    //! @param[in]      row     the row index at which the logging group is placed in the list
    //!
    //-----------------------------------------------------------------------------
    virtual void insertRow(int row, QString item, const QModelIndex &parent = QModelIndex()) ;

    //-----------------------------------------------------------------------------
    //! Method that removes the logging group at the given index from the list.
    //!
    //! @param[in]      row     the row index of the logging group to be removed
    //-----------------------------------------------------------------------------
    virtual void removeRow(int row, const QModelIndex &parent = QModelIndex()) ;

    //-----------------------------------------------------------------------------
    //! Method which updates the list according to the current entries of
    //! #loggingGroups.
    //-----------------------------------------------------------------------------
    void update();

private:
    ExperimentInterface::LoggingGroups * const loggingGroups;   //!< the string list of logging groups
};

#endif // LOGGINGGROUPSPRESENTER_H
