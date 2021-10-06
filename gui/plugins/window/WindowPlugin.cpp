/********************************************************************************
 * Copyright (c) 2017 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

/*
//-----------------------------------------------------------------------------
//! @file WindowPlugin.cpp
//! @defgroup   windowPlugin Window Plugin
//! @brief The Window Plugin (WP) is dedicated for managing the View of the application.
//!
//! \htmlonly
//! <a name="dependencies" id="dependencies"></a><h2 class="groupheader">Dependencies</h2>
//! \endhtmlonly
//!
//! \image html plugin_window__inherit__graph.svg
//!
//! \section windowPluginDef Window Plugin
//!
//! @details The Window Plugin (WP) is dedicated for managing the View of openPASS application.
//! It defines the main window view which includes:
//! - a left side bar,
//! - a widget view that changes according to the selected button on the side bar
//!
//! \image html Example1View.png
//!
//! This structure enables creating different views for each plugin (if required).
//! The window plugin registers the plugin view with its corresponding button placed
//! on the side bar. It also registers the corresponding menu bar of the plugin view.\n
//! When creating a plugin with a view, it should systematically include the @link WindowInterface.h Window Interface@endlink of the @link plugin Window Plugin@endlink
//! in order to use its methods for registering and deregistering the View of the plugin and create a button on the side bar of the main window application
//! (see Example <a href="_create__plugin_with__view-example.html">Create_Plugin_wth_View</a>).
//!
//! \note The design of the widget view and the menu bar of a plugin is set in the plugin itself.
//!
//! \note Not necessary all plugins have views (e.g. @link componentPlugin Component Plugin@endlink).
//!
//!
//-----------------------------------------------------------------------------
*/

#include "WindowPlugin.h"

#include "Models/WindowModel.h"
#include "Presenters/WindowPresenter.h"
#include "Views/WindowView.h"
#include "openPASS/PluginManagerInterface.h"

WindowPlugin::WindowPlugin(QObject *const parent) :
    QObject(parent), windowModel(nullptr), windowPresenter(nullptr), windowView(nullptr)
{
}

bool WindowPlugin::initialize()
{
    PluginManagerInterface *const plugins =
        PluginManagerInterface::instance();
    if (plugins)
    {
        windowModel = new WindowModel();
        windowPresenter = new WindowPresenter(windowModel);
        windowView = new WindowView(windowPresenter);
        connect(plugins, &PluginManagerInterface::started,
                windowView, &WindowView::initialize_view);
        return true;
    }
    return false;
}

bool WindowPlugin::deinitialize()
{
    delete windowView;
    delete windowPresenter;
    delete windowModel;
    return true;
}
