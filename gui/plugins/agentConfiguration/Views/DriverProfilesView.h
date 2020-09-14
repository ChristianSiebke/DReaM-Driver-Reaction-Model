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
//! @file  DriverProfilesView.h
//! @ingroup agentConfigurationPlugin
//! @brief This class defines the view for editing driver profiles. The view
//!        constitutes a combo box for switching between different driver
//!        profiles, add-, remove- and rename buttons and a table view for editing
//!        the parameters of the driver profile.
//!        Further information on how to use this view can be found in the GUI
//!        tutorial.
//-----------------------------------------------------------------------------

#ifndef DRIVERPROFILESVIEW_H
#define DRIVERPROFILESVIEW_H

#include <QTableView>

#include "WidgetView.h"

namespace Ui {
class DriverProfilesView;
}

//-----------------------------------------------------------------------------
//! @brief This class defines the view for editing driver profiles. The view
//!        constitutes a combo box for switching between different driver
//!        profiles, add-, remove- and rename buttons and a table view for editing
//!        the parameters of the driver profile.
//!        Further information on how to use this view can be found in the GUI
//!        tutorial.
//-----------------------------------------------------------------------------
class DriverProfilesView : public WidgetView
{
    Q_OBJECT

public:
    explicit DriverProfilesView(QWidget *parent = nullptr);
    ~DriverProfilesView();

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted after the add-button for driver profiles has been clicked and
    //! a name for a new profile has been typed in. Transmits the name of the new
    //! driver profile.
    //!
    //! @param[in]      name        the name of the new driver profile
    //-----------------------------------------------------------------------------
    void addDriverProfile(QString const &name);

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the remove-button for driver profiles has been clicked.
    //! Transmits the name of the driver profile currently selected in the combo box,
    //! which is the one to be removed.
    //!
    //! @param[in]      name        the name of the driver profile to be removed
    //-----------------------------------------------------------------------------
    void removeDriverProfile(QString const &name);

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the rename-button has been pressed and a new name for
    //! the current profile has been typed into the dialog box. Transmits the new
    //! and old name.
    //!
    //! @param[in]      oldName        the old name of the renamed driver profile
    //! @param[in]      newName        the new name of the driver profile
    //-----------------------------------------------------------------------------
    void editedName(QString const &oldName, QString const &newName);

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the current entry of the combo box for available driver
    //! profiles has been changed by the user. Transmit the newly selected driver
    //! profile.
    //!
    //! @param[in]      name        the name of the selected driver profile
    //-----------------------------------------------------------------------------
    void selectionChanged(QString const &name);

    //-----------------------------------------------------------------------------
    //! Signal for transmitting the filepath of the driver conifg (so far, not in
    //! use).
    //!
    //! @param[in]      filepath    the new filepath of the driver config
    //-----------------------------------------------------------------------------
    void selectedDriverConfig(QString const &filepath);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which emits the #selectionChanged signal after the user has made a
    //! selection in the drop down menu of available driver profiles.
    //!
    //! @param[in]      name        the name of the newly selected profile
    //-----------------------------------------------------------------------------
    void on_Name_activated(const QString &name);

    //-----------------------------------------------------------------------------
    //! Slot triggered when the user clicks the add-button for driver profiles.
    //! Opens a dialog asking the user to type in a name of the driver profile. After
    //! the user has typed in a valid name, it is transmitted by the #addDriverProfile
    //! signal.
    //-----------------------------------------------------------------------------
    void on_add_clicked();

    //-----------------------------------------------------------------------------
    //! Slot triggered when the user clicks the remove-button for driver profiles.
    //! Emits the #removeDriverProfile signal transmitting the profile currently
    //! displayed in the drop down menu (i.e. the one to be removed).
    //-----------------------------------------------------------------------------
    void on_remove_clicked();

    //-----------------------------------------------------------------------------
    //! Slot triggered when the user clicks the rename-button for driver profiles.
    //! Opens a dialog asking the user to type in a new name for the currently
    //! selected driver profile. After the user has typed in a valid name, the
    //! #editedName signal is emitted.
    //-----------------------------------------------------------------------------
    void on_edit_clicked();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot triggered by clicking the browse-button for the driver configuration
    //! file. Opens a file dialog asking the user to select a file name. After
    //! a valid file has been selected, the filename is transmitted using the
    //! #selectedDriverConfig signal. Note, that currently no driver config is
    //! defined and, hence, this GUI element is temporarily disabled.
    //-----------------------------------------------------------------------------
    void on_Browse_clicked();

public:
    //-----------------------------------------------------------------------------
    //! Method which provides access to the table view for parameters.
    //-----------------------------------------------------------------------------
    QTableView *getParameterView() const;

public:
    //-----------------------------------------------------------------------------
    //! Method which sets the driver profile name to be currently shown in the drop
    //! down menu for driver profiles.
    //!
    //! @param[in]      name        the driver profile to be currently shown in the
    //!                             corresponding drop down menu
    //-----------------------------------------------------------------------------
    void setCurrentDriver(QString const &name);

    //-----------------------------------------------------------------------------
    //! Method which adds the given driver profile name to the driver selection
    //! list (drop down menu for driver profiles).
    //!
    //! @param[in]      name        the driver profile's name to be added to the
    //!                             selection list
    //-----------------------------------------------------------------------------
    void addDriverView(QString const &name);

    //-----------------------------------------------------------------------------
    //! Method which removes the given driver profile name from the driver selection
    //! list (drop down menu for driver profiles).
    //!
    //! @param[in]      name        the driver profile's name to be removed from the
    //!                             selection list
    //-----------------------------------------------------------------------------
    void removeDriverView(QString const &name);

    //-----------------------------------------------------------------------------
    //! Method which clears the selection list for driver profiles (drop down menu)
    //! and inserts the given list.
    //!
    //! @param[in]      names       the list of driver profile names to be listed in
    //!                             the corresponding drop down menu
    //-----------------------------------------------------------------------------
    void setDriverSelectionItems(QStringList const &names);

public:
    //-----------------------------------------------------------------------------
    //! Method which sets the filepath shown displayed in the driver config view to
    //! the one passed as an argument.
    //!
    //! @param[in]      filepath    the filepath to be shown by the driver config view
    //-----------------------------------------------------------------------------
    void setConfigView(QString const &filepath);

public:
    //-----------------------------------------------------------------------------
    //! Method which enables or disables the GUI elements of this view, apart from
    //! the add-button for adding driver profiles. Disabling usually happens when
    //! there is no driver profile configured. Note that since the driver configuration
    //! file has not been developed so far, the corresponding GUI elements will be
    //! always disabled.
    //!
    //! @param[in]      enable        the flag for enabling (true) or disabling
    //!                               (false) the GUI elements
    //-----------------------------------------------------------------------------
    void enableConfiguration(bool enable);

private:
    //-----------------------------------------------------------------------------
    //! Reimplementation of the event filter for catching the resize-events of the
    //! parameter table's view. It takes care of the proper column width ratio after
    //! resizing.
    //!
    //! @param[in]      watched     the watched object
    //! @param[in]      event       the event to be catched
    //!
    //! @return                     always "false" as the event should be catched but
    //!                             not filtered
    //-----------------------------------------------------------------------------
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::DriverProfilesView *ui; //!< the ui object containing all GUI elements of this view
};

#endif // DRIVERPROFILESVIEW_H
