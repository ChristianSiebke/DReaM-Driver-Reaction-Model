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
//! @file AgentConfigurationPlugin.h
//! @defgroup   agentConfigurationPlugin Agent Configuration Plugin
//! @brief The Agent Configuration Plugin provides an interface for creating
//!        agent profiles to be used in a scenarion-based traffic simulation.
//!
//! \section agentConfigurationPluginDef Agent Configuration Plugin
//!
//! @details - Definitions: agent profiles, driver profiles, vehicle profiles (XML)
//!          - *xml vs. *ui file (logic with dynamic and static modes)
//-----------------------------------------------------------------------------


#ifndef AGENTCONFIGURATIONPLUGIN_H
#define AGENTCONFIGURATIONPLUGIN_H

#include "openPASS/PluginInterface.h"
#include <QObject>

class AgentConfigurationModel;
class AgentConfigurationPresenter;
class AgentConfigurationCombinationPresenter;
class AgentConfigurationView;

class AgentConfigurationPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "openPASS.AgentConfiguration")
    Q_INTERFACES(PluginInterface)

public:
    explicit AgentConfigurationPlugin(QObject * const parent = nullptr);
    virtual ~AgentConfigurationPlugin() = default;

public:
    virtual bool initialize() override;
    virtual bool deinitialize() override;

protected:
    AgentConfigurationModel * agentConfigurationModel;
    AgentConfigurationPresenter * agentConfigurationPresenter;
    AgentConfigurationCombinationPresenter * agentConfigurationCombinationPresenter;
    AgentConfigurationView * agentConfigurationView;
};

#endif // AGENTCONFIGURATIONPLUGIN_H
