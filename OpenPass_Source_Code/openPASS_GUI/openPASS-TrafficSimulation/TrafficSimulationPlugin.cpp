/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "TrafficSimulationPlugin.h"

#include "openPASS-Window/WindowInterface.h"
#include "openPASS-Project/ProjectInterface.h"

TrafficSimulationPlugin::TrafficSimulationPlugin(QObject * const parent)
    : QObject(parent)
    , trafficSimulationModel(nullptr)
    , trafficSimulationPresenter(nullptr)
    , trafficSimulationView(nullptr)
{

}

bool TrafficSimulationPlugin::initialize()
{
    WindowInterface * const window = WindowInterface::instance();
    ProjectInterface * const project = ProjectInterface::instance();

    if(window && project)
    {
        // allocate pointers to models, presenters and views
        trafficSimulationModel = new TrafficSimulationModel;
        trafficSimulationView = new TrafficSimulationView(window);
        trafficSimulationPresenter = new TrafficSimulationPresenter(trafficSimulationModel, trafficSimulationView, project);

        return true;
    }

    return false;
}

bool TrafficSimulationPlugin::deinitialize()
{
    // delete models, presenters and views
    delete trafficSimulationPresenter;
    delete trafficSimulationView;
    delete trafficSimulationModel;
    return true;
}
