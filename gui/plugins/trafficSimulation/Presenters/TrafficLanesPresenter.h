/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
//-----------------------------------------------------------------------------
//! @file  TrafficLanesPresenter.h
//! @ingroup trafficSimulationPlugin
//! @brief This class defines the presenter object which handles events coming from
//!        the view (TrafficLanesView) and edits the model accordingly (TrafficInterface).
//!
//!        More specifically, it manages the user interactions for the assignment of agent
//!        profiles to traffic lanes
//!
//! @note  This presenter class emits signals after editing the model which are noticed by the
//!        TrafficDisplayPresenter class. Normally, these signals should be emitted by the model
//!        itself but since it invokes template class instances, for which Qt does not support the
//!        signal-slot mechanism, signals are exceptionally handled by this presenter class.
//-----------------------------------------------------------------------------
#ifndef TRAFFICLANESPRESENTER_H
#define TRAFFICLANESPRESENTER_H

#include <QObject>

#include "Views/TrafficLanesView.h"
#include "openPASS-TrafficSimulation/TrafficInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class defines the presenter object which handles events coming from
//!        the view (TrafficLanesView) and edits the model accordingly (TrafficInterface).
//!
//!        More specifically, it manages the user interactions for the assignment of agent
//!        profiles to traffic lanes
//!
//! @note  This presenter class emits signals after editing the model which are noticed by the
//!        TrafficDisplayPresenter class. Normally, these signals should be emitted by the model
//!        itself but since it invokes template class instances, for which Qt does not support the
//!        signal-slot mechanism, signals are exceptionally handled by this presenter class.
//-----------------------------------------------------------------------------
class TrafficLanesPresenter : public QObject
{
    Q_OBJECT
public:
    explicit TrafficLanesPresenter(QList<QString> *agentProfiles,
                                   TrafficInterface *traffic,
                                   TrafficLanesView *trafficLanesView,
                                   QObject *parent = 0);
Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted when an agent profile has been added to a regular lane
    //! in the model.
    //-----------------------------------------------------------------------------
    void RegularLaneAgent_added();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when an agent profile has been removed from a regular lane
    //! in the model.
    //-----------------------------------------------------------------------------
    void RegularLaneAgent_removed();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the probability of a regular lane agent's profile has
    //! been changed in the model.
    //-----------------------------------------------------------------------------
    void RegularLaneAgent_probabilityChanged();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when an agent profile has been added to the rightmost lane
    //! in the model.
    //-----------------------------------------------------------------------------
    void RightmostLaneAgent_added();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when an agent profile has been removed from the rightmost lane
    //! in the model.
    //-----------------------------------------------------------------------------
    void RightmostLaneAgent_removed();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the probability of a rightmost lane agent's profile has
    //! been changed in the model.
    //-----------------------------------------------------------------------------
    void RightmostLaneAgent_probabilityChanged();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the whole configuration has been cleared, i.e. all agent
    //! profiles have been removed from the regular and rightmost lanes.
    //-----------------------------------------------------------------------------
    void cleared();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the add-button has been clicked in the regular lanes
    //! section of TrafficLanesView. Adds the transmitted agent profile to the map of
    //! regular lane agents and creates the corresponding view. The agent profile is
    //! only added if it has not been already incorporated.
    //!
    //! @param[in]      name        the name of the agent profile to be added
    //-----------------------------------------------------------------------------
    void addRegularLaneAgent(QString const &name);

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the add-button has been clicked in the rightmost lane
    //! section of TrafficLanesView. Adds the transmitted agent profile to the map of
    //! rightmost lane agents and creates the corresponding view. The agent profile is
    //! only added if it has not been already incorporated.
    //!
    //! @param[in]      name        the name of the agent profile to be added
    //-----------------------------------------------------------------------------
    void addRightmostLaneAgent(QString const &name);

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the remove-button has been clicked for
    //! a regular lane agent view. Removes the agent profile with the transmitted ID
    //! from the map and destroys the corresponding view. Nothing will be done if no
    //! agent profile of the given ID exists.
    //!
    //! @param[in]      id        the ID of the agent profile to be removed
    //-----------------------------------------------------------------------------
    void removeRegularLaneAgent(unsigned int id);

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the remove-button has been clicked for
    //! a rightmost lane agent view. Removes the agent profile with the transmitted ID
    //! from the map and destroys the corresponding view. Nothing will be done if no
    //! agent profile of the given ID exists.
    //!
    //! @param[in]      id        the ID of the agent profile to be removed
    //-----------------------------------------------------------------------------
    void removeRightmostLaneAgent(unsigned int id);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the displayed probability value of a regular
    //! lane agent has been edited by the user. Adopts the transmitted value (second
    //! argument) for the model probability of the agent profile with the given ID (first
    //! argument).
    //!
    //! @param[in]      id        the ID of the agent profile
    //! @param[in]      value     the probability to be adopted
    //-----------------------------------------------------------------------------
    void setRegularLaneAgentProbability(unsigned int id, double value);

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the displayed probability value of a rightmost
    //! lane agent has been edited by the user. Adopts the transmitted value (second
    //! argument) for the model probability of the agent profile with the given ID (first
    //! argument).
    //!
    //! @param[in]      id        the ID of the agent profile
    //! @param[in]      value     the probability to be adopted
    //-----------------------------------------------------------------------------
    void setRightmostLaneAgentProbability(unsigned int id, double value);

public:
    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the map of available agent profiles (Agent Configuration
    //! Plugin) has been cleared. Deletes all regular and rightmost lane agent views and empties
    //! the selection lists of the comboboxes.
    //-----------------------------------------------------------------------------
    void clearAgentSelectionView();

    void setAgentSelectionViews(const QStringList &names);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which acts as #removeRightmostLaneAgent but uses a name instead of the ID.
    //! This slot is connected to the removed signal of the agent map in the Agent Configuration
    //! Plugin which works with names instead of IDs.
    //!
    //! @param[in]      name        the name of the rightmost agent profile to be removed
    //-----------------------------------------------------------------------------
    void removeRightmostLaneAgentByName(QString name);

    //-----------------------------------------------------------------------------
    //! Slot which acts as #removeRegularLaneAgent but uses a name instead of the ID.
    //! This slot is connected to the removed signal of the agent map in the Agent Configuration
    //! Plugin which works with names instead of IDs.
    //!
    //! @param[in]      name        the name of the regular agent profile to be removed
    //-----------------------------------------------------------------------------
    void removeRegularLaneAgentByName(QString name);

public:
    //-----------------------------------------------------------------------------
    //! Method which updates the whole TrafficLanesView widget to display the
    //! entire model configuration. Will be called after loading a combination xml
    //! file.
    //-----------------------------------------------------------------------------
    void updateView();

private:
    QList<QString> *agentProfiles;
    TrafficInterface *traffic;          //!< a pointer to the traffic model edited by this presenter
    TrafficLanesView *trafficLanesView; //!< a pointer to the editor view whose user events are managed by this presenter
};

#endif // TRAFFICLANESPRESENTER_H
