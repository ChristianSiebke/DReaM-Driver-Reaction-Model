/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

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
    void setCurrentVehicleProfile(QString const &name);

private Q_SLOTS:
    void addVehicleProfile(QString const & name);
    void removeVehicleProfile(QString const &name);
    void editVehicleProfileName(QString const &oldName, QString const &newName);

private Q_SLOTS:
    void setSystemConfig();
    void addSystemProfile(QString const &type);
    void removeSystemProfile(QString const &type);

private Q_SLOTS:
    void addSystem(QString  const &type , QString const & selectionItem);
    void removeSystem( QString  const &type, int const &row );

private Q_SLOTS:
    void setModelCatalogue();
    void setModel(QString const &modelName);

private:
    void updateSystemList();
    bool systemsConsistent();
    void updateSystemsView();

public Q_SLOTS:
    void loadModelCatalogue(QString const &filepath);
    void refreshModelCatalogue();
    void refreshSystemConfig();

public:
    void updateView();
    void updateVehicleSelectionItems();

private:
    friend class AgentConfigurationPresenter;

public:
    QMap <VehicleProfileItemInterface::Name , QMap<QString, SystemProfilePresenter*>*> * getSystemProfilePresenters();

private:
    VehicleProfileItemInterface * currentVehicleProfile;
    VehicleProfileMapInterface * const vehicleProfiles;
    VehicleProfilesView * const vehicleProfilesView;
    SystemInterface * const systemInterface;
    ProjectInterface * const project;
    QUndoStack * const undoStack;

private:
    QMap <VehicleProfileItemInterface::Name , QMap<QString, SystemProfilePresenter*>*> systemProfilePresenters;
    QMap <QString, QList<QString> > systemList;
    QList<QString> modelList;
    static double default_probability;
};

namespace VehicleProfileCommands {

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
