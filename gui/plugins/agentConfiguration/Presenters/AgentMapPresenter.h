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
//! @file  AgentMapPresenter.h
//! @ingroup agentConfigurationPlugin
//! @brief This file contains the class AgentMapPresenter defining the
//!        presenter object for agent profiles as well as the classes for all
//!        undo-command objects used by AgentMapPresenter.
//!
//!        The AgentMapPresenter class handles user events from the
//!        view (AgentMapView) and edits the model data (AgentMapInterface)
//!        accordingly. Since all actions performed on the model data (i.e. the
//!        *.ui file) should be undoable, the presenter instantiates the
//!        corresponding undo-commands and pushes them onto the undo stack.
//!        The actual operation on the model is then performed in these command
//!        object using the re-implementations of the undo/redo methods. See
//!        Qt's undo framework for further details.
//-----------------------------------------------------------------------------

#ifndef AGENTMAPPRESENTER_H
#define AGENTMAPPRESENTER_H

#include <QObject>
#include <QUndoStack>

#include "Presenters/DriverProfilesMapPresenter.h"
#include "Presenters/VehicleProfilesMapPresenter.h"
#include "Views/AgentMapView.h"
#include "openPASS-AgentConfiguration/AgentMapInterface.h"
#include "openPASS-AgentConfiguration/DriverProfileMapInterface.h"
#include "openPASS-AgentConfiguration/VehicleProfileMapInterface.h"
#include "openPASS-Project/ProjectInterface.h"

//-----------------------------------------------------------------------------
//! @brief The AgentMapPresenter class handles user events from the
//!        view (AgentMapView) and edits the model data (AgentMapInterface)
//!        accordingly. Since all actions performed on the model data (i.e. the
//!        *.ui file) should be undoable, the presenter instantiates the
//!        corresponding undo-commands and pushes them onto the undo stack.
//!        The actual operation on the model is then performed in these command
//!        object using the re-implementations of the undo/redo methods. See
//!        Qt's undo framework for further details.
//-----------------------------------------------------------------------------
class AgentMapPresenter : public QObject
{
    Q_OBJECT
public:
    explicit AgentMapPresenter(AgentMapInterface *agents,
                               VehicleProfileMapInterface *vehicleProfiles,
                               DriverProfileMapInterface *driverProfiles,
                               AgentMapView *agentsView,
                               ProjectInterface *const project,
                               QUndoStack *const undoStack,
                               QObject *parent = nullptr);
    virtual ~AgentMapPresenter() = default;

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot triggered by the AgentMapView::selectionChanged signal. Performs an
    //! undoable set-operation on the currently edited agent profile setting it to
    //! the transmitted profile. Updates the view accordingly.
    //!
    //! @param[in]      name        the agent profile which should be currently
    //!                             edited
    //-----------------------------------------------------------------------------
    void setCurrentAgent(const QString &name);

    //-----------------------------------------------------------------------------
    //! Slot triggered by the AgentMapView::typeChanged signal. Performs an
    //! undoable set-operation on the type ("static" or "dynamic") of the currently
    //! edited agent profile and updates the view accordingly.
    //!
    //! @param[in]      type        the new type of the currently edited profile
    //-----------------------------------------------------------------------------
    void setType(const QString &type);

    //-----------------------------------------------------------------------------
    //! Slot triggered by the AgentMapView::add signal. Performs an
    //! undoable add-operation on the agent profiles map. Sets the newly created agent
    //! profile as the currently edited agent profile and updates the view accordingly.
    //!
    //! @param[in]      name        the name of the new agent profile
    //-----------------------------------------------------------------------------
    void addAgent(QString const &name);

    //-----------------------------------------------------------------------------
    //! Slot triggered by the AgentMapView::remove signal. Performs an
    //! undoable remove-operation on the agent profiles map removing the currently
    //! selected agent profile. If the list of available profiles is not empty, it
    //! sets the currently edited profile to the first entry of the list.
    //!
    //! @param[in]      name        the name of the agent profile to be removed
    //-----------------------------------------------------------------------------
    void removeAgent(QString const &name);

    //-----------------------------------------------------------------------------
    //! Slot triggered by the AgentMapView::modifiedName signal. Performs an
    //! undoable rename-operation on the name of the currently edited agent profile.
    //!
    //! @param[in]      oldName        the old name of the agent profile
    //! @param[in]      newName        the new name of the agent profile
    //-----------------------------------------------------------------------------
    void editAgent(QString const &oldName, QString const &newName);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot triggered by the AgentMapView::addVehicleProfile signal. Performs an
    //! undoable add-operation on the table of vehicle profiles
    //! (VehicleProfilesMapPresenter) of the current agent profile. The default
    //! selection of the vehicle profile is the first profile that has not been
    //! already added to the current agent profile.
    //!
    //! @note   this slot is only used for dynamic agent profiles
    //-----------------------------------------------------------------------------
    void addVehicleProfile();

    //-----------------------------------------------------------------------------
    //! Slot triggered by the AgentMapView::removeVehicleProfile signal. Performs an
    //! undoable remove-operation on given row of the vehicle profiles' table
    //! (VehicleProfilesMapPresenter) of the current agent profile.
    //!
    //! @note   this slot is only used for dynamic agent profiles
    //! @param[in]      row     the row to be removed from the vehicle profiles'
    //!                         table
    //-----------------------------------------------------------------------------
    void removeVehicleProfile(const int &row);

    //-----------------------------------------------------------------------------
    //! Slot triggered by the VehicleProfileMapInterface::added,
    //! VehicleProfileMapInterface::removed and VehicleProfileMapInterface::modifiedName
    //! signals. Updates the list of the vehicle profiles' combo box (in AgentMapView)
    //! according to changes made in the corresponding vehicle profile model data.
    //!
    //! @note   this slot is only used for dynamic agent profiles
    //-----------------------------------------------------------------------------
    void updateVehicleSelectionItems();

    //-----------------------------------------------------------------------------
    //! Slot triggered by the VehicleProfilesMapPresenter::checkProbabilities signal.
    //! Calculates the probability sum of the vehicle profiles' assigned to the
    //! currently edited agent profile. Displays an error message if probability sum
    //! unequals 1.
    //!
    //! @note   this slot is only used for dynamic agent profiles
    //-----------------------------------------------------------------------------
    void checkVehicleProbabilities();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot triggered by the AgentMapView::addDriverProfile signal. Performs an
    //! undoable add-operation on the table of driver profiles
    //! (DriverProfilesMapPresenter) of the current agent profile. The default
    //! selection of the driver profile is the first profile that has not been
    //! already added to the current agent profile.
    //!
    //! @note   this slot is only used for dynamic agent profiles
    //-----------------------------------------------------------------------------
    void addDriverProfile();

    //-----------------------------------------------------------------------------
    //! Slot triggered by the AgentMapView::removeDriverProfile signal. Performs an
    //! undoable remove-operation on given row of the driver profiles' table
    //! (DriverProfilesMapPresenter) of the current agent profile.
    //!
    //! @note   this slot is only used for dynamic agent profiles
    //! @param[in]      row     the row to be removed from the driver profiles'
    //!                         table
    //-----------------------------------------------------------------------------
    void removeDriverProfile(const int &row);

    //-----------------------------------------------------------------------------
    //! Slot triggered by the DriverProfileMapInterface::added,
    //! DriverProfileMapInterface::removed and DriverProfileMapInterface::modifiedName
    //! signals. Updates the list of the driver profiles' combo box (in AgentMapView)
    //! according to changes made in the corresponding driver profile model data.
    //!
    //! @note   this slot is only used for dynamic agent profiles
    //-----------------------------------------------------------------------------
    void updateDriverSelectionItems();

    //-----------------------------------------------------------------------------
    //! Slot triggered by the DriverProfilesMapPresenter::checkProbabilities signal.
    //! Calculates the probability sum of the driver profiles' assigned to the
    //! currently edited agent profile. Displays an error message if probability sum
    //! unequals 1.
    //!
    //! @note   this slot is only used for dynamic agent profiles
    //-----------------------------------------------------------------------------
    void checkDriverProbabilities();

public Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot triggered by the AgentMapView::setSystemConfig signal. Opens a file dialog
    //! for the user to select a system config XML file. After a non-empty selection has
    //! been made, an undoable set-operation on the system config of the currently
    //! edited agent profile ist performed. The system config is thereby set to the
    //! one selected by the user and the view is updated accordingly.
    //!
    //! @note   this slot is only used for static agent profiles
    //-----------------------------------------------------------------------------
    void setSystemConfig();

    //-----------------------------------------------------------------------------
    //! Slot triggered by the AgentMapView::refreshSystemConfig and
    //! ProjectInterface::update signals. Re-loads the
    //! currently selected system config XML file and updates the view accordingly.
    //! In particular, this method can be used to update GUI error messages after
    //! the file(path) has been edited by the user.
    //!
    //! @note   this slot is only used for static agent profiles
    //-----------------------------------------------------------------------------
    void refreshSystemConfig();

    //-----------------------------------------------------------------------------
    //! Slot which is used to scan the given system config for available systems.
    //! Updates the selection of the corresponding combo box (in AgentMapView).
    //!
    //! @note   this slot is only used for static agent profiles
    //! @param[in]      systemConfig        the system config to be parsed
    //-----------------------------------------------------------------------------
    void setSystemItems(QString const &systemConfig);

    //-----------------------------------------------------------------------------
    //! Slot triggered by the AgentMapView::selectedSystem signal. First extracts Id
    //! and title of the given system list entry. Then, performs an undoable assignment
    //! of the system to the currently edited agent profile.
    //!
    //! @note   this slot is only used for static agent profiles
    //! @param[in]      system        the list description of the system to be assigned
    //!                               to the current agent profile
    //-----------------------------------------------------------------------------
    void setSystem(QString const &system);

public Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot triggered by the AgentMapView::setModelCatalogue signal. Opens a file dialog
    //! for the user to select a vehicle model catalogue file. After a non-empty selection has
    //! been made, an undoable set-operation for the model catalogue of the currently
    //! edited agent profile ist performed. The model catalogue is thereby set to the
    //! one selected by the user and the view is updated accordingly.
    //!
    //! @note   this slot is only used for static agent profiles
    //-----------------------------------------------------------------------------
    void setModelCatalogue();

    //-----------------------------------------------------------------------------
    //! Slot triggered by the AgentMapView::refreshModelCatalogue and
    //! ProjectInterface::update signals. Re-loads the currently selected
    //! vehicle model catalogue and updates the view accordingly.
    //! In particular, this method can be used to update GUI error messages after
    //! the file(path) has been edited by the user.
    //!
    //! @note   this slot is only used for static agent profiles
    //-----------------------------------------------------------------------------
    void refreshModelCatalogue();

    //-----------------------------------------------------------------------------
    //! Slot which is used to scan the model catalogue file for available vehicle
    //! models. Updates the selection of the corresponding combo box (in AgentMapView).
    //!
    //! @note   this slot is only used for static agent profiles
    //! @param[in]      modelCatalogue        the model catalogue to be parsed
    //-----------------------------------------------------------------------------
    void setModelItems(QString const &modelCatalogue);

    //-----------------------------------------------------------------------------
    //! Slot triggered by the AgentMapView::setModel signal. Performs an undoable
    //! assignment of the given vehicle model to the currently edited agent profile.
    //!
    //! @note   this slot is only used for static agent profiles
    //! @param[in]      model        the vehicle model to be assigned
    //-----------------------------------------------------------------------------
    void setModel(QString const &model);

public:
    //-----------------------------------------------------------------------------
    //! Method which updates the complete view (AgentMapView) according to the model
    //! data of the current agent profile.
    //-----------------------------------------------------------------------------
    void updateView();

    //-----------------------------------------------------------------------------
    //! Method which updates the combo box list of available agent profiles to the
    //! ones currently defined in the model data (AgentMapInterface).
    //-----------------------------------------------------------------------------
    void updateAgentSelectionItems();

private:
    //-----------------------------------------------------------------------------
    //! Method which is called by the constructor. Instantiates and sets up the
    //! presenter object (VehicleProfilesMapPresenter) of the table view for the
    //! vehicle profiles assigned to the current agent profile. Follows the concept
    //! of Qt's Model/View concept, in particular for implementing tables.
    //-----------------------------------------------------------------------------
    void setupVehicleProfilesMap();

    //-----------------------------------------------------------------------------
    //! Method which is called by the constructor. Instantiates and sets up the
    //! presenter object (DriverProfilesMapPresenter) of the table view for the
    //! driver profiles assigned to the current agent profile. Follows the concept
    //! of Qt's Model/View concept, in particular for implementing tables.
    //-----------------------------------------------------------------------------
    void setupDriverProfilesMap();

private:
    friend class AgentConfigurationPresenter;

private:
    //-----------------------------------------------------------------------------
    //! Method which scans the vehicle profiles map (VehicleProfileMapInterface)
    //! for the first entry which is not already assigned to the currently edited
    //! agent profile yet. This method is used in addVehicleProfile for the default
    //! selection after a vehicle profile has been added. Returns an empty name, if
    //! no unused vehicle profile could be found.
    //!
    //! @return     the first vehicle profile being not assigned to the current
    //!             agent profile
    //-----------------------------------------------------------------------------
    AgentItemInterface::VehicleProfile findUnusedVehicleProfile();

    //-----------------------------------------------------------------------------
    //! Method which scans the driver profiles map (DriverProfileMapInterface)
    //! for the first entry which is not already assigned to the currently edited
    //! agent profile yet. This method is used in addDriverProfile for the default
    //! selection after a driver profile has been added. Returns an empty name, if
    //! no unused driver profile could be found.
    //!
    //! @return     the first driver profile being not assigned to the current
    //!             agent profile
    //-----------------------------------------------------------------------------
    AgentItemInterface::DriverProfile findUnusedDriverProfile();

private:
    AgentMapInterface *const agents;                   //!< the pointer to the map of agent profiles (edited by this presenter)
    VehicleProfileMapInterface *const vehicleProfiles; //!< the pointer to the vehicle profiles' map
    DriverProfileMapInterface *const driverProfiles;   //!< the pointer to the driver profiles' map
    AgentItemInterface *currentAgent;                  //!< a pointer to the agent profile currently edited

private:
    AgentMapView *const agentsView;                  //!< the pointer to the view handled by this presenter, i.e. the view of the agent profiles' map
    VehicleProfilesMapPresenter *vehicleProfilesMap; //!< the pointer to the presenter object of the table for assigning vehicle profiles to the current agent profile
    DriverProfilesMapPresenter *driverProfilesMap;   //!< the pointer to the presenter object of the table for assigning driver profiles to the current agent profile

private:
    ProjectInterface *const project; //!< the pointer to the project interface for accessing the path settings
    QUndoStack *const undoStack;     //!< the pointer to the undo stack

private:
    QList<QString> *vehicleProfilesList; //!< a pointer to a list of vehicle profiles containing all profiles registered in VehicleProfileMapInterface
    QList<QString> *driverProfilesList;  //!< a pointer to a list of driver profiles containing all profiles registered in DriverProfileMapInterface
    QList<QString> systemList;           //!< a list of all systems available in the currently selected system config
    QList<QString> modelList;            //!< a list of all vehicle models available in the currently selected vehicle model catalogue
    static double default_probability;   //!< a default probability
};

namespace AgentProfileCommands {

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable set-operations on the
//! currently selected agent profile.
//-----------------------------------------------------------------------------
class SetCurrentAgent : public QUndoCommand
{
public:
    SetCurrentAgent(AgentMapInterface const *const agents,
                    AgentItemInterface **const currentAgent,
                    AgentItemInterface::Name const newAgent,
                    VehicleProfilesMapPresenter *const vehicleProfilesMap,
                    DriverProfilesMapPresenter *const driverProfilesMap,
                    AgentMapPresenter *const agentMap,
                    QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    AgentMapInterface const *const agents;
    AgentItemInterface **const currentAgent;
    AgentItemInterface::Name const newAgent;
    AgentItemInterface::Name const oldAgent;

private:
    AgentMapPresenter *const agentsMap;
    VehicleProfilesMapPresenter *const vehicleProfilesMap;
    DriverProfilesMapPresenter *const driverProfilesMap;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable add-operations on the
//! map of agent profiles (AgentMapInterface).
//-----------------------------------------------------------------------------
class AddAgentProfile : public QUndoCommand
{
public:
    AddAgentProfile(AgentMapInterface *const agents,
                    AgentMapView *const agentsView,
                    AgentMapInterface::Name const &name,
                    QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    AgentMapInterface *const agents;
    AgentMapView *const agentsView;
    AgentMapInterface::Name const name;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable remove-operations on the
//! map of agent profiles (AgentMapInterface).
//-----------------------------------------------------------------------------
class RemoveAgentProfile : public QUndoCommand
{
public:
    RemoveAgentProfile(AgentMapInterface *const agents,
                       AgentMapView *const agentsView,
                       AgentMapInterface::Name const &name,
                       QUndoCommand *parent = nullptr);
    virtual ~RemoveAgentProfile() override;

public:
    void redo() override;
    void undo() override;

private:
    AgentMapInterface *const agents;
    AgentMapView *const agentsView;
    AgentMapInterface::Name const name;
    AgentItemInterface const *const agent;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable renaming-operations on the
//! map of agent profiles (AgentMapInterface).
//-----------------------------------------------------------------------------
class EditAgentProfile : public QUndoCommand
{
public:
    EditAgentProfile(AgentMapInterface *const agents,
                     AgentMapView *const agentsView,
                     AgentItemInterface::Name const &oldName,
                     AgentItemInterface::Name const &newName,
                     QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    AgentMapInterface *const agents;
    AgentMapView *const agentsView;
    AgentItemInterface::Name const oldName;
    AgentItemInterface::Name const newName;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable set-operations on the
//! type (static or dynamic) of an agent profile.
//-----------------------------------------------------------------------------
class EditAgentType : public QUndoCommand
{
public:
    EditAgentType(AgentMapInterface *const agents,
                  AgentItemInterface::Name const &agentName,
                  AgentItemInterface::Type const &newType,
                  AgentMapPresenter *const agentsPresenter,
                  QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    AgentMapInterface *const agents;
    AgentMapInterface::Name const agentName;
    AgentItemInterface::Type const newType;
    AgentItemInterface::Type const oldType;
    AgentMapPresenter *const agentsPresenter;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable set-operations on the
//! system config file of the currently edited agent profile (for static profiles
//! only).
//-----------------------------------------------------------------------------
class EditAgentSystemConfig : public QUndoCommand
{
public:
    EditAgentSystemConfig(AgentMapInterface *const agents,
                          AgentMapView *const agentsView,
                          AgentMapPresenter *const agentsPresenter,
                          AgentItemInterface::Name const &agentName,
                          AgentItemInterface::SystemConfig const &newSystemConfig,
                          QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    AgentMapInterface *const agents;
    AgentMapView *const agentsView;
    AgentMapPresenter *const agentsPresenter;
    AgentItemInterface::Name const agentName;
    AgentItemInterface::SystemConfig const newSystemConfig;
    AgentItemInterface::SystemConfig const oldSystemConfig;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable set-operations on the
//! system of the currently edited agent profile (for static profiles
//! only).
//-----------------------------------------------------------------------------
class EditAgentSystem : public QUndoCommand
{
public:
    EditAgentSystem(AgentMapInterface *const agents,
                    AgentMapView *const agentsView,
                    AgentItemInterface::Name const &agentName,
                    AgentItemInterface::SystemTitle const &newTitle,
                    AgentItemInterface::SystemID const &newId,
                    QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    AgentMapInterface *const agents;
    AgentMapView *const agentsView;
    AgentItemInterface::Name const agentName;
    AgentItemInterface::SystemTitle const newTitle;
    AgentItemInterface::SystemID const newId;
    AgentItemInterface::SystemTitle const oldTitle;
    AgentItemInterface::SystemID const oldId;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable set-operations on the
//! vehicle model catalogue of the currently edited agent profile (for static
//! profiles only).
//-----------------------------------------------------------------------------
class EditAgentModelCatalogue : public QUndoCommand
{
public:
    EditAgentModelCatalogue(AgentMapInterface *const agents,
                            AgentMapView *const agentsView,
                            AgentMapPresenter *const agentsPresenter,
                            AgentItemInterface::Name const &agentName,
                            AgentItemInterface::VehicleModelCatalogue const &newModelCatalogue,
                            QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    AgentMapInterface *const agents;
    AgentMapView *const agentsView;
    AgentMapPresenter *const agentsPresenter;
    AgentItemInterface::Name const agentName;
    AgentItemInterface::VehicleModelCatalogue const newModelCatalogue;
    AgentItemInterface::VehicleModelCatalogue const oldModelCatalogue;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable set-operations on the
//! vehicle model of the currently edited agent profile (for static
//! profiles only).
//-----------------------------------------------------------------------------
class EditAgentModel : public QUndoCommand
{
public:
    EditAgentModel(AgentMapInterface *const agents,
                   AgentMapView *const agentsView,
                   AgentItemInterface::Name const &agentName,
                   AgentItemInterface::VehicleModel const &newModel,
                   QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    AgentMapInterface *const agents;
    AgentMapView *const agentsView;
    AgentItemInterface::Name const agentName;
    AgentItemInterface::VehicleModel const newModel;
    AgentItemInterface::VehicleModel const oldModel;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable add-operations on the
//! driver profiles table of the currently edited agent profile (for dynamic
//! agent profiles only).
//-----------------------------------------------------------------------------
class AddDriverProfile : public QUndoCommand
{
public:
    AddDriverProfile(DriverProfilesMapPresenter *const drivers,
                     AgentItemInterface::DriverProfile const &profile,
                     AgentItemInterface::Probability const &probability,
                     QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    DriverProfilesMapPresenter *const drivers;
    AgentItemInterface::DriverProfile const profile;
    AgentItemInterface::Probability const probability;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable remove-operations on the
//! driver profiles table of the currently edited agent profile (for dynamic
//! agent profiles only).
//-----------------------------------------------------------------------------
class RemoveDriverProfile : public QUndoCommand
{
public:
    RemoveDriverProfile(DriverProfilesMapPresenter *const drivers,
                        int const &row,
                        QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    DriverProfilesMapPresenter *const drivers;
    int const row;
    AgentItemInterface::DriverProfile const profile;
    AgentItemInterface::Probability const probability;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable add-operations on the
//! vehicle profiles table of the currently edited agent profile (for dynamic
//! agent profiles only).
//-----------------------------------------------------------------------------
class AddVehicleProfile : public QUndoCommand
{
public:
    AddVehicleProfile(VehicleProfilesMapPresenter *const vehicles,
                      AgentItemInterface::VehicleProfile const &profile,
                      AgentItemInterface::Probability const &probability,
                      QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    VehicleProfilesMapPresenter *const vehicles;
    AgentItemInterface::VehicleProfile const profile;
    AgentItemInterface::Probability const probability;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable remove-operations on the
//! vehicle profiles table of the currently edited agent profile (for dynamic
//! agent profiles only).
//-----------------------------------------------------------------------------
class RemoveVehicleProfile : public QUndoCommand
{
public:
    RemoveVehicleProfile(VehicleProfilesMapPresenter *const vehicles,
                         int const &row,
                         QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    VehicleProfilesMapPresenter *const vehicles;
    int const row;
    AgentItemInterface::VehicleProfile const profile;
    AgentItemInterface::Probability const probability;
};

} // namespace AgentProfileCommands

#endif // AGENTMAPPRESENTER_H
