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
//! @file  TrafficPresenter.h
//! @ingroup trafficSimulationPlugin
//! @brief This class defines the presenter object which handles user events coming
//!        from the view (TrafficView) and edits the model accordingly
//!        (TrafficInterface).
//-----------------------------------------------------------------------------
#ifndef TRAFFICPRESENTER_H
#define TRAFFICPRESENTER_H

#include "interfaces/openPASS-TrafficSimulation/TrafficInterface.h"
#include "Presenters/TrafficItemMapPresenter.h"
#include "Views/TrafficView.h"

#include <QObject>
#include <QUndoStack>

//-----------------------------------------------------------------------------
//! @brief This class defines the presenter object which handles user events coming
//!        from the view (TrafficView) and edits the model accordingly
//!        (TrafficInterface).
//-----------------------------------------------------------------------------
class TrafficPresenter : public QObject
{
    Q_OBJECT
public:
    explicit TrafficPresenter(TrafficInterface * const traffic,
                              TrafficView * const trafficView,
                              QList<QString> * const agentList,
                              QUndoStack * const undoStack,
                              QObject *parent = nullptr);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is connected to the TrafficView::modifiedName signal. Sets the
    //! traffic configuration's name to the one transmitted by the view.
    //!
    //! @param[in]      name        the traffic name transmitted by the view
    //-----------------------------------------------------------------------------
    void setName(QString const & name);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot is connected to the TrafficView::addVolume signal. Calls the
    //! TrafficItemMapPresenter::addRow method of the presenter of the volume map.
    //! This will create a new traffic volume with default value and probability.
    //-----------------------------------------------------------------------------
    void addVolume();

    //-----------------------------------------------------------------------------
    //! Slot which is connected to the TrafficView::removeVolume signal. Calls the
    //! EnvironmentMapPresenter::removeRow method of the presenter the volume map.
    //! This will remove the traffic volume associated with the transmitted row
    //! index.
    //!
    //! @param[in]      row     the row of the table whose entry should
    //!                         be deleted
    //-----------------------------------------------------------------------------
    void removeVolume(const int &row);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot is connected to the TrafficView::addPlatoonRate signal. Calls the
    //! TrafficItemMapPresenter::addRow method of the presenter of the platoon
    //! rates map.  This will create a new platoon rate with default value and
    //! probability.
    //-----------------------------------------------------------------------------
    void addPlatoonRate();

    //-----------------------------------------------------------------------------
    //! Slot which is connected to the TrafficView::removePlatoonRate signal. Calls the
    //! EnvironmentMapPresenter::removeRow method of the corresponding presenter object.
    //! This will remove the platoon rate associated with the transmitted row
    //! index.
    //!
    //! @param[in]      row     the row of the table whose entry should
    //!                         be deleted
    //-----------------------------------------------------------------------------
    void removePlatoonRate(const int &row);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot is connected to the TrafficView::addVelocity signal. Calls the
    //! TrafficItemMapPresenter::addRow method of the presenter of the velocities
    //! map. This will create a new velocity parameter with default value and
    //! probability.
    //-----------------------------------------------------------------------------
    void addVelocity();

    //-----------------------------------------------------------------------------
    //! Slot which is connected to the TrafficView::removeVelocity signal. Calls the
    //! EnvironmentMapPresenter::removeRow method of the corresponding presenter object.
    //! This will remove the velocity parameter associated with the transmitted row
    //! index.
    //!
    //! @param[in]      row     the row of the table whose entry should
    //!                         be deleted
    //-----------------------------------------------------------------------------
    void removeVelocity(const int &row);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot is connected to the TrafficView::addHomogeneity signal. Calls the
    //! TrafficItemMapPresenter::addRow method of the presenter of the homogeneities
    //! map. This will create a new homogeneity parameter with default value and
    //! probability.
    //-----------------------------------------------------------------------------
    void addHomogeneity();

    //-----------------------------------------------------------------------------
    //! Slot which is connected to the TrafficView::removeHomogeneity signal. Calls the
    //! EnvironmentMapPresenter::removeRow method of the corresponding presenter object.
    //! This will remove the homogeneity parameter associated with the transmitted row
    //! index.
    //!
    //! @param[in]      row     the row of the table whose entry should
    //!                         be deleted
    //-----------------------------------------------------------------------------
    void removeHomogeneity(const int &row);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot is connected to the TrafficView::addRegularLaneAgent signal. Calls the
    //! TrafficItemMapPresenter::addRow method of the corresponding presenter.
    //! If #agentList is not empty, this will create a new regular lane agent with default
    //! probability.
    //-----------------------------------------------------------------------------
    void addRegularLaneAgent();

    //-----------------------------------------------------------------------------
    //! Slot which is connected to the TrafficView::removeRegularLaneAgent signal. Calls the
    //! EnvironmentMapPresenter::removeRow method of the corresponding presenter object.
    //! This will remove the regular lane agent associated with the transmitted row
    //! index.
    //!
    //! @param[in]      row     the row of the table whose entry should
    //!                         be deleted
    //-----------------------------------------------------------------------------
    void removeRegularLaneAgent(const int &row);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot is connected to the TrafficView::addRightmostLaneAgent signal. Calls the
    //! TrafficItemMapPresenter::addRow method of the corresponding presenter.
    //! If #agentList is not empty, this will create a new rightmost lane agent with
    //! default probability.
    //-----------------------------------------------------------------------------
    void addRightmostLaneAgent();

    //-----------------------------------------------------------------------------
    //! Slot which is connected to the TrafficView::removeRightmostLaneAgent signal. Calls the
    //! EnvironmentMapPresenter::removeRow method of the corresponding presenter object.
    //! This will remove the rightmost lane agent associated with the transmitted row
    //! index.
    //!
    //! @param[in]      row     the row of the table whose entry should
    //!                         be deleted
    //-----------------------------------------------------------------------------
    void removeRightmostLaneAgent(const int &row);

public Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is connected, with the TrafficInterface::edited signal in order
    //! to check the probability sum of all traffic items. Sets the error status
    //! of all the items to "true" if probabilities do not sum up to 1.
    //-----------------------------------------------------------------------------
    void checkProbabilities();

    //-----------------------------------------------------------------------------
    //! Slot which is connected, with the TrafficInterface::edited signal in order
    //! to check whether all selected agents exist in the profiles catalogue.
    //-----------------------------------------------------------------------------
    void checkAgents();

private:
    //-----------------------------------------------------------------------------
    //! An initializer method which configures the table view of the traffic volumes map.
    //! Creates the table's presenter object (TrafficItemMapPresenter),
    //! sets up the delegates for editing the table entries and establishes signal-slot
    //! connections for adding and removing table rows.
    //-----------------------------------------------------------------------------
    void setupVolumes();

    //-----------------------------------------------------------------------------
    //! An initializer method which configures the table view of the platoon rates map.
    //! Creates the table's presenter object (TrafficItemMapPresenter),
    //! sets up the delegates for editing the table entries and establishes signal-slot
    //! connections for adding and removing table rows.
    //-----------------------------------------------------------------------------
    void setupPlatoonRates();

    //-----------------------------------------------------------------------------
    //! An initializer method which configures the table view of the velocities map.
    //! Creates the table's presenter object (TrafficItemMapPresenter),
    //! sets up the delegates for editing the table entries and establishes signal-slot
    //! connections for adding and removing table rows.
    //-----------------------------------------------------------------------------
    void setupVelocities();

    //-----------------------------------------------------------------------------
    //! An initializer method which configures the table view of the homogeneities map.
    //! Creates the table's presenter object (TrafficItemMapPresenter),
    //! sets up the delegates for editing the table entries and establishes signal-slot
    //! connections for adding and removing table rows.
    //-----------------------------------------------------------------------------
    void setupHomogeneities();

    //-----------------------------------------------------------------------------
    //! An initializer method which configures the table view of the regular lane
    //! agents map. Creates the table's presenter object (TrafficItemMapPresenter),
    //! sets up the delegates for editing the table entries and establishes signal-slot
    //! connections for adding and removing table rows.
    //-----------------------------------------------------------------------------
    void setupRegularLaneAgents();

    //-----------------------------------------------------------------------------
    //! An initializer method which configures the table view of the rightmost lane
    //! agents map. Creates the table's presenter object (TrafficItemMapPresenter),
    //! sets up the delegates for editing the table entries and establishes signal-slot
    //! connections for adding and removing table rows.
    //-----------------------------------------------------------------------------
    void setupRightmostLaneAgents();

public:
    //-----------------------------------------------------------------------------
    //! Method which updates the view (TrafficView) to the current state of the
    //! model (TrafficModel / TrafficInterface).
    //-----------------------------------------------------------------------------
    void updateView();


private:
    TrafficInterface * const traffic;                       //!< a pointer to the model of the traffic configuration
    TrafficView * const trafficView;                        //!< a pointer to the view of the traffic configuration

private:
    TrafficItemMapPresenter * const volumes;                //!< a pointer to the presenter object connecting the traffic volumes map to its table view
    TrafficItemMapPresenter * const platoonRates;           //!< a pointer to the presenter object connecting the platoon rates map to its table view
    TrafficItemMapPresenter * const velocities;             //!< a pointer to the presenter object connecting the velocities map to its table view
    TrafficItemMapPresenter * const homogeneities;          //!< a pointer to the presenter object connecting the homogeneities map to its table view
    TrafficItemMapPresenter * const regularLaneAgents;      //!< a pointer to the presenter object connecting the regular lane agents map to its table view
    TrafficItemMapPresenter * const rightmostLaneAgents;    //!< a pointer to the presenter object connecting the rightmost lane agents map to its table view

private:
    static QString const default_volume;                    //!< a default value for the traffic volume
    static QString const default_platoonRate;               //!< a default value for the platoon rate
    static QString const default_velocity;                  //!< a default value for the velocity
    static QString const default_homogeneity;               //!< a default value for the homogeneity
    static double const default_probability;                //!< a default value for a selection probability

private:
    QList<QString> * const agentList;                       //!< a pointer to the list of available agent profiles (defined in a ProfilesCatalog.xml)

private:
    QUndoStack * const undoStack;                           //!< a pointer to the undo stack

};

//-----------------------------------------------------------------------------
//! @brief Command class for adding rows to the table of traffic settings.
//-----------------------------------------------------------------------------
class AddTrafficItem : public QUndoCommand
{

public:
    AddTrafficItem(TrafficItemMapPresenter * const table,
                   QString const default_value,
                   const double default_probability,
                   QString tableName = "",
                   QUndoCommand * parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    TrafficItemMapPresenter * const table;
    QString const default_value;
    double const default_probability;

};

//-----------------------------------------------------------------------------
//! @brief Command class for removing rows from the table of traffic settings.
//-----------------------------------------------------------------------------
class RemoveTrafficItem : public QUndoCommand
{

public:
    RemoveTrafficItem(TrafficItemMapPresenter * const table,
                      int const row,
                      QString tableName="",
                      QUndoCommand * parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    TrafficItemMapPresenter * const table;
    int const row;
    TrafficItemInterface::Value const value;
    TrafficItemInterface::Probability const probability;

};


//-----------------------------------------------------------------------------
//! @brief Command class for editing the traffic's name.
//-----------------------------------------------------------------------------
class EditTrafficName : public QUndoCommand
{

public:
    EditTrafficName(TrafficInterface * const traffic,
                    TrafficView * const trafficView,
                    TrafficInterface::Name const &newName,
                    QUndoCommand * parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    TrafficInterface * const traffic;
    TrafficView * const trafficView;
    TrafficInterface::Name const newName;
    TrafficInterface::Name const oldName;
};


#endif // TRAFFICPRESENTER_H
