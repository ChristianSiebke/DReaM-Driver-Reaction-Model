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
//! @file  TrafficDisplayPresenter.h
//! @ingroup trafficSimulationPlugin
//! @brief This class defines the presenter which controls the content to be
//!        displayed in the TrafficDisplayView widget.
//-----------------------------------------------------------------------------

#ifndef TRAFFICDISPLAYPRESENTER_H
#define TRAFFICDISPLAYPRESENTER_H

#include <QObject>

#include "Presenters/TrafficPresenter.h"
#include "Views/TrafficDisplayView.h"
#include "openPASS-TrafficSimulation/TrafficInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class defines the presenter which controls the content to be
//!        displayed in the TrafficDisplayView widget.
//-----------------------------------------------------------------------------
class TrafficDisplayPresenter : public QObject
{
    Q_OBJECT
public:
    explicit TrafficDisplayPresenter(TrafficInterface *traffic,
                                     TrafficDisplayView *trafficDisplayView,
                                     QList<QString> *const agentList,
                                     QObject *parent = nullptr);

    ~TrafficDisplayPresenter() = default;

public Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Method which sets all displayed values according to the model data. Will be
    //! called after loading a combination xml file.
    //-----------------------------------------------------------------------------
    void updateView();

private:
    //-----------------------------------------------------------------------------
    //! Slot which is called when the name of the traffic configuration has changed in the model.
    //! Updates the displayed name and triggers a status check.
    //-----------------------------------------------------------------------------
    void setTrafficNameView();

    //-----------------------------------------------------------------------------
    //! Slot which is called when the user edits the traffic volumes map. Updates
    //! the displayed number of traffic volumes accordingly and triggers a status check.
    //-----------------------------------------------------------------------------
    void setVolumesView();

    //-----------------------------------------------------------------------------
    //! Slot which is called when the user edits the platoon reates map. Updates
    //! the displayed number of platoon rates accordingly and triggers a status check.
    //-----------------------------------------------------------------------------
    void setPlatoonRatesView();

    //-----------------------------------------------------------------------------
    //! Slot which is called when the user edits the velocities map. Updates
    //! the displayed number of velocities accordingly and triggers a status check.
    //-----------------------------------------------------------------------------
    void setVelocitiesView();

    //-----------------------------------------------------------------------------
    //! Slot which is called when the user edits the homogeneities map. Updates
    //! the displayed number of homogeneities accordingly and triggers a status check.
    //-----------------------------------------------------------------------------
    void setHomogeneitiesView();

    //-----------------------------------------------------------------------------
    //! Slot which is called when the user edits the regular lane agents map. Updates
    //! the displayed number of agents on the regular lane accordingly and triggers a
    //! status check.
    //-----------------------------------------------------------------------------
    void setRegularLaneView();

    //-----------------------------------------------------------------------------
    //! Slot which is called when the user edits the rightmost lane agents map. Updates
    //! the displayed number of agents on the rightmost lane accordingly and triggers a
    //! status check.
    //-----------------------------------------------------------------------------
    void setRightmostLaneView();

private:
    //-----------------------------------------------------------------------------
    //! Method which checks whether the given probabilities (stored in a list) sum
    //! up to 1.
    //!
    //! @param[in]  probabilities       the list of probabilities to be checked
    //! @return                         "true" if probabilities sum up to 1, otherwise
    //!                                 "false"
    //-----------------------------------------------------------------------------
    bool checkProbabilities(TrafficItemMapInterface *const items);

    //-----------------------------------------------------------------------------
    //! Method which checks whether the agent profiles assigned to the lanes
    //! are defined in the profiles catalogue referenced by the traffic simulation
    //! plugin.
    //!
    //! @return             "true" if all agent profiles are defined in the profiles
    //!                     catalogue, "false" otherwise
    //-----------------------------------------------------------------------------
    bool checkAgentProfiles();

public:
    //-----------------------------------------------------------------------------
    //! Method which determines the status of the traffic settings. If all necessary
    //! configurations are made, the method displays the status "complete" and sets
    //! the frame of the display widget to green color. If "incomplete", the frame
    //! will be set to red color. Momentarily, the criterion for a complete configuration
    //! is that all forms in the TrafficView, TrafficParameters and TrafficLanes widgets
    //! are filled.
    //-----------------------------------------------------------------------------
    void setStatus();

private:
    TrafficInterface *traffic;              //!< a pointer to the traffic model (interface) which is read by this presenter
    TrafficDisplayView *trafficDisplayView; //!< a pointer to the display view whose content is managed by this presenter
    QList<QString> *const agentList;        //!< a pointer to the list of available agent profiles (defined in a ProfilesCatalog.xml)
};

#endif // TRAFFICDISPLAYPRESENTER_H
