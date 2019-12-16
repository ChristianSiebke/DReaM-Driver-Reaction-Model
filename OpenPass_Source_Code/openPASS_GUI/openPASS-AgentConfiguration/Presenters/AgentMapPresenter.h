/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef AGENTMAPPRESENTER_H
#define AGENTMAPPRESENTER_H

#include "openPASS-AgentConfiguration/AgentMapInterface.h"
#include "openPASS-AgentConfiguration/VehicleProfileMapInterface.h"
#include "openPASS-AgentConfiguration/DriverProfileMapInterface.h"
#include "openPASS-Project/ProjectInterface.h"

#include "Presenters/VehicleProfilesMapPresenter.h"
#include "Presenters/DriverProfilesMapPresenter.h"

#include "Views/AgentMapView.h"

#include <QObject>
#include <QUndoStack>

class AgentMapPresenter : public QObject
{
    Q_OBJECT
public:
    explicit AgentMapPresenter(AgentMapInterface * agents,
                               VehicleProfileMapInterface *vehicleProfiles,
                               DriverProfileMapInterface *driverProfiles,
                               AgentMapView * agentsView,
                               ProjectInterface * const project,
                               QUndoStack * const undoStack,
                               QObject *parent = nullptr);
    virtual ~AgentMapPresenter() = default;


private Q_SLOTS:
    void setCurrentAgent(const QString &name);
    void setType(const QString &type);
    void addAgent(QString const & name);
    void removeAgent(QString const & name);
    void editAgent(QString const & oldName, QString const & newName);

private Q_SLOTS:
    void addVehicleProfile();
    void removeVehicleProfile( const int &row);
    void updateVehicleSelectionItems();
    void checkVehicleProbabilities();

private Q_SLOTS:
    void addDriverProfile();
    void removeDriverProfile(const int &row);
    void updateDriverSelectionItems();
    void checkDriverProbabilities();

public Q_SLOTS:
    void setSystemConfig();
    void refreshSystemConfig();
    void setSystemItems(QString const &systemConfig);
    void setSystem(QString const &system);

public Q_SLOTS:
    void setModelCatalogue();
    void refreshModelCatalogue();
    void setModelItems(QString const &modelCatalogue);
    void setModel(QString const &model);

public:
    void updateView();
    void updateAgentSelectionItems();

private:
    void setupVehicleProfilesMap();
    void setupDriverProfilesMap();

private:
    friend class AgentConfigurationPresenter;

private:
    AgentItemInterface::VehicleProfile findUnusedVehicleProfile();
    AgentItemInterface::DriverProfile findUnusedDriverProfile();

private:
    AgentMapInterface * const agents;
    VehicleProfileMapInterface * const vehicleProfiles;
    DriverProfileMapInterface * const driverProfiles;
    AgentItemInterface * currentAgent;

private:
    AgentMapView * const agentsView;
    VehicleProfilesMapPresenter * vehicleProfilesMap;
    DriverProfilesMapPresenter * driverProfilesMap;

private:
    ProjectInterface * const project;
    QUndoStack * const undoStack;

private:
    QList<QString> *vehicleProfilesList;
    QList<QString> *driverProfilesList;
    QList<QString> systemList;
    QList<QString> modelList;
    static double default_probability;

};

namespace AgentProfileCommands {

class SetCurrentAgent : public QUndoCommand
{

public:
    SetCurrentAgent(AgentMapInterface const * const agents,
                    AgentItemInterface ** const currentAgent,
                    AgentItemInterface::Name const newAgent,
                    VehicleProfilesMapPresenter * const vehicleProfilesMap,
                    DriverProfilesMapPresenter * const driverProfilesMap,
                    AgentMapPresenter * const agentMap,
                    QUndoCommand * parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    AgentMapInterface const * const agents;
    AgentItemInterface ** const currentAgent;
    AgentItemInterface::Name const newAgent;
    AgentItemInterface::Name const oldAgent;

private:
    AgentMapPresenter * const agentsMap;
    VehicleProfilesMapPresenter * const vehicleProfilesMap;
    DriverProfilesMapPresenter * const driverProfilesMap;

};

class AddAgentProfile : public QUndoCommand
{

public:
    AddAgentProfile(AgentMapInterface * const agents,
                    AgentMapView * const agentsView,
                    AgentMapInterface::Name const &name,
                    QUndoCommand * parent = nullptr);
public:
    void redo() override;
    void undo() override;

private:
    AgentMapInterface * const agents;
    AgentMapView * const agentsView;
    AgentMapInterface::Name const name;
};

class RemoveAgentProfile : public QUndoCommand
{

public:
    RemoveAgentProfile(AgentMapInterface * const agents,
                       AgentMapView * const agentsView,
                       AgentMapInterface::Name const &name,
                       QUndoCommand * parent = nullptr);
    virtual ~RemoveAgentProfile() override;

public:
    void redo() override;
    void undo() override;

private:
    AgentMapInterface * const agents;
    AgentMapView * const agentsView;
    AgentMapInterface::Name const name;
    AgentItemInterface const * const agent;
};

class EditAgentProfile : public QUndoCommand
{

public:
    EditAgentProfile(AgentMapInterface * const agents,
                     AgentMapView * const agentsView,
                     AgentItemInterface::Name const &oldName,
                     AgentItemInterface::Name const &newName,
                     QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    AgentMapInterface * const agents;
    AgentMapView * const agentsView;
    AgentItemInterface::Name const oldName;
    AgentItemInterface::Name const newName;
};

class EditAgentType : public QUndoCommand
{
public:
    EditAgentType(AgentMapInterface * const agents,
                  AgentItemInterface::Name const &agentName,
                  AgentItemInterface::Type const &newType,
                  AgentMapPresenter * const agentsPresenter,
                  QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    AgentMapInterface * const agents;
    AgentMapInterface::Name const agentName;
    AgentItemInterface::Type const newType;
    AgentItemInterface::Type const oldType;
    AgentMapPresenter * const agentsPresenter;
};

class EditAgentSystemConfig : public QUndoCommand
{

public:
    EditAgentSystemConfig(AgentMapInterface * const agents,
                          AgentMapView * const agentsView,
                          AgentMapPresenter * const agentsPresenter,
                          AgentItemInterface::Name const &agentName,
                          AgentItemInterface::SystemConfig const &newSystemConfig,
                          QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    AgentMapInterface * const agents;
    AgentMapView * const agentsView;
    AgentMapPresenter * const agentsPresenter;
    AgentItemInterface::Name const agentName;
    AgentItemInterface::SystemConfig const newSystemConfig;
    AgentItemInterface::SystemConfig const oldSystemConfig;
};

class EditAgentSystem: public QUndoCommand
{

public:
    EditAgentSystem(AgentMapInterface * const agents,
                    AgentMapView * const agentsView,
                    AgentItemInterface::Name const &agentName,
                    AgentItemInterface::SystemTitle const &newTitle,
                    AgentItemInterface::SystemID const &newId,
                    QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    AgentMapInterface * const agents;
    AgentMapView * const agentsView;
    AgentItemInterface::Name const agentName;
    AgentItemInterface::SystemTitle const newTitle;
    AgentItemInterface::SystemID const newId;
    AgentItemInterface::SystemTitle const oldTitle;
    AgentItemInterface::SystemID const oldId;

};

class EditAgentModelCatalogue : public QUndoCommand
{
public:
    EditAgentModelCatalogue(AgentMapInterface * const agents,
                            AgentMapView * const agentsView,
                            AgentMapPresenter * const agentsPresenter,
                            AgentItemInterface::Name const &agentName,
                            AgentItemInterface::VehicleModelCatalogue const &newModelCatalogue,
                            QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    AgentMapInterface * const agents;
    AgentMapView * const agentsView;
    AgentMapPresenter * const agentsPresenter;
    AgentItemInterface::Name const agentName;
    AgentItemInterface::VehicleModelCatalogue const newModelCatalogue;
    AgentItemInterface::VehicleModelCatalogue const oldModelCatalogue;
};

class EditAgentModel : public QUndoCommand
{
public:
    EditAgentModel(AgentMapInterface * const agents,
                   AgentMapView * const agentsView,
                   AgentItemInterface::Name const &agentName,
                   AgentItemInterface::VehicleModel const &newModel,
                   QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    AgentMapInterface * const agents;
    AgentMapView * const agentsView;
    AgentItemInterface::Name const agentName;
    AgentItemInterface::VehicleModel const newModel;
    AgentItemInterface::VehicleModel const oldModel;
};

class AddDriverProfile : public QUndoCommand
{

public:
    AddDriverProfile(DriverProfilesMapPresenter * const drivers,
                     AgentItemInterface::DriverProfile const &profile,
                     AgentItemInterface::Probability const &probability,
                     QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    DriverProfilesMapPresenter * const drivers;
    AgentItemInterface::DriverProfile const profile;
    AgentItemInterface::Probability const probability;

};

class RemoveDriverProfile : public QUndoCommand
{

public:
    RemoveDriverProfile(DriverProfilesMapPresenter * const drivers,
                        int const &row,
                        QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    DriverProfilesMapPresenter * const drivers;
    int const row;
    AgentItemInterface::DriverProfile const profile;
    AgentItemInterface::Probability const probability;

};

class AddVehicleProfile : public QUndoCommand
{

public:
    AddVehicleProfile(VehicleProfilesMapPresenter * const vehicles,
                      AgentItemInterface::VehicleProfile const &profile,
                      AgentItemInterface::Probability const &probability,
                      QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    VehicleProfilesMapPresenter * const vehicles;
    AgentItemInterface::VehicleProfile const profile;
    AgentItemInterface::Probability const probability;

};

class RemoveVehicleProfile : public QUndoCommand
{

public:
    RemoveVehicleProfile(VehicleProfilesMapPresenter * const vehicles,
                         int const &row,
                         QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    VehicleProfilesMapPresenter * const vehicles;
    int const row;
    AgentItemInterface::VehicleProfile const profile;
    AgentItemInterface::Probability const probability;

};

}

#endif // AGENTMAPPRESENTER_H
