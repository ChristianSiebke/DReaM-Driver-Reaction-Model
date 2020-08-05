/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "openPASS-System/SystemInterface.h"

#include "Models/VehicleProfileItemModel.h"

#include "Presenters/SystemProfilePresenter.h"
#include "Presenters/VehicleProfilesPresenter.h"

#include "DelegateComboBoxView.h"
#include "DelegateDoubleView.h"

#include <QPair>
#include <QStringList>
#include <QFile>
#include <QXmlStreamReader>
#include <QFileDialog>
#include <QFileInfo>


double VehicleProfilesPresenter::default_probability = 0.5;

VehicleProfilesPresenter::VehicleProfilesPresenter(VehicleProfileMapInterface *vehicleProfiles,
                                                   VehicleProfilesView *vehicleProfilesView,
                                                   SystemInterface * systemInterface,
                                                   ProjectInterface * const project,
                                                   QUndoStack * const undoStack,
                                                   QObject *parent)
    : QObject(parent)
    , currentVehicleProfile(nullptr)
    , vehicleProfiles(vehicleProfiles)
    , vehicleProfilesView(vehicleProfilesView)
    , systemInterface(systemInterface)
    , project(project)
    , undoStack(undoStack)
{
    connect(vehicleProfilesView, &VehicleProfilesView::selectedVehicleProfile, this, &VehicleProfilesPresenter::setCurrentVehicleProfile);

    connect(vehicleProfilesView, &VehicleProfilesView::addVehicleProfile, this, &VehicleProfilesPresenter::addVehicleProfile);
    connect(vehicleProfilesView, &VehicleProfilesView::removeVehicleProfile, this, &VehicleProfilesPresenter::removeVehicleProfile);
    connect(vehicleProfilesView, &VehicleProfilesView::modifiedName, this, &VehicleProfilesPresenter::editVehicleProfileName);

    connect(vehicleProfilesView, &VehicleProfilesView::changeSystemConfig, this, &VehicleProfilesPresenter::setSystemConfig);
    connect(vehicleProfilesView, &VehicleProfilesView::refreshSystemConfig, this, &VehicleProfilesPresenter::refreshSystemConfig);

    connect(vehicleProfilesView, &VehicleProfilesView::refreshModelCatalogue, this, &VehicleProfilesPresenter::refreshModelCatalogue);
    connect(vehicleProfilesView, &VehicleProfilesView::changeModelCatalogue, this, &VehicleProfilesPresenter::setModelCatalogue);
    connect(vehicleProfilesView, &VehicleProfilesView::modifiedModel, this, &VehicleProfilesPresenter::setModel);

    connect(vehicleProfilesView, &VehicleProfilesView::addSystemProfile, this, &VehicleProfilesPresenter::addSystemProfile);
    connect(vehicleProfilesView, &VehicleProfilesView::removeSystemProfile, this, &VehicleProfilesPresenter::removeSystemProfile);

    connect(project, &ProjectInterface::update, this, &VehicleProfilesPresenter::refreshSystemConfig);
    connect(project, &ProjectInterface::update, this, &VehicleProfilesPresenter::refreshModelCatalogue);
}

VehicleProfilesPresenter::~VehicleProfilesPresenter()
{
    for(auto key : systemProfilePresenters.keys())
    {
        qDeleteAll(systemProfilePresenters.value(key)->values());
        delete systemProfilePresenters.value(key);
    }
}

void VehicleProfilesPresenter::setCurrentVehicleProfile(const QString &name)
{ 
    if(currentVehicleProfile && currentVehicleProfile->getName() != name)
        undoStack->push(new VehicleProfileCommands::SetCurrentVehicle(vehicleProfiles, &currentVehicleProfile, name, this));
}


void VehicleProfilesPresenter::addVehicleProfile(const QString &name)
{
    undoStack->beginMacro(QString("added vehicle profile %1").arg(name));
    undoStack->push(new VehicleProfileCommands::AddVehicleProfile(vehicleProfiles, vehicleProfilesView, name,&systemProfilePresenters));
    undoStack->push(new VehicleProfileCommands::SetCurrentVehicle(vehicleProfiles, &currentVehicleProfile, name, this));
    undoStack->endMacro();
}

void VehicleProfilesPresenter::removeVehicleProfile(const QString &name)
{
    if(currentVehicleProfile && currentVehicleProfile->getName() == name)
    {
        QList<VehicleProfileItemInterface::Name> vehicleList = vehicleProfiles->keys();
        vehicleList.removeAll(name);
        VehicleProfileItemInterface::Name vehicleName = vehicleList.isEmpty() ? "" : vehicleList.first();

        undoStack->beginMacro(QString("removed vehicle profile %1").arg(name));
        undoStack->push(new  VehicleProfileCommands::SetCurrentVehicle(vehicleProfiles, &currentVehicleProfile, vehicleName, this));
        undoStack->push(new  VehicleProfileCommands::RemoveVehicleProfile(vehicleProfiles, vehicleProfilesView, name, &systemProfilePresenters));
        undoStack->endMacro();
    }
}

void VehicleProfilesPresenter::editVehicleProfileName(const QString &oldName, const QString &newName)
{
    if(vehicleProfiles->contains(oldName) && !vehicleProfiles->contains(newName))
    {
        undoStack->push(new VehicleProfileCommands::EditVehicleProfile(vehicleProfiles, vehicleProfilesView, oldName, newName));
    }

}

void VehicleProfilesPresenter::setSystemConfig()
{
    QString const filepath = QFileDialog::getOpenFileName(
                   vehicleProfilesView, QObject::tr("openPASS / Choose system configuration"),
                   project->getConfigPath(), QStringLiteral("systemConfig (*.xml)"));

    QString systemConfig = project->relativeToConfigPath(filepath);

    if(currentVehicleProfile && (systemConfig!=currentVehicleProfile->getSystemConfig()) && !systemConfig.isEmpty() )
    {

        undoStack->push(new VehicleProfileCommands::SetSystemConfig(vehicleProfiles, vehicleProfilesView, currentVehicleProfile->getName(), systemConfig, this));
    }
}

void VehicleProfilesPresenter::refreshSystemConfig()
{
    if(currentVehicleProfile)
    {
        QString filepath = currentVehicleProfile->getSystemConfig();
        vehicleProfilesView->setSystemConfigError("");
        vehicleProfilesView->setSystemProfilesError("");

        if(filepath.isEmpty())
        {
            vehicleProfilesView->setSystemProfileSelection(QStringList());
            return;
        }

        bool dynamicMode = true;
        QString path = project->absoluteToConfigPath(filepath);

        if(systemInterface->load(path, &dynamicMode))
            updateSystemList();
        else
        {
            vehicleProfilesView->setSystemConfigError("System Config is invalid!");
            return;
        }

        // update the list of available systems and system profiles.
        vehicleProfilesView->setSystemProfileSelection(systemList.keys());

        if(!QFileInfo::exists(project->absoluteToConfigPath(filepath)))
            vehicleProfilesView->setSystemConfigError("System Config not accessible! Check configuration path in Master Configuration.");


        // check whether systems in current profile exist in systemConfig (systemList)
        if(!systemsConsistent())
        {
            vehicleProfilesView->setSystemProfilesError("Systems below are undefined!");
        }
    }
}

void VehicleProfilesPresenter::updateSystemList()
{

    systemList.clear();

    // Go through all systems and register ID and Title
    for(auto system : systemInterface->getSystems()->values())
    {
        SystemComponentItemInterface * currentAlgo = nullptr;
        QString systemListItem = system->getTitle() + QString(" (ID=%1)").arg(system->getID());

        // find the component of type "Algorithm"
        for ( auto component : system->getComponents()->values())
            if(component->getType() == SystemComponentItemInterface::Type::Algorithm)
            {
                currentAlgo = component;
                break;
            }

        if(currentAlgo)
        {
            // determine the library of the algo (which is the type in Profiles Catalogue)
            QString type = currentAlgo->getTitle();

            // add the system ID and Title to the list under the determined type
            if(systemList.contains(type))
                systemList[type].append(systemListItem);
            else
                systemList.insert(type, QList<QString>({systemListItem}));
        }
    }
}


void VehicleProfilesPresenter::addSystemProfile(const QString &type)
{
    if(currentVehicleProfile && !currentVehicleProfile->getSystemProfiles()->keys().contains(type) && !type.isEmpty())
    {
        undoStack->push(new VehicleProfileCommands::AddSystemProfile(vehicleProfiles,
                                                                     currentVehicleProfile->getName(),
                                                                     type,
                                                                     systemProfilePresenters.value(currentVehicleProfile->getName()),
                                                                     this));
    }
}

void VehicleProfilesPresenter::removeSystemProfile(const QString &type)
{
    if(currentVehicleProfile && systemList.keys().contains(type))
    {
        undoStack->push(new VehicleProfileCommands::RemoveSystemProfile(vehicleProfiles,
                                                                        currentVehicleProfile->getName(),
                                                                        type,
                                                                        systemProfilePresenters[currentVehicleProfile->getName()],
                                                                        this));
    }
}


void VehicleProfilesPresenter::addSystem(QString  const &type, const QString &selectionItem)
{
    // find the system profile presenter
    if(currentVehicleProfile)
    {
        SystemProfilePresenter * systemProfilePresenter = systemProfilePresenters.value(currentVehicleProfile->getName())->value(type);

        if(systemProfilePresenter)
        {
            VehicleProfileItemInterface::System system;

            // string has pattern "Title (ID=0)"
            system.second = selectionItem.section( " (ID=", 0, 0);
            QString ID = selectionItem.section( " (ID=", 1, 1);
            ID.chop(1);
            system.first = ID.toUInt();

            undoStack->push(new VehicleProfileCommands::AddSystem(systemProfilePresenter, system, default_probability));
        }
    }
}

void VehicleProfilesPresenter::removeSystem(const QString &type, const int &row)
{
    SystemProfilePresenter * systemProfilePresenter = systemProfilePresenters.value(currentVehicleProfile->getName())->value(type);

    if(systemProfilePresenter && systemProfilePresenter->rowCount()!=0)
        undoStack->push(new VehicleProfileCommands::RemoveSystem(systemProfilePresenter, row));
}

void VehicleProfilesPresenter::setModelCatalogue()
{
    QString const filepath = QFileDialog::getOpenFileName(
                vehicleProfilesView, QObject::tr("openPASS / Choose Vehicle Models Catalogue"),
                project->getConfigPath(), QStringLiteral("VehicleModelsCatalogue (*.xosc)"));

    QString modelCatalogue = project->relativeToConfigPath(filepath);

    if(currentVehicleProfile && !filepath.isEmpty())
    {

        VehicleProfileItemInterface::ModelCatalogue oldModelCatalogue = currentVehicleProfile->getModelCatalogue();

        undoStack->beginMacro(QString("edited vehicle model catalogue of vehicle profile %1").arg(currentVehicleProfile->getName()));

        undoStack->push(new VehicleProfileCommands::SetModelCatalogue(vehicleProfiles, vehicleProfilesView,  currentVehicleProfile->getName(), modelCatalogue, this));

        if(modelCatalogue!=oldModelCatalogue && !modelList.isEmpty())
            undoStack->push(new VehicleProfileCommands::SetModel(vehicleProfiles, vehicleProfilesView, currentVehicleProfile->getName(), modelList.last()));

        undoStack->endMacro();
    }
}

void VehicleProfilesPresenter::refreshModelCatalogue()
{
    if(currentVehicleProfile)
    {
        QString modelCatalogue = currentVehicleProfile->getModelCatalogue();
        loadModelCatalogue(modelCatalogue);
        vehicleProfilesView->setModelCatalogueError("");

        if(modelCatalogue.isEmpty())
            return;

        if(!QFileInfo::exists(project->absoluteToConfigPath(modelCatalogue)))
        {
            vehicleProfilesView->setModelCatalogueError("Model catalogue not accessible! Check configuration path in Master Configuration.");
            vehicleProfilesView->enableModelView(false);
            return;
        }

        if(modelList.contains(currentVehicleProfile->getModel()))
        {
            vehicleProfilesView->enableModelView(true);
            vehicleProfilesView->setModel(currentVehicleProfile->getModel());
        }
        else if (!currentVehicleProfile->getModel().isEmpty()){
            vehicleProfilesView->setModelCatalogueError("Model does not exist in model catalogue!");
            vehicleProfilesView->enableModelView(false);
        }
    }
}

void VehicleProfilesPresenter::loadModelCatalogue(const QString &filepath)
{
    QString path = project->absoluteToConfigPath(filepath);
    QFile file(path);

    modelList.clear();
    if(currentVehicleProfile && file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text) )
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
    vehicleProfilesView->setModelSelectionItems(modelList);
}

void VehicleProfilesPresenter::setModel(const QString &modelName)
{
    if(currentVehicleProfile)
        undoStack->push(new VehicleProfileCommands::SetModel(vehicleProfiles, vehicleProfilesView, currentVehicleProfile->getName(), modelName));
}


void VehicleProfilesPresenter::updateView()
{

    if(currentVehicleProfile)
    {

       vehicleProfilesView->setCurrentVehicle(currentVehicleProfile->getName());

       vehicleProfilesView->enableConfiguration(true);

        // update system views
       updateSystemsView();

       // update model catalogue view
       refreshModelCatalogue();
       vehicleProfilesView->setModelCatalogue(currentVehicleProfile->getModelCatalogue());

    }
    else
    {
        vehicleProfilesView->clearSystemProfileViews();

        vehicleProfilesView->enableConfiguration(false);
        vehicleProfilesView->setSystemConfig("");
        vehicleProfilesView->setSystemProfileSelection(QList<QString>());
        vehicleProfilesView->setModelCatalogue("");
        vehicleProfilesView->setModelSelectionItems(QStringList());
        vehicleProfilesView->setModel("");

        vehicleProfilesView->setSystemConfigError("");
        vehicleProfilesView->setSystemProfilesError("");
        vehicleProfilesView->setModelCatalogueError("");
    }
}

void VehicleProfilesPresenter::updateSystemsView()
{
   vehicleProfilesView->setSystemConfig(currentVehicleProfile->getSystemConfig());
   refreshSystemConfig();

   vehicleProfilesView->clearSystemProfileViews();
   if(systemProfilePresenters.value(currentVehicleProfile->getName()))
   {
        for(auto type : systemProfilePresenters.value(currentVehicleProfile->getName())->keys())
        {
            // create the view and connect it to the presenter
            SystemProfileView * systemProfileView = vehicleProfilesView->addSystemProfileView(type);
            SystemProfilePresenter * systemProfilePresenter = systemProfilePresenters.value(currentVehicleProfile->getName())->value(type);

            systemProfileView->getSystemView()->setModel(systemProfilePresenter);
            systemProfileView->getSystemView()->setItemDelegateForColumn(2, new DelegateDoubleView(0, 1, 0.1, 2, "system's probability", undoStack, this) );
            systemProfileView->setSelectionItems( systemList.value(type) );

            connect(systemProfileView, &SystemProfileView::addSystem, this, &VehicleProfilesPresenter::addSystem);
            connect(systemProfileView, &SystemProfileView::removeSystem, this, &VehicleProfilesPresenter::removeSystem);

            if(systemProfilePresenter)
                systemProfilePresenter->update();
        }
   }
}

// checks whether the registered systems is consistent with systemList (e.g. system Config)
bool VehicleProfilesPresenter::systemsConsistent()
{
    for(auto type : currentVehicleProfile->getSystemProfiles()->keys())
    {
        if(!systemList.contains(type))
            return false;

        for(auto system : currentVehicleProfile->getSystems(type)->keys())
        {
            QString listItem = QString("%1 (ID=%2)").arg(system.second).arg(system.first);
            if( !systemList.value(type).contains(listItem) )
                return false;
        }
    }

    return true;
}

void VehicleProfilesPresenter::updateVehicleSelectionItems()
{
    vehicleProfilesView->setVehicleProfileSelectionItems(vehicleProfiles->keys());
}

using namespace VehicleProfileCommands;

SetCurrentVehicle::SetCurrentVehicle(VehicleProfileMapInterface const * const vehicles,
                                     VehicleProfileItemInterface ** const currentVehicle,
                                     VehicleProfileItemInterface::Name const newVehicle,
                                     VehicleProfilesPresenter * const vehiclesPresenter,
                                     QUndoCommand *parent)
    : QUndoCommand (parent)
    , vehicles(vehicles)
    , currentVehicle(currentVehicle)
    , newVehicle(newVehicle)
    , oldVehicle(*currentVehicle ? (*currentVehicle)->getName() : "")
    , vehiclesPresenter(vehiclesPresenter)
{
    setText(QString("changed vehicle profile: %1 -> %2").arg(oldVehicle).arg(newVehicle));
}

void SetCurrentVehicle::redo()
{
    *currentVehicle = vehicles->getItem(newVehicle);
    vehiclesPresenter->updateView();
}

void SetCurrentVehicle::undo()
{
    *currentVehicle = vehicles->getItem(oldVehicle);
    vehiclesPresenter->updateView();
}

AddVehicleProfile::AddVehicleProfile(VehicleProfileMapInterface * const vehicles,
                                     VehicleProfilesView * const vehiclesView,
                                     VehicleProfileMapInterface::Name const &name,
                                     QMap <VehicleProfileItemInterface::Name , QMap<QString, SystemProfilePresenter*> * > * const systemProfilePresenters,
                                     QUndoCommand * parent)
    : QUndoCommand (parent)
    , vehicles(vehicles)
    , vehiclesView(vehiclesView)
    , name(name)
    , systemProfilePresenters(systemProfilePresenters)
    , addedSystemProfiles(new QMap<QString, SystemProfilePresenter*>)
{
    setText(QString("added vehicle profile %1").arg(name));
}

AddVehicleProfile::~AddVehicleProfile()
{
    if(addedSystemProfiles)
    {
        qDeleteAll(addedSystemProfiles->values());
        delete addedSystemProfiles;
    }
}

void AddVehicleProfile::redo()
{
    vehicles->add(name);
    vehiclesView->addVehicleProfileSelectionItem(name);
    systemProfilePresenters->insert(name, addedSystemProfiles);
    addedSystemProfiles = nullptr;
}

void AddVehicleProfile::undo()
{
    addedSystemProfiles = systemProfilePresenters->take(name);
    vehicles->remove(name);
    vehiclesView->removeVehicleProfileSelectionItem(name);
}

RemoveVehicleProfile::RemoveVehicleProfile(VehicleProfileMapInterface * const vehicles,
                                           VehicleProfilesView * const vehiclesView,
                                           VehicleProfileMapInterface::Name const &name,
                                           QMap<VehicleProfileItemInterface::Name, QMap<QString, SystemProfilePresenter *> *> * const systemProfilePresenters,
                                           QUndoCommand * parent)
    : QUndoCommand(parent)
    , vehicles(vehicles)
    , vehiclesView(vehiclesView)
    , name(name)
    , vehicle(new VehicleProfileItemModel(vehicles->getItem(name)))
    , systemProfilePresenters(systemProfilePresenters)
{
    setText(QString("removed vehicle profile %1").arg(name));
}

RemoveVehicleProfile::~RemoveVehicleProfile()
{
    if(removedSystemProfiles)
        qDeleteAll(removedSystemProfiles->values());

    delete vehicle;
}

void RemoveVehicleProfile::redo()
{
    removedSystemProfiles = systemProfilePresenters->take(name);
    for(auto presenter : removedSystemProfiles->values())
        presenter->setSystems(nullptr);

    vehicles->remove(name);
    vehiclesView->removeVehicleProfileSelectionItem(name);
}

void RemoveVehicleProfile::undo()
{
    vehicles->add(name, vehicle);
    vehiclesView->addVehicleProfileSelectionItem(name);

    for(auto type : vehicles->getItem(name)->getSystemProfiles()->keys())
        removedSystemProfiles->value(type)->setSystems(vehicles->getItem(name)->getSystems(type));

    systemProfilePresenters->insert(name, removedSystemProfiles);
    removedSystemProfiles = nullptr;
}

EditVehicleProfile::EditVehicleProfile(VehicleProfileMapInterface * const vehicles,
                                       VehicleProfilesView * const vehiclesView,
                                       VehicleProfileMapInterface::Name const &oldName,
                                       VehicleProfileMapInterface::Name const &newName,
                                       QUndoCommand *parent)
    : QUndoCommand (parent)
    , vehicles(vehicles)
    , vehiclesView(vehiclesView)
    , oldName(oldName)
    , newName(newName)
{
    setText(QString("edited vehicle profile's name %1 -> %2").arg(oldName).arg(newName));
}

void EditVehicleProfile::redo()
{
    VehicleProfileItemInterface * const vehicle = vehicles->getItem(oldName);
    vehicles->setName(vehicle, newName);
    vehiclesView->removeVehicleProfileSelectionItem(oldName);
    vehiclesView->addVehicleProfileSelectionItem(newName);
}

void EditVehicleProfile::undo()
{
    VehicleProfileItemInterface * const vehicle = vehicles->getItem(newName);
    vehicles->setName(vehicle, oldName);
    vehiclesView->removeVehicleProfileSelectionItem(newName);
    vehiclesView->addVehicleProfileSelectionItem(oldName);
}

SetModelCatalogue::SetModelCatalogue(VehicleProfileMapInterface * const vehicles,
                                     VehicleProfilesView * const vehicleProfilesView,
                                     VehicleProfileItemInterface::Name const &vehicleName,
                                     VehicleProfileItemInterface::ModelCatalogue const &newModelCatalogue,
                                     VehicleProfilesPresenter * const vehicleProfilesPresenter,
                                     QUndoCommand *parent)
    : QUndoCommand(parent)
    , vehicles(vehicles)
    , vehicleProfilesView(vehicleProfilesView)
    , vehicleName(vehicleName)
    , newModelCatalogue(newModelCatalogue)
    , oldModelCatalogue(vehicles->getItem(vehicleName)->getModelCatalogue())
    , vehicleProfilesPresenter(vehicleProfilesPresenter)
{
    setText(QString("set system config of vehicle profile %1 to %2").arg(vehicleName).arg(newModelCatalogue));
}

void SetModelCatalogue::redo()
{
    vehicles->getItem(vehicleName)->setModelCatalogue(newModelCatalogue);
    vehicleProfilesView->setModelCatalogue(newModelCatalogue);
    vehicleProfilesPresenter->refreshModelCatalogue();
}

void SetModelCatalogue::undo()
{
    vehicles->getItem(vehicleName)->setModelCatalogue(oldModelCatalogue);
    vehicleProfilesView->setModelCatalogue(oldModelCatalogue);
    vehicleProfilesPresenter->refreshModelCatalogue();
}

SetModel::SetModel(VehicleProfileMapInterface * const vehicles,
                   VehicleProfilesView * const vehicleProfilesView,
                   VehicleProfileItemInterface::Name const &vehicleName,
                   VehicleProfileItemInterface::Model const &newModel,
                   QUndoCommand *parent)
    : QUndoCommand(parent)
    , vehicles(vehicles)
    , vehicleProfilesView(vehicleProfilesView)
    , vehicleName(vehicleName)
    , newModel(newModel)
    , oldModel(vehicles->getItem(vehicleName)->getModel())
{
    setText(QString("edited vehicle model of vehicle profile %1").arg(vehicleName));
}

void SetModel::redo()
{
    vehicles->getItem(vehicleName)->setModel(newModel);
    vehicleProfilesView->setModel(newModel);
}

void SetModel::undo()
{
    vehicles->getItem(vehicleName)->setModel(oldModel);
    vehicleProfilesView->setModel(oldModel);
}

SetSystemConfig::SetSystemConfig(VehicleProfileMapInterface * const vehicles,
                                 VehicleProfilesView * const vehicleProfilesView,
                                 VehicleProfileItemInterface::Name const &vehicleName,
                                 VehicleProfileItemInterface::SystemConfig const &newSystemConfig,
                                 VehicleProfilesPresenter * const vehicleProfilesPresenter,
                                 QUndoCommand *parent)
    : QUndoCommand(parent)
    , vehicles(vehicles)
    , vehicleProfilesView(vehicleProfilesView)
    , vehicleName(vehicleName)
    , newSystemConfig(newSystemConfig)
    , oldSystemConfig(vehicles->getItem(vehicleName)->getSystemConfig())
    , vehicleProfilesPresenter(vehicleProfilesPresenter)
{
    setText(QString("set system config of vehicle profile %1 to %2").arg(vehicleName).arg(newSystemConfig));
}

void SetSystemConfig::redo()
{
    vehicles->getItem(vehicleName)->setSystemConfig(newSystemConfig);
    vehicleProfilesView->setSystemConfig(newSystemConfig);
    vehicleProfilesPresenter->refreshSystemConfig();
}

void SetSystemConfig::undo()
{
    vehicles->getItem(vehicleName)->setSystemConfig(oldSystemConfig);
    vehicleProfilesView->setSystemConfig(oldSystemConfig);
    vehicleProfilesPresenter->refreshSystemConfig();
}

AddSystemProfile::AddSystemProfile(VehicleProfileMapInterface * const vehicles,
                                   VehicleProfileItemInterface::Name const &vehicleName,
                                   QString const &type,
                                   QMap<QString, SystemProfilePresenter*> * const systemProfilePresenters,
                                   VehicleProfilesPresenter * const vehicleProfilesPresenter,
                                   QUndoCommand *parent)
    : QUndoCommand(parent)
    , vehicles(vehicles)
    , vehicleName(vehicleName)
    , type(type)
    , vehicleProfilesPresenter(vehicleProfilesPresenter)
    , systemProfilePresenters(systemProfilePresenters)
    , systemProfilePresenter(new SystemProfilePresenter(nullptr))
{
    setText(QString("added system profile of type %1 to vehicle profile %2").arg(type).arg(vehicleName));
}

AddSystemProfile::~AddSystemProfile()
{
    if(deletable && !systemProfilePresenters->values().contains(systemProfilePresenter) )
    {
        delete systemProfilePresenter;
    }
}

void AddSystemProfile::redo()
{
    VehicleProfileItemInterface::Systems * const systems = new VehicleProfileItemInterface::Systems;
    vehicles->getItem(vehicleName)->getSystemProfiles()->insert(type, systems);
    systemProfilePresenter->setSystems(systems);
    systemProfilePresenters->insert(type, systemProfilePresenter);

    vehicleProfilesPresenter->updateView();

    deletable = false;
}

void AddSystemProfile::undo()
{
    systemProfilePresenters->remove(type);
    delete vehicles->getItem(vehicleName)->getSystemProfiles()->take(type);
    systemProfilePresenter->setSystems(nullptr);

    vehicleProfilesPresenter->updateView();

    deletable = true;
}

RemoveSystemProfile::RemoveSystemProfile(VehicleProfileMapInterface * const vehicles,
                                         VehicleProfileItemInterface::Name const &vehicleName,
                                         QString const &type,
                                         QMap<QString, SystemProfilePresenter*> * const systemProfilePresenters,
                                         VehicleProfilesPresenter * const vehicleProfilesPresenter,
                                         QUndoCommand *parent)
    : QUndoCommand(parent)
    , vehicles(vehicles)
    , vehicleName(vehicleName)
    , type(type)
    , vehicleProfilesPresenter(vehicleProfilesPresenter)
    , systemProfilePresenters(systemProfilePresenters)
    , systemProfilePresenter(systemProfilePresenters->value(type))
{
    setText(QString("removed system profile of type %1 from vehicle profile %2").arg(type).arg(vehicleName));
}

RemoveSystemProfile::~RemoveSystemProfile()
{
    if(deletable && !systemProfilePresenters->values().contains(systemProfilePresenter))
    {
        delete systemProfilePresenter;
    }
}

void RemoveSystemProfile::redo()
{
    systemProfilePresenters->remove(type);
    delete vehicles->getItem(vehicleName)->getSystemProfiles()->take(type);
    systemProfilePresenter->setSystems(nullptr);

    vehicleProfilesPresenter->updateView();

    deletable = true;
}

void RemoveSystemProfile::undo()
{
    VehicleProfileItemInterface::Systems * const systems = new VehicleProfileItemInterface::Systems;
    vehicles->getItem(vehicleName)->getSystemProfiles()->insert(type, systems);
    systemProfilePresenter->setSystems(systems);
    systemProfilePresenters->insert(type, systemProfilePresenter);

    vehicleProfilesPresenter->updateView();

    deletable = false;
}


AddSystem::AddSystem(SystemProfilePresenter * const systems,
                     VehicleProfileItemInterface::System const &system,
                     VehicleProfileItemInterface::Probability const &probability,
                     QUndoCommand * parent)
    : QUndoCommand (parent)
    , systems(systems)
    , system(system)
    , probability(probability)
{
    setText(QString("added system of type %1").arg(systems->objectName()));
}

void AddSystem::redo()
{
    systems->addRow(systems->rowCount(), system, probability);
}

void AddSystem::undo()
{
    systems->removeRow(systems->rowCount()-1);
}

RemoveSystem::RemoveSystem(SystemProfilePresenter * const systems,
                           int const &row,
                           QUndoCommand * parent)
    : QUndoCommand(parent)
    , systems(systems)
    , row(row)
    , system( systems->data(systems->index(row,0)).toUInt(),
              systems->data(systems->index(row,1)).toString())
    , probability(systems->data(systems->index(row, 2)).toDouble())
{
    setText(QString("removed system of type %1").arg(systems->objectName()));
}

void RemoveSystem::redo()
{
    systems->removeRow(row);
}

void RemoveSystem::undo()
{
    systems->addRow(row, system, probability);
}
