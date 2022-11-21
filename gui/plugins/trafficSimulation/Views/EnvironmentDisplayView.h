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
//! @file  EnvironmentDisplayView.h
//! @ingroup trafficSimulationPlugin
//! @brief This class provides a display for a quick overview of the environment configuration.
//!
//!        It contains an #ui object describing the views appearence (defined in EnvironmentDisplayView.ui).
//!        This ui object comprises a form layout with labels showing the configuration that has been
//!        edited by EnvironmentView. The frame of the view can be coloured in order to notify the user
//!        whether the configuration is complete or not.
//-----------------------------------------------------------------------------
#ifndef ENVIRONMENTDISPLAYVIEW_H
#define ENVIRONMENTDISPLAYVIEW_H

#include <QMap>
#include <QString>

#include "WidgetView.h"

namespace Ui {
class EnvironmentDisplayView;
}

//-----------------------------------------------------------------------------
//! @brief This class provides a display for a quick overview of the environment configuration.
//!
//!        It contains an #ui object describing the views appearence (defined in EnvironmentDisplayView.ui).
//!        This ui object comprises a form layout with labels showing the configuration that has been
//!        edited by EnvironmentView. The frame of the view can be coloured in order to notify the user
//!        whether the configuration is complete or not.
//-----------------------------------------------------------------------------
class EnvironmentDisplayView : public WidgetView
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
    explicit EnvironmentDisplayView(QWidget *parent = 0);
    ~EnvironmentDisplayView();

public:
    //-----------------------------------------------------------------------------
    //! Method for setting the name which should be displayed for the environment.
    //!
    //! @param[in]      name        the name to be displayed for the environment
    //-----------------------------------------------------------------------------
    void setEnvironmentName(QString name);

    //-----------------------------------------------------------------------------
    //! Method for setting the displayed number of time of day map entries.
    //!
    //! @param[in]      number        the number of time of day map entries
    //-----------------------------------------------------------------------------
    void setTimeOfDay(int number);

    //-----------------------------------------------------------------------------
    //! Method for setting the displayed number of visibility distance map entries.
    //!
    //! @param[in]      number        the number of visibility distance map entries
    //-----------------------------------------------------------------------------
    void setVisibilityDistance(int number);

    //-----------------------------------------------------------------------------
    //! Method for setting the displayed number of friction map entries.
    //!
    //! @param[in]      number        the number of friction map entries
    //-----------------------------------------------------------------------------
    void setFriction(int number);

    //-----------------------------------------------------------------------------
    //! Method for setting the displayed number of weather map entries.
    //!
    //! @param[in]      number        the number of weather map entries
    //-----------------------------------------------------------------------------
    void setWeather(int number);

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
    Ui::EnvironmentDisplayView *ui;  //!< the ui object containing the visualization elements of this display view
};

#endif // ENVIRONMENTDISPLAYVIEW_H
