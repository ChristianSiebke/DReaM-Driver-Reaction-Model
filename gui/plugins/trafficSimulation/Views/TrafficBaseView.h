/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
//-----------------------------------------------------------------------------
//! @file  TrafficBaseView.h
//! @ingroup trafficSimulationPlugin
//! @brief This class provides a resizable container widget for the content of
//!        the traffic tab.
//!
//!        The items inserted into this widget are the instances of
//!        TrafficView, TrafficParametersView and TrafficLanesView.
//!        Further, the paint event is reimplemented such that the parameters and lanes
//!        views are connected via lines to the title widget (TrafficView).
//!        This visualizes that both configurations (parameters and lanes) are necessary
//!        for a complete traffic configuration.
//-----------------------------------------------------------------------------
#ifndef TRAFFICBASEVIEW_H
#define TRAFFICBASEVIEW_H

#include <QPaintEvent>

#include "Views/TrafficLanesView.h"
#include "Views/TrafficParametersView.h"
#include "Views/TrafficView.h"
#include "WidgetView.h"

//-----------------------------------------------------------------------------
//! @brief This class provides a resizable container widget for the content of
//!        the traffic tab's scroll area.
//!
//!        The items inserted into this widget are the instances of
//!        TrafficView, TrafficParametersView and TrafficLanesView.
//!        Further, the paint event is reimplemented such that the parameters and lanes
//!        views are connected via lines to the title widget (TrafficView).
//!        This visualizes that both configurations (parameters and lanes) are necessary
//!        for a complete traffic configuration.
//-----------------------------------------------------------------------------
class TrafficBaseView : public WidgetView
{
    Q_OBJECT

public:
    explicit TrafficBaseView(QWidget *parent = 0);
    ~TrafficBaseView() = default;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the traffic title view (TrafficView).
    //!
    //! @return             the traffic's title view (TrafficView).
    //-----------------------------------------------------------------------------
    TrafficView *getTrafficView() const;

    //-----------------------------------------------------------------------------
    //! Method which returns the configuration widget for traffic parameters
    //! (TrafficParametersView).
    //!
    //! @return             the configuration widget for traffic parameters
    //-----------------------------------------------------------------------------
    TrafficParametersView *getTrafficParametersView() const;

    //-----------------------------------------------------------------------------
    //! Method which returns the configuration widget for for assigning agent profiles
    //! to lanes (TrafficLanesView).
    //!
    //! @return             the configuration widget for lane-agent assignment (TrafficLanesView)
    //-----------------------------------------------------------------------------
    TrafficLanesView *getTrafficLanesView() const;

public Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which updates the size of this base view such that all child widgets
    //! (TrafficView, TrafficParametersView and TrafficLanesView) are visible.
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
    //! lines between the traffic's title widget (TrafficView) and
    //! the parameter and lane views is organised.
    //!
    //! @param[in]      event      the paint event (Qt's QPaintEvent class)
    //-----------------------------------------------------------------------------
    void paintEvent(QPaintEvent *event) override;

public:
    TrafficView *trafficView;                     //!< A pointer to the traffic configuration's title view
    TrafficLanesView *trafficLanesView;           //!< A pointer to the traffic parameter configuration widget (TrafficParametersView)
    TrafficParametersView *trafficParametersView; //!< A pointer to the widget for lane-agent assignement (TrafficLanesView)
};

#endif // TRAFFICBASEVIEW_H
