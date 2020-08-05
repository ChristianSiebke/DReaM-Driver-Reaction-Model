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
//! @file  EnvironmentPresenter.h
//! @ingroup trafficSimulationPlugin
//! @brief This class defines the presenter object which handles user events coming
//!        from the view (EnvironmentView) and edits the model accordingly
//!        (EnvironmentInterface).
//-----------------------------------------------------------------------------
#ifndef ENVIRONMENTPRESENTER_H
#define ENVIRONMENTPRESENTER_H

#include "openPASS-TrafficSimulation/EnvironmentInterface.h"
#include "Views/EnvironmentView.h"
#include "Presenters/EnvironmentMapPresenter.h"

#include <QObject>
#include <QUndoStack>

//-----------------------------------------------------------------------------
//! @brief This class defines the presenter object which handles user events coming
//!        from the view (EnvironmentView) and edits the model accordingly
//!        (EnvironmentInterface).
//-----------------------------------------------------------------------------
class EnvironmentPresenter : public QObject
{
    Q_OBJECT

public:
    explicit EnvironmentPresenter(EnvironmentInterface * const environment,
                                  EnvironmentView * const environmentView,
                                  QUndoStack * const undoStack,
                                  QObject *parent = nullptr);

    ~EnvironmentPresenter() = default;

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is connected to the EnvironmentView::modifiedName signal. Sets the
    //! model's value of the environment name to the one transmitted by the view.
    //!
    //! @param[in]      name        the environment name transmitted by the view
    //-----------------------------------------------------------------------------
    void setName(QString const & name);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is connected to the EnvironmentView::addTimeOfDay signal. Calls the
    //! EnvironmentMapPresenter::addRow method of the presenter object of the time
    //! of day map.
    //-----------------------------------------------------------------------------
    void addTimeOfDay();

    //-----------------------------------------------------------------------------
    //! Slot which is connected to the EnvironmentView::removeTimeOfDay signal. Calls the
    //! EnvironmentMapPresenter::removeRow method of the presenter object of the time
    //! of day map.
    //!
    //! @param[in]      row     the row of the table whose environment parameter should
    //!                         be deleted
    //-----------------------------------------------------------------------------
    void removeTimeOfDay(const int &row);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is connected to the EnvironmentView::addVisibilityDistance signal.
    //! Calls the EnvironmentMapPresenter::addRow method of the presenter object of
    //! the map of visibility distances.
    //-----------------------------------------------------------------------------
    void addVisibilityDistance();

    //-----------------------------------------------------------------------------
    //! Slot which is connected to the EnvironmentView::removeVisibilityDistance signal.
    //! Calls the EnvironmentMapPresenter::removeRow method of the presenter object of
    //! the map of vistibility distances.
    //!
    //! @param[in]      row     the row of the table whose environment parameter should
    //!                         be deleted
    //-----------------------------------------------------------------------------
    void removeVisibilityDistance(const int &row);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is connected to the EnvironmentView::addFriction signal.
    //! Calls the EnvironmentMapPresenter::addRow method of the presenter object of
    //! the map of friction coefficients.
    //-----------------------------------------------------------------------------
    void addFriction();

    //-----------------------------------------------------------------------------
    //! Slot which is connected to the EnvironmentView::removeFriction signal.
    //! Calls the EnvironmentMapPresenter::removeRow method of the presenter object of
    //! the map of friction.
    //!
    //! @param[in]      row     the row of the table whose environment parameter should
    //!                         be deleted
    //-----------------------------------------------------------------------------
    void removeFriction(const int &row);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is connected to the EnvironmentView::addWeather signal.
    //! Calls the EnvironmentMapPresenter::addRow method of the presenter object of
    //! the map of weather parameters.
    //-----------------------------------------------------------------------------
    void addWeather();

    //-----------------------------------------------------------------------------
    //! Slot which is connected to the EnvironmentView::removeWeather signal.
    //! Calls the EnvironmentMapPresenter::removeRow method of the presenter object of
    //! the map of weather parameters.
    //!
    //! @param[in]      row     the row of the table whose environment parameter should
    //!                         be deleted
    //-----------------------------------------------------------------------------
    void removeWeather(const int &row);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which checks the probability sums of all environment parameters. If for
    //! a particular parameter map the probabilitiy sum deviates from 1, a hint will
    //! be displayed in the corresponding view.
    //-----------------------------------------------------------------------------
    void checkProbabilities();

private:
    //-----------------------------------------------------------------------------
    //! An initializer method which configures the table view of the time
    //! of day parameters. Creates the table's presenter object (EnvironmentMapPresenter),
    //! sets up the delegates for editing the table entries and establishes signal-slot
    //! connections for adding and removing table rows.
    //-----------------------------------------------------------------------------
    void SetupTimeOfDay();

    //-----------------------------------------------------------------------------
    //! An initializer method which configures the table view of visibility distances
    //! Creates the table's presenter object (EnvironmentMapPresenter),
    //! sets up the delegates for editing the table entries and establishes signal-slot
    //! connections for adding and removing table rows.
    //-----------------------------------------------------------------------------
    void SetupVisibilityDistance();

    //-----------------------------------------------------------------------------
    //! An initializer method which configures the table view of friction coefficients.
    //! Creates the table's presenter object (EnvironmentMapPresenter),
    //! sets up the delegates for editing the table entries and establishes signal-slot
    //! connections for adding and removing table rows.
    //-----------------------------------------------------------------------------
    void SetupFriction();

    //-----------------------------------------------------------------------------
    //! An initializer method which configures the table view of  weather parameters.
    //! Creates the table's presenter object (EnvironmentMapPresenter),
    //! sets up the delegates for editing the table entries and establishes signal-slot
    //! connections for adding and removing table rows.
    //-----------------------------------------------------------------------------
    void SetupWeather();

public:
    //-----------------------------------------------------------------------------
    //! Method which updates the view (EnvironmentView) to the current state of the
    //! model (EnvironmentModel / EnvironmentInterface).
    //-----------------------------------------------------------------------------
    void updateView();

private:
    EnvironmentInterface * const environment;                         //!< a pointer to the model of the environment configuration
    EnvironmentView * const environmentView;                          //!< a pointer to the view of the environment configuration
    QUndoStack * const undoStack;                                     //!< a pointer to the undoStack

private:
    EnvironmentMapPresenter * const timeOfDayPresenter;               //!< a pointer to the presenter object connecting the time of day map to its table view
    EnvironmentMapPresenter * const visibilityDistancePresenter;      //!< a pointer to the presenter object connecting the visibility distance map to its table view
    EnvironmentMapPresenter * const frictionPresenter;                //!< a pointer to the presenter object connecting the frition map to its table view
    EnvironmentMapPresenter * const weatherPresenter;                 //!< a pointer to the presenter object connecting the weather map to its table view

private:
   static double const default_probability;                           //!< a default selection probability
   static QString const default_timeOfDay;                            //!< a default time of day parameter
   static QString const default_visibilityDistance;                   //!< a default visbility distance
   static QString const default_friction;                             //!< a default friction coefficient
   static QString const default_weather;                              //!< a default weather parameter
   static QStringList const weatherList;                              //!< a list of availbale weather parameters (the user can choose from this list)
};


//-----------------------------------------------------------------------------
//! @brief Command class for adding rows to the table of environment settings.
//-----------------------------------------------------------------------------
class AddEnvironmentItem : public QUndoCommand
{

public:
    AddEnvironmentItem(EnvironmentMapPresenter * const table,
                       QString const default_value,
                       const double default_probability,
                       QString tableName = "",
                       QUndoCommand * parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    EnvironmentMapPresenter * const table;
    QString const default_value;
    double const default_probability;

};

//-----------------------------------------------------------------------------
//! @brief Command class for removing rows from the table of environment settings.
//-----------------------------------------------------------------------------
class RemoveEnvironmentItem : public QUndoCommand
{

public:
    RemoveEnvironmentItem(EnvironmentMapPresenter * const table,
                          int const row,
                          QString tableName="",
                          QUndoCommand * parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    EnvironmentMapPresenter * const table;
    int const row;
    EnvironmentItemInterface::Value const value;
    EnvironmentItemInterface::Probability const probability;
};

//-----------------------------------------------------------------------------
//! @brief Command class editing the environment's name.
//-----------------------------------------------------------------------------
class EditEnvironmentName : public QUndoCommand
{

public:
    EditEnvironmentName(EnvironmentInterface * const environment,
                        EnvironmentView * const environmentView,
                        EnvironmentInterface::Name const &newName,
                        QUndoCommand * parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    EnvironmentInterface * const environment;
    EnvironmentView * const environmentView;
    EnvironmentInterface::Name const newName;
    EnvironmentInterface::Name const oldName;
};


#endif // ENVIRONMENTPRESENTER_H
