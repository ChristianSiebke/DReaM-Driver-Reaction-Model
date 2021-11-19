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
//! @file  ScenarioPresenter.h
//! @ingroup trafficSimulationPlugin
//! @brief This class defines the presenter object which handles user events coming
//!        from the view (ScenarioView) and edits the model accordingly (ScenarioInterface).
//-----------------------------------------------------------------------------
#ifndef SCENARIOPRESENTER_H
#define SCENARIOPRESENTER_H

#include <QList>
#include <QMap>
#include <QObject>
#include <QUndoStack>

#include "Presenters/ScenarioAgentsPresenter.h"
#include "Views/ScenarioView.h"
#include "openPASS-Project/ProjectInterface.h"
#include "openPASS-TrafficSimulation/ScenarioInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class defines the presenter object which handles user events coming
//!        from the view (ScenarioView) and edits the model accordingly (ScenarioInterface).
//-----------------------------------------------------------------------------
class ScenarioPresenter : public QObject
{
    Q_OBJECT
public:
    explicit ScenarioPresenter(ScenarioInterface *scenario,
                               ScenarioView *scenarioView,
                               ProjectInterface *const project,
                               QUndoStack *const undoStack,
                               QObject *parent = nullptr);
    virtual ~ScenarioPresenter() = default;

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is connected to the ScenarioView::modifiedName signal. It will
    //! set the model's value for the scenario name to the one transmitted by the view.
    //!
    //! @param[in]      name        the scenario name transmitted by the view
    //-----------------------------------------------------------------------------
    void setName(QString const &name);

    //-----------------------------------------------------------------------------
    //! Slot which is connected to the ScenarioView::modifiedScenarioFile signal. It will
    //! set the model's value for the open scenario filename to the one transmitted by the view.
    //!
    //! @param[in]      filename        the open scenario filename transmitted by the view
    //-----------------------------------------------------------------------------
    void setScenarioFile();

    //-----------------------------------------------------------------------------
    //! Slot which refreshes the current scenario file. This means that it checks
    //! whether the file exists and updates the lists of defined agent profiles.
    //-----------------------------------------------------------------------------
    void refreshScenarioFile();

public:
    //-----------------------------------------------------------------------------
    //! This method sets all display values of the view to the model values. This method
    //! is called after loading a combination XML file in order to show the loaded
    //! configuration.
    //-----------------------------------------------------------------------------
    void updateView();

private:
    //-----------------------------------------------------------------------------
    //! This method fills the #scenarioAgentsList map with all agent profiles
    //! referenced in the currently selected scenario file.
    //-----------------------------------------------------------------------------
    void setScenarioAgentsList();

private:
    ScenarioAgentsPresenter *scenarioAgentsPresenter; //!< a pointer to the presenter object of the scenario agent map
    ScenarioInterface *scenario;                      //!< a pointer to the scenario model whose values are edited by this presenter
    ScenarioView *scenarioView;                       //!< a pointer to the scenario view whose events are handled by this presenter
    ProjectInterface *const project;                  //!< a pointer to the project interface, needed for accessing the configs path
    QUndoStack *const undoStack;                      //!< a pointer to the undo stack

private:
    QMap<QString, QList<QString>> scenarioAgentsList; //!< a map which holds a list of scenario agents (values) for each profiles catalogue (keys)
};

//-----------------------------------------------------------------------------
//! @brief Command class editing the scenario's name.
//-----------------------------------------------------------------------------
class EditScenarioName : public QUndoCommand
{
public:
    EditScenarioName(ScenarioInterface *const scenario,
                     ScenarioView *const scenarioView,
                     ScenarioInterface::Name const &newName,
                     QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    ScenarioInterface *const scenario;
    ScenarioView *const scenarioView;
    ScenarioInterface::Name const newName;
    ScenarioInterface::Name const oldName;
};

//-----------------------------------------------------------------------------
//! @brief Command class editing the OpenSCENARIO file
//-----------------------------------------------------------------------------
class EditScenarioFile : public QUndoCommand
{
public:
    EditScenarioFile(ScenarioInterface *const scenario,
                     ScenarioView *const scenarioView,
                     ScenarioInterface::ScenarioFile const &newFile,
                     QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    ScenarioInterface *const scenario;
    ScenarioView *const scenarioView;
    ScenarioInterface::ScenarioFile const newFile;
    ScenarioInterface::ScenarioFile const oldFile;
};

#endif // SCENARIOPRESENTER_H
