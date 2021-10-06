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
//! @file TrafficSimulationPlugin.h
//! @defgroup   trafficSimulationPlugin Traffic Simulation Plugin
//! @brief The Traffic Simulation Plugin provides a graphical user interface for
//!        configuring scenario-based simulations.
//!
//! \section trafficSimulationPluginDef Traffic Simulation Plugin
//!
//! @details The Traffic Simulation Plugin provides a graphical user interface
//! for configuring scenario-based simulations. For a complete configuration of
//! a traffic simulation, the user needs to specify the experiment settings,
//! the scenario configuration (based on an OpenSCENARIO file), the environment
//! and the surrounding traffic. The outcome of the whole plugin is the
//! simulationConfig XML file, whose structure defines the structure of the
//! classes of this plugin.
//!
//! The plugin is implemented using a model-presenter-view architectual pattern.
//! The model classes define the basic objects for containing the  values of
//! the XML elements of the simulationConfig XML file. They also provide setter
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
//-----------------------------------------------------------------------------

#ifndef TRAFFICSIMULATIONPLUGIN_H
#define TRAFFICSIMULATIONPLUGIN_H

#include <QObject>

#include "Models/TrafficSimulationModel.h"
#include "Presenters/TrafficSimulationPresenter.h"
#include "Views/TrafficSimulationView.h"
#include "openPASS/PluginInterface.h"

class TrafficSimulationPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "openPASS.TrafficSimulation")
    Q_INTERFACES(PluginInterface)

public:
    explicit TrafficSimulationPlugin(QObject *const parent = nullptr);
    virtual ~TrafficSimulationPlugin() = default;

public:
    virtual bool initialize() override;
    virtual bool deinitialize() override;

protected:
    TrafficSimulationModel *trafficSimulationModel;
    TrafficSimulationPresenter *trafficSimulationPresenter;
    TrafficSimulationView *trafficSimulationView;
};

#endif // TRAFFICSIMULATIONPLUGIN_H
