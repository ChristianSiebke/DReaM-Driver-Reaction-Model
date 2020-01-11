/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "TrafficDisplayPresenter.h"

TrafficDisplayPresenter::TrafficDisplayPresenter(TrafficInterface *traffic,
                                                 TrafficDisplayView *trafficDisplayView,
                                                 QList<QString> * const agentList,
                                                 QObject *parent)
    : QObject(parent)
    , traffic(traffic)
    , trafficDisplayView(trafficDisplayView)
    , agentList(agentList)
{

    connect(traffic, &TrafficInterface::modifiedName, this, &TrafficDisplayPresenter::setTrafficNameView);
    connect(traffic, &TrafficInterface::edited, this, &TrafficDisplayPresenter::updateView);

    updateView();
    setStatus();
}


void TrafficDisplayPresenter::setTrafficNameView()
{
    trafficDisplayView->setTrafficName(traffic->getName());

    setStatus();
}


void TrafficDisplayPresenter::setVolumesView()
{
    trafficDisplayView->setVolumes(traffic->getVolumes()->count());
    setStatus();
}

void TrafficDisplayPresenter::setPlatoonRatesView()
{
    trafficDisplayView->setPlatoonRates(traffic->getPlatoonRates()->count());
    setStatus();
}


void TrafficDisplayPresenter::setVelocitiesView()
{
   trafficDisplayView->setVelocities(traffic->getVelocities()->count());
   setStatus();
}

void TrafficDisplayPresenter::setHomogeneitiesView()
{
   trafficDisplayView->setHomogeneities(traffic->getHomogeneities()->count());
   setStatus();
}

void TrafficDisplayPresenter::setRegularLaneView()
{
    trafficDisplayView->setRegularLaneAgents(traffic->getAgentsRegularLane()->count());
    setStatus();
}

void TrafficDisplayPresenter::setRightmostLaneView()
{
    trafficDisplayView->setRightmostLaneAgents(traffic->getAgentsRightMostLane()->count());
    setStatus();
}


void TrafficDisplayPresenter::updateView()
{
    setTrafficNameView();

    setVolumesView();
    setPlatoonRatesView();
    setVelocitiesView();
    setHomogeneitiesView();

    setRegularLaneView();
    setRightmostLaneView();

    setStatus();
}

void TrafficDisplayPresenter::setStatus()
{
    bool ok = true;

    if(traffic->getVolumes()->count() == 0 || !checkProbabilities(traffic->getVolumes()))
        ok = false;

    else if(traffic->getPlatoonRates()->count() == 0 || !checkProbabilities(traffic->getPlatoonRates()))
        ok = false;

    else if(traffic->getVelocities()->count() == 0 || !checkProbabilities(traffic->getVelocities()))
        ok = false;

    else if(traffic->getHomogeneities()->count() == 0 || !checkProbabilities(traffic->getHomogeneities()))
        ok = false;

    else if(traffic->getAgentsRegularLane()->count() == 0 || !checkProbabilities(traffic->getAgentsRegularLane()))
        ok = false;

    else if(traffic->getAgentsRightMostLane()->count() == 0 || !checkProbabilities(traffic->getAgentsRightMostLane()))
        ok = false;

    else if(!checkAgentProfiles())
        ok = false;


    if(ok && traffic->getName().isEmpty())
        trafficDisplayView->setStatus(TrafficDisplayView::StatusType::Warning, "No name specified");

    else if(ok)
        trafficDisplayView->setStatus(TrafficDisplayView::StatusType::OK, "OK");

    else
        trafficDisplayView->setStatus(TrafficDisplayView::StatusType::Error, "Error");

    traffic->setStatus(ok);
}

bool TrafficDisplayPresenter::checkProbabilities(TrafficItemMapInterface * const items)
{
    double sum = 0.0;

    for(auto item : items->values())
        sum += item->getProbability();

    return sum == 1.0;
}


bool TrafficDisplayPresenter::checkAgentProfiles()
{

    // determine list of all used agent profiles
    QList<TrafficItemInterface*> allAgents = traffic->getAgentsRegularLane()->values()
                                            + traffic->getAgentsRightMostLane()->values();

    // check whether used agent profiles are contained in the agentList!
    // (agentList reflects all defined agent profiles in the profiles catalogue)
    for(auto agent : allAgents)
    {
        TrafficItemInterface::Value name = agent->getValue();
        if(!agentList->contains(name))
            return false;
    }

    return true;
}
