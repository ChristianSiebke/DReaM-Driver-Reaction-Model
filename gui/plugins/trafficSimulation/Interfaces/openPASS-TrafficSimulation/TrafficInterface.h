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
//! @file  TrafficInterface.h
//! @ingroup trafficSimulationPlugin
//! @brief The Traffic Interface provides setter and getter methods for the combination
//!        XML subelements listed under the <TrafficConfig> element.
//!
//!        These comprise the maps for registering traffic volumes, platoon rates,
//!        velocities, homogeneities as well as for managing the distribution of agents
//!        on regular lanes and the rightmost lane. The corresponding maps are defined
//!        by the template class called TrafficItemMapInterface, whereas the aforementioned
//!        objects are defined in the class TrafficItemInterface.
//-----------------------------------------------------------------------------

#ifndef TRAFFICINTERFACE_H
#define TRAFFICINTERFACE_H

#include <QObject>

#include "openPASS-TrafficSimulation/TrafficItemMapInterface.h"

//-----------------------------------------------------------------------------
//! @brief The Traffic Interface provides setter and getter methods for the combination
//!        XML subelements listed under the <TrafficConfig> element.
//!
//!        These comprise the maps for registering traffic volumes, platoon rates,
//!        velocities, homogeneities as well as for managing the distribution of agents
//!        on regular lanes and the rightmost lane. The corresponding maps are defined
//!        by the template class called TrafficItemMapInterface , whereas the aforementioned
//!        objects are defined in the class TrafficItemInterface.
//-----------------------------------------------------------------------------
class TrafficInterface : public QObject
{
    Q_OBJECT

public:
    using Name = QString;                          //!< The name of the traffic configuration
    using Volumes = TrafficItemMapInterface;       //!< The map for registering traffic volumes
    using PlatoonRates = TrafficItemMapInterface;  //!< The map for registering platoon rates
    using Velocities = TrafficItemMapInterface;    //!< The map for registering velocities
    using Homogeneities = TrafficItemMapInterface; //!< The map for registering homogeneities
    using TrafficAgents = TrafficItemMapInterface; //!< The map for registering agent profiles
    using Status = bool;                           //!< Type characterizing the status of the environment settings

public:
    explicit TrafficInterface(QObject *parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~TrafficInterface() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted when the name of the traffic setup has been modified.
    //-----------------------------------------------------------------------------
    void modifiedName();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the traffic configuration has been edited.
    //-----------------------------------------------------------------------------
    void edited();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the traffic's status has changed.
    //-----------------------------------------------------------------------------
    void modifiedStatus();

public:
    //-----------------------------------------------------------------------------
    //! Sets the name of the traffic setup to the one passed as an argument.
    //!
    //! @param[in]      _name       the new name for the traffic setup
    //-----------------------------------------------------------------------------
    virtual void setName(Name const &_name) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the current name of the traffic setup.
    //!
    //! @return                the current name of the traffic setup
    //-----------------------------------------------------------------------------
    virtual Name getName() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Returns the map for registering traffic volumes.
    //!
    //! @return                the map of traffic volumes
    //-----------------------------------------------------------------------------
    virtual Volumes *getVolumes() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the map for registering platoon rates.
    //!
    //! @return                the map of platoon rates
    //-----------------------------------------------------------------------------
    virtual PlatoonRates *getPlatoonRates() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the map for registering velocities.
    //!
    //! @return                the map of velocities
    //-----------------------------------------------------------------------------
    virtual Velocities *getVelocities() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the map for registering homogeneities.
    //!
    //! @return                the map of homogeneities
    //-----------------------------------------------------------------------------
    virtual Homogeneities *getHomogeneities() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the map for registering agent profiles for regular lanes.
    //!
    //! @return               the map of agent profiles for regular lanes
    //-----------------------------------------------------------------------------
    virtual TrafficAgents *getAgentsRegularLane() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the map for registering agent profiles for the rightmost lane.
    //!
    //! @return               the map of agent profiles for the rightmost lane
    //-----------------------------------------------------------------------------
    virtual TrafficAgents *getAgentsRightMostLane() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method for setting the status of the traffic configuration to the
    //! value passed as an argument.
    //!
    //! @param[in]      _status     the status
    //-----------------------------------------------------------------------------
    virtual void setStatus(Status const &_status) = 0;

    //-----------------------------------------------------------------------------
    //! Method  which returns the current status of the traffic settings.
    //!
    //! @return                 the current status of the traffic configuration
    //-----------------------------------------------------------------------------
    virtual Status getStatus() const = 0;
};

#endif // TRAFFICINTERFACE_H
