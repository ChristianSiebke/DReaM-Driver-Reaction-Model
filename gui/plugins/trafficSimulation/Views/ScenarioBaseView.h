/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
//-----------------------------------------------------------------------------
//! @file  ScenarioBaseView.h
//! @ingroup trafficSimulationPlugin
//! @brief This class provides a resizable container widget for the content of
//!        the scenario tab's scroll area.
//!
//!        The item inserted into this widget is the instance of
//!        ScenarioView.
//-----------------------------------------------------------------------------
#ifndef SCENARIOBASEVIEW_H
#define SCENARIOBASEVIEW_H

#include "Views/ScenarioView.h"
#include "WidgetView.h"

//-----------------------------------------------------------------------------
//! @brief This class provides a resizable container widget for the content of
//!        the scenario tab.
//!
//!        The item inserted into this widget is the instance of
//!        ScenarioView.
//-----------------------------------------------------------------------------
class ScenarioBaseView : public WidgetView
{
    Q_OBJECT

public:
    ScenarioBaseView(QWidget *parent = 0);
    ~ScenarioBaseView() = default;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the scenario configuration editor widget.
    //!
    //! @return             The scenario configuration editor widget (ScenarioView)
    //-----------------------------------------------------------------------------
    ScenarioView *getScenarioView() const;

public Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which updates the size of this base view such that the child widget
    //! (ScenarioView) is visible.
    //-----------------------------------------------------------------------------
    void updateSize();

public:
    //-----------------------------------------------------------------------------
    //! Method which adjusts the positioning and size of the child widget. The
    //! size of the child widget is determined as a  fraction of the widget size passed
    //! as an argument (usually the TrafficSimulationView itself).
    //!
    //! @param[in]      mainWindow      the widget from which the size of the child widget
    //!                                 is derived (usually TrafficSimulationView)
    //-----------------------------------------------------------------------------
    void adjustWidgets(QWidget *mainWindow);

public:
    ScenarioView *scenarioView; //!< A pointer to the scenario configuration widget
};

#endif // SCENARIOBASEVIEW_H
