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
//! @file  VehicleProfilesPresenter.h
//! @ingroup agentConfigurationPlugin
//! @brief This file contains the class VehicleProfilesPresenter defining the
//!        presenter object for vehicle profiles as well as the classes for all
//!        undo-command objects used by VehicleProfilesPresenter.
//!
//!        The VehicleProfilesPresenter class handles user events from the
//!        view (VehicleProfilesView) and edits the model data (VehicleProfileMapInterface)
//!        accordingly. Since all actions performed on the model data (i.e. the
//!        *.ui file) should be undoable, the presenter instantiates the
//!        corresponding undo-commands and pushes them onto the undo stack.
//!        The actual operation on the model is then performed in these command
//!        object using the re-implementations of the undo/redo methods. See
//!        Qt's undo framework for further details.
//-----------------------------------------------------------------------------

#ifndef VEHICLEPROFILEMAPPRESENTER_H
#define VEHICLEPROFILEMAPPRESENTER_H

#include "openPASS-System/SystemInterface.h"
#include "openPASS-AgentConfiguration/VehicleProfileMapInterface.h"
#include "openPASS-AgentConfiguration/SensorProfileMapInterface.h"
#include "openPASS-AgentConfiguration/VehicleComponentProfileMapInterface.h"
#include "openPASS-Project/ProjectInterface.h"

#include "Views/VehicleProfilesView.h"

#include <QObject>
#include <QMap>
#include <QList>
#include <QUndoStack>

class SystemProfilePresenter;


//-----------------------------------------------------------------------------
//! @brief This class defines the presenter object for vehicle profiles which
//!        handles user events from the view (VehicleProfilesView) and edits
//!        the model data (VehicleProfileMapInterface) accordingly.
//!
//!        Since all actions performed on the model data (i.e. the
//!        *.ui file) should be undoable, the presenter instantiates the
//!        corresponding undo-commands and pushes them onto the undo stack.
//!        The actual operation on the model is then performed in these command
//!        object using the re-implementations of the undo/redo methods. See
//!        Qt's undo framework for further details.
//-----------------------------------------------------------------------------
class VehicleProfilesPresenter : public QObject
{
    Q_OBJECT

public:
    explicit VehicleProfilesPresenter(VehicleProfileMapInterface * vehicleProfiles,
                                      VehicleProfilesView * vehicleProfilesView,
                                      SystemInterface *systemInterface,
                                      ProjectInterface * const project,
                                      QUndoStack * const undoStack,
                                      QObject *parent = nullptr);

    ~VehicleProfilesPresenter();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot triggered by the VehicleProfilesView::selectedVehicleProfile signal.
    //! Performs an undoable set-operation on the currently edited vehicle profile
    //! setting it to the transmitted profile. Updates the view accordingly.
    //!
    //! @param[in]      name        the vehicle profile which should be currently
    //!                             edited
    //-----------------------------------------------------------------------------
    void setCurrentVehicleProfile(QString const &name);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot triggered by the VehicleProfilesView::addVehicleProfile signal. Performs an
    //! undoable add-operation on the vehicle profiles map. Sets the newly created vehicle
    //! profile as the currently edited profile and updates the view accordingly.
    //!
    //! @param[in]      name        the name of the new driver profile
    //-----------------------------------------------------------------------------
    void addVehicleProfile(QString const & name);

    //-----------------------------------------------------------------------------
    //! Slot triggered by the VehicleProfilesView::removeVehicleProfile signal. Performs an
    //! undoable remove-operation on the vehicle profiles map removing the currently
    //! selected profile. If the list of available profiles is not empty, it
    //! sets the currently edited profile to the first entry of the list.
    //!
    //! @param[in]      name        the name of the vehicle profile to be removed
    //-----------------------------------------------------------------------------
    void removeVehicleProfile(QString const &name);

    //-----------------------------------------------------------------------------
    //! Slot triggered by the VehicleProfilesView::modifiedName signal. Performs an
    //! undoable rename-operation on the name of the currently edited vehicle profile.
    //!
    //! @param[in]      oldName        the old name of the vehicle profile
    //! @param[in]      newName        the new name of the vehicle profile
    //-----------------------------------------------------------------------------
    void editVehicleProfileName(QString const &oldName, QString const &newName);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot triggered by the VehicleProfilesView::changeSystemConfig signal. Opens a
    //! file dialog for the user to select a system config XML file. After a non-empty
    //! selection has been made, an undoable set-operation on the system config of the
    //! currently edited vehicle profile ist performed. The system config is thereby
    //! set to the one selected by the user and the view is updated accordingly.
    //-----------------------------------------------------------------------------
    void setSystemConfig();

    //-----------------------------------------------------------------------------
    //! Slot, triggered by the VehicleProfilesView::addSystemProfile signal. Undoably
    //! adds a map for systems of the given algorithm type and creates a corresponding
    //! table view and presenter object.
    //!
    //! @note   The aforementioned map of systems is called system profile in this
    //!         implementation.
    //!
    //! @param[in]      type        the algorithm type for the map of systems to be
    //!                             created
    //-----------------------------------------------------------------------------
    void addSystemProfile(QString const &type);

    //-----------------------------------------------------------------------------
    //! Slot, triggered by the VehicleProfilesView::removeSystemProfile signal. Undoably
    //! removes the map for systems of the given algorithm type, destroys the
    //! corresponding table view and deregisters the corresponding presenter object.
    //!
    //! @note   The aforementioned map of systems is called system profile in this
    //!         implementation.
    //!
    //! @param[in]      type        the algorithm type for the map of systems to be
    //!                             removed
    //-----------------------------------------------------------------------------
    void removeSystemProfile(QString const &type);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot, triggered by the SystemProfileView::addSystem signal. Undoably
    //! adds the given system selection item to the systems map of the given algorithm
    //! type.
    //!
    //! @param[in]      type            the algorithm type specifying the map of systems
    //!                                 to which a system is to be added
    //! @param[in]      selectionItem   the item of the system combo box that has been
    //!                                 selected for adding
    //-----------------------------------------------------------------------------
    void addSystem(QString  const &type , QString const & selectionItem);

    //-----------------------------------------------------------------------------
    //! Slot, triggered by the SystemProfileView::removeSystem signal. Undoably
    //! removes the specified row from the systems table (map) of the given algorithm
    //! type.
    //!
    //! @param[in]      type            the algorithm type specifying the map of systems
    //!                                 from which a system is to be removed
    //! @param[in]      row             the row index of the system to be removed
    //-----------------------------------------------------------------------------
    void removeSystem( QString  const &type, int const &row );

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot triggered by the VehicleProfilesView::changeModelCatalogue signal.
    //! Opens a file dialog for the user to select a vehicle model catalogue file.
    //! After a non-empty selection has been made, an undoable set-operation for the
    //! model catalogue of the currently edited vehicle profile ist performed.
    //! The model catalogue is thereby set to the one selected by the user and the view
    //! is updated accordingly.
    //-----------------------------------------------------------------------------
    void setModelCatalogue();

    //-----------------------------------------------------------------------------
    //! Slot triggered by the VehicleProfilesView::modifiedModel signal. Performs an
    //! undoable assignment of the given vehicle model to the currently edited
    //! vehicle profile.
    //!
    //! @param[in]      model        the vehicle model to be assigned
    //-----------------------------------------------------------------------------
    void setModel(QString const &modelName);

private:
    //-----------------------------------------------------------------------------
    //! Method which updates the list of all available systems in the currently
    //! selected system config, sorted by the algorithm types.
    //-----------------------------------------------------------------------------
    void updateSystemList();

    //-----------------------------------------------------------------------------
    //! Method which checks whether all assigned systems are consistent with the
    //! currently selected system config file.
    //!
    //! @return             true, if systems are consistent with system config,
    //!                     false, otherwise
    //-----------------------------------------------------------------------------
    bool systemsConsistent();

    //-----------------------------------------------------------------------------
    //! Method which completely re-builds the view of all system tables which are
    //! included into the currently edited vehicle profile. Is called by the
    //! updateView method.
    //-----------------------------------------------------------------------------
    void updateSystemsView();

public Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Method which scans the vehicle model catalogue specified by the given
    //! filepath and stores the vehicle model names defined therein in #modelList.
    //!
    //! @param[in]      filepath        the file path of the vehicle model catalogue
    //!                                 to be scanned for models
    //-----------------------------------------------------------------------------
    void loadModelCatalogue(QString const &filepath);

    //-----------------------------------------------------------------------------
    //! Slot triggered by the VehicleProfilesView::refreshModelCatalogue and
    //! ProjectInterface::update signals. Re-loads the currently selected vehicle
    //! model catalogue and updates the error messages accordingly.
    //-----------------------------------------------------------------------------
    void refreshModelCatalogue();

    //-----------------------------------------------------------------------------
    //! Slot triggered by the VehicleProfilesView::refreshSystemConfig and
    //! ProjectInterface::update signals. Updates the system list of the currently
    //! selected system config file and upates the error messages accordingly.
    //-----------------------------------------------------------------------------
    void refreshSystemConfig();

public:
    //-----------------------------------------------------------------------------
    //! Method which completely re-builds the view according to the model data
    //! of the currently selected vehicle profile.
    //-----------------------------------------------------------------------------
    void updateView();

    //-----------------------------------------------------------------------------
    //! Method which updates the combo box list of available vehicle profiles to the
    //! ones currently defined in the model data (VehicleProfileMapInterface).
    //-----------------------------------------------------------------------------
    void updateVehicleSelectionItems();

private:
    friend class AgentConfigurationPresenter;

private:
    VehicleProfileItemInterface * currentVehicleProfile;            //!< a pointer to the vehicle profile to be currently edited
    VehicleProfileMapInterface * const vehicleProfiles;             //!< the pointer to the map of vehicle profiles
    VehicleProfilesView * const vehicleProfilesView;                //!< the pointer to the view displaying the currently edited profile
    SystemInterface * const systemInterface;                        //!< a pointer to the system interface of the System editor. Serves as a container into which the system config can be loaded.
    ProjectInterface * const project;                               //!< a pointer to the project interface for accessing the config file structure
    QUndoStack * const undoStack;                                   //!< a pointer to the undo stack

private:
    QMap <VehicleProfileItemInterface::Name , QMap<QString, SystemProfilePresenter*>*> systemProfilePresenters; //!< a container for each vehicle profile that stores all table presenter objects for systems of a certain algorithm type
    QMap <QString, QList<QString> > systemList;                     //!< the list of all systems in the current system config sorted by the algorithm type
    QList<QString> modelList;                                       //!< the list of all models in the current vehicle model catalogue
    static double default_probability;                              //!< a default probability
};

namespace VehicleProfileCommands {

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable set-operations on the
//! currently selected vehicle profile.
//-----------------------------------------------------------------------------
class SetCurrentVehicle : public QUndoCommand
{
public:
    SetCurrentVehicle(VehicleProfileMapInterface const * const vehicles,
                      VehicleProfileItemInterface ** const currentVehicle,
                      VehicleProfileItemInterface::Name const newVehicle,
                      VehicleProfilesPresenter * const vehiclesPresenter,
                      QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    VehicleProfileMapInterface const * const vehicles;
    VehicleProfileItemInterface ** const currentVehicle;
    VehicleProfileItemInterface::Name const newVehicle;
    VehicleProfileItemInterface::Name const oldVehicle;
    VehicleProfilesPresenter  * const vehiclesPresenter;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable add-operations on the
//! map of vehicle profiles (VehicleProfileMapInterface).
//-----------------------------------------------------------------------------
class AddVehicleProfile : public QUndoCommand
{

public:
    AddVehicleProfile(VehicleProfileMapInterface * const vehicles,
                      VehicleProfilesView * const vehiclesView,
                      VehicleProfileMapInterface::Name const &name,
                      QMap <VehicleProfileItemInterface::Name , QMap<QString, SystemProfilePresenter*> * > * const systemProfilePresenters,
                      QUndoCommand * parent = nullptr);

    ~AddVehicleProfile() override;
public:
    void redo() override;
    void undo() override;

private:
    VehicleProfileMapInterface * const vehicles;
    VehicleProfilesView * const vehiclesView;
    VehicleProfileMapInterface::Name const name;
    QMap <VehicleProfileItemInterface::Name , QMap<QString, SystemProfilePresenter*> * > * const systemProfilePresenters;
    QMap<QString, SystemProfilePresenter*> * addedSystemProfiles;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable remove-operations on the
//! map of vehicle profiles (VehicleProfileMapInterface).
//-----------------------------------------------------------------------------
class RemoveVehicleProfile : public QUndoCommand
{

public:
    RemoveVehicleProfile(VehicleProfileMapInterface * const vehicles,
                         VehicleProfilesView * const vehiclesView,
                         VehicleProfileMapInterface::Name const &name,
                         QMap <VehicleProfileItemInterface::Name , QMap<QString, SystemProfilePresenter*> * > * const systemProfilePresenters,
                         QUndoCommand * parent = nullptr);
    virtual ~RemoveVehicleProfile() override;

public:
    void redo() override;
    void undo() override;

private:
    VehicleProfileMapInterface * const vehicles;
    VehicleProfilesView * const vehiclesView;
    VehicleProfileMapInterface::Name const name;
    VehicleProfileMapInterface::Item const * const vehicle;
    QMap <VehicleProfileItemInterface::Name , QMap<QString, SystemProfilePresenter*>*> * const systemProfilePresenters;
    QMap<QString, SystemProfilePresenter*> * removedSystemProfiles;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable renaming-operations on the
//! map of vehicle profiles (VehicleProfileMapInterface).
//-----------------------------------------------------------------------------
class EditVehicleProfile : public QUndoCommand
{

public:
    EditVehicleProfile(VehicleProfileMapInterface * const vehicles,
                       VehicleProfilesView * const vehiclesView,
                       VehicleProfileMapInterface::Name const &oldName,
                       VehicleProfileMapInterface::Name const &newName,
                       QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    VehicleProfileMapInterface * const vehicles;
    VehicleProfilesView * const vehiclesView;
    VehicleProfileMapInterface::Name const oldName;
    VehicleProfileMapInterface::Name const newName;

};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable set-operations on the
//! vehicle model catalogue of the currently edited vehicle profile.
//-----------------------------------------------------------------------------
class SetModelCatalogue : public QUndoCommand
{

public:
    SetModelCatalogue(VehicleProfileMapInterface * const vehicles,
                      VehicleProfilesView * const vehicleProfilesView,
                      VehicleProfileItemInterface::Name const &vehicleName,
                      VehicleProfileItemInterface::ModelCatalogue const &newModelCatalogue,
                      VehicleProfilesPresenter * const vehicleProfilesPresenter,
                      QUndoCommand *parent = nullptr);
public:
    void redo() override;
    void undo() override;

private:
    VehicleProfileMapInterface * const vehicles;
    VehicleProfilesView * const vehicleProfilesView;
    VehicleProfileItemInterface::Name const vehicleName;
    VehicleProfileItemInterface::ModelCatalogue const newModelCatalogue;
    VehicleProfileItemInterface::ModelCatalogue const oldModelCatalogue;
    VehicleProfilesPresenter * const vehicleProfilesPresenter;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable set-operations on the
//! vehicle model of the currently edited vehicle profile.
//-----------------------------------------------------------------------------
class SetModel : public QUndoCommand
{

public:
    SetModel(VehicleProfileMapInterface * const vehicles,
             VehicleProfilesView * const vehicleProfilesView,
             VehicleProfileItemInterface::Name const &vehicleName,
             VehicleProfileItemInterface::Model const &newModel,
             QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    VehicleProfileMapInterface * const vehicles;
    VehicleProfilesView * const vehicleProfilesView;
    VehicleProfileItemInterface::Name const vehicleName;
    VehicleProfileItemInterface::Model const newModel;
    VehicleProfileItemInterface::Model const oldModel;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable set-operations on the
//! system config of the currently edited vehicle profile.
//-----------------------------------------------------------------------------
class SetSystemConfig : public QUndoCommand
{

public:
    SetSystemConfig(VehicleProfileMapInterface * const vehicles,
                    VehicleProfilesView * const vehicleProfilesView,
                    VehicleProfileItemInterface::Name const &vehicleName,
                    VehicleProfileItemInterface::SystemConfig const &newSystemConfig,
                    VehicleProfilesPresenter * const vehicleProfilesPresenter,
                    QUndoCommand *parent = nullptr);
public:
    void redo() override;
    void undo() override;

private:
    VehicleProfileMapInterface * const vehicles;
    VehicleProfilesView * const vehicleProfilesView;
    VehicleProfileItemInterface::Name const vehicleName;
    VehicleProfileItemInterface::SystemConfig const newSystemConfig;
    VehicleProfileItemInterface::SystemConfig const oldSystemConfig;
    VehicleProfilesPresenter * const vehicleProfilesPresenter;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoably creating a new map for
//! systems of a specific algorithm type to be assigned to the current vehicle
//! profile.
//-----------------------------------------------------------------------------
class AddSystemProfile : public QUndoCommand
{

public:
    AddSystemProfile(VehicleProfileMapInterface * const vehicles,
                     VehicleProfileItemInterface::Name const &vehicleName,
                     QString const &type,
                     QMap<QString, SystemProfilePresenter*> * const systemProfilePresenters,
                     VehicleProfilesPresenter * const vehicleProfilesPresenter,
                     QUndoCommand *parent = nullptr);

    ~AddSystemProfile() override;

public:
    void redo() override;
    void undo() override;

private:
    VehicleProfileMapInterface * const vehicles;
    VehicleProfileItemInterface::Name const vehicleName;
    QString const type;
    VehicleProfilesPresenter * const vehicleProfilesPresenter;
    QMap<QString, SystemProfilePresenter*> * const systemProfilePresenters;
    SystemProfilePresenter * const systemProfilePresenter;
    bool deletable;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoably removing the map for
//! systems of a specific algorithm type assigned to the current vehicle
//! profile.
//-----------------------------------------------------------------------------
class RemoveSystemProfile : public QUndoCommand
{
public:
    RemoveSystemProfile(VehicleProfileMapInterface * const vehicles,
                        VehicleProfileItemInterface::Name const &vehicleName,
                        QString const &type,
                        QMap<QString, SystemProfilePresenter*> * const systemProfilePresenters,
                        VehicleProfilesPresenter * const vehicleProfilesPresenter,
                        QUndoCommand *parent = nullptr);

    ~RemoveSystemProfile() override;

public:
    void redo() override;
    void undo() override;

private:
    VehicleProfileMapInterface * const vehicles;
    VehicleProfileItemInterface::Name const vehicleName;
    QString const type;
    VehicleProfilesPresenter * const vehicleProfilesPresenter;
    QMap<QString, SystemProfilePresenter*> * const systemProfilePresenters;
    SystemProfilePresenter * const systemProfilePresenter;
    bool deletable;
};


//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable add-operations on the
//! map of systems of a specific algorithm type.
//-----------------------------------------------------------------------------
class AddSystem : public QUndoCommand
{

public:
    AddSystem(SystemProfilePresenter * const systems,
              VehicleProfileItemInterface::System const &system,
              VehicleProfileItemInterface::Probability const &probability,
              QUndoCommand * parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    SystemProfilePresenter * const systems;
    VehicleProfileItemInterface::System  const system;
    VehicleProfileItemInterface::Probability const probability;
};

//-----------------------------------------------------------------------------
//! @brief This class defines the command for undoable remove-operations on the
//! map of systems of a specific algorithm type.
//-----------------------------------------------------------------------------
class RemoveSystem : public QUndoCommand
{

public:
    RemoveSystem(SystemProfilePresenter * const systems,
                 int const &row,
                 QUndoCommand * parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    SystemProfilePresenter * const systems;
    int const row;
    VehicleProfileItemInterface::System  const system;
    VehicleProfileItemInterface::Probability const probability;
};

}

#endif // VEHICLEPROFILEMAPPRESENTER_H
