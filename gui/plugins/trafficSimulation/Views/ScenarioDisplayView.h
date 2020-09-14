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
//! @file  ScenarioDisplayView.h
//! @ingroup trafficSimulationPlugin
//! @brief This class provides a display for a quick overview of the scenario configuration.
//!
//!        It contains an #ui object describing the views appearence (defined in ScenarioDisplayView.ui).
//!        This ui object comprises a form layout with labels showing the configuration that has been
//!        edited by ScenarioView. The frame of the view can be coloured in order to notify the user
//!        whether the configuration is complete or not.
//-----------------------------------------------------------------------------
#ifndef SCENARIODISPLAYVIEW_H
#define SCENARIODISPLAYVIEW_H

#include <QMap>

#include "WidgetView.h"

namespace Ui {
class ScenarioDisplayView;
}

//-----------------------------------------------------------------------------
//! @brief This class provides a display for a quick overview of the scenario configuration.
//!
//!        It contains an #ui object describing the views appearence (defined in ScenarioDisplayView.ui).
//!        This ui object comprises a form layout with labels showing the configuration that has been
//!        edited by ScenarioView. The frame of the view can be coloured in order to notify the user
//!        whether the configuration is complete or not.
//-----------------------------------------------------------------------------
class ScenarioDisplayView : public WidgetView
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
    explicit ScenarioDisplayView(QWidget *parent = nullptr);
    ~ScenarioDisplayView();

public:
    //-----------------------------------------------------------------------------
    //! Method for setting the name which should be displayed for the scenario.
    //!
    //! @param[in]      name        the name to be displayed for the scenario
    //-----------------------------------------------------------------------------
    void setScenarioName(QString name);

    //-----------------------------------------------------------------------------
    //! Method for setting the name which should be displayed for the open scenario file.
    //!
    //! @param[in]      scenarioFile        the name to be displayed for the open scenario file
    //-----------------------------------------------------------------------------
    void setScenarioFile(QString scenarioFile);

    //-----------------------------------------------------------------------------
    //! Method for setting the number of scenario agents which should be displayed.
    //!
    //! @param[in]      scenarioAgents        the number of scenario agents to be displayed
    //-----------------------------------------------------------------------------
    void setScenarioAgents(int scenarioAgents);

    //-----------------------------------------------------------------------------
    //! Method which determines the frame color according to the status type passed
    //! as a first argument. Further, sets the status message to the second argument.
    //!
    //! @param[in]      status      the status type (defined in enum class #StatusType)
    //! @param[in]      message     the message to be displayed as a status.
    //-----------------------------------------------------------------------------
    void setStatus(StatusType status, QString message);
    //  void setStatus(StatusType status);

private:
    QMap<StatusType, QString> color; //!< a color map which determines the color code according to the status type
    Ui::ScenarioDisplayView *ui;     //!< the ui object containing the visualization elements of this display view
};

#endif // SCENARIODISPLAYVIEW_H
