/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "TrafficModel.h"

#include "TrafficItemMapModel.h"

TrafficModel::TrafficModel(QObject *parent)
  : TrafficInterface(parent)
  , name(QString(""))
  , volumes(new TrafficItemMapModel(this))
  , platoonRates(new TrafficItemMapModel(this))
  , velocities(new TrafficItemMapModel(this))
  , homogeneities(new TrafficItemMapModel(this))
  , agentsRegularLane(new TrafficItemMapModel(this))
  , agentsRightMostLane(new TrafficItemMapModel(this))
{ 

    connect(volumes, &TrafficItemMapInterface::edited, this, &TrafficInterface::edited);
    connect(platoonRates, &TrafficItemMapInterface::edited, this, &TrafficInterface::edited);
    connect(velocities, &TrafficItemMapInterface::edited, this, &TrafficInterface::edited);
    connect(homogeneities, &TrafficItemMapInterface::edited, this, &TrafficInterface::edited);
    connect(agentsRegularLane, &TrafficItemMapInterface::edited, this, &TrafficInterface::edited);
    connect(agentsRightMostLane, &TrafficItemMapInterface::edited, this, &TrafficInterface::edited);
}

TrafficModel::~TrafficModel()
{
    delete volumes;
    delete platoonRates;
    delete velocities;
    delete homogeneities;
    delete agentsRegularLane;
    delete agentsRightMostLane;
}


TrafficInterface::Name TrafficModel::getName() const
{
    return name;
}

void TrafficModel::setName(Name const &_name)
{
    name = _name;
    Q_EMIT modifiedName();
}

TrafficInterface::Volumes * TrafficModel::getVolumes() const
{
    return volumes;
}

TrafficInterface::PlatoonRates * TrafficModel::getPlatoonRates() const
{
    return platoonRates;
}

TrafficInterface::Velocities * TrafficModel::getVelocities() const
{
    return velocities;
}

TrafficInterface::Homogeneities * TrafficModel::getHomogeneities() const
{
    return homogeneities;
}

TrafficInterface::TrafficAgents * TrafficModel::getAgentsRightMostLane() const
{
    return agentsRightMostLane;
}

TrafficInterface::TrafficAgents * TrafficModel::getAgentsRegularLane() const
{
    return agentsRegularLane;
}

void TrafficModel::setStatus(const Status &_status)
{
    status = _status;
    Q_EMIT modifiedStatus();
}

TrafficInterface::Status TrafficModel::getStatus() const
{
    return status;
}
