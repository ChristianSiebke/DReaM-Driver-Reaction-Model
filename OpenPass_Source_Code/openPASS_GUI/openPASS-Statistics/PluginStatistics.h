/*********************************************************************
* Copyright (c) 2019 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef PLUGINHISTOGRAMS_H
#define PLUGINHISTOGRAMS_H

#include <QObject>

#include "openPASS/PluginInterface.h"
#include "Models/ModelStatistics.h"
#include "Views/ViewStatistics.h"
#include "Presenters/PresenterStatistics.h"

class ModelStatistics;
class PresenterStatistics;
class ViewStatistics;

class PluginStatistics : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "openPASS.Statistics")
    Q_INTERFACES(PluginInterface)

public:
    explicit PluginStatistics(QObject *const parent = nullptr);
    virtual ~PluginStatistics() = default;

public:
    virtual bool initialize() override;
    virtual bool deinitialize() override;

protected:
    ModelStatistics *modelStatistics;
    ViewStatistics *viewStatistics;
    PresenterStatistics *presenterStatistics;
};

#endif // PLUGINHISTOGRAMS_H
