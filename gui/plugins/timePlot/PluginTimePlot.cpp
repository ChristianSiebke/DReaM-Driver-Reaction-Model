/*********************************************************************
* Copyright (c) 2019 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "PluginTimePlot.h"

#include "openPASS-Window/WindowInterface.h"

PluginTimePlot::PluginTimePlot(QObject *const parent) :
    QObject(parent), modelTimePlot(nullptr), viewTimePlot(nullptr), presenterTimePlot(nullptr)
{
}

bool PluginTimePlot::initialize()
{
    WindowInterface *const window = WindowInterface::instance();
    if (window)
    {
        modelTimePlot = new ModelTimePlot();
        viewTimePlot = new ViewTimePlot(window);
        presenterTimePlot = new PresenterTimePlot(modelTimePlot, viewTimePlot);

        return true;
    }
    return false;
}

bool PluginTimePlot::deinitialize()
{
    delete presenterTimePlot;
    delete viewTimePlot;
    delete modelTimePlot;
    return true;
}
