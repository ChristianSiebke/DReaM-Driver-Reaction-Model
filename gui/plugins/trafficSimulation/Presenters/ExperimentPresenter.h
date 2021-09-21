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
//! @file  ExperimentPresenter.h
//! @ingroup trafficSimulationPlugin
//! @brief This class defines the presenter object which handles user events coming
//!        from the view (ExperimentView) and edits the model accordingly (ExperimentInterface).
//-----------------------------------------------------------------------------
#ifndef EXPERIMENTPRESENTER_H
#define EXPERIMENTPRESENTER_H

#include <QObject>
#include <QUndoStack>

#include "Presenters/LibrariesPresenter.h"
#include "Presenters/LoggingGroupsPresenter.h"
#include "Views/ExperimentView.h"
#include "openPASS-Project/ProjectInterface.h"
#include "openPASS-TrafficSimulation/ExperimentInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class defines the presenter object which handles user events coming
//!        from the view (ExperimentView) and edits the model accordingly (ExperimentInterface).
//-----------------------------------------------------------------------------
class ExperimentPresenter : public QObject
{
    Q_OBJECT

public:
    explicit ExperimentPresenter(ExperimentInterface *const experiment,
                                 ExperimentView *const experimentView,
                                 ProjectInterface *const project,
                                 QUndoStack *const undoStack,
                                 QObject *parent = nullptr);

    virtual ~ExperimentPresenter() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted when the start simulation button has been clicked. Will
    //! trigger a process (started in TrafficSimulationPresenter) executing the
    //! simulation with the current simulation settings of this plugin.
    //-----------------------------------------------------------------------------
    void startSimulation();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is connected to the ExperimentView::Invocations_valueChanged signal.
    //! It will set the model's value for the number of invocations to the one transmitted
    //! by the view.
    //!
    //! @param[in]      _numberInvocations      the number of invocations transmitted by the view
    //-----------------------------------------------------------------------------
    void setNumberInvocations(ExperimentInterface::NumberInvocations _numberInvocations);

    //-----------------------------------------------------------------------------
    //! Slot which is connected to the ExperimentView::Seed_valueChanged signal.
    //! It will set the model's value for the random seed to the one transmitted by the view.
    //!
    //! @param[in]      _seed      the random seed transmitted by the view
    //-----------------------------------------------------------------------------
    void setSeed(ExperimentInterface::Seed _seed);

    //-----------------------------------------------------------------------------
    //! Slot which adds a logging group to the logging groups list.
    //-----------------------------------------------------------------------------
    void addLoggingGroup();

    //-----------------------------------------------------------------------------
    //! Slot which removes the logging groups placed in the given row of the logging
    //! groups list.
    //!
    //! @param[in]      row     the row of the logging groups list to be removed
    //-----------------------------------------------------------------------------
    void removeLoggingGroup(int const &row);

    //-----------------------------------------------------------------------------
    //! Slot which adds a library to the libraries table.
    //-----------------------------------------------------------------------------
    void addLibrary();

    //-----------------------------------------------------------------------------
    //! Slot which removes the library placed in the given row of the  libraries
    //! table.
    //!
    //! @param[in]      row     the row of the libraries table to be removed
    //-----------------------------------------------------------------------------
    void removeLibrary(int const &row);

private:
    //-----------------------------------------------------------------------------
    //! An initializer method which configures the table view of the libraries.
    //! Creates the table's presenter object (LibrariesPresenter),
    //! sets up the delegates for editing the table entries and establishes signal-slot
    //! connections for adding and removing table rows.
    //-----------------------------------------------------------------------------
    void setupLibraries();

    //-----------------------------------------------------------------------------
    //! An initializer method which configures the list view of the logging groups.
    //! Creates the list's presenter object (LoggingGroupsPresenter),
    //! sets up the combobox delegate for editing the list entries and establishes signal-slot
    //! connections for adding and removing list entries.
    //-----------------------------------------------------------------------------
    void setupLoggingGroups();

public:
    //-----------------------------------------------------------------------------
    //! This method sets all display values of the view to the model values. This method
    //! is called after loading a combination XML file in order to show the loaded
    //! configuration.
    //-----------------------------------------------------------------------------
    void updateView();

private:
    ExperimentInterface *const experiment; //!< a pointer to the experiment model whose values are edited by this presenter
    ExperimentView *const experimentView;  //!< a ponter to the experiment's view whose events are handled by this presenter
    ProjectInterface *const project;       //!< a ponter to the project interface in order to access the filepath
    QUndoStack *const undoStack;           //!< a pointer to the undo stack

private:
    LibrariesPresenter *const librariesPresenter; //!< a pointer to the presenter object of the libraries map
    static QStringList libraryTypesList;          //!< a list of possible library types

private:
    LoggingGroupsPresenter *const loggingGroupsPresenter; //!< a pointer to the presenter object of the logging groups list
    static QStringList loggingGroupsList;                 //!< a selection list of possible logging groups
};

//-----------------------------------------------------------------------------
//! @brief Command class for adding libraries to the libraries table.
//-----------------------------------------------------------------------------
class AddLibrary : public QUndoCommand
{
public:
    AddLibrary(LibrariesPresenter *const table,
               QString const &libType,
               QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    LibrariesPresenter *const table;
    QString const libType;
};

//-----------------------------------------------------------------------------
//! @brief Command class for removing libraries from the libraries table.
//-----------------------------------------------------------------------------
class RemoveLibrary : public QUndoCommand
{
public:
    RemoveLibrary(LibrariesPresenter *const table,
                  int const &row,
                  QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    LibrariesPresenter *const table;
    int const row;
    QString const libType;
    QString const lib;
};

//-----------------------------------------------------------------------------
//! @brief Command class for adding logging groups to the logging groups list.
//-----------------------------------------------------------------------------
class AddLoggingGroup : public QUndoCommand
{
public:
    AddLoggingGroup(LoggingGroupsPresenter *const list,
                    QString const &name,
                    QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    LoggingGroupsPresenter *const list;
    QString const name;
};

//-----------------------------------------------------------------------------
//! @brief Command class for removing logging groups from the logging groups list.
//-----------------------------------------------------------------------------
class RemoveLoggingGroup : public QUndoCommand
{
public:
    RemoveLoggingGroup(LoggingGroupsPresenter *const list,
                       int const &row,
                       QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    LoggingGroupsPresenter *const list;
    int const row;
    QString const name;
};

//-----------------------------------------------------------------------------
//! @brief Command class editing the number of invocations.
//-----------------------------------------------------------------------------
class EditNumberInvocations : public QUndoCommand
{
public:
    EditNumberInvocations(ExperimentInterface *const experiment,
                          ExperimentView *const experimentView,
                          ExperimentInterface::NumberInvocations const &newValue,
                          QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    ExperimentInterface *const experiment;
    ExperimentView *const experimentView;
    ExperimentInterface::NumberInvocations const newValue;
    ExperimentInterface::NumberInvocations const oldValue;
};

//-----------------------------------------------------------------------------
//! @brief Command class editing the number of invocations.
//-----------------------------------------------------------------------------
class EditRandomSeed : public QUndoCommand
{
public:
    EditRandomSeed(ExperimentInterface *const experiment,
                   ExperimentView *const experimentView,
                   ExperimentInterface::Seed const &newValue,
                   QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    ExperimentInterface *const experiment;
    ExperimentView *const experimentView;
    ExperimentInterface::Seed const newValue;
    ExperimentInterface::Seed const oldValue;
};

#endif // EXPERIMENTPRESENTER_H
