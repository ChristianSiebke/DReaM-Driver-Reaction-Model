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
//! @file  TrafficSimulationPresenter.h
//! @ingroup trafficSimulationPlugin
//! @brief This class defines the presenter object for the main view of the Traffic Simulation Plugin and
//!        instantiates all other presenters.
//!
//!        The major user events to be handled are those from the action menu bar, i.e. loading, saving and
//!        clearing the whole configuration.
//-----------------------------------------------------------------------------

#ifndef TRAFFICSIMULATIONPRESENTER_H
#define TRAFFICSIMULATIONPRESENTER_H

#include <QObject>
#include <QShortcut>
#include <QUndoStack>

#include "Views/TrafficSimulationView.h"
#include "openPASS-Project/ProjectInterface.h"
#include "openPASS-TrafficSimulation/TrafficSimulationInterface.h"

class ExperimentPresenter;
class EnvironmentPresenter;
class EnvironmentMapPresenter;
class EnvironmentDisplayPresenter;
class ScenarioPresenter;
class ScenarioDisplayPresenter;
class TrafficPresenter;
class TrafficDisplayPresenter;

//-----------------------------------------------------------------------------
//! @brief This class defines the presenter object for the main view of the Traffic Simulation Plugin and
//!        instantiates all other presenters.
//!
//!        The major user events to be handled are those from the action menu bar, i.e. loading, saving and
//!        clearing the whole configuration.
//-----------------------------------------------------------------------------
class TrafficSimulationPresenter : public QObject
{
    Q_OBJECT
public:
    explicit TrafficSimulationPresenter(TrafficSimulationInterface *trafficSimulation,
                                        TrafficSimulationView *trafficSimulationView,
                                        ProjectInterface *const project,
                                        QObject *parent = nullptr);

    virtual ~TrafficSimulationPresenter() = default;

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is triggered after the user has clicked the save-button in the action menu.
    //! Will open file dialog to select a filepath for the simulationConfig xml file to be saved.
    //! It calls the TrafficSimulationXMLSaveModel::save routine for saving the momentary
    //! configuration to a simulationConfig XML file.
    //!
    //-----------------------------------------------------------------------------
    void save();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered after the user has clicked the load-button in the action menu.
    //! Will open file dialog to select a filepath for the simulationConfig xml file to be loaded.
    //! It calls the TrafficSimulationXMLLoadModel::load routine for loading a simulationConfig XML
    //! file.
    //-----------------------------------------------------------------------------
    void load();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered after the user has clicked the new-button in the action menu
    //! and confirmed that the worksheet should be cleared. It calls the TrafficSimulationInterface::clear
    //! routine in order to clear the whole configuration and views.
    //-----------------------------------------------------------------------------
    void clear();

    //-----------------------------------------------------------------------------
    //! Slot which displays information in the status bar depending on the current
    //! tab.
    //!
    //! @param[in]      index       the current tab index
    //-----------------------------------------------------------------------------
    void updateStatusBar(int index);

    //-----------------------------------------------------------------------------
    //! Slot which opens the file dialog for the user to select a profiles catalouge
    //!  XML file.
    //! Further, it scans the chosen profile catalogue for agent profiles and updates
    //! #agentProfiles accordingly. If the profile catalogue is changed, all
    //! agent profiles included into the traffic settings (TrafficInterface) are
    //! deleted. Before, a dialog window with a warning is displayed requiring a
    //! user confirmation.
    //!
    //! @param[in]      filename       the filename of the profile catalogue to be
    //!                                adopted
    //-----------------------------------------------------------------------------
    void setProfileCatalogue();

    //-----------------------------------------------------------------------------
    //! Slot which displays the profile catalogues view. Further, it loads the
    //! agent list, defined in the profiles catalogue. If any errors occur, these
    //! will be displayed in the GUI as well.
    //-----------------------------------------------------------------------------
    void updateCatalogueView();

    //-----------------------------------------------------------------------------
    //! Slot which checks the status of the configuration for each tab, i.e. the
    //! experiment, environment, scenario and traffic tabs. If all configurations
    //! are complete and correct, the start button for the simulation will be enabled.
    //-----------------------------------------------------------------------------
    void checkStatus();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the simulation button is pressed. It creates
    //! a process for executing the simulation defined in the corresponding
    //! simulation path (see ProjectInterface). Before, it saves the current settings
    //! to the file 'simulationConfig.xml'. If such a file already exists, the user will
    //! be asked whether to override it or not.
    //-----------------------------------------------------------------------------
    void startSimulation();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the simulation has terminated. It will
    //! pop up an info window with the exit code the simulation has terminated with as
    //! well as the location of the log file.
    //-----------------------------------------------------------------------------
    void simulationFinished(int exitCode);

private:
    //-----------------------------------------------------------------------------
    //! Method, which copies all experiment settings from the given argument to
    //! #trafficSimulation.
    //!
    //! @param[in]      from        the traffic simulation instance to copy from
    //-----------------------------------------------------------------------------
    void copyExperimentConfig(TrafficSimulationInterface const *const from);

    //-----------------------------------------------------------------------------
    //! Method, which copies all environment settings from the given argument to
    //! #trafficSimulation.
    //!
    //! @param[in]      from        the traffic simulation instance to copy from
    //-----------------------------------------------------------------------------
    void copyEnvironmentConfig(TrafficSimulationInterface const *const from);

    //-----------------------------------------------------------------------------
    //! Method, which copies all scenario settings from the given argument to
    //! #trafficSimulation.
    //!
    //! @param[in]      from        the traffic simulation instance to copy from
    //-----------------------------------------------------------------------------
    void copyScenarioConfig(TrafficSimulationInterface const *const from);

    //-----------------------------------------------------------------------------
    //! Method, which copies all traffic settings from the given argument to
    //! #trafficSimulation.
    //!
    //! @param[in]      from        the traffic simulation instance to copy from
    //-----------------------------------------------------------------------------
    void copyTrafficConfig(TrafficSimulationInterface const *const from);

private:
    //-----------------------------------------------------------------------------
    //! Method, which resets all setting made in the experiment config to the
    //! default and empties all tables.
    //-----------------------------------------------------------------------------
    void clearExperimentConfig();

    //-----------------------------------------------------------------------------
    //! Method, which resets all setting made in the environment config to the
    //! default and empties all tables.
    //-----------------------------------------------------------------------------
    void clearEnvironmentConfig();

    //-----------------------------------------------------------------------------
    //! Method, which resets all setting made in the scenario config to the
    //! default and empties all tables.
    //-----------------------------------------------------------------------------
    void clearScenarioConfig();

    //-----------------------------------------------------------------------------
    //! Method, which resets all setting made in the traffic config to the
    //! default and empties all tables.
    //-----------------------------------------------------------------------------
    void clearTrafficConfig();

private:
    QList<QString> agentProfiles; //!< a list of agent profiles defined in the profile catalogue

private:
    TrafficSimulationInterface *const trafficSimulation; //!< a pointer to the model class edited by this presenter
    TrafficSimulationView *const trafficSimulationView;  //!< a pointer to the editor view whose user events are handled by this presenter
    ProjectInterface *const project;                     //!< a pointer to the project settings (provides access to the path settings
    QUndoStack *const undoStack;                         //!< a pointer to the undo stack

private:
    ExperimentPresenter *const experimentPresenter; //!< a pointer to the experiment presenter instantiated here

    EnvironmentPresenter *const environmentPresenter;               //!< a pointer to the environment presenter instantiated here
    EnvironmentDisplayPresenter *const environmentDisplayPresenter; //!< a pointer to the environment display presenter instantiated here

    ScenarioPresenter *const scenarioPresenter;               //!< a pointer to the scenario presenter instantiated here
    ScenarioDisplayPresenter *const scenarioDisplayPresenter; //!< a pointer to the scenario display presenter instantiated here

    TrafficPresenter *const trafficPresenter;               //!< a pointer to the traffic presenter instantiated here
    TrafficDisplayPresenter *const trafficDisplayPresenter; //!< a pointer to the traffic display presenter instantiated here

private:
    QShortcut *const redo; //!< shortcut for redo
    QShortcut *const undo; //!< shortcut for undo
};

//-----------------------------------------------------------------------------
//! @brief Command class for editing the profiles catalogue.
//-----------------------------------------------------------------------------
class EditProfilesCatalogue : public QUndoCommand
{
public:
    EditProfilesCatalogue(TrafficSimulationInterface *const trafficSimulation,
                          TrafficSimulationView *const trafficSimulationView,
                          TrafficSimulationInterface::ProfilesCatalogue const &newCatalogue,
                          QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    TrafficSimulationInterface *const trafficSimulation;
    TrafficSimulationView *const trafficSimulationView;
    TrafficSimulationInterface::ProfilesCatalogue const newCatalogue;
    TrafficSimulationInterface::ProfilesCatalogue const oldCatalogue;
    //    QList<QString> agentList;
};

#endif // TRAFFICSIMULATIONPRESENTER_H
