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
//! @file  TrafficDisplayView.h
//! @ingroup trafficSimulationPlugin
//! @brief This class provides a display for a quick overview of the traffic configuration.
//!
//!        It contains an #ui object describing the views appearence (defined in TrafficDisplayView.ui).
//!        This ui object comprises a form layout with labels showing the configuration that has been
//!        edited by TrafficView, i.e. the number of traffic parameters and lane agent profiles.
//!        The frame of the view can be coloured in order to notify the user whether the configuration
//!        is complete or not.
//-----------------------------------------------------------------------------
#ifndef TRAFFICDISPLAYVIEW_H
#define TRAFFICDISPLAYVIEW_H

#include <QMap>

#include "WidgetView.h"

namespace Ui {
class TrafficDisplayView;
}

//-----------------------------------------------------------------------------
//! @brief This class provides a display for a quick overview of the traffic configuration.
//!
//!        It contains an #ui object describing the views appearence (defined in TrafficDisplayView.ui).
//!        This ui object comprises a form layout with labels showing the configuration that has been
//!        edited by TrafficView, i.e. the number of traffic parameters and lane agent profiles.
//!        The frame of the view can be coloured in order to notify the user whether the configuration
//!        is complete or not.
//-----------------------------------------------------------------------------
class TrafficDisplayView : public WidgetView
{
    Q_OBJECT

public:
    enum class StatusType
    { //!< enum class for classifying the status (will be used to determine the color of the frame)
        Error = 0,
        OK = 1,
        Warning = 2
    };

public:
    explicit TrafficDisplayView(QWidget *parent = 0);
    ~TrafficDisplayView();

public:
    //-----------------------------------------------------------------------------
    //! Method for setting the name which should be displayed for the traffic configuration.
    //!
    //! @param[in]      name        the name to be displayed for the traffic configuration
    //-----------------------------------------------------------------------------
    void setTrafficName(QString name);

    //-----------------------------------------------------------------------------
    //! Method for setting the displayed number of traffic volumes.
    //!
    //! @param[in]      number        the number of traffic volumes
    //-----------------------------------------------------------------------------
    void setVolumes(int number);

    //-----------------------------------------------------------------------------
    //! Method for setting the displayed number of platoon rates.
    //!
    //! @param[in]      number        the number of platoon rates
    //-----------------------------------------------------------------------------
    void setPlatoonRates(int number);

    //-----------------------------------------------------------------------------
    //! Method for setting the displayed number of velocities.
    //!
    //! @param[in]      number        the number of velocities
    //-----------------------------------------------------------------------------
    void setVelocities(int number);

    //-----------------------------------------------------------------------------
    //! Method for setting the displayed number of homogeneities.
    //!
    //! @param[in]      number        the number of homogeneities.
    //-----------------------------------------------------------------------------
    void setHomogeneities(int number);

    //-----------------------------------------------------------------------------
    //! Method for setting the displayed number of regular lane agents.
    //!
    //! @param[in]      number        the number of regular lane agents
    //-----------------------------------------------------------------------------
    void setRegularLaneAgents(int number);

    //-----------------------------------------------------------------------------
    //! Method for setting the displayed number of rightmost lane agents.
    //!
    //! @param[in]      number        the number of rightmost lane agents
    //-----------------------------------------------------------------------------
    void setRightmostLaneAgents(int number);

    //-----------------------------------------------------------------------------
    //! Method which determines the frame color according to the status type passed
    //! as a first argument. Further, sets the status message to the second argument.
    //!
    //! @param[in]      status      the status type (defined in enum class #StatusType)
    //! @param[in]      message     the message to be displayed as a status.
    //-----------------------------------------------------------------------------
    void setStatus(StatusType status, QString message);

private:
    QMap<StatusType, QString> color; //!< a color map which determines the color code according to the status type
    Ui::TrafficDisplayView *ui;      //!< the ui object containing the visualization elements of this display view
};

#endif // TRAFFICDISPLAYVIEW_H
