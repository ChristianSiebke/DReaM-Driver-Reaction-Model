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
//! @file  ExperimentBaseView.h
//! @ingroup trafficSimulationPlugin
//! @brief This class provides a resizable container widget for the content of
//!        the experiment tab's scroll area.
//!
//!        The items inserted into this widget are the instances of
//!        ExperimentView, ScenarioDisplayView, EnvironmentDisplayView and TrafficDisplayView.
//!        Further, the paint event is reimplemented such that the display views are connected
//!        via lines to the experiment configuration widget. This will visualize the dependency
//!        of a working traffic simulation configuration to the traffic, scenario and environment
//!        configurations.
//-----------------------------------------------------------------------------
#ifndef EXPERIMENTBASEVIEW_H
#define EXPERIMENTBASEVIEW_H

#include "Views/ExperimentView.h"
#include "Views/ScenarioDisplayView.h"
#include "Views/TrafficDisplayView.h"
#include "Views/EnvironmentDisplayView.h"

#include "WidgetView.h"

//-----------------------------------------------------------------------------
//! @brief This class provides a resizable container widget for the content of
//!        the experiment tab.
//!
//!
//!        The items inserted into this widget are the instances of
//!        ExperimentView, ScenarioDisplayView, EnvironmentDisplayView and TrafficDisplayView.
//!        Further, the paint event is reimplemented such that the display views are connected
//!        via lines to the experiment configuration widget. This will visualize the dependency
//!        of a working traffic simulation configuration to the traffic, scenario and environment
//!        configurations.
//-----------------------------------------------------------------------------
class ExperimentBaseView : public WidgetView
{

    Q_OBJECT

public:
    ExperimentBaseView(QWidget *parent = 0);
    ~ExperimentBaseView() = default;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the experiment configuration editor widget.
    //!
    //! @return             The experiment configuration editor widget (ExperimentView)
    //-----------------------------------------------------------------------------
    ExperimentView * getExperimentView() const;

    //-----------------------------------------------------------------------------
    //! Method which returns the widget displaying infos on the environment configuration.
    //!
    //! @return             The environment configuration display widget (EnvironmentDisplayView)
    //-----------------------------------------------------------------------------
    EnvironmentDisplayView * getEnvironmentDisplayView() const;

    //-----------------------------------------------------------------------------
    //! Method which returns the widget displaying infos on the scenario configuration.
    //!
    //! @return             The scenario configuration display widget (ScenarioDisplayView)
    //-----------------------------------------------------------------------------
    ScenarioDisplayView * getScenarioDisplayView() const;

    //-----------------------------------------------------------------------------
    //! Method which returns the widget displaying infos on the traffic configuration.
    //!
    //! @return             The traffic configuration display widget (TrafficDisplayView)
    //-----------------------------------------------------------------------------
    TrafficDisplayView * getTrafficDisplayView() const;


public Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which updates the size of this base view such that all child widgets
    //! (ExperimentView, ScenarioDisplayView, TrafficDisplayView and EnvironmentDisplayView)
    //! are visible.
    //-----------------------------------------------------------------------------
    void updateSize();

public:
    //-----------------------------------------------------------------------------
    //! Method which adjusts the positioning and size of the child widgets. The
    //! size of the child widgets is determined as fractions of the widget size passed
    //! as an argument (usually the TrafficSimulationView itself).
    //!
    //! @param[in]      mainWindow      the widget from which the sizes of the child widgets
    //!                                 are derived (usually TrafficSimulationView)
    //-----------------------------------------------------------------------------
    void adjustWidgets(QWidget *mainWindow);

protected:
    //-----------------------------------------------------------------------------
    //! Reimplementation of the paint event. Here, the painting of the connection
    //! lines between the experiment configuration widget (ExperimentView) and
    //! the display views is organised.
    //!
    //! @param[in]      event      the paint event (Qt's QPaintEvent class)
    //-----------------------------------------------------------------------------
    void paintEvent(QPaintEvent * event) override;

public:
    ExperimentView * experimentView;                //!< A pointer to the experiment configuration widget
    ScenarioDisplayView * scenarioDisplayView;      //!< A pointer to the display widget showing the scenario configuration
    TrafficDisplayView * trafficDisplayView;        //!< A pointer to the display widget showing the traffic configuration
    EnvironmentDisplayView * environmentDisplayView;
};

#endif // EXPERIMENTBASEVIEW_H
