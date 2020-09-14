/*********************************************************************
* Copyright (c) 2019 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "PluginStatistics.h"

#include "openPASS-Window/WindowInterface.h"

PluginStatistics::PluginStatistics(QObject *const parent) :
    QObject(parent), modelStatistics(nullptr), viewStatistics(nullptr), presenterStatistics(nullptr)
{
}

bool PluginStatistics::initialize()
{
    WindowInterface *const window = WindowInterface::instance();
    if (window)
    {
        modelStatistics = new ModelStatistics();
        viewStatistics = new ViewStatistics(window);
        presenterStatistics = new PresenterStatistics(modelStatistics, viewStatistics);

        return true;
    }
    return false;
}

bool PluginStatistics::deinitialize()
{
    delete presenterStatistics;
    delete viewStatistics;
    delete modelStatistics;
    return true;
}
