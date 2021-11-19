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
//! @file  ScenarioDisplayPresenter.h
//! @ingroup trafficSimulationPlugin
//! @brief This class defines the presenter which controls the content to be
//!        displayed in the ScenarioDisplayView widget.
//-----------------------------------------------------------------------------
#ifndef SCENARIODISPLAYPRESENTER_H
#define SCENARIODISPLAYPRESENTER_H

#include <QObject>

#include "Views/ScenarioDisplayView.h"
#include "openPASS-Project/ProjectInterface.h"
#include "openPASS-TrafficSimulation/ScenarioInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class defines the presenter which controls the content to be
//!        displayed in the ScenarioDisplayView widget.
//-----------------------------------------------------------------------------
class ScenarioDisplayPresenter : public QObject
{
    Q_OBJECT

public:
    explicit ScenarioDisplayPresenter(ScenarioInterface *scenario,
                                      ScenarioDisplayView *scenarioDisplayView,
                                      ProjectInterface *const project,
                                      QObject *parent = nullptr);

    ~ScenarioDisplayPresenter() = default;

public Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is called when the name of the scenario has changed in the model.
    //! Updates the displayed name and triggers a status check.
    //-----------------------------------------------------------------------------
    void setScenarioNameView();

    //-----------------------------------------------------------------------------
    //! Slot which is called when the open scenario filename changed in the model.
    //! Updates the displayed scenario filename and triggers a status check.
    //-----------------------------------------------------------------------------
    void setScenarioFileView();

    //-----------------------------------------------------------------------------
    //! Slot which is called when the map of scenario agents has been edited in the model.
    //! Updates the displayed number of scenario agents and triggers a status check.
    //-----------------------------------------------------------------------------
    void setScenarioAgentsView();

public:
    //-----------------------------------------------------------------------------
    //! Method which determines the status of the scenario settings. If all necessary
    //! configurations are made, the method displays the status "complete" and sets
    //! the frame of the display widget to green color. If "incomplete", the frame
    //! will be set to red color. Momentarily, the criterion for a complete configuration
    //! is that all forms in the ScenarioView widget are filled.
    //-----------------------------------------------------------------------------
    void setStatus();

    //-----------------------------------------------------------------------------
    //! Method which sets all displayed values according to the model data. Will be
    //! called after loading a combination xml file.
    //-----------------------------------------------------------------------------
    void updateView();

private:
    void setScenarioAgentsList();
    bool scenarioAgentsDefined();

private:
    ScenarioInterface *scenario;              //!< a pointer to the scenario model (interface) which is read by this presenter
    ScenarioDisplayView *scenarioDisplayView; //!< a pointer to the display view whose content is managed by this presenter
    ProjectInterface *const project;          //!< a pointer to the project interface in order to access the file system

private:
    QMap<QString, QList<QString>> scenarioAgentsList; //!< a map which holds a list of scenario agents (values) for each profiles catalogue (keys)
};

#endif // SCENARIODISPLAYPRESENTER_H
