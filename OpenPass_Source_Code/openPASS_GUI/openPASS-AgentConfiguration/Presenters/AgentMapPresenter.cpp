/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "Models/AgentMapModel.h"
#include "Presenters/AgentMapPresenter.h"

#include "DelegateComboBoxView.h"
#include "DelegateDoubleView.h"

#include <QFileDialog>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QFile>

double AgentMapPresenter::default_probability = 0.5;

AgentMapPresenter::AgentMapPresenter(AgentMapInterface *agents,
                                     VehicleProfileMapInterface *vehicleProfiles,
                                     DriverProfileMapInterface *driverProfiles,
                                     AgentMapView *agentsView,
                                     ProjectInterface * const project,
                                     QUndoStack * const undoStack,
                                     QObject *parent)
    : QObject(parent)
    , agents(agents)
    , vehicleProfiles(vehicleProfiles)
    , driverProfiles(driverProfiles)
    , currentAgent(nullptr)
    , agentsView(agentsView)
    , project(project)
    , undoStack(undoStack)
    , vehicleProfilesList(new QList<QString>() )
    , driverProfilesList(new QList<QString>() )
{
    // Connections concerning the Agent Profiles
    connect(agentsView, &AgentMapView::selectionChanged, this, &AgentMapPresenter::setCurrentAgent);
    connect(agentsView,&AgentMapView::add, this, &AgentMapPresenter::addAgent);
    connect(agentsView, &AgentMapView::remove, this, &AgentMapPresenter::removeAgent);
    connect(agentsView, &AgentMapView::modifiedName, this, &AgentMapPresenter::editAgent);
    connect(agentsView,&AgentMapView::typeChanged, this, &AgentMapPresenter::setType);

    // Connections concerning vehicle profiles
    connect(vehicleProfiles, &VehicleProfileMapInterface::added, this, &AgentMapPresenter::updateVehicleSelectionItems);
    connect(vehicleProfiles, &VehicleProfileMapInterface::removed, this, &AgentMapPresenter::updateVehicleSelectionItems);
    connect(vehicleProfiles, &VehicleProfileMapInterface::modifiedName, this, &AgentMapPresenter::updateVehicleSelectionItems);

    // Connections concerning driver profiles
    connect(driverProfiles, &DriverProfileMapInterface::added, this, &AgentMapPresenter::updateDriverSelectionItems);
    connect(driverProfiles, &DriverProfileMapInterface::removed, this, &AgentMapPresenter::updateDriverSelectionItems);
    connect(driverProfiles, &DriverProfileMapInterface::modifiedName, this, &AgentMapPresenter::updateDriverSelectionItems);


    // Connections concerning the system Config
    connect(agentsView, &AgentMapView::setSystemConfig, this, &AgentMapPresenter::setSystemConfig);
    connect(agentsView, &AgentMapView::refreshSystemConfig, this, &AgentMapPresenter::refreshSystemConfig);
    connect(agentsView, &AgentMapView::selectedSystem, this, &AgentMapPresenter::setSystem);

    // connections concerning the model catalogue
    connect(agentsView, &AgentMapView::setModel, this, &AgentMapPresenter::setModel);
    connect(agentsView, &AgentMapView::setModelCatalogue, this, &AgentMapPresenter::setModelCatalogue);
    connect(agentsView, &AgentMapView::refreshModelCatalogue, this, &AgentMapPresenter::refreshModelCatalogue);

    // connections relevant for the communication with project plugin
    connect(project, &ProjectInterface::update, this, &AgentMapPresenter::refreshSystemConfig);
    connect(project, &ProjectInterface::update, this, &AgentMapPresenter::refreshModelCatalogue);

    setupVehicleProfilesMap();
    setupDriverProfilesMap();
}

void AgentMapPresenter::setupVehicleProfilesMap()
{
    vehicleProfilesMap = new VehicleProfilesMapPresenter;

    QTableView * vehicleProfilesMapView = agentsView->getVehicleProfilesView();
    vehicleProfilesMapView->setItemDelegateForColumn(0, new DelegateComboBoxView( vehicleProfilesList,"vehicle profile", undoStack, this));
    vehicleProfilesMapView->setItemDelegateForColumn(1, new DelegateDoubleView(0, 1, 0.1,2, "vehicle profile's probability", undoStack,this));
    vehicleProfilesMapView->setModel(vehicleProfilesMap);

    connect(agentsView, &AgentMapView::addVehicleProfile, this, &AgentMapPresenter::addVehicleProfile);
    connect(agentsView, &AgentMapView::removeVehicleProfile, this, &AgentMapPresenter::removeVehicleProfile);
    connect(vehicleProfilesMap, &VehicleProfilesMapPresenter::checkProbabilities, this, &AgentMapPresenter::checkVehicleProbabilities);
}

void AgentMapPresenter::setupDriverProfilesMap()
{
    driverProfilesMap = new DriverProfilesMapPresenter;

    QTableView * driverProfilesMapView = agentsView->getDriverProfilesView();
    driverProfilesMapView->setItemDelegateForColumn(0, new DelegateComboBoxView( driverProfilesList, "dirver profile", undoStack, this));
    driverProfilesMapView->setItemDelegateForColumn(1, new DelegateDoubleView(0, 1, 0.1,2,"driver profile's probability", undoStack, this));
    driverProfilesMapView->setModel(driverProfilesMap);

    connect(agentsView, &AgentMapView::addDriverProfile, this, &AgentMapPresenter::addDriverProfile);
    connect(agentsView, &AgentMapView::removeDriverProfile, this, &AgentMapPresenter::removeDriverProfile);
    connect(driverProfilesMap, &DriverProfilesMapPresenter::checkProbabilities, this, &AgentMapPresenter::checkDriverProbabilities);
}

void AgentMapPresenter::setCurrentAgent(const QString &name)
{

    if(currentAgent && currentAgent->getName() != name)
        undoStack->push(new AgentProfileCommands::SetCurrentAgent(agents, &currentAgent,name, vehicleProfilesMap, driverProfilesMap, this));
}

void AgentMapPresenter::setType(const QString &type)
{
    undoStack->push(new AgentProfileCommands::EditAgentType(agents, currentAgent->getName(), type, this));
}

void AgentMapPresenter::addAgent(const QString &name)
{
    undoStack->beginMacro(QString("added agent profile %1").arg(name));

    undoStack->push(new AgentProfileCommands::AddAgentProfile(agents, agentsView, name));
    undoStack->push(new AgentProfileCommands::SetCurrentAgent(agents, &currentAgent, name, vehicleProfilesMap, driverProfilesMap,this));

    undoStack->endMacro();
}


void AgentMapPresenter::removeAgent(const QString &name)
{
    if(currentAgent && currentAgent->getName() == name)
    {
        QList<AgentItemInterface::Name> agentList = agents->keys();
        agentList.removeAll(name);
        AgentItemInterface::Name agentName = agentList.isEmpty() ? "" : agentList.first();

        undoStack->beginMacro(QString("removed agent profile %1").arg(name));
        undoStack->push(new AgentProfileCommands::SetCurrentAgent(agents, &currentAgent,agentName , vehicleProfilesMap, driverProfilesMap, this));
        undoStack->push(new AgentProfileCommands::RemoveAgentProfile(agents, agentsView, name));
        undoStack->endMacro();
    }
}


void AgentMapPresenter::editAgent(const QString &oldName, const QString &newName)
{
    if(!agents->keys().contains(newName))
        undoStack->push(new AgentProfileCommands::EditAgentProfile(agents, agentsView, oldName, newName));
}

void AgentMapPresenter::addVehicleProfile()
{
    AgentItemInterface::VehicleProfile profile = findUnusedVehicleProfile();

    if( !vehicleProfilesList->isEmpty() && !profile.isEmpty() )
        undoStack->push(new AgentProfileCommands::AddVehicleProfile(vehicleProfilesMap, profile, default_probability));
}

AgentItemInterface::VehicleProfile AgentMapPresenter::findUnusedVehicleProfile()
{
    for( auto profile : *vehicleProfilesList)
    {
        if ( !currentAgent->getVehicleProfiles()->contains(profile) )
            return profile;
    }

    return AgentItemInterface::VehicleProfile();
}

AgentItemInterface::VehicleProfile AgentMapPresenter::findUnusedDriverProfile()
{
    for( auto profile : *driverProfilesList)
    {
        if ( !currentAgent->getDriverProfiles()->contains(profile) )
            return profile;
    }

    return AgentItemInterface::DriverProfile();
}

void AgentMapPresenter::removeVehicleProfile(const int &row)
{
    if(vehicleProfilesMap->rowCount() !=0)
        undoStack->push(new AgentProfileCommands::RemoveVehicleProfile(vehicleProfilesMap, row));
}

void AgentMapPresenter::updateVehicleSelectionItems()
{
    vehicleProfilesList->clear();
    vehicleProfilesList->append( vehicleProfiles->keys() );
}

void AgentMapPresenter::checkVehicleProbabilities()
{
    if(currentAgent)
    {
        double sum = 0;
        for(auto probability : currentAgent->getVehicleProfiles()->values())
            sum += probability;

        agentsView->setVehicleProbabilityError(sum!=1.0 && !currentAgent->getVehicleProfiles()->isEmpty());
    }
}

void AgentMapPresenter::addDriverProfile()
{
    AgentItemInterface::DriverProfile profile = findUnusedDriverProfile();

    if( !driverProfilesList->isEmpty() && !profile.isEmpty() )
        undoStack->push(new AgentProfileCommands::AddDriverProfile(driverProfilesMap, profile, default_probability));
}

void AgentMapPresenter::removeDriverProfile(const int &row)
{
    if(driverProfilesMap->rowCount() !=0)
        undoStack->push(new AgentProfileCommands::RemoveDriverProfile(driverProfilesMap, row));
}

void AgentMapPresenter::updateDriverSelectionItems()
{
    driverProfilesList->clear();
    driverProfilesList->append( driverProfiles->keys() );
}

void AgentMapPresenter::checkDriverProbabilities()
{
    if(currentAgent)
    {
        double sum = 0;
        for(auto probability : currentAgent->getDriverProfiles()->values())
            sum += probability;

        agentsView->setDriverProbabilityError(sum!=1.0 && !currentAgent->getDriverProfiles()->isEmpty());
    }
}

void AgentMapPresenter::updateView()
{
    // clear the whole view
    agentsView->clear();

    // enable configuration depending on current agent
    agentsView->enableConfiguration( currentAgent != nullptr );

    if(currentAgent)
    {
        agentsView->setCurrentAgent(currentAgent->getName());

        // Set the Type property
        agentsView->setCurrentType(currentAgent->getType());

        if(currentAgent->getType() == "Dynamic")
        {
            vehicleProfilesMap->update();
            updateVehicleSelectionItems();

            driverProfilesMap->update();
            updateDriverSelectionItems();
        }
        if(currentAgent->getType() == "Static")
        {
            // refresh system Config
            refreshSystemConfig();
            agentsView->setCurrentSystemConfig(currentAgent->getSystemConfig());

            // refresh model catalogue & set current model
            refreshModelCatalogue();
            agentsView->setCurrentModelCatalogue(currentAgent->getVehicleModelCatalogue());
        }
    }

}


void AgentMapPresenter::updateAgentSelectionItems()
{
    agentsView->setAgentSelectionItems( agents->keys() );
}

void AgentMapPresenter::setSystem(const QString &system)
{
    // construct the system title
    QString title = system.section(" (ID: ",0,0);

    // construct the system ID
    QString ID = system.section(" (ID: ",1,1);

    // to remove the ")" at the end
    ID.chop(1);

    undoStack->push(new AgentProfileCommands::EditAgentSystem(agents, agentsView, currentAgent->getName(), title, ID.toUInt()));
}

void AgentMapPresenter::setSystemConfig()
{
    QString const filepath = QFileDialog::getOpenFileName(agentsView, QObject::tr("openPASS / Choose system configuration file"),
                                                          project->getConfigPath(), QStringLiteral("systemConfig (*.xml)") );

    AgentItemInterface::SystemConfig systemConfig = project->relativeToConfigPath(filepath);

    if(currentAgent && !filepath.isEmpty())
    {
        AgentItemInterface::SystemConfig oldSystemConfig = currentAgent->getSystemConfig();

        undoStack->beginMacro(QString("edited system config of agent profile %1").arg(currentAgent->getName()));

        undoStack->push(new AgentProfileCommands::EditAgentSystemConfig(agents, agentsView, this, currentAgent->getName(), systemConfig));

        if(systemConfig != oldSystemConfig && !systemList.isEmpty())
            setSystem(systemList.last());

        undoStack->endMacro();
    }
}

void AgentMapPresenter::refreshSystemConfig()
{
    if(currentAgent)
    {
        QString systemConfig = currentAgent->getSystemConfig();
        setSystemItems(systemConfig);

        agentsView->setSystemConfigError("");

        if(systemConfig.isEmpty())
            return;

        if(!QFileInfo::exists(project->absoluteToConfigPath(systemConfig)))
        {
            agentsView->setSystemConfigError("System config not accessible! Check configuration path in Project plugin.");
            agentsView->enableSystemView(false);
            return;
        }

        QString currentSystem = currentAgent->getSystemTitle()
                                + QString(" (ID: %1)").arg(currentAgent->getSystemID());

        if(systemList.contains(currentSystem))
        {
            agentsView->enableSystemView(true);
            agentsView->setCurrentSystem(currentSystem);
        }
        else if(!currentAgent->getSystemTitle().isEmpty()){
            agentsView->setSystemConfigError("System does not exist in system config!");
            agentsView->enableSystemView(false);
        }
    }
}

void AgentMapPresenter::setSystemItems(const QString &systemConfig)
{
    unsigned int id = 0;
    QString title;
    QString path = project->absoluteToConfigPath(systemConfig);
    QFile filepath(path);

    systemList.clear();

    if( filepath.exists() && filepath.open(QIODevice::ReadOnly | QIODevice::Text))
    {

        QXmlStreamReader xml(&filepath);
        xml.readNextStartElement();
        if(xml.name() == "systems")
        {
            while(xml.readNextStartElement())
            {
                if(xml.name() == "system")
                {
                    while(xml.readNextStartElement())
                    {
                        if(xml.name() == "id")
                            id = xml.readElementText().toUInt();
                        else if( xml.name() == "title")
                            title = xml.readElementText();
                        else
                            xml.skipCurrentElement();
                    }
                    systemList << QString("%1 (ID: %2)").arg(title).arg(id);
                }
                else
                    xml.skipCurrentElement();
            }
        }
    }

   agentsView->setSystemSelectionItems(systemList);
}

void AgentMapPresenter::setModel(const QString &model)
{
    if(currentAgent)
        undoStack->push(new AgentProfileCommands::EditAgentModel(agents, agentsView, currentAgent->getName(), model));
}

void AgentMapPresenter::setModelCatalogue()
{
    QString const filepath = QFileDialog::getOpenFileName(
                agentsView, QObject::tr("openPASS / Choose Vehicle Model Catalogue"),
                project->getConfigPath(), QStringLiteral("VehicleModelCatalogue (*.xosc)"));

    AgentItemInterface::VehicleModelCatalogue modelCatalogue = project->relativeToConfigPath(filepath);

    if(currentAgent && !filepath.isEmpty())
    {
        AgentItemInterface::VehicleModelCatalogue oldModelCatalogue = currentAgent->getVehicleModelCatalogue();

        undoStack->beginMacro(QString("edited vehicle model catalogue of agent profile %1").arg(currentAgent->getName()));

        undoStack->push(new AgentProfileCommands::EditAgentModelCatalogue(agents, agentsView, this, currentAgent->getName(), modelCatalogue));

        if(modelCatalogue!=oldModelCatalogue && !modelList.isEmpty())
            undoStack->push(new AgentProfileCommands::EditAgentModel(agents, agentsView, currentAgent->getName(), modelList.last()));

        undoStack->endMacro();
    }

}

void AgentMapPresenter::refreshModelCatalogue()
{
    if(currentAgent)
    {
        QString modelCatalogue = currentAgent->getVehicleModelCatalogue();
        setModelItems(modelCatalogue);
        agentsView->setModelCatalogueError("");

        if(modelCatalogue.isEmpty())
            return;

        if(!QFileInfo::exists(project->absoluteToConfigPath(modelCatalogue)))
        {
            agentsView->setModelCatalogueError("Model catalogue not accessible! Check configuration path in Project plugin.");
            agentsView->enableModelView(false);
            return;
        }

        if(modelList.contains(currentAgent->getVehicleModel()))
        {
            agentsView->enableModelView(true);
            agentsView->setCurrentModel(currentAgent->getVehicleModel());
        }
        else if(!currentAgent->getVehicleModel().isEmpty()){
            agentsView->setModelCatalogueError("Model does not exist in model catalogue!");
            agentsView->enableModelView(false);
        }
    }
}


void AgentMapPresenter::setModelItems(const QString &modelCatalogue)
{
    QString path = project->absoluteToConfigPath(modelCatalogue);
    QFile file(path);

    modelList.clear();

    if(file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text) )
    {

        QXmlStreamReader xml(&file);
        xml.readNextStartElement();

        while( xml.readNextStartElement() )
        {
            if(xml.name() == QString("Catalog"))
            {
                while( xml.readNextStartElement() )
                {
                    if(xml.name() == QString("Vehicle"))
                    {
                        modelList << xml.attributes().value("name").toString();
                        xml.skipCurrentElement();
                    }
                    else
                        xml.skipCurrentElement();
                }
            }
            else
                xml.skipCurrentElement();
        }
    }
    agentsView->setModelSelectionItems(modelList);
}

using namespace AgentProfileCommands;

SetCurrentAgent::SetCurrentAgent(AgentMapInterface const * const agents,
                                 AgentItemInterface  ** const currentAgent,
                                 AgentItemInterface::Name const newAgent,
                                 VehicleProfilesMapPresenter * const vehicleProfilesMap,
                                 DriverProfilesMapPresenter * const driverProfilesMap,
                                 AgentMapPresenter * const agentMap,
                                 QUndoCommand * parent)
    : QUndoCommand (parent)
    , agents(agents)
    , currentAgent(currentAgent)
    , newAgent(newAgent)
    , oldAgent(*currentAgent ? (*currentAgent)->getName() : "")
    , agentsMap(agentMap)
    , vehicleProfilesMap(vehicleProfilesMap)
    , driverProfilesMap(driverProfilesMap)
{
     setText(QString("changed agent profile: %1 -> %2").arg(oldAgent).arg(newAgent));
}

void SetCurrentAgent::redo()
{
    // set the reference to the agent (model) item currently selected in combo box
    *currentAgent = agents->getItem(newAgent);

    if(*currentAgent)
    {
        // adjust vehicle profiles presenter for the table view
        vehicleProfilesMap->setVehicleProfiles( (*currentAgent)->getVehicleProfiles() );

        // adjust vehicle profiles presenter for the table view
        driverProfilesMap->setDriverProfiles( (*currentAgent)->getDriverProfiles());
    }
    else
    {
        vehicleProfilesMap->setVehicleProfiles(nullptr);
        driverProfilesMap->setDriverProfiles(nullptr);
    }

    agentsMap->updateView();
}


void SetCurrentAgent::undo()
{
    // set the reference to the agent (model) item currently selected in combo box
    *currentAgent = agents->getItem(oldAgent);

    if(*currentAgent)
    {
        // adjust vehicle profiles presenter for the table view
        vehicleProfilesMap->setVehicleProfiles((*currentAgent)->getVehicleProfiles() );

        // adjust vehicle profiles presenter for the table view
        driverProfilesMap->setDriverProfiles((*currentAgent)->getDriverProfiles());
    }
    else
    {
        vehicleProfilesMap->setVehicleProfiles(nullptr);
        driverProfilesMap->setDriverProfiles(nullptr);
    }

    agentsMap->updateView();
}

AddAgentProfile::AddAgentProfile(AgentMapInterface * const agents,
                                 AgentMapView * const agentsView,
                                 AgentMapInterface::Name const &name,
                                 QUndoCommand * parent)
    : QUndoCommand(parent)
    , agents(agents)
    , agentsView(agentsView)
    , name(name)
{
    setText(QString("added agent profile %1").arg(name));
}

void AddAgentProfile::redo()
{
    agents->add(name);
    agentsView->addAgent(name);
}

void AddAgentProfile::undo()
{
    agents->remove(name);
    agentsView->removeAgent(name);
}

RemoveAgentProfile::RemoveAgentProfile(AgentMapInterface * const agents,
                                       AgentMapView * const agentsView,
                                       AgentMapInterface::Name const &name,
                                       QUndoCommand * parent)
    : QUndoCommand(parent)
    , agents(agents)
    , agentsView(agentsView)
    , name(name)
    , agent(new AgentItemModel(agents->getItem(name)))
{
    setText(QString("removed agent profile %1").arg(name));
}

RemoveAgentProfile::~RemoveAgentProfile()
{
    delete agent;
}

void RemoveAgentProfile::redo()
{
    agents->remove(name);
    agentsView->removeAgent(name);
}

void RemoveAgentProfile::undo()
{
    agents->add(name, agent);
    agentsView->addAgent(name);
}

AddDriverProfile::AddDriverProfile(DriverProfilesMapPresenter * const drivers,
                                   AgentItemInterface::DriverProfile const &profile,
                                   AgentItemInterface::Probability const &probability,
                                   QUndoCommand *parent)
    : QUndoCommand(parent)
    , drivers(drivers)
    , profile(profile)
    , probability(probability)
{
    setText(QString("included driver profile %1 into agent profile").arg(profile));
}

void AddDriverProfile::redo()
{
    drivers->addRow(drivers->rowCount(), profile, probability);
}

void AddDriverProfile::undo()
{
    drivers->removeRow(drivers->rowCount()-1);
}

RemoveDriverProfile::RemoveDriverProfile(DriverProfilesMapPresenter * const drivers,
                                         int const &row,
                                         QUndoCommand *parent)
    : QUndoCommand (parent)
    , drivers(drivers)
    , row(row)
    , profile(drivers->data(drivers->index(row,0)).toString())
    , probability(drivers->data(drivers->index(row,1)).toDouble())
{
    setText(QString("removed driver profile %1 from agent profile").arg(profile));
}

void RemoveDriverProfile::redo()
{
    drivers->removeRow(row);
}

void RemoveDriverProfile::undo()
{
    drivers->addRow(row, profile, probability);
}

EditAgentProfile::EditAgentProfile(AgentMapInterface * const agents,
                                   AgentMapView * const agentsView,
                                   AgentItemInterface::Name const &oldName,
                                   AgentItemInterface::Name const &newName,
                                   QUndoCommand *parent)
    : QUndoCommand(parent)
    , agents(agents)
    , agentsView(agentsView)
    , oldName(oldName)
    , newName(newName)
{
    setText(QString("edited agent profile's name %1 -> %2").arg(oldName).arg(newName));
}

void EditAgentProfile::redo()
{
    AgentItemInterface * const agent = agents->getItem(oldName);
    agents->setName(agent, newName);
    agentsView->removeAgent(oldName);
    agentsView->addAgent(newName);
}

void EditAgentProfile::undo()
{
    AgentItemInterface * const agent = agents->getItem(newName);
    agents->setName(agent, oldName);
    agentsView->removeAgent(newName);
    agentsView->addAgent(oldName);
}

EditAgentType::EditAgentType(AgentMapInterface * const agents,
                             AgentItemInterface::Name const &agentName,
                             AgentItemInterface::Type const &newType,
                             AgentMapPresenter * const agentsPresenter,
                             QUndoCommand *parent)
    : QUndoCommand(parent)
    , agents(agents)
    , agentName(agentName)
    , newType(newType)
    , oldType(agents->getItem(agentName)->getType())
    , agentsPresenter(agentsPresenter)
{
    setText(QString("edited type of agent profile %1").arg(agentName));
}

void EditAgentType::redo()
{
    agents->getItem(agentName)->setType(newType);
    agentsPresenter->updateView();
}

void EditAgentType::undo()
{
    agents->getItem(agentName)->setType(oldType);
    agentsPresenter->updateView();
}

EditAgentSystemConfig::EditAgentSystemConfig(AgentMapInterface * const agents,
                                             AgentMapView * const agentsView,
                                             AgentMapPresenter * const agentsPresenter,
                                             AgentItemInterface::Name const &agentName,
                                             AgentItemInterface::SystemConfig const &newSystemConfig,
                                             QUndoCommand *parent)
    : QUndoCommand(parent)
    , agents(agents)
    , agentsView(agentsView)
    , agentsPresenter(agentsPresenter)
    , agentName(agentName)
    , newSystemConfig(newSystemConfig)
    , oldSystemConfig(agents->getItem(agentName)->getSystemConfig())
{
    setText(QString("edited system config of agent profile %1").arg(agentName));
}

void EditAgentSystemConfig::redo()
{
    agentsPresenter->setSystemItems(newSystemConfig);
    agents->getItem(agentName)->setSystemConfig(newSystemConfig);
    agentsView->setCurrentSystemConfig(newSystemConfig);
}

void EditAgentSystemConfig::undo()
{
    agentsPresenter->setSystemItems(oldSystemConfig);
    agents->getItem(agentName)->setSystemConfig(oldSystemConfig);
    agentsView->setCurrentSystemConfig(oldSystemConfig);
}

EditAgentSystem::EditAgentSystem(AgentMapInterface * const agents,
                                 AgentMapView * const agentsView,
                                 AgentItemInterface::Name const &agentName,
                                 AgentItemInterface::SystemTitle const &newTitle,
                                 AgentItemInterface::SystemID const &newId,
                                 QUndoCommand *parent)
    : QUndoCommand(parent)
    , agents(agents)
    , agentsView(agentsView)
    , agentName(agentName)
    , newTitle(newTitle)
    , newId(newId)
    , oldTitle(agents->getItem(agentName)->getSystemTitle())
    , oldId(agents->getItem(agentName)->getSystemID())
{
    setText(QString("changed system of agent profile %1").arg(agentName));
}

void EditAgentSystem::redo()
{
    agents->getItem(agentName)->setSystemTitle(newTitle);
    agents->getItem(agentName)->setSystemID(newId);
    agentsView->setCurrentSystem(QString("%1 (ID: %2)").arg(newTitle).arg(newId));
}

void EditAgentSystem::undo()
{
    agents->getItem(agentName)->setSystemTitle(oldTitle);
    agents->getItem(agentName)->setSystemID(oldId);
    agentsView->setCurrentSystem(QString("%1 (ID: %2)").arg(oldTitle).arg(oldId));
}

EditAgentModelCatalogue::EditAgentModelCatalogue(AgentMapInterface * const agents,
                                                 AgentMapView * const agentsView,
                                                 AgentMapPresenter * const agentsPresenter,
                                                 AgentItemInterface::Name const &agentName,
                                                 AgentItemInterface::VehicleModelCatalogue const &newModelCatalogue,
                                                 QUndoCommand *parent)
    : QUndoCommand(parent)
    , agents(agents)
    , agentsView(agentsView)
    , agentsPresenter(agentsPresenter)
    , agentName(agentName)
    , newModelCatalogue(newModelCatalogue)
    , oldModelCatalogue(agents->getItem(agentName)->getVehicleModelCatalogue())
{
    setText(QString("edited vehicle model catalogue of agent profile %1").arg(agentName));
}

void EditAgentModelCatalogue::redo()
{
    agentsPresenter->setModelItems(newModelCatalogue);
    agents->getItem(agentName)->setVehicleModelCatalogue(newModelCatalogue);
    agentsView->setCurrentModelCatalogue(newModelCatalogue);
    agentsPresenter->refreshModelCatalogue();
}

void EditAgentModelCatalogue::undo()
{
    agentsPresenter->setModelItems(oldModelCatalogue);
    agents->getItem(agentName)->setVehicleModelCatalogue(oldModelCatalogue);
    agentsView->setCurrentModelCatalogue(oldModelCatalogue);
    agentsPresenter->refreshModelCatalogue();
}

EditAgentModel::EditAgentModel(AgentMapInterface * const agents,
                               AgentMapView * const agentsView,
                               AgentItemInterface::Name const &agentName,
                               AgentItemInterface::VehicleModel const &newModel,
                               QUndoCommand *parent)
    : QUndoCommand(parent)
    , agents(agents)
    , agentsView(agentsView)
    , agentName(agentName)
    , newModel(newModel)
    , oldModel(agents->getItem(agentName)->getVehicleModel())
{
    setText(QString("edited vehicle model of agent profile %1").arg(agentName));
}

void EditAgentModel::redo()
{
    agents->getItem(agentName)->setVehicleModel(newModel);
    agentsView->setCurrentModel(newModel);
}

void EditAgentModel::undo()
{
    agents->getItem(agentName)->setVehicleModel(oldModel);
    agentsView->setCurrentModel(oldModel);
}

AddVehicleProfile::AddVehicleProfile(VehicleProfilesMapPresenter * const vehicles,
                                     AgentItemInterface::VehicleProfile const &profile,
                                     AgentItemInterface::Probability const &probability,
                                     QUndoCommand *parent)
    : QUndoCommand(parent)
    , vehicles(vehicles)
    , profile(profile)
    , probability(probability)
{
    setText(QString("included vehicle profile %1 into agent profile").arg(profile));
}

void AddVehicleProfile::redo()
{
    vehicles->addRow(vehicles->rowCount(), profile, probability);
}

void AddVehicleProfile::undo()
{
    vehicles->removeRow(vehicles->rowCount() - 1);
}

RemoveVehicleProfile::RemoveVehicleProfile(VehicleProfilesMapPresenter * const vehicles,
                                           int const &row,
                                           QUndoCommand *parent)
    : QUndoCommand (parent)
    , vehicles(vehicles)
    , row(row)
    , profile(vehicles->data(vehicles->index(row,0)).toString())
    , probability(vehicles->data(vehicles->index(row,1)).toDouble())
{
    setText(QString("removed vehicle profile %1 from agent profile").arg(profile));
}

void RemoveVehicleProfile::redo()
{
    vehicles->removeRow(row);
}

void RemoveVehicleProfile::undo()
{
    vehicles->addRow(row, profile, probability);
}
