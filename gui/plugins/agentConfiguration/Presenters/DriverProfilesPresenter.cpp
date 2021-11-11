/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "Presenters/DriverProfilesPresenter.h"

#include <QFile>

#include "DelegateComboBoxView.h"
#include "DelegateDoubleView.h"
#include "Models/DriverProfileItemModel.h"
#include "Models/ParameterMapModel.h"

DriverProfilesPresenter::DriverProfilesPresenter(DriverProfileMapInterface *drivers,
                                                 DriverProfilesView *driversView,
                                                 QUndoStack *const undoStack,
                                                 QObject *parent) :
    QObject(parent), drivers(drivers), currentDriver(nullptr), driversView(driversView), undoStack(undoStack)
{
    parameters = new ParameterMapPresenter;
    driversView->getParameterView()->setModel(parameters);

    connect(driversView, &DriverProfilesView::addDriverProfile, this, &DriverProfilesPresenter::addDriver);
    connect(driversView, &DriverProfilesView::removeDriverProfile, this, &DriverProfilesPresenter::removeDriver);
    connect(driversView, &DriverProfilesView::editedName, this, &DriverProfilesPresenter::editDriver);

    connect(driversView, &DriverProfilesView::selectionChanged, this, &DriverProfilesPresenter::setCurrentDriver);
    connect(driversView, &DriverProfilesView::selectedDriverConfig, this, &DriverProfilesPresenter::setDriverConfig);

    driversView->enableConfiguration(false);
}

void DriverProfilesPresenter::setCurrentDriver(const QString &name)
{
    if (currentDriver && currentDriver->getName() != name)
        undoStack->push(new DriverProfileCommands::SetCurrentDriver(drivers, &currentDriver, name, parameters, this));
}

void DriverProfilesPresenter::addDriver(const QString &name)
{
    undoStack->beginMacro(QString("added driver profile %1").arg(name));
    undoStack->push(new DriverProfileCommands::AddDriverProfile(drivers, driversView, name));
    undoStack->push(new DriverProfileCommands::SetCurrentDriver(drivers, &currentDriver, name, parameters, this));
    undoStack->endMacro();
}

void DriverProfilesPresenter::removeDriver(const QString &name)
{
    if (currentDriver && currentDriver->getName() == name)
    {
        QList<DriverProfileItemInterface::Name> driversList = drivers->keys();
        driversList.removeAll(name);
        DriverProfileItemInterface::Name driverName = driversList.isEmpty() ? "" : driversList.first();

        undoStack->beginMacro(QString("removed driver profile %1").arg(name));
        undoStack->push(new DriverProfileCommands::SetCurrentDriver(drivers, &currentDriver, driverName, parameters, this));
        undoStack->push(new DriverProfileCommands::RemoveDriverProfile(drivers, driversView, name));
        undoStack->endMacro();
    }
}

void DriverProfilesPresenter::editDriver(const QString &oldName, const QString &newName)
{
    if (drivers->contains(oldName) && !drivers->contains(newName))
    {
        undoStack->push(new DriverProfileCommands::EditDriverProfile(drivers, driversView, newName, oldName));
    }
}

void DriverProfilesPresenter::setDriverConfig(QString const &filepath)
{
    if (currentDriver)
    {
        if (filepath != currentDriver->getConfig())
        {
            currentDriver->setConfig(filepath);
            updateView();
        }
    }
}

void DriverProfilesPresenter::updateView()
{
    // delete parameter views and correponding presenters
    driversView->setConfigView("");

    if (currentDriver)
    {
        // display name
        driversView->setCurrentDriver(currentDriver->getName());

        // set the config file
        driversView->setConfigView(currentDriver->getConfig());
        parameters->setParameters(currentDriver->getParameters());
        setParameterDelegates();
    }

    driversView->enableConfiguration(currentDriver != nullptr);
}

void DriverProfilesPresenter::updateDriverSelectionItems()
{
    driversView->setDriverSelectionItems(drivers->keys());
}

void DriverProfilesPresenter::setParameterDelegates()
{
    int row;
    int num_rows = parameters->rowCount();
    QTableView *parametersView = driversView->getParameterView();

    // remove existing delegates
    qDeleteAll(findChildren<QStyledItemDelegate *>());

    for (row = 0; row < num_rows; row++)
    {
        QModelIndex index = parameters->index(row, 0);

        QVariant type = parameters->data(index, Qt::DisplayRole);

        if (type.toString() == "Double")
            parametersView->setItemDelegateForRow(row, new DelegateDoubleView(std::numeric_limits<double>::lowest(),
                                                                              std::numeric_limits<double>::max(),
                                                                              0.1, 1, "driver model parameter's", undoStack, this));
        else if (type.toString() == "Int")
            parametersView->setItemDelegateForRow(row, new DelegateDoubleView(-255, 255, 1, 0, "driver model parameter's", undoStack, this));

        else if (type.toString() == "Bool")
            parametersView->setItemDelegateForRow(row, new DelegateComboBoxView(&bool_list, "driver model parameter's", undoStack, this));
    }
}

using namespace DriverProfileCommands;

SetCurrentDriver::SetCurrentDriver(DriverProfileMapInterface const *const drivers,
                                   DriverProfileItemInterface **const currentDriver,
                                   DriverProfileItemInterface::Name const newDriver,
                                   ParameterMapPresenter *const parameters,
                                   DriverProfilesPresenter *const driversPresenter,
                                   QUndoCommand *parent) :
    QUndoCommand(parent), drivers(drivers), currentDriver(currentDriver), newDriver(newDriver), oldDriver(*currentDriver ? (*currentDriver)->getName() : ""), parameters(parameters), driversPresenter(driversPresenter)
{
    setText(QString("changed driver profile: %1 -> %2").arg(oldDriver).arg(newDriver));
}

void SetCurrentDriver::redo()
{
    *currentDriver = drivers->getItem(newDriver);

    if (*currentDriver)
        parameters->setParameters((*currentDriver)->getParameters());
    else
        parameters->setParameters(nullptr);

    driversPresenter->updateView();
}

void SetCurrentDriver::undo()
{
    *currentDriver = drivers->getItem(oldDriver);

    if (*currentDriver)
        parameters->setParameters((*currentDriver)->getParameters());
    else
        parameters->setParameters(nullptr);

    driversPresenter->updateView();
}

EditDriverProfile::EditDriverProfile(DriverProfileMapInterface *const drivers,
                                     DriverProfilesView *const driversView,
                                     DriverProfileMapInterface::Name const &newName,
                                     DriverProfileMapInterface::Name const &oldName,
                                     QUndoCommand *parent) :
    QUndoCommand(parent), drivers(drivers), driversView(driversView), newName(newName), oldName(oldName)
{
    setText(QString("edited driver profile's name %1 -> %2").arg(oldName).arg(newName));
}

void EditDriverProfile::redo()
{
    drivers->setName(drivers->getItem(oldName), newName);
    driversView->removeDriverView(oldName);
    driversView->addDriverView(newName);
}

void EditDriverProfile::undo()
{
    drivers->setName(drivers->getItem(newName), oldName);
    driversView->removeDriverView(newName);
    driversView->addDriverView(oldName);
}

AddDriverProfile::AddDriverProfile(DriverProfileMapInterface *const drivers,
                                   DriverProfilesView *const driversView,
                                   DriverProfileMapInterface::Name const &name,
                                   QUndoCommand *parent) :
    QUndoCommand(parent), drivers(drivers), driversView(driversView), name(name), parameters(new ParameterMapModel)
{
    setText(QString("added driver profile %1").arg(name));
    setParameters();
}

AddDriverProfile::AddDriverProfile(DriverProfileMapInterface *const drivers,
                                   DriverProfilesView *const driversView,
                                   DriverProfileMapInterface::Name const &name,
                                   DriverProfileItemInterface::Parameters const *const parameterMap,
                                   QUndoCommand *parent) :
    QUndoCommand(parent), drivers(drivers), driversView(driversView), name(name), parameters(new ParameterMapModel)
{
    setText(QString("added driver profile %1").arg(name));

    for (auto parameter : parameterMap->values())
        parameters->add(parameter->getID(), parameter);
}

AddDriverProfile::~AddDriverProfile()
{
    delete parameters;
}

void AddDriverProfile::addParameter(DriverProfileItemInterface::Parameters *const parameterMap,
                                    ParameterItemInterface::Key const &key,
                                    ParameterItemInterface::Type const &type,
                                    ParameterItemInterface::Unit const &unit,
                                    ParameterItemInterface::Value const &value)
{
    ParameterMapInterface::ID id = parameterMap->generateID();
    parameterMap->add(id);
    ParameterItemInterface *param = parameterMap->getItem(id);

    param->setKey(key);
    param->setType(type);
    param->setUnit(unit);
    param->setValue(value);
}

void AddDriverProfile::setParameters()
{
    // NOTE: Momentarily, the one and only driver model is hard-coded
    addParameter(parameters, "Type", "String", "", "AlgorithmAgentFollowingDriverModel");
    addParameter(parameters, "AlgorithmLateralModule", "String", "", "Algorithm_LateralAfdm");
    addParameter(parameters, "AlgorithmLongitudinalModule", "String", "", "Algorithm_LongitudinalAfdm");

    addParameter(parameters, "VelocityWish", "Double", "m/s", "15.0");
    addParameter(parameters, "Delta", "Double", "m", "4.0");
    addParameter(parameters, "TGapWish", "Double", "s", "1.5");
    addParameter(parameters, "MinDistance", "Double", "m", "2.0");
    addParameter(parameters, "MaxAcceleration", "Double", "m/s^2", "2.0");
    addParameter(parameters, "MaxDeceleration", "Double", "m/s^2", "2.0");
}

void AddDriverProfile::redo()
{
    drivers->add(name);

    for (auto parameter : parameters->values())
        drivers->getItem(name)->getParameters()->add(parameter->getID(), parameter);

    driversView->addDriverView(name);
}

void AddDriverProfile::undo()
{
    drivers->remove(name);
    driversView->removeDriverView(name);
}

RemoveDriverProfile::RemoveDriverProfile(DriverProfileMapInterface *const drivers,
                                         DriverProfilesView *const driversView,
                                         DriverProfileMapInterface::Name const &name,
                                         QUndoCommand *parent) :
    QUndoCommand(parent), drivers(drivers), driversView(driversView), name(name), driver(new DriverProfileItemModel(drivers->getItem(name)))
{
    setText(QString("removed driver profile %1").arg(name));
}

RemoveDriverProfile::~RemoveDriverProfile()
{
    delete driver;
}

void RemoveDriverProfile::redo()
{
    drivers->remove(name);
    driversView->removeDriverView(name);
}

void RemoveDriverProfile::undo()
{
    drivers->add(name, driver);
    driversView->addDriverView(name);
}
