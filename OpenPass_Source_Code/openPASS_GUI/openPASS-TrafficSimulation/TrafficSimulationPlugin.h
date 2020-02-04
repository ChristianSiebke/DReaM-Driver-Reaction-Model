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
//! @file TrafficSimulationPlugin.h
//! @defgroup   trafficSimulationPlugin Traffic Simulation Plugin
//! @brief The Traffic Simulation Plugin provides a user interface for configuring
//!        scenario-based simulations.
//!
//! \section trafficSimulationPluginDef Traffic Simulation Plugin
//!
//! @details The Traffic Simulation Plugin provides a user interface for configuring
//! scenario-based simulations. For a complete configuration of a traffic simulation
//! the user needs to specify the experiment settings (e.g. start time, end time etc.),
//! the scenario configuration (based on an open scenario file), the environment and
//! the surrounding traffic. The outcome of the whole plugin is the slaveConfig XML file,
//! which also defines the implementation logic of the classes.
//!
//! The plugin is implemented using a model-presenter-view architectual pattern. The model classes
//! define the basic objects for containing the  values of the XML elements
//! of the slaveConfig XML file. They also provide setter and getter methods for editing its content.
//! In all cases, the model classes do not know about the presenters and views.
//! The view classes define the appearence and and UI objects the user can interact with. In
//! this implementation, the views are entirely passive, i.e. they contain no logic of the application
//! and, especially, do not know about the model objects.
//! All user interfactions with the view lead to the emisson of signals which are noticed by the presenters.
//! These presenter classes define the logic of the application, i.e. all signals emitted by views are connected to slots
//! which edit the model using its setter and getter methods. Sometimes, models also send signals after they
//! have been edited such that the views can be updated to display the new model values. This always happens
//! when the same model is presented to more than one view.
//-----------------------------------------------------------------------------

#ifndef TRAFFICSIMULATIONPLUGIN_H
#define TRAFFICSIMULATIONPLUGIN_H

#include "openPASS/PluginInterface.h"

#include "Models/TrafficSimulationModel.h"
#include "Presenters/TrafficSimulationPresenter.h"
#include "Views/TrafficSimulationView.h"

#include <QObject>

class TrafficSimulationPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "openPASS.TrafficSimulation")
    Q_INTERFACES(PluginInterface)

public:
    explicit TrafficSimulationPlugin(QObject * const parent = nullptr);
    virtual ~TrafficSimulationPlugin() = default;

public:
    virtual bool initialize() override;
    virtual bool deinitialize() override;

protected:
    TrafficSimulationModel * trafficSimulationModel;
    TrafficSimulationPresenter * trafficSimulationPresenter;
    TrafficSimulationView * trafficSimulationView;

};

#endif // TRAFFICSIMULATIONPLUGIN_H
