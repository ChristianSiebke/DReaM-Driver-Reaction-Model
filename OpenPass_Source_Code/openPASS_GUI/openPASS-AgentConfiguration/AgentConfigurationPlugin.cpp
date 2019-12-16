/****************************************************************************** 
* Copyright (c) 2019 Volkswagen Group of America.
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/ 
/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "AgentConfigurationPlugin.h"

#include "openPASS-Window/WindowInterface.h"
#include "openPASS-System/SystemInterface.h"

#include "Models/AgentConfigurationModel.h"
#include "Presenters/AgentConfigurationPresenter.h"
#include "Presenters/AgentConfigurationCombinationPresenter.h"
#include "Views/AgentConfigurationView.h"

AgentConfigurationPlugin::AgentConfigurationPlugin(QObject * const parent)
    : QObject(parent)
    , agentConfigurationModel(nullptr)
    , agentConfigurationPresenter(nullptr)
    , agentConfigurationView(nullptr)
{
}

bool AgentConfigurationPlugin::initialize()
{
    WindowInterface * const window = WindowInterface::instance();
    ProjectInterface * const project = ProjectInterface::instance();
    SystemInterface * const system = SystemInterface::instance();

    if (window && system && project)
    {
        SystemInterface * systemModel = system->createSystemModel();
        agentConfigurationModel = new AgentConfigurationModel;
        agentConfigurationView = new AgentConfigurationView(window);
        agentConfigurationPresenter = new AgentConfigurationPresenter(agentConfigurationModel, agentConfigurationView, systemModel, project);
        agentConfigurationCombinationPresenter = new AgentConfigurationCombinationPresenter(agentConfigurationModel, agentConfigurationView, systemModel, project);

        return true;
    }
    return false;
}

bool AgentConfigurationPlugin::deinitialize()
{
    delete agentConfigurationCombinationPresenter;
    delete agentConfigurationPresenter;
    delete agentConfigurationView;
    delete agentConfigurationModel;

    return true;
}
