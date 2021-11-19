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
//! @file  DriverProfilesPresenter.h
//! @ingroup agentConfigurationPlugin
//! @brief This file contains the class DriverProfilesPresenter defining the
//!        presenter object for driver profiles as well as the classes for all
//!        undo-command objects used by DriverProfilesPresenter.
//!
//!        The DriverProfilesPresenter class handles user events from the
//!        view (DriverProfilesView) and edits the model data (DriverProfileMapInterface)
//!        accordingly. Since all actions performed on the model data (i.e. the
//!        *.ui file) should be undoable, the presenter instantiates the
//!        corresponding undo-commands and pushes them onto the undo stack.
//!        The actual operation on the model is then performed in these command
//!        object using the re-implementations of the undo/redo methods. See
//!        Qt's undo framework for further details.
//-----------------------------------------------------------------------------

#ifndef DRIVERPROFILESPRESENTER_H
#define DRIVERPROFILESPRESENTER_H

#include <QObject>
#include <QUndoStack>

#include "Presenters/ParameterMapPresenter.h"
#include "Views/DriverProfilesView.h"
#include "openPASS-AgentConfiguration/DriverProfileMapInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class defines the presenter object for driver profiles which
//!        handles user events from the view (DriverProfilesView) and edits
//!        the model data (DriverProfileMapInterface) accordingly.
//!
//!        Since all actions performed on the model data (i.e. the
//!        *.ui file) should be undoable, the presenter instantiates the
//!        corresponding undo-commands and pushes them onto the undo stack.
//!        The actual operation on the model is then performed in these command
//!        object using the re-implementations of the undo/redo methods. See
//!        Qt's undo framework for further details.
//-----------------------------------------------------------------------------
class DriverProfilesPresenter : public QObject
{
public:
    DriverProfilesPresenter(DriverProfileMapInterface *drivers,
                            DriverProfilesView *driversView,
                            QUndoStack *const undoStack,
                            QObject *parent = nullptr);
    virtual ~DriverProfilesPresenter() override = default;

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot triggered by the DriverProfilesView::selectionChanged signal. Performs an
    //! undoable set-operation on the currently edited driver profile setting it to the
    //! transmitted profile. Updates the view accordingly.
    //!
    //! @param[in]      name        the driver profile which should be currently
    //!                             edited
    //-----------------------------------------------------------------------------
    void setCurrentDriver(const QString &name);

    //-----------------------------------------------------------------------------
    //! Slot triggered by the DriverProfilesView::addDriverProfile signal. Performs an
    //! undoable add-operation on the driver profiles map. Sets the newly created driver
    //! profile as the currently edited profile and updates the view accordingly.
    //!
    //! @param[in]      name        the name of the new driver profile
    //-----------------------------------------------------------------------------
    void addDriver(const QString &name);

    //-----------------------------------------------------------------------------
    //! Slot triggered by the DriverProfilesView::removeDriverProfile signal. Performs an
    //! undoable remove-operation on the driver profiles map removing the currently
    //! selected profile. If the list of available profiles is not empty, it
    //! sets the currently edited profile to the first entry of the list.
    //!
    //! @param[in]      name        the name of the driver profile to be removed
    //-----------------------------------------------------------------------------
    void removeDriver(const QString &name);

    //-----------------------------------------------------------------------------
    //! Slot triggered by the DriverProfilesView::editedName signal. Performs an
    //! undoable rename-operation on the name of the currently edited driver profile.
    //!
    //! @param[in]      oldName        the old name of the driver profile
    //! @param[in]      newName        the new name of the driver profile
    //-----------------------------------------------------------------------------
    void editDriver(const QString &oldName, const QString &newName);

    //-----------------------------------------------------------------------------
    //! Slot for setting the driver configuration file.
    //!
    //! @note this slot is not yet in use as no driver configuration file
    //! has been developed yet.
    //!
    //! @param[in]     filepath        the file path of the driver configuration file
    //-----------------------------------------------------------------------------
    void setDriverConfig(const QString &filepath);

private:
    //-----------------------------------------------------------------------------
    //! Method which sets the delegates of the parameter table's cells according to
    //! the current model data. The delegates depend on the datatype of the
    //! parameters. This method is called in updateView.
    //-----------------------------------------------------------------------------
    void setParameterDelegates();

public:
    //-----------------------------------------------------------------------------
    //! Method which updates the complete view (DriverProfilesView) according to the
    //! model data of the current driver profile.
    //-----------------------------------------------------------------------------
    void updateView();

    //-----------------------------------------------------------------------------
    //! Method which updates the combo box list of available driver profiles to the
    //! ones currently defined in the model data (DriverProfileMapInterface).
    //-----------------------------------------------------------------------------
    void updateDriverSelectionItems();

private:
    DriverProfileMapInterface *const drivers;     //!< the map of driver profiles
    DriverProfileItemInterface *currentDriver;    //!< a pointer to the currently edited driver profile
    DriverProfilesView *const driversView;        //!< a pointer to the view for driver profiles
    QUndoStack *const undoStack;                  //!< a pointer to the undo stack
    ParameterMapPresenter *parameters;            //!< a pointer to the presenter object for the parameters of a driver profile
    QList<QString> bool_list = {"true", "false"}; //!< a combo box selection list for a parameter delegate representing a boolian variable

private:
    friend class AgentConfigurationPresenter;
};

namespace DriverProfileCommands {

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable set-operations on the
//! currently selected driver profile.
//-----------------------------------------------------------------------------
class SetCurrentDriver : public QUndoCommand
{
public:
    SetCurrentDriver(DriverProfileMapInterface const *const drivers,
                     DriverProfileItemInterface **const currentDriver,
                     DriverProfileItemInterface::Name const newDriver,
                     ParameterMapPresenter *const parameters,
                     DriverProfilesPresenter *const driversPresenter,
                     QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    DriverProfileMapInterface const *const drivers;
    DriverProfileItemInterface **const currentDriver;
    DriverProfileItemInterface::Name const newDriver;
    DriverProfileItemInterface::Name const oldDriver;
    ParameterMapPresenter *const parameters;
    DriverProfilesPresenter *const driversPresenter;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable renaming-operations on the
//! map of driver profiles (DriverProfileMapInterface).
//-----------------------------------------------------------------------------
class EditDriverProfile : public QUndoCommand
{
public:
    EditDriverProfile(DriverProfileMapInterface *const drivers,
                      DriverProfilesView *const driversView,
                      DriverProfileMapInterface::Name const &newName,
                      DriverProfileMapInterface::Name const &oldName,
                      QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    DriverProfileMapInterface *const drivers;
    DriverProfilesView *const driversView;
    DriverProfileMapInterface::Name const newName;
    DriverProfileMapInterface::Name const oldName;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable add-operations on the
//! map of driver profiles (DriverProfileMapInterface).
//-----------------------------------------------------------------------------
class AddDriverProfile : public QUndoCommand
{
public:
    AddDriverProfile(DriverProfileMapInterface *const drivers,
                     DriverProfilesView *const driversView,
                     DriverProfileMapInterface::Name const &name,
                     QUndoCommand *parent = nullptr);

    AddDriverProfile(DriverProfileMapInterface *const drivers,
                     DriverProfilesView *const driversView,
                     DriverProfileMapInterface::Name const &name,
                     DriverProfileItemInterface::Parameters const *const parameterMap,
                     QUndoCommand *parent = nullptr);

    virtual ~AddDriverProfile() override;

public:
    void redo() override;
    void undo() override;

private:
    void addParameter(DriverProfileItemInterface::Parameters *const parameterMap,
                      ParameterItemInterface::Key const &key,
                      ParameterItemInterface::Type const &type,
                      ParameterItemInterface::Unit const &unit,
                      ParameterItemInterface::Value const &value);
    void setParameters();

private:
    DriverProfileMapInterface *const drivers;
    DriverProfilesView *const driversView;
    DriverProfileMapInterface::Name const name;
    DriverProfileItemInterface::Parameters *const parameters;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable remove-operations on the
//! map of driver profiles (DriverProfileMapInterface).
//-----------------------------------------------------------------------------
class RemoveDriverProfile : public QUndoCommand
{
public:
    RemoveDriverProfile(DriverProfileMapInterface *const drivers,
                        DriverProfilesView *const driversView,
                        DriverProfileMapInterface::Name const &name,
                        QUndoCommand *parent = nullptr);

    virtual ~RemoveDriverProfile() override;

public:
    void redo() override;
    void undo() override;

private:
    DriverProfileMapInterface *const drivers;
    DriverProfilesView *const driversView;
    DriverProfileMapInterface::Name const name;
    DriverProfileItemInterface const *const driver;
};

} // namespace DriverProfileCommands
#endif // DRIVERPROFILESPRESENTER_H
