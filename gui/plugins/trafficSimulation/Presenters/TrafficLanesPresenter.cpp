/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "TrafficLanesPresenter.h"

TrafficLanesPresenter::TrafficLanesPresenter(QList<QString> *agentProfiles,
                                             TrafficInterface *traffic,
                                             TrafficLanesView *trafficLanesView,
                                             QObject *parent) :
    QObject(parent), agentProfiles(agentProfiles), traffic(traffic), trafficLanesView(trafficLanesView)
{
    connect(trafficLanesView, &TrafficLanesView::addRegularLaneAgent, this, &TrafficLanesPresenter::addRegularLaneAgent);
    connect(trafficLanesView, &TrafficLanesView::addRightmostLaneAgent, this, &TrafficLanesPresenter::addRightmostLaneAgent);

    connect(trafficLanesView, &TrafficLanesView::removeRegularLaneAgent, this, &TrafficLanesPresenter::removeRegularLaneAgent);
    connect(trafficLanesView, &TrafficLanesView::removeRightmostLaneAgent, this, &TrafficLanesPresenter::removeRightmostLaneAgent);

    connect(trafficLanesView, &TrafficLanesView::RegularLaneAgent_probabilityChanged, this, &TrafficLanesPresenter::setRegularLaneAgentProbability);
    connect(trafficLanesView, &TrafficLanesView::RightmostLaneAgent_probabilityChanged, this, &TrafficLanesPresenter::setRightmostLaneAgentProbability);
}

void TrafficLanesPresenter::addRegularLaneAgent(const QString &name)
{
    unsigned int id = traffic->getAgentsRegularLane()->generateID();
    QList<QString> names;

    for (TrafficItemInterface<QString> *agent : traffic->getAgentsRegularLane()->values())
        names.append(agent->getValue());

    if (agentProfiles->contains(name) && !names.contains(name))
    {
        traffic->getAgentsRegularLane()->add(id, name);
        trafficLanesView->addRegularLaneAgentView(id, name, 0.5);
        Q_EMIT RegularLaneAgent_added();
    }
}

void TrafficLanesPresenter::addRightmostLaneAgent(const QString &name)
{
    unsigned int id = traffic->getAgentsRightMostLane()->generateID();
    QList<QString> names;

    for (TrafficItemInterface<QString> *agent : traffic->getAgentsRightMostLane()->values())
        names.append(agent->getValue());

    if (agentProfiles->contains(name) && !names.contains(name))
    {
        traffic->getAgentsRightMostLane()->add(id, name);
        trafficLanesView->addRightmostLaneAgentView(id, name, 0.5);
        Q_EMIT RightmostLaneAgent_added();
    }
}

void TrafficLanesPresenter::removeRegularLaneAgent(unsigned int id)
{
    if (traffic->getAgentsRegularLane()->contains(id))
    {
        traffic->getAgentsRegularLane()->remove(id);
        trafficLanesView->removeRegularLaneAgentView(id);
        Q_EMIT RegularLaneAgent_removed();
    }
}

void TrafficLanesPresenter::removeRightmostLaneAgent(unsigned int id)
{
    if (traffic->getAgentsRightMostLane()->contains(id))
    {
        traffic->getAgentsRightMostLane()->remove(id);
        trafficLanesView->removeRightmostLaneAgentView(id);
        Q_EMIT RightmostLaneAgent_removed();
    }
}

void TrafficLanesPresenter::setRegularLaneAgentProbability(unsigned int id, double value)
{
    if (traffic->getAgentsRegularLane()->contains(id))
    {
        traffic->getAgentsRegularLane()->getItem(id)->setProbability(value);
        Q_EMIT RegularLaneAgent_probabilityChanged();
    }
}

void TrafficLanesPresenter::setRightmostLaneAgentProbability(unsigned int id, double value)
{
    if (traffic->getAgentsRightMostLane()->contains(id))
    {
        traffic->getAgentsRightMostLane()->getItem(id)->setProbability(value);
        Q_EMIT RightmostLaneAgent_probabilityChanged();
    }
}

void TrafficLanesPresenter::setAgentSelectionViews(const QStringList &names)
{
    trafficLanesView->setAgentSelectionItems(names);
}

void TrafficLanesPresenter::clearAgentSelectionView()
{
    trafficLanesView->clearTrafficLanesView();
    trafficLanesView->setAgentSelectionItems(QStringList());
    Q_EMIT cleared();
}

void TrafficLanesPresenter::removeRegularLaneAgentByName(QString name)
{
    QMap<QString, unsigned int> map;
    TrafficItemMapInterface<QString> *agentMap = traffic->getAgentsRegularLane();

    for (TrafficItemInterface<QString> *agent : agentMap->values())
        map.insert(agent->getValue(), agentMap->getID(agent));

    if (map.contains(name))
    {
        removeRegularLaneAgent(map[name]);
        trafficLanesView->removeRegularLaneAgentView(map[name]);
        Q_EMIT RegularLaneAgent_removed();
    }
}

void TrafficLanesPresenter::removeRightmostLaneAgentByName(QString name)
{
    QMap<QString, unsigned int> map;
    TrafficItemMapInterface<QString> *agentMap = traffic->getAgentsRightMostLane();

    for (TrafficItemInterface<QString> *agent : agentMap->values())
        map.insert(agent->getValue(), agentMap->getID(agent));

    if (map.contains(name))
    {
        removeRightmostLaneAgent(map[name]);
        trafficLanesView->removeRightmostLaneAgentView(map[name]);
        Q_EMIT RightmostLaneAgent_removed();
    }
}

void TrafficLanesPresenter::updateView()
{
    trafficLanesView->clearTrafficLanesView();
    Q_EMIT cleared();

    // Update selection lists of combo boxes
    trafficLanesView->setAgentSelectionItems(*agentProfiles);

    // Fill agents into regular lane
    TrafficItemMapInterface<QString> *agentMap = traffic->getAgentsRegularLane();

    for (TrafficItemInterface<QString> *agent : agentMap->values())
    {
        trafficLanesView->addRegularLaneAgentView(agentMap->getID(agent), agent->getValue(), agent->getProbability());
        Q_EMIT RegularLaneAgent_added();
    }

    // Fill agents into rightmost lane
    agentMap = traffic->getAgentsRightMostLane();

    for (TrafficItemInterface<QString> *agent : agentMap->values())
    {
        trafficLanesView->addRightmostLaneAgentView(agentMap->getID(agent), agent->getValue(), agent->getProbability());
        Q_EMIT RightmostLaneAgent_added();
    }
}
