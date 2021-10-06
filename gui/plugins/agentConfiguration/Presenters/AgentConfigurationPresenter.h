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
//! @file  AgentConfigurationPresenter.h
//! @ingroup agentConfigurationPlugin
//! @brief This class defines the presenter object which handles events from the
//!        view (AgentConfigurationView) and edits the model data
//!        (AgentConfigurationInterface) accordingly.
//-----------------------------------------------------------------------------

#ifndef AGENTCONFIGURATIONPRESENTER_H
#define AGENTCONFIGURATIONPRESENTER_H

#include <QObject>
#include <QShortcut>
#include <QUndoStack>

#include "Presenters/AgentMapPresenter.h"
#include "Presenters/DriverProfilesPresenter.h"
#include "Presenters/VehicleProfilesPresenter.h"
#include "Views/AgentConfigurationView.h"
#include "openPASS-AgentConfiguration/AgentConfigurationInterface.h"
#include "openPASS-Project/ProjectInterface.h"
#include "openPASS-System/SystemInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class defines the presenter object which handles events from the
//!        view (AgentConfigurationView) and edits the model data
//!        (AgentConfigurationInterface) accordingly.
//-----------------------------------------------------------------------------
class AgentConfigurationPresenter : public QObject
{
    Q_OBJECT

public:
    explicit AgentConfigurationPresenter(AgentConfigurationInterface *const agentConfiguration,
                                         AgentConfigurationView *const agentConfigurationView,
                                         SystemInterface *const system,
                                         ProjectInterface *const project,
                                         QObject *parent = nullptr);
    virtual ~AgentConfigurationPresenter() = default;

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot triggered by the AgentConfigurationView::SaveSetup signal. Opens the
    //! filedialog to let the user select a file and calls
    //! AgentConfigurationXMLSaveModel::save in order to save the Profiles Catalogue's
    //! *.ui file.
    //-----------------------------------------------------------------------------
    void save();

    //-----------------------------------------------------------------------------
    //! Slot triggered by the AgentConfigurationView::LoadSetup signal. Loads the
    //! *.ui file selected by the user. First, clears the whole model (clear-methods
    //! of this class) and, then, copies the data to the model (copy-methods of this
    //! class). The views are also updated accordingly.
    //-----------------------------------------------------------------------------
    void load();

    //-----------------------------------------------------------------------------
    //! Slot triggered by the AgentConfigurationView::ClearSetup signal. Clears the
    //! whole model data (using the clear-methods of this class) and also empties
    //! the views accordingly.
    //-----------------------------------------------------------------------------
    void clear();

private:
    //-----------------------------------------------------------------------------
    //! Method which performs an undoable copy of the agent profiles from the given
    //! instance of AgentConfigurationInterface. The copied profiles are appended
    //! and the views are updated accordingly.
    //!
    //! @param[in]      from        the pointer to the model data to copy from
    //-----------------------------------------------------------------------------
    void copyAgentProfiles(AgentConfigurationInterface const *const from);

    //-----------------------------------------------------------------------------
    //! Method which performs an undoable copy of the driver profiles from the given
    //! instance of AgentConfigurationInterface. The copied profiles are appended
    //! and the views are updated accordingly.
    //!
    //! @param[in]      from        the pointer to the model data to copy from
    //-----------------------------------------------------------------------------
    void copyDriverProfiles(AgentConfigurationInterface const *const from);

    //-----------------------------------------------------------------------------
    //! Method which performs an undoable copy of the vehicle profiles from the given
    //! instance of AgentConfigurationInterface. The copied profiles are appended
    //! and the views are updated accordingly.
    //!
    //! @param[in]      from        the pointer to the model data to copy from
    //-----------------------------------------------------------------------------
    void copyVehicleProfiles(AgentConfigurationInterface const *const from);

private:
    //-----------------------------------------------------------------------------
    //! Method which performs an undoable clear operation on all agent profiles
    //! currently created.
    //-----------------------------------------------------------------------------
    void clearAgentProfiles();

    //-----------------------------------------------------------------------------
    //! Method which performs an undoable clear operation on all driver profiles
    //! currently created.
    //-----------------------------------------------------------------------------
    void clearDriverProfiles();

    //-----------------------------------------------------------------------------
    //! Method which performs an undoable clear operation on all vehicle profiles
    //! currently created.
    //-----------------------------------------------------------------------------
    void clearVehicleProfiles();

private:
    AgentConfigurationInterface *const agentConfiguration; //!< pointer to the overall model interface of this plugin
    AgentConfigurationView *const agentConfigurationView;  //!< pointer to the overall view of this plugin
    ProjectInterface *const project;                       //!< pointer to the project interface for accessing the file structure of openPASS

private:
    QUndoStack *const undoStack; //!< pointer to the undo stack
    QShortcut *const redo;       //!< shortcut for performing a redo action
    QShortcut *const undo;       //!< shortcut for performing an undo action

private:
    AgentMapPresenter *const agentsPresenter;                 //!< pointer to the presenter object of agent profiles
    VehicleProfilesPresenter *const vehicleProfilesPresenter; //!< pointer to the presenter object of vehicle profiles
    DriverProfilesPresenter *const driverProfilesPresenter;   //!< pointer to the presenter object of driver profiles
};

#endif // AGENTCONFIGURATIONPRESENTER_H
