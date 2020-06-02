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
//! @file  EnvironmentDisplayPresenter.h
//! @ingroup trafficSimulationPlugin
//! @brief This class defines the presenter which controls the content to be
//!        displayed in the EnvironmentDisplayView widget.
//-----------------------------------------------------------------------------

#ifndef ENVIRONMENTDISPLAYPRESENTER_H
#define ENVIRONMENTDISPLAYPRESENTER_H

#include "interfaces/openPASS-TrafficSimulation/EnvironmentInterface.h"
//#include "Presenters/EnvironmentPresenter.h"
#include "Views/EnvironmentDisplayView.h"

#include <QObject>

//-----------------------------------------------------------------------------
//! @brief This class defines the presenter which controls the content to be
//!        displayed in the EnvironmentDisplayView widget.
//-----------------------------------------------------------------------------
class EnvironmentDisplayPresenter : public QObject
{
    Q_OBJECT
public:
    explicit EnvironmentDisplayPresenter(EnvironmentInterface * environment,
                                         EnvironmentDisplayView * environmentDisplayView,
                                         QObject *parent = nullptr);
    ~EnvironmentDisplayPresenter() = default;

public Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Method which sets all displayed values according to the model data. Will be
    //! called after loading a combination xml file.
    //-----------------------------------------------------------------------------
    void updateView();

private:
    //-----------------------------------------------------------------------------
    //! Slot which is called when the name of the environment configuration has changed in the model.
    //! Updates the displayed name and triggers a status check.
    //-----------------------------------------------------------------------------
    void setEnvironmentNameView();

    //-----------------------------------------------------------------------------
    //! Slot which is called when the user edits the time of day map. Updates
    //! the displayed number of map entries accordingly and triggers a status check.
    //-----------------------------------------------------------------------------
    void setTimeOfDayView();

    //-----------------------------------------------------------------------------
    //! Slot which is called when the user edits the visibility distance map. Updates
    //! the displayed number of map entries accordingly and triggers a status check.
    //-----------------------------------------------------------------------------
    void setVisibilityDistanceView();

    //-----------------------------------------------------------------------------
    //! Slot which is called when the user edits the friction coefficient map. Updates
    //! the displayed number of map entries accordingly and triggers a status check.
    //-----------------------------------------------------------------------------
    void setFrictionView();

    //-----------------------------------------------------------------------------
    //! Slot which is called when the user edits the weather map. Updates
    //! the displayed number of map entries accordingly and triggers a status check.
    //-----------------------------------------------------------------------------
    void setWeatherView();

    //-----------------------------------------------------------------------------
    //! Method which checks whether the probabilities of the environment items
    //! registered in the given map sum up to 1.
    //!
    //! @param[in]  items       the map of environment items to be checked
    //! @return                 "true" if probabilities sum up to 1, otherwise
    //!                         "false"
    //-----------------------------------------------------------------------------
    bool checkProbabilities(QMap<EnvironmentInterface::ID, EnvironmentInterface::Item*> * const items);

public:
    //-----------------------------------------------------------------------------
    //! Method which determines the status of the environment settings. If all necessary
    //! configurations are made, the method displays the status "complete" and sets
    //! the frame of the display widget to green color. If "incomplete", the frame
    //! will be set to red color. Momentarily, the criterion for a complete configuration
    //! is that there is at least one item for the time of day, visibility distance,
    //! friction and weather.
    //-----------------------------------------------------------------------------
    void setStatus();

private:
    EnvironmentInterface * environment;                    //!< a pointer to the environment model (interface) which is read by this presenter
    EnvironmentDisplayView * environmentDisplayView;       //!< a pointer to the display view whose content is managed by this presenter
};

#endif // ENVIRONMENTDISPLAYPRESENTER_H
