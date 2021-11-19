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
//! @file  AgentMapView.h
//! @ingroup agentConfigurationPlugin
//! @brief This class defines the view for editing agent profiles. The view mainly
//!        constitutes a drop down menu for selecting a profile to be
//!        edited as well as buttons for removing and adding agent profiles.
//!        Further, a menu for editing the type ("dynamic" or "static")
//!        is provided whose selection switches between two different views. If
//!        the type "dynamic" is selected, two tables for assigning driver and
//!        vehicle profiles with probabilities appear. If type "static" is selected,
//!        the view is reduced to a selection menu for a system config + system
//!        as well as a vehicle model catalogue + model. The switching between
//!        the two modes is realized via a stacked widget whose pages contain
//!        the GUI elements being exclusive for the selected mode.
//!        Further information on how to use this view can be found in the GUI
//!        tutorial.
//-----------------------------------------------------------------------------

#ifndef AGENTMAPVIEW_H
#define AGENTMAPVIEW_H

#include <QTableView>
#include <QWidget>

#include "WidgetView.h"

namespace Ui {
class AgentMapView;
}

//-----------------------------------------------------------------------------
//! @brief This class defines the view for editing agent profiles. The view mainly
//!        constitutes a drop down menu for selecting a profile to be
//!        edited as well as buttons for removing and adding agent profiles.
//!        Further, a menu for editing the type ("dynamic" or "static")
//!        is provided whose selection switches between two different views. If
//!        the type "dynamic" is selected, two tables for assigning driver and
//!        vehicle profiles with probabilities appear. If type "static" is selected,
//!        the view is reduced to a selection menu for a system config + system
//!        as well as a vehicle model catalogue + model. The switching between
//!        the two modes is realized via a stacked widget whose pages contain
//!        the GUI elements being exclusive for the selected mode.
//!        Further information on how to use this view can be found in the GUI
//!        tutorial.
//-----------------------------------------------------------------------------
class AgentMapView : public WidgetView
{
    Q_OBJECT

public:
    explicit AgentMapView(QWidget *parent = nullptr);
    ~AgentMapView();

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted after the add-button for agent profiles has been clicked and
    //! a name for a new profile has been typed in. Transmits the name of the new
    //! agent profile.
    //!
    //! @param[in]      name        the name of the new agent profile
    //-----------------------------------------------------------------------------
    void add(QString const &name);

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the remove-button for agent profiles has been clicked.
    //! Transmits the name of the agent profile currently selected in the combo box,
    //! which is the one to be removed.
    //!
    //! @param[in]      name        the name of the agent profile to be removed
    //-----------------------------------------------------------------------------
    void remove(QString const &name);

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the current entry of the combo box for available agent
    //! profiles has been changed by the user. Transmit the newly selected agent
    //! profile.
    //!
    //! @param[in]      name        the name of the new agent profile selected
    //-----------------------------------------------------------------------------
    void selectionChanged(QString const &name);

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the current entry of the combo box for the agent type
    //! (dynamic or static) has changed. Transmits the newly selected type.
    //!
    //! @param[in]      type        the newly selected type of the agent profile
    //-----------------------------------------------------------------------------
    void typeChanged(QString const &type);

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the rename-button has been pressed. Transmits the new
    //! name inserted by the user as well as the old name of the currently selected
    //! agent profile.
    //!
    //! @param[in]      oldName        the old name of the renamed agent profile
    //! @param[in]      newName        the new name of the agent profile
    //-----------------------------------------------------------------------------
    void modifiedName(QString const &oldName, QString const &newName);

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted after add-button for vehicle profiles has been clicked by
    //! the user (only in "dynamic" mode).
    //-----------------------------------------------------------------------------
    void addVehicleProfile();

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the remove-button for vehicle profiles has been clicked
    //! by the user (only in "dynamic" mode). Transmits the row index of the profile
    //! to be removed (i.e. the one having keyboard focus).
    //!
    //! @param[in]      row     the row index of the profile to be removed
    //-----------------------------------------------------------------------------
    void removeVehicleProfile(const int &row);

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted after add-button for driver profiles has been clicked by
    //! the user (only in "dynamic" mode).
    //-----------------------------------------------------------------------------
    void addDriverProfile();

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the remove-button for driver profiles has been clicked
    //! by the user (only in "dynamic" mode). Transmits the row index of the profile
    //! to be removed (i.e. the one having keyboard focus).
    //!
    //! @param[in]      row     the row index of the profile to be removed
    //-----------------------------------------------------------------------------
    void removeDriverProfile(const int &row);

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted after the refresh-button of the system config has been pressed
    //! (only in "static" mode).
    //-----------------------------------------------------------------------------
    void refreshSystemConfig();

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the browse-button of the system config has been pressed
    //! (only in "static" mode).
    //-----------------------------------------------------------------------------
    void setSystemConfig();

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the current entry in the system selection box has
    //! been changed by the user. Transmits the current selection.
    //!
    //! @param[in]      system      the current system selection
    //-----------------------------------------------------------------------------
    void selectedSystem(QString const &system);

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the refresh-button of the vehicle model catalogue
    //! has been pressed (only in "static" mode).
    //-----------------------------------------------------------------------------
    void refreshModelCatalogue();

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the browse-button of the vehicle model catalogue
    //! has been pressed (only in "static" mode).
    //-----------------------------------------------------------------------------
    void setModelCatalogue();

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the current entry in the vehicle model selection box has
    //! been changed by the user. Transmits the current selection.
    //!
    //! @param[in]      model      the current vehicle model selection
    //-----------------------------------------------------------------------------
    void setModel(QString const &model);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which emits the #add signal after the add-button for agent profiles
    //! has been added.
    //-----------------------------------------------------------------------------
    void on_add_clicked();

    //-----------------------------------------------------------------------------
    //! Slot which emits the #remove signal after the remove-button for agent profiles
    //! has been added.
    //-----------------------------------------------------------------------------
    void on_remove_clicked();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered after the rename-button for the agent profile has
    //! been clicked. Opens a Dialog box asking the user to enter a new name for
    //! the currently selected agent profile.
    //-----------------------------------------------------------------------------
    void on_edit_clicked();

    //-----------------------------------------------------------------------------
    //! Slot which emits the #selectionChanged signal after changing the current
    //! selection of the agent profile selection box.
    //!
    //! @param[in]      name        the new agent profile of the selection box
    //-----------------------------------------------------------------------------
    void on_AgentProfileSelection_activated(const QString &name);

    //-----------------------------------------------------------------------------
    //! Slot which emits the #typeChanged signal after changing the current
    //! selection of the type selection box.
    //!
    //! @param[in]      type        the new type of the selection box
    //-----------------------------------------------------------------------------
    void on_Type_activated(const QString &type);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which emits the #addVehicleProfile signal after the add-button for
    //! vehicle profiles has been clicked (for dynamic mode only).
    //-----------------------------------------------------------------------------
    void on_addVehicle_clicked();

    //-----------------------------------------------------------------------------
    //! Slot which emits the #removeVehicleProfile signal after the remove-button for
    //! vehicle profiles has been clicked (for dynamic mode only).
    //-----------------------------------------------------------------------------
    void on_removeVehicle_clicked();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which emits the #addDriverProfile signal after the add-button for
    //! driver profiles has been clicked (for dynamic mode only).
    //-----------------------------------------------------------------------------
    void on_addDriver_clicked();

    //-----------------------------------------------------------------------------
    //! Slot which emits the #removeDriverProfile signal after the remove-button for
    //! driver profiles has been clicked (for dynamic mode only).
    //-----------------------------------------------------------------------------
    void on_removeDriver_clicked();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which emits the #selectedSystem signal after the current entry of the
    //! system selection box is changed by the user (for static mode only).
    //!
    //! @param[in]      system      the new entry of the system selection box
    //-----------------------------------------------------------------------------
    void on_system_activated(const QString &system);

    //-----------------------------------------------------------------------------
    //! Slot which emits the #setSystemConfig signal after the browse-button for
    //! the system config file has been clicked.
    //-----------------------------------------------------------------------------
    void on_systemConfigBrowse_clicked();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the refresh-button for a system config file is
    //! clicked. Emits the #refreshSystemConfig signal.
    //-----------------------------------------------------------------------------
    void on_systemConfigRefresh_clicked();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which emits the #setModelCatalogue signal after the browse-button for
    //! the vehicle model catalogue has been clicked.
    //-----------------------------------------------------------------------------
    void on_ModelCatalogueBrowse_clicked();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the refresh-button for a vehicle model catalogue
    //! is clicked. Emits the #refreshModelCatalogue signal.
    //-----------------------------------------------------------------------------
    void on_ModelCatalogueRefresh_clicked();

    //-----------------------------------------------------------------------------
    //! Slot which emits the #setModel signal after the current entry of the
    //! vehicle model selection box is changed by the user (for static mode only).
    //!
    //! @param[in]      model      the new entry of the vehicle model selection box
    //-----------------------------------------------------------------------------
    void on_Model_activated(const QString &model);

public:
    //-----------------------------------------------------------------------------
    //! Method which provides access to the table view for vehicle profiles.
    //!
    //! @return                     the table view (QTableView) for vehicle profiles
    //-----------------------------------------------------------------------------
    QTableView *getVehicleProfilesView() const;

    //-----------------------------------------------------------------------------
    //! Method which provides access to the table view for driver profiles.
    //!
    //! @return                     the table view (QTableView) for driver profiles
    //-----------------------------------------------------------------------------
    QTableView *getDriverProfilesView() const;

public:
    //-----------------------------------------------------------------------------
    //! Method which adds the given agent profile name to the agent selection
    //! list (drop down menu for agent profiles).
    //!
    //! @param[in]      name        the agent profile's name to be added to the
    //!                             selection list
    //-----------------------------------------------------------------------------
    void addAgent(QString const &name);

    //-----------------------------------------------------------------------------
    //! Method which removes the given agent profile name from the agent selection
    //! list (drop down menu for agent profiles).
    //!
    //! @param[in]      name        the agent profile's name to be removed from
    //!                             the selection list
    //-----------------------------------------------------------------------------
    void removeAgent(QString const &name);

    //-----------------------------------------------------------------------------
    //! Method which clears the selection list for agent profiles (drop down menu)
    //! and inserts the given list.
    //!
    //! @param[in]      names       the list of agent profile names to be listed in
    //!                             the corresponding drop down menu
    //-----------------------------------------------------------------------------
    void setAgentSelectionItems(QStringList const &names);

    //-----------------------------------------------------------------------------
    //! Method which sets the agent profile name to be currently shown in the drop
    //! down menu for agent profiles.
    //!
    //! @param[in]      name        the agent profile to be currently shown in the
    //!                             corresponding drop down menu
    //-----------------------------------------------------------------------------
    void setCurrentAgent(QString const &name);

    //-----------------------------------------------------------------------------
    //! Method which sets the agent profile type (dynamic or static) shown in the
    //! corresponding drop down menu to the one passed as an argument.
    //!
    //! @param[in]      type        the agent profile type to be shown in the
    //!                             corresponding drop down menu
    //-----------------------------------------------------------------------------
    void setCurrentType(QString const &type);

    //-----------------------------------------------------------------------------
    //! Method which clears the current selection elements for static agent
    //! profiles and sets the type and mode back to default "dynamic". Note, that
    //! clearing the table views for vehicle and driver profiles (in dynamic mode)
    //! is done elsewhere using the corresponding reimplementation of QAbstractTableModel.
    //-----------------------------------------------------------------------------
    void clear();

public:
    //-----------------------------------------------------------------------------
    //! Method which the currently displayed system config file to the one passed
    //! as an argument.
    //!
    //! @param[in]      systemConfig        the system config to be displayed
    //-----------------------------------------------------------------------------
    void setCurrentSystemConfig(const QString &systemConfig);

    //-----------------------------------------------------------------------------
    //! Method which the sets the systems' selection list (drop down menu) to the
    //! one passed as an argument.
    //!
    //! @param[in]      systemItems        the list of systems to be offered by the
    //!                                    menu
    //-----------------------------------------------------------------------------
    void setSystemSelectionItems(const QStringList &systemItems);

    //-----------------------------------------------------------------------------
    //! Method which the sets the system currently displayed by the systems' drop
    //! down menu to the one passed as an argument.
    //!
    //! @param[in]      system        the list of systems
    //-----------------------------------------------------------------------------
    void setCurrentSystem(const QString &system);

    //-----------------------------------------------------------------------------
    //! Method which the currently displayed vehicle model catalogue to the one
    //! passed as an argument.
    //!
    //! @param[in]      modelCatalogue        the vehicle model catalogue to be
    //!                                       displayed
    //-----------------------------------------------------------------------------
    void setCurrentModelCatalogue(const QString &modelCatalogue);

    //-----------------------------------------------------------------------------
    //! Method which the sets the vehicle models' selection list (drop down menu)
    //! to the one passed as an argument.
    //!
    //! @param[in]      modelItems        the list of vehicle models to be offered
    //!                                   by the menu
    //-----------------------------------------------------------------------------
    void setModelSelectionItems(const QStringList &modelItems);

    //-----------------------------------------------------------------------------
    //! Method which the sets the vehicle model currently displayed by the vehicle
    //!  models' drop down menu to the one passed as an argument.
    //!
    //! @param[in]      model         the vehicle model to be displayed
    //-----------------------------------------------------------------------------
    void setCurrentModel(const QString &model);

public:
    //-----------------------------------------------------------------------------
    //! Method which enables or disables the GUI elements of this view, apart from
    //! the add-button for adding agent profiles. Disabling usually happens when
    //! there is no agent profile configured.
    //!
    //! @param[in]      enable        the flag for enabling (true) or disabling
    //!                               (false) the GUI elements
    //-----------------------------------------------------------------------------
    void enableConfiguration(bool enable);

    //-----------------------------------------------------------------------------
    //! Method which enables or disables the drop down menu for vehicle models (for
    //! static agent profiles). Disabling the menu usually happens when there is no
    //! vehicle model catalogue loaded.
    //!
    //! @param[in]      enable        the flag for enabling (true) or disabling
    //!                               (false) the vehicle models' menu
    //-----------------------------------------------------------------------------
    void enableModelView(bool enable);

    //-----------------------------------------------------------------------------
    //! Method which enables or disables the drop down menu for systems (for
    //! static agent profiles). Disabling the menu usually happens when there is no
    //! system config XML file loaded.
    //!
    //! @param[in]      enable        the flag for enabling (true) or disabling
    //!                               (false) the systems' menu
    //-----------------------------------------------------------------------------
    void enableSystemView(bool enable);

public:
    //-----------------------------------------------------------------------------
    //! Method which displays the given error message (red color) next to the GUI
    //! element for the system config (for static agent profiles).
    //!
    //! @param[in]      message      the error message to be displayed
    //-----------------------------------------------------------------------------
    void setSystemConfigError(QString const &message);

    //-----------------------------------------------------------------------------
    //! Method which displays the given error message (red color) next to the GUI
    //! element for editing the vehicle model catalogue (for static agent profiles).
    //!
    //! @param[in]      message      the error message to be displayed
    //-----------------------------------------------------------------------------
    void setModelCatalogueError(QString const &message);

    //-----------------------------------------------------------------------------
    //! Method puts a probability error message above the driver profiles' table
    //! if given error flag is "true".
    //!
    //! @param[in]      error      the error flag
    //-----------------------------------------------------------------------------
    void setDriverProbabilityError(bool error);

    //-----------------------------------------------------------------------------
    //! Method puts a probability error message above the vehicle profiles' table
    //! if given error flag is "true".
    //!
    //! @param[in]      error      the error flag
    //-----------------------------------------------------------------------------
    void setVehicleProbabilityError(bool error);

private:
    //-----------------------------------------------------------------------------
    //! Reimplementation of the event filter for catching the resize-events of the
    //! driver and vehicle profiles' table view. It takes care of the proper column
    //! width ratio after resizing.
    //!
    //! @param[in]      watched     the watched object
    //! @param[in]      event       the event to be catched
    //!
    //! @return                     always "false" as the event should be catched but
    //!                             not filtered
    //-----------------------------------------------------------------------------
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::AgentMapView *ui; //!< the ui object containing all GUI elements of this view
};

#endif // AGENTMAPVIEW_H
