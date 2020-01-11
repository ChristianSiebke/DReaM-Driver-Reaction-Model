/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef DRIVERPROFILESPRESENTER_H
#define DRIVERPROFILESPRESENTER_H

#include "openPASS-AgentConfiguration/DriverProfileMapInterface.h"
#include "Presenters/ParameterMapPresenter.h"
#include "Views/DriverProfilesView.h"

#include <QObject>
#include <QUndoStack>


class DriverProfilesPresenter : public QObject
{

public:
     DriverProfilesPresenter(DriverProfileMapInterface * drivers,
                             DriverProfilesView * driversView,
                             QUndoStack * const undoStack,
                             QObject * parent = nullptr);
    ~DriverProfilesPresenter() = default;

private Q_SLOTS:
    void setCurrentDriver(const QString &name);
    void addDriver(const QString &name);
    void removeDriver(const QString &name);
    void editDriver(const QString &oldName, const QString &newName);

private Q_SLOTS:
    void setDriverConfig(const QString &filepath);

private:
    void setParameterDelegates();

public:
    void updateView();
    void updateDriverSelectionItems();

private:
    DriverProfileMapInterface * const drivers;
    DriverProfileItemInterface * currentDriver;
    DriverProfilesView * const driversView;
    QUndoStack * const undoStack;
    ParameterMapPresenter * parameters;
    QList<QString> bool_list = {"true","false"};

private:
    friend class AgentConfigurationPresenter;
};


namespace DriverProfileCommands {


class SetCurrentDriver : public QUndoCommand
{

public:
    SetCurrentDriver(DriverProfileMapInterface const * const drivers,
                     DriverProfileItemInterface ** const currentDriver,
                     DriverProfileItemInterface::Name const newDriver,
                     ParameterMapPresenter * const parameters,
                     DriverProfilesPresenter * const driversPresenter,
                     QUndoCommand *parent = nullptr);

public:
    void redo() override;
    void undo() override;

private:
    DriverProfileMapInterface const * const drivers;
    DriverProfileItemInterface ** const currentDriver;
    DriverProfileItemInterface::Name const newDriver;
    DriverProfileItemInterface::Name const oldDriver;
    ParameterMapPresenter * const parameters;
    DriverProfilesPresenter * const driversPresenter;
};

class EditDriverProfile : public QUndoCommand
{

public:
    EditDriverProfile(DriverProfileMapInterface * const drivers,
                      DriverProfilesView * const driversView,
                      DriverProfileMapInterface::Name const &newName,
                      DriverProfileMapInterface::Name const &oldName,
                      QUndoCommand *parent = nullptr);
public:
    void redo() override;
    void undo() override;

private:
    DriverProfileMapInterface * const drivers;
    DriverProfilesView * const driversView;
    DriverProfileMapInterface::Name const newName;
    DriverProfileMapInterface::Name const oldName;
};

class AddDriverProfile : public QUndoCommand
{
public:
    AddDriverProfile(DriverProfileMapInterface * const drivers,
                     DriverProfilesView * const driversView,
                     DriverProfileMapInterface::Name const &name,
                     QUndoCommand *parent = nullptr);

    AddDriverProfile(DriverProfileMapInterface * const drivers,
                     DriverProfilesView * const driversView,
                     DriverProfileMapInterface::Name const &name,
                     DriverProfileItemInterface::Parameters const * const parameterMap,
                     QUndoCommand *parent = nullptr);

    virtual ~AddDriverProfile() override;

public:
    void redo() override;
    void undo() override;

private:
   void addParameter(DriverProfileItemInterface::Parameters * const parameterMap,
                      ParameterItemInterface::Key const & key,
                      ParameterItemInterface::Type const & type,
                      ParameterItemInterface::Unit const & unit,
                      ParameterItemInterface::Value const & value);
   void setParameters();

private:
    DriverProfileMapInterface * const drivers;
    DriverProfilesView * const driversView;
    DriverProfileMapInterface::Name const name;
    DriverProfileItemInterface::Parameters * const parameters;
};

class RemoveDriverProfile: public QUndoCommand
{
public:
    RemoveDriverProfile(DriverProfileMapInterface * const drivers,
                        DriverProfilesView * const driversView,
                        DriverProfileMapInterface::Name const &name,
                        QUndoCommand *parent = nullptr);

    virtual ~RemoveDriverProfile() override;

public:
    void redo() override;
    void undo() override;

private:
    DriverProfileMapInterface * const drivers;
    DriverProfilesView * const driversView;
    DriverProfileMapInterface::Name const name;
    DriverProfileItemInterface const * const driver;

};

}
#endif // DRIVERPROFILESPRESENTER_H
