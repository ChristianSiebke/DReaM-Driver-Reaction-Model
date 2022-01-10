/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "Presenters/AgentConfigurationPresenter.h"

#include <QFileDialog>

#include "Models/AgentConfigurationModel.h"
#include "Models/AgentConfigurationXMLLoadModel.h"
#include "Models/AgentConfigurationXMLSaveModel.h"

AgentConfigurationPresenter::AgentConfigurationPresenter(AgentConfigurationInterface *const agentConfiguration,
                                                         AgentConfigurationView *const agentConfigurationView,
                                                         SystemInterface *const system,
                                                         ProjectInterface *const project,
                                                         QObject *parent) :
    QObject(parent), agentConfiguration(agentConfiguration), agentConfigurationView(agentConfigurationView), project(project), undoStack(new QUndoStack(this)), redo(new QShortcut(QKeySequence(tr("Ctrl+Y", "redo")), agentConfigurationView)), undo(new QShortcut(QKeySequence(tr("Ctrl+Z", "undo")), agentConfigurationView)), agentsPresenter(new AgentMapPresenter(agentConfiguration->getAgents(), agentConfiguration->getVehicleProfiles(), agentConfiguration->getDriverProfiles(), agentConfigurationView->getAgentsView(), project, undoStack, this)), vehicleProfilesPresenter(new VehicleProfilesPresenter(agentConfiguration->getVehicleProfiles(), agentConfigurationView->getVehicleProfilesView(), system, project, undoStack, this)), driverProfilesPresenter(new DriverProfilesPresenter(agentConfiguration->getDriverProfiles(), agentConfigurationView->getDriverProfilesView(), undoStack, this))
{
    connect(agentConfigurationView, &AgentConfigurationView::SaveSetup, this, &AgentConfigurationPresenter::save);
    connect(agentConfigurationView, &AgentConfigurationView::LoadSetup, this, &AgentConfigurationPresenter::load);
    connect(agentConfigurationView, &AgentConfigurationView::ClearSetup, this, &AgentConfigurationPresenter::clear);

    // configuration of undo view, undo shortcuts and undo buttons
    agentConfigurationView->getUndoView()->setStack(undoStack);
    connect(redo, &QShortcut::activated, undoStack, &QUndoStack::redo);
    connect(undo, &QShortcut::activated, undoStack, &QUndoStack::undo);
    connect(agentConfigurationView, &AgentConfigurationView::Undo, undoStack, &QUndoStack::undo);
    connect(agentConfigurationView, &AgentConfigurationView::Redo, undoStack, &QUndoStack::redo);
}

void AgentConfigurationPresenter::save()
{
    QString const filepath = QFileDialog::getSaveFileName(
        agentConfigurationView, QObject::tr("openPASS / Save setup"),
        project->getConfigPath(), QStringLiteral("ProfilesCatalogue (*.ui)"));

    if (!filepath.isEmpty())
        AgentConfigurationXMLSaveModel::save(filepath, agentConfiguration);
}

void AgentConfigurationPresenter::load()
{
    bool success;
    QString const filepath = QFileDialog::getOpenFileName(
        agentConfigurationView, QObject::tr("openPASS / Load setup"),
        project->getConfigPath(), QStringLiteral("ProfilesCatalogue (*.ui)"));

    AgentConfigurationInterface *loadedXML = new AgentConfigurationModel;
    success = AgentConfigurationXMLLoadModel::load(filepath, loadedXML);

    if (success)
    {
        undoStack->beginMacro(QString("loaded GUI file (*.ui) %1").arg(filepath));

        clearDriverProfiles();
        clearVehicleProfiles();
        clearAgentProfiles();

        copyDriverProfiles(loadedXML);
        copyVehicleProfiles(loadedXML);
        copyAgentProfiles(loadedXML);

        undoStack->endMacro();
    }
}

void AgentConfigurationPresenter::clear()
{
    undoStack->beginMacro("cleared worksheet");

    clearDriverProfiles();
    clearVehicleProfiles();
    clearAgentProfiles();

    undoStack->endMacro();
}

void AgentConfigurationPresenter::clearAgentProfiles()
{
    QList<QString> agentList = agentConfiguration->getAgents()->keys();
    while (!agentList.isEmpty())
    {
        QString name = agentsPresenter->currentAgent->getName();
        agentList.removeAll(name);
        agentsPresenter->removeAgent(name);
    }
}

void AgentConfigurationPresenter::clearDriverProfiles()
{
    QList<QString> driverList = agentConfiguration->getDriverProfiles()->keys();
    while (!driverList.isEmpty())
    {
        QString name = driverProfilesPresenter->currentDriver->getName();
        driverList.removeAll(name);
        driverProfilesPresenter->removeDriver(name);
    }
}

void AgentConfigurationPresenter::clearVehicleProfiles()
{
    QList<QString> vehicleList = agentConfiguration->getVehicleProfiles()->keys();
    while (!vehicleList.isEmpty())
    {
        QString name = vehicleProfilesPresenter->currentVehicleProfile->getName();
        vehicleList.removeAll(name);
        vehicleProfilesPresenter->removeVehicleProfile(name);
    }
}

void AgentConfigurationPresenter::copyAgentProfiles(const AgentConfigurationInterface *const from)
{
    for (auto name : from->getAgents()->keys())
    {
        // 1. add the new agent
        agentsPresenter->addAgent(name);

        // 2. set agent type
        agentsPresenter->setType(from->getAgents()->getItem(name)->getType());

        if (from->getAgents()->getItem(name)->getType() == "Dynamic")
        {
            // 3.a copy all driver profiles
            auto loadedDrivers = from->getAgents()->getItem(name)->getDriverProfiles();
            auto drivers = agentsPresenter->driverProfilesMap;

            for (auto driver : loadedDrivers->keys())
            {
                undoStack->push(new AgentProfileCommands::AddDriverProfile(drivers, driver, loadedDrivers->value(driver)));
            }

            // 4.a copy all vehicle profiles
            auto loadedVehicles = from->getAgents()->getItem(name)->getVehicleProfiles();
            auto vehicles = agentsPresenter->vehicleProfilesMap;

            for (auto vehicle : loadedVehicles->keys())
            {
                undoStack->push(new AgentProfileCommands::AddVehicleProfile(vehicles, vehicle, loadedVehicles->value(vehicle)));
            }
        }
        else if (from->getAgents()->getItem(name)->getType() == "Static")
        {
            // 3.b set system config and system
            undoStack->push(new AgentProfileCommands::EditAgentSystemConfig(agentConfiguration->getAgents(),
                                                                            agentConfigurationView->getAgentsView(),
                                                                            agentsPresenter,
                                                                            name,
                                                                            from->getAgents()->getItem(name)->getSystemConfig()));

            undoStack->push(new AgentProfileCommands::EditAgentSystem(agentConfiguration->getAgents(),
                                                                      agentConfigurationView->getAgentsView(), name,
                                                                      from->getAgents()->getItem(name)->getSystemTitle(),
                                                                      from->getAgents()->getItem(name)->getSystemID()));

            // 4.b. set model catalogue and model
            undoStack->push(new AgentProfileCommands::EditAgentModelCatalogue(agentConfiguration->getAgents(),
                                                                              agentConfigurationView->getAgentsView(),
                                                                              agentsPresenter,
                                                                              name,
                                                                              from->getAgents()->getItem(name)->getVehicleModelCatalogue()));
            undoStack->push(new AgentProfileCommands::EditAgentModel(agentConfiguration->getAgents(),
                                                                     agentConfigurationView->getAgentsView(),
                                                                     name,
                                                                     from->getAgents()->getItem(name)->getVehicleModel()));
        }
    }
}

void AgentConfigurationPresenter::copyDriverProfiles(const AgentConfigurationInterface *const from)
{
    for (auto name : from->getDriverProfiles()->keys())
    {
        undoStack->push(new DriverProfileCommands::AddDriverProfile(agentConfiguration->getDriverProfiles(),
                                                                    agentConfigurationView->getDriverProfilesView(),
                                                                    name,
                                                                    from->getDriverProfiles()->getItem(name)->getParameters()));

        undoStack->push(new DriverProfileCommands::SetCurrentDriver(agentConfiguration->getDriverProfiles(),
                                                                    &(driverProfilesPresenter->currentDriver),
                                                                    name,
                                                                    driverProfilesPresenter->parameters,
                                                                    driverProfilesPresenter));
    }
}

void AgentConfigurationPresenter::copyVehicleProfiles(const AgentConfigurationInterface *const from)
{
    for (auto name : from->getVehicleProfiles()->keys())
    {
        // 1. add the vehicle profile
        vehicleProfilesPresenter->addVehicleProfile(name);

        // 2. adopt model catalouge and model
        undoStack->push(new VehicleProfileCommands::SetModelCatalogue(agentConfiguration->getVehicleProfiles(),
                                                                      agentConfigurationView->getVehicleProfilesView(),
                                                                      name,
                                                                      from->getVehicleProfiles()->getItem(name)->getModelCatalogue(),
                                                                      vehicleProfilesPresenter));
        vehicleProfilesPresenter->setModel(from->getVehicleProfiles()->getItem(name)->getModel());

        // 3. adopt system config, system profiles and systems
        undoStack->push(new VehicleProfileCommands::SetSystemConfig(agentConfiguration->getVehicleProfiles(),
                                                                    agentConfigurationView->getVehicleProfilesView(),
                                                                    name,
                                                                    from->getVehicleProfiles()->getItem(name)->getSystemConfig(),
                                                                    vehicleProfilesPresenter));

        for (auto type : from->getVehicleProfiles()->getItem(name)->getSystemProfiles()->keys())
        {
            vehicleProfilesPresenter->addSystemProfile(type);

            for (auto system : from->getVehicleProfiles()->getItem(name)->getSystems(type)->keys())
                undoStack->push(new VehicleProfileCommands::AddSystem(vehicleProfilesPresenter->systemProfilePresenters.value(name)->value(type),
                                                                      system, from->getVehicleProfiles()->getItem(name)->getSystems(type)->value(system)));
        }
    }
}
