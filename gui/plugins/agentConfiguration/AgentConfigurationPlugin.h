/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file AgentConfigurationPlugin.h
//! @defgroup   agentConfigurationPlugin Agent Configuration Plugin
//! @brief The Agent Configuration Plugin provides a graphical user interface
//!        for creating agent profiles to be used in a scenarion-based
//!        traffic simulation.
//!
//! \section agentConfigurationPluginDef Agent Configuration Plugin
//!
//! @details The Agent Configuration Plugin provides a graphical user interface
//! for configuring agent profiles to be used in a scenario-based simulation.
//! Agent profiles can be of "dynamic" or "static" type. Static agent profiles
//! contain a static definition of an agent by a vehicle model and a system
//! from a system config XML file. The system exactly determines the agent's
//! tasks during a simulation cycle. In contrast, dynamic agent profiles only
//! refer to vehicle and driver profiles from which an agent is stochastically
//! built and spawned in the simulation world. The corresponding vehicle
//! and driver profiles are also defined in the profiles catalogue and, hence,
//! configured in this plugin as well. Vehicle profiles contain instructions on
//! how to build a vehicle. They contain a vehicle model as well as optional
//! safety systems that can be assigned to probabilities which determine whether
//! it is instantiated in a simulation. Furthermore, the concrete parameterisation
//! of these systems can be made stochastic as well. Driver profiles describe the
//! drivers behaviour (driver model) and the parameterization. So far, only one
//! driver model (which follows the agent in front) is implemented. For further
//! details on the profile-based agent configuration ansat, please consult the
//! documentation of the simulation framework.
//!
//! It should be noted that there is a temporary coexistence of two XML files
//! for storing the definition of agent profiles. On one hand, there
//! is the actual XML-file required by the simulation core and, on the other
//! hand, there is a ui-file which restores the view of the  GUI. The
//! reason for this coexistence is, that during the development  process, it
//! turned out that vehicle profiles of the XML-file contain pieces of information
//! which is usually provided by a system config file. In order to maintain a
//! consistent way of defining systems via a system config in the system editor,
//! we decided to write an exporter which gathers these pieces of information
//! from a system config file. Thus, when configuring vehicle profiles via the GUI,
//! the user selects systems from a system config and assigns them to probabilties.
//! In the exporter, the definitions are then exported to the XML-file required
//! by the simulation core. In order to be also able to restore the GUI's view,
//! using the reference to a system config, the ui-file has been developed as
//! a temporary work around.
//!
//! This plugin is implemented using a model-presenter-view architectual pattern.
//! The model classes define the basic objects for containing the  values of
//! the XML elements of the profile catalogue XML file. They also provide setter
//! and getter methods for editing its content. In all cases, the model classes
//! do not know about the presenters and views. The view classes define the
//! appearence and and UI objects the user can interact with. In this
//! implementation, the views are entirely passive, i.e. they contain no logic
//! of the application and, especially, do not know about the model objects.
//!
//! All user interfactions with the view lead to the emisson of signals which
//! are noticed by the presenters. These presenter classes define the logic
//! of the application, i.e. all signals emitted by views are connected to slots
//! which edit the model using its setter and getter methods. Sometimes, models
//! also send signals after they have been edited such that the views can be
//! updated to display the new model values. This usually happens when the same
//! model is presented to more than one view. All operations of the presenter
//! on model data can be undone and redone. For this purpose, Qt's Undo Framework
//! is applied, i.e. all commands are encapsulated into command objects whose
//! classes re-implement the redo/undo methods of the base class QUndoCommand.
//! These classes are usually declared in the same file which also contains
//! the presenter class. The corresponding presenter object usually
//! instantiates the command object and pushes it to the undo stack.
//!
//-----------------------------------------------------------------------------

#ifndef AGENTCONFIGURATIONPLUGIN_H
#define AGENTCONFIGURATIONPLUGIN_H

#include <QObject>

#include "openPASS/PluginInterface.h"

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
    explicit AgentConfigurationPlugin(QObject *const parent = nullptr);
    virtual ~AgentConfigurationPlugin() override = default;

public:
    virtual bool initialize() override;
    virtual bool deinitialize() override;

protected:
    AgentConfigurationModel *agentConfigurationModel;
    AgentConfigurationPresenter *agentConfigurationPresenter;
    AgentConfigurationCombinationPresenter *agentConfigurationCombinationPresenter;
    AgentConfigurationView *agentConfigurationView;
};

#endif // AGENTCONFIGURATIONPLUGIN_H
