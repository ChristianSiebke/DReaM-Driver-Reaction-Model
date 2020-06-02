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
//! @file  TrafficModel.h
//! @ingroup trafficSimulationPlugin
//! @brief The TrafficModel class implements the functionality of the
//!        TrafficInterface class.
//-----------------------------------------------------------------------------

#ifndef TRAFFICMODEL_H
#define TRAFFICMODEL_H

#include "interfaces/openPASS-TrafficSimulation/TrafficInterface.h"

//-----------------------------------------------------------------------------
//! @brief The TrafficModel class implements the functionality of the
//!        TrafficInterface class.
//-----------------------------------------------------------------------------
class TrafficModel : public TrafficInterface
{
    Q_OBJECT


public:
    TrafficModel(QObject *parent = nullptr);
    virtual ~TrafficModel() override;

public:
    virtual void setName(Name const &_name) override;
    virtual Name getName() const override;

public:
    virtual Volumes * getVolumes() const override;
    virtual PlatoonRates * getPlatoonRates() const override;
    virtual Velocities * getVelocities() const override;
    virtual Homogeneities * getHomogeneities() const override;
    virtual TrafficAgents * getAgentsRegularLane() const override;
    virtual TrafficAgents * getAgentsRightMostLane() const override;

public:
    virtual void setStatus(Status const &_status) override;
    virtual Status getStatus() const override;

public:
    Name name;                                  //!< the name of the traffic configuration
    Volumes * const volumes;                    //!< the map of traffic volumes
    PlatoonRates * const platoonRates;          //!< the map of platoon rates
    Velocities * const velocities;              //!< the map of velocities
    Homogeneities * const homogeneities;        //!< the map of homogeneities
    TrafficAgents * const agentsRegularLane;    //!< the map of traffic agents driving on regular lanes
    TrafficAgents * const agentsRightMostLane;  //!< the map of traffic agents driving on the rightmost lane
    Status status;                              //!< the current status of the settings. "true" means that the configuration is principally ready for simulation
};

#endif // TRAFFICMODEL_H
