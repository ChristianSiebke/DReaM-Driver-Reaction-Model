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
//! @file  ScenarioAgentsPresenter.h
//! @ingroup trafficSimulationPlugin
//! @brief This class describes the presenter object of the scenario agents map.
//!
//!        In the spirit of Qt's Model/View framework it constitutes the wrapper
//!        object connecting the model data (scenario agents map defined in
//!        ScenarioInterface) with the interface defined in QAbstractTableModel.
//!        This interface can be then utilized by instances Qt's QTableView to
//!        display the map entries in a table format.
//!        Since the table should only display the scenario agents, there is no
//!        setData method implemented here.
//!
//!        See https://doc.qt.io/qt-5/model-view-programming.html for Qt's
//!        model/view framework.
//-----------------------------------------------------------------------------

#ifndef SCENARIOAGENTSPRESENTER_H
#define SCENARIOAGENTSPRESENTER_H

#include <QAbstractTableModel>

#include "openPASS-TrafficSimulation/ScenarioInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class describes the presenter object of the scenario agents map.
//!
//!        In the spirit of Qt's Model/View framework it constitutes the wrapper
//!        object connecting the model data (scenario agents map defined in
//!        ScenarioInterface) with the interface defined in QAbstractTableModel.
//!        This interface can be then utilized by instances Qt's QTableView to
//!        display the map entries in a table format.
//!        Since the table should only display the scenario agents, there is no
//!        setData method implemented here.
//!
//!        See https://doc.qt.io/qt-5/model-view-programming.html for Qt's
//!        model/view framework.
//-----------------------------------------------------------------------------
class ScenarioAgentsPresenter : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ScenarioAgentsPresenter(ScenarioInterface::Entities *const entities,
                                     QMap<QString, QList<QString>> const &scenarioAgentsList,
                                     QObject *parent = nullptr);
    ~ScenarioAgentsPresenter() = default;

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

    //-----------------------------------------------------------------------------
    //! Reimplementation of the rowCount method. Returns the number of
    //! rows in the table which is, in fact, the number of scenario agents.
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

public:
    //-----------------------------------------------------------------------------
    //! Method which updates the table model according to the current entries of
    //! #entitites.
    //-----------------------------------------------------------------------------
    void update();

private:
    QMap<int, QString> RowToName;                            //! a map translating row indices to scenario agent names
    ScenarioInterface::Entities *const entities;             //!< the map of scenario agents (called 'entities' in the XOSC file standard)
    QStringList _columnHeaders;                              //!< the column header's info
    QMap<QString, QList<QString>> const &scenarioAgentsList; //!< a map which holds a list of scenario agents (values) for each profiles catalogue (keys)
};

#endif // SCENARIOAGENTSPRESENTER_H
