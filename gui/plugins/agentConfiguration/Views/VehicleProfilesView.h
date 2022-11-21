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
//! @file  VehicleProfilesView.h
//! @ingroup agentConfigurationPlugin
//! @brief This class defines the view for editing vehicle profiles.
//!        The view constitutes a combo box for selecting and buttons
//!        for adding, removing and renaming vehicle profiles. Further, GUI
//!        elements for vehicle model catalogue and vehicle model as well as
//!        for the system config are provided. Systems defined in a system config
//!        are sorted by the algorithm type used therein. For the corresponding
//!        selection of an algorithm type, a combo box including add- and remove-
//!        buttons is set up. The dynamic instantiation of the table views for
//!        systems of a certain algorithm type (SystemProfileView)
//!        is performed in the presenter object (VehicleProfilesPresenter). This
//!        object calls public methods of this class in order to instantiate the
//!        table views and and lay them out in this widget.
//-----------------------------------------------------------------------------

#ifndef VEHICLEPROFILESVIEW_H
#define VEHICLEPROFILESVIEW_H

#include <QString>
#include <QStringList>

#include "Views/SystemProfileView.h"
#include "Views/WidgetView.h"

namespace Ui {
class VehicleProfilesView;
}

//-----------------------------------------------------------------------------
//! @brief This class defines the view for editing vehicle profiles.
//!        The view constitutes a combo box for selecting and buttons
//!        for adding, removing and renaming vehicle profiles. Further, GUI
//!        elements for vehicle model catalogue and vehicle model as well as
//!        for the system config are provided. Systems defined in a system config
//!        are sorted by the algorithm type used therein. For the corresponding
//!        selection of an algorithm type, a combo box including add- and remove-
//!        buttons is set up. The dynamic instantiation of the table views for
//!        systems of a certain algorithm type (SystemProfileView)
//!        is performed in the presenter object (VehicleProfilesPresenter). This
//!        object calls public methods of this class in order to instantiate the
//!        table views and and lay them out in this widget.
//-----------------------------------------------------------------------------
class VehicleProfilesView : public WidgetView
{
    Q_OBJECT

public:
    explicit VehicleProfilesView(QWidget *parent = nullptr);
    ~VehicleProfilesView();

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted after the add-button for vehicle profiles has been clicked and
    //! a name for a new profile has been typed in. Transmits the name of the new
    //! vehicle profile.
    //!
    //! @param[in]      name        the name of the new vehicle profile
    //-----------------------------------------------------------------------------
    void addVehicleProfile(QString const &name);

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the remove-button for vehicle profiles has been clicked.
    //! Transmits the name of the vehicle profile currently selected in the combo box,
    //! which is the one to be removed.
    //!
    //! @param[in]      name        the name of the vehicle profile to be removed
    //-----------------------------------------------------------------------------
    void removeVehicleProfile(QString const &name);

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the rename-button has been pressed and a new name for
    //! the current profile has been typed into the dialog box. Transmits the new
    //! and old name.
    //!
    //! @param[in]      oldName        the old name of the renamed vehicle profile
    //! @param[in]      newName        the new name of the vehicle profile
    //-----------------------------------------------------------------------------
    void modifiedName(QString const &oldName, QString const &newName);

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the current entry of the combo box for available vehicle
    //! profiles has been changed by the user. Transmit the newly selected vehicle
    //! profile.
    //!
    //! @param[in]      name        the name of the selected vehicle profile
    //-----------------------------------------------------------------------------
    void selectedVehicleProfile(QString const &name);

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted after the refresh-button of the vehicle model catalogue
    //! has been pressed.
    //-----------------------------------------------------------------------------
    void refreshModelCatalogue();

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the browse-button of the vehicle model catalogue
    //! has been pressed.
    //-----------------------------------------------------------------------------
    void changeModelCatalogue();

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the current entry in the vehicle models' combo box has
    //! been changed by the user. Transmits the new selection.
    //!
    //! @param[in]      model      the current vehicle model selection
    //-----------------------------------------------------------------------------
    void modifiedModel(QString const &ModelName);

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted after the browse-button of the system config file
    //! has been pressed.
    //-----------------------------------------------------------------------------
    void changeSystemConfig();

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the refresh-button of the system config file
    //! has been pressed.
    //-----------------------------------------------------------------------------
    void refreshSystemConfig();

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the add-button of an algorithm type has been clicked.
    //! Transmits the algorithm type of the current selection in the combo box. This
    //! signal will be handled by the presenter object (VehicleProfilesPresenter)
    //! which creates the table view for assigning systems of the given algo
    //! type to this profile (instance of SystemProfileView).
    //!
    //! @param[in]      type        the algorithm type of
    //-----------------------------------------------------------------------------
    void addSystemProfile(QString const &type);

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the remove-button of an algorithm type has been clicked.
    //! Transmits the algorithm type of the current selection in the combo box. This
    //! signal will be handled by the presenter object (VehicleProfilesPresenter)
    //! which instantiates the table view for assigning systems of the given algo
    //! type to this profile (instance of SystemProfileView).
    //!
    //! @param[in]      type        the algorithm type of
    //-----------------------------------------------------------------------------
    void removeSystemProfile(QString const &type);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot triggered when the user changes the vehicle profile displayed
    //! by the corresponding combo box. Emits the #selectedVehicleProfile signal with
    //! the name of the newly selected profile.
    //!
    //! @param[in]      name        the name of the selected vehicle profile
    //-----------------------------------------------------------------------------
    void on_Name_activated(const QString &name);

    //-----------------------------------------------------------------------------
    //! Slot triggered when the user clicks the add-button for vehicle
    //! profiles and has typed in a valid name. Emits the #addVehicleProfile
    //! signal transmitting the new name.
    //-----------------------------------------------------------------------------
    void on_add_clicked();

    //-----------------------------------------------------------------------------
    //! Slot triggered when the user clicks the remove-button for vehicle
    //! profiles. Emits the #removeVehicleProfile signal transmitting the name of
    //! the vehicle profile that is currently displayed by the corresponding combo
    //! box (the profile to be removed).
    //-----------------------------------------------------------------------------
    void on_remove_clicked();

    //-----------------------------------------------------------------------------
    //! Slot triggered when the user clicks the rename-button for vehicle
    //! profiles. Opens a dialog box asking the user to type in a valid name. Emits
    //! the #modifiedName signal transmitting the new and old names of the coresponding
    //! vehicle profile.
    //-----------------------------------------------------------------------------
    void on_edit_clicked();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot triggered when the user clicks the refresh-button for the vehicle model
    //! catalogue. Emits the #refreshModelCatalogue signal.
    //-----------------------------------------------------------------------------
    void on_modelCatalogue_Refresh_clicked();

    //-----------------------------------------------------------------------------
    //! Slot triggered when the user clicks the browse-button for the vehicle model
    //! catalogue. Emits the #changeModelCatalogue signal.
    //-----------------------------------------------------------------------------
    void on_modelCatalogue_Browse_clicked();

    //-----------------------------------------------------------------------------
    //! Slot triggered when the user changes the current entry of the vehicle model's
    //! combo box. Emits the #modifiedModel signal transmitting the newly selected
    //! vehicle model's name.
    //!
    //! @param[in]      model       the newly selected vehicle model
    //-----------------------------------------------------------------------------
    void on_Model_activated(const QString &model);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot triggered when the user clicks the refresh-button for the system config
    //! file. Emits the #refreshSystemConfig signal.
    //-----------------------------------------------------------------------------
    void on_systemConfigRefresh_clicked();

    //-----------------------------------------------------------------------------
    //! Slot triggered when the user clicks the browse-button for the system config
    //! file. Emits the #changeSystemConfig signal.
    //-----------------------------------------------------------------------------
    void on_systemConfigBrowse_clicked();

    //-----------------------------------------------------------------------------
    //! Slot triggered when the user clicks the add-button next to the algorithm
    //! type combo box. Emits the #addSystemProfile signal transmitting the current
    //! algorithm type displayed by the combo box.
    //-----------------------------------------------------------------------------
    void on_addSystemProfile_clicked();

    //-----------------------------------------------------------------------------
    //! Slot triggered when the user clicks the remove-button next to the algorithm
    //! type combo box. Emits the #removeSystemProfile signal transmitting the current
    //! algorithm type displayed by the combo box.
    //-----------------------------------------------------------------------------
    void on_removeSystemProfile_clicked();

public:
    //-----------------------------------------------------------------------------
    //! Method which sets the vehicle profile name to be currently shown in the combo
    //! box for vehicle profiles.
    //!
    //! @param[in]      name        the vehicle profile to be currently shown in the
    //!                             corresponding combo box
    //-----------------------------------------------------------------------------
    void setCurrentVehicle(QString const &name);

    //-----------------------------------------------------------------------------
    //! Method which adds the given vehicle profile name to the selection
    //! list in the combo box for vehicle profiles. Further enables the whole view.
    //!
    //! @param[in]      name        the vehicle profile's name to be added to the
    //!                             selection list
    //-----------------------------------------------------------------------------
    void addVehicleProfileSelectionItem(QString const &name);

    //-----------------------------------------------------------------------------
    //! Method which removes the given vehicle profile name from the selection
    //! list in the combo box for vehicle profiles.
    //!
    //! @param[in]      name        the vehicle profile's name to be removed from the
    //!                             selection list
    //-----------------------------------------------------------------------------
    void removeVehicleProfileSelectionItem(QString const &name);

    //-----------------------------------------------------------------------------
    //! Method which clears the selection list for vehicle profiles (combo box)
    //! and inserts the given list.
    //!
    //! @param[in]      names       the list of vehicle profile names to be listed in
    //!                             the corresponding combo box
    //-----------------------------------------------------------------------------
    void setVehicleProfileSelectionItems(QStringList const &names);

public:
    //-----------------------------------------------------------------------------
    //! Method which sets the currently displayed vehicle model catalogue to the one
    //! passed as an argument.
    //!
    //! @param[in]      filepath        the vehicle model catalogue's file path to be
    //!                                 displayed
    //-----------------------------------------------------------------------------
    void setModelCatalogue(QString const &filepath);

    //-----------------------------------------------------------------------------
    //! Method which the sets the vehicle model currently displayed by the vehicle
    //! models' combo box to the one passed as an argument.
    //!
    //! @param[in]      system        the list of systems
    //-----------------------------------------------------------------------------
    void setModel(QString const &model);

    //-----------------------------------------------------------------------------
    //! Method which the sets the vehicle models' selection list (combo box)
    //! to the one passed as an argument.
    //!
    //! @param[in]      models        the list of vehicle models to be offered
    //!                               by the menu
    //-----------------------------------------------------------------------------
    void setModelSelectionItems(QStringList const &models);

    //-----------------------------------------------------------------------------
    //! Method which displays the given error message (red color) next to the GUI
    //! element for editing the vehicle model catalogue.
    //!
    //! @param[in]      message      the error message to be displayed
    //-----------------------------------------------------------------------------
    void setModelCatalogueError(QString const &message);

    //-----------------------------------------------------------------------------
    //! Method which enables or disables the combo box for vehicle models. Disabling
    //! the menu usually happens when there is no vehicle model catalogue loaded.
    //!
    //! @param[in]      enable        the flag for enabling (true) or disabling
    //!                               (false) the vehicle models' menu
    //-----------------------------------------------------------------------------
    void enableModelView(bool enable);

public:
    //-----------------------------------------------------------------------------
    //! Method which enables or disables the GUI elements of this view, apart from
    //! the add-button for adding vehicle profiles. Disabling usually happens when
    //! there is no vehicle profile configured.
    //!
    //! @param[in]      enable        the flag for enabling (true) or disabling
    //!                               (false) the GUI elements
    //-----------------------------------------------------------------------------
    void enableConfiguration(bool enable);

public:
    //-----------------------------------------------------------------------------
    //! Method which sets the currently displayed system config file to the one
    //! passed as an argument.
    //!
    //! @param[in]      systemConfig    the system config's file path to be
    //!                                 displayed
    //-----------------------------------------------------------------------------
    void setSystemConfig(QString const &systemConfig);

    //-----------------------------------------------------------------------------
    //! Method which the sets the selection list of algorithm types (referred to
    //! as system profiles in the code) in the corresponding combo box to the one
    //! passed as an argument.
    //!
    //! @param[in]      profiles        the list of algorithm types
    //-----------------------------------------------------------------------------
    void setSystemProfileSelection(QList<QString> profiles);

    //-----------------------------------------------------------------------------
    //! Method which displays the given error message (red color) next to the GUI
    //! element for the system config file.
    //!
    //! @param[in]      message      the error message to be displayed
    //-----------------------------------------------------------------------------
    void setSystemConfigError(QString const &message);

    //-----------------------------------------------------------------------------
    //! Method which displays the given error message (red color) above the tables
    //! in which systems of a certain algorithm type are assigned to probalities.
    //!
    //! @param[in]      message      the error message to be displayed
    //-----------------------------------------------------------------------------
    void setSystemProfilesError(QString const &message);

public:
    //-----------------------------------------------------------------------------
    //! Method which creates an instance of SystemProfileView, i.e. a table for
    //! assigning systems of the given algorithm type to probabilities. The created
    //! view is added to the bottom of the layout of this view and a pointer to it
    //! is returned.
    //!
    //! @param[in]      type      the (algorithm) type of the system table view to be
    //!                           created
    //!
    //! @return                   a pointer to the newly created view (instance of
    //!                           SystemProfileView)
    //-----------------------------------------------------------------------------
    SystemProfileView *addSystemProfileView(QString const &type);

    //-----------------------------------------------------------------------------
    //! Method which deletes the instance of SystemProfileView, i.e. the table for
    //! assigning systems of the given (algorithm) type to probabilities.
    //!
    //! @param[in]      type      the (algorithm) type of the system table view to be
    //!                           deleted
    //-----------------------------------------------------------------------------
    void removeSystemProfileView(QString const &type);

    //-----------------------------------------------------------------------------
    //! Method which deletes all instances of SystemProfileView, i.e. the table for
    //! assigning systems to probabilities.
    //-----------------------------------------------------------------------------
    void clearSystemProfileViews();

private:
    Ui::VehicleProfilesView *ui; //!< the ui object defining the GUI elements of this view
};

#endif // VEHICLEPROFILESVIEW_H
