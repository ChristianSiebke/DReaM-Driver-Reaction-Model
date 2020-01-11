/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef AGENTCONFIGURATIONPRESENTER_H
#define AGENTCONFIGURATIONPRESENTER_H

#include "openPASS-System/SystemInterface.h"
#include "openPASS-AgentConfiguration/AgentConfigurationInterface.h"
#include "openPASS-Project/ProjectInterface.h"

#include "Presenters/AgentMapPresenter.h"
#include "Presenters/VehicleProfilesPresenter.h"
#include "Presenters/DriverProfilesPresenter.h"

#include "Views/AgentConfigurationView.h"

#include <QObject>
#include <QUndoStack>
#include <QShortcut>

class AgentConfigurationPresenter : public QObject
{
    Q_OBJECT

public:
    explicit AgentConfigurationPresenter(AgentConfigurationInterface * const agentConfiguration,
                                         AgentConfigurationView * const agentConfigurationView,
                                         SystemInterface * const system,
                                         ProjectInterface * const project,
                                         QObject *parent = nullptr);
    virtual ~AgentConfigurationPresenter() = default;

private Q_SLOTS:
    void save();
    void load();
    void clear();

private:
    void copyAgentProfiles(AgentConfigurationInterface const * const from);
    void copyDriverProfiles(AgentConfigurationInterface const * const from);
    void copyVehicleProfiles(AgentConfigurationInterface const * const from);

private:
    void clearAgentProfiles();
    void clearDriverProfiles();
    void clearVehicleProfiles();

private:
    AgentConfigurationInterface * const agentConfiguration;
    AgentConfigurationView * const agentConfigurationView;
    SystemInterface * const system;
    ProjectInterface * const project;

private:
    QUndoStack * const undoStack;
    QShortcut * const redo;
    QShortcut * const undo;

private:
    AgentMapPresenter * const agentsPresenter;
    VehicleProfilesPresenter * const vehicleProfilesPresenter;
    DriverProfilesPresenter * const driverProfilesPresenter;
};

#endif // AGENTCONFIGURATIONPRESENTER_H
