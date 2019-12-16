/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef AGENTCONFIGURATIONVIEW_H
#define AGENTCONFIGURATIONVIEW_H

#include "openPASS-Window/WindowInterface.h"

#include "AgentMapView.h"
#include "VehicleProfilesView.h"
#include "DriverProfilesView.h"

#include <QString>
#include <QWidget>
#include <QResizeEvent>
#include <QUndoView>

namespace Ui {
class AgentConfigurationView;
}

class AgentConfigurationPresenter;

class AgentConfigurationView : public QWidget
{
    Q_OBJECT

public:
    explicit AgentConfigurationView(WindowInterface * const window,
                                    QWidget *parent = nullptr);
    virtual ~AgentConfigurationView();

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted after the save-button in the action menu has been clicked.
    //-----------------------------------------------------------------------------
    void SaveSetup();

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the load-button in the action menu has been clicked.
    //-----------------------------------------------------------------------------
    void LoadSetup();

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the export-button in the action menu has been clicked.
    //-----------------------------------------------------------------------------
    void ExportSetup();

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the clear-button in the action menu has been clicked
    //! and the user warning has been confirmed.
    //-----------------------------------------------------------------------------
    void ClearSetup();

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the undo-button in the action menu has been clicked.
    //-----------------------------------------------------------------------------
    void Undo();

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the redo-button in the action menu has been clicked.
    //-----------------------------------------------------------------------------
    void Redo();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the save-button of the action menu has been clicked.
    //! Prompts a file dialog box to let the user select a filepath. Afterwards the signal
    //! SaveSetup is emitted transmitting the selected filepath.
    //-----------------------------------------------------------------------------
    void actionSave();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the load-button of the action menu has been clicked.
    //! Prompts a file dialog box to let the user select a filepath. Afterwards the signal
    //! LoadSetup is emitted transmitting the selected filepath.
    //-----------------------------------------------------------------------------
    void actionLoad();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the export-button of the action menu has been clicked.
    //! Prompts a file dialog box to let the user select a filepath. Afterwards the signal
    //! ExportSetup is emitted transmitting the selected filepath.
    //-----------------------------------------------------------------------------
    void actionExport();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the clear-button of the action menu has been clicked.
    //! Prompts a dialog box to inform the user that the whole worksheet is going to be
    //! cleared. If user confirms, the #ClearSetup signal is emitted.
    //-----------------------------------------------------------------------------
    void actionClear();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the undo-button of the action menu has been clicked.
    //! Will trigger the undo of the previous action.
    //-----------------------------------------------------------------------------
    void actionUndo();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the redo-button of the action menu has been clicked.
    //! Will trigger the redo of the previously undone action.
    //-----------------------------------------------------------------------------
    void actionRedo();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the command history button is clicked. Will
    //! show or hide the command history.
    //-----------------------------------------------------------------------------
    void actionHistory();

public:
    //-----------------------------------------------------------------------------
    //! Method that returns the undo view.
    //!
    //! @return             the undo view
    //-----------------------------------------------------------------------------
    QUndoView * getUndoView();



public:
    AgentMapView * getAgentsView() const;
    VehicleProfilesView * getVehicleProfilesView() const;
    DriverProfilesView * getDriverProfilesView() const;


private:
    static WindowInterface::ID const ViewID;

private:
    WindowInterface * const window;
    QUndoView * const undoView;                         //!< the view of the undo stack

private:
    WindowInterface::Action * const actionMenuSave;     //!< The action menu object for saving a profiles catalogue file
    WindowInterface::Action * const actionMenuExport;   //!< The action menu object for exporting an old  profiles catalogue file
    WindowInterface::Action * const actionMenuLoad;     //!< The action menu object for loading a profiles catalogue file
    WindowInterface::Action * const actionMenuClear;     //!< The action menu object for clearing the entire worksheet
    WindowInterface::Action * const actionMenuUndo;     //!< The action menu object for undoing actions
    WindowInterface::Action * const actionMenuRedo;     //!< The action menu object for redoing actions
    WindowInterface::Action * const actionMenuHistory;  //!< The action menu object for showing the command history

private:
    Ui::AgentConfigurationView *ui;
    AgentMapView * agentsView;
    VehicleProfilesView * vehicleProfilesView;
    DriverProfilesView * driverProfilesView;


};

#endif // AGENTCONFIGURATIONVIEW_H
