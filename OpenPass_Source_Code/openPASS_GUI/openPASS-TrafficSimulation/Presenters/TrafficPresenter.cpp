/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "TrafficPresenter.h"

#include "DelegateComboBoxView.h"
#include "DelegateDoubleView.h"

QString const TrafficPresenter::default_volume = "600";
QString const TrafficPresenter::default_platoonRate = "0.50";
QString const TrafficPresenter::default_velocity = "90";
QString const TrafficPresenter::default_homogeneity = "0.80";
double const TrafficPresenter::default_probability = 0.5;

TrafficPresenter::TrafficPresenter(TrafficInterface * const traffic,
                                   TrafficView * const trafficView,
                                   QList<QString> * const agentList,
                                   QUndoStack * const undoStack,
                                   QObject *parent)
    : QObject(parent)
    , traffic(traffic)
    , trafficView(trafficView)
    , volumes(new TrafficItemMapPresenter(traffic->getVolumes(),this))
    , platoonRates(new TrafficItemMapPresenter(traffic->getPlatoonRates(),this))
    , velocities(new TrafficItemMapPresenter(traffic->getVelocities(),this))
    , homogeneities(new TrafficItemMapPresenter(traffic->getHomogeneities(),this))
    , regularLaneAgents(new TrafficItemMapPresenter(traffic->getAgentsRegularLane(),this))
    , rightmostLaneAgents(new TrafficItemMapPresenter(traffic->getAgentsRightMostLane(),this))
    , agentList(agentList)
    , undoStack(undoStack)
{
   setupVolumes();
   setupPlatoonRates();
   setupVelocities();
   setupHomogeneities();

   setupRegularLaneAgents();
   setupRightmostLaneAgents();

   connect(trafficView, &TrafficView::modifiedName, this, &TrafficPresenter::setName);
   connect(traffic, &TrafficInterface::edited, this, &TrafficPresenter::checkProbabilities);
   connect(traffic, &TrafficInterface::edited, this, &TrafficPresenter::checkAgents);
}

void TrafficPresenter::checkProbabilities()
{
    // 1. Check probabilities for volumes
    double sum = 0;
    for( auto item : traffic->getVolumes()->values() )
        sum += item->getProbability();

    trafficView->setVolumes_ProbabilityError( sum!=1.0 && traffic->getVolumes()->count()!= 0 );

    // 2. Check probabilities for platoon rates
    sum = 0;
    for( auto item : traffic->getPlatoonRates()->values() )
        sum += item->getProbability();

    trafficView->setPlatoonRates_ProbabilityError(sum!=1.0 && traffic->getPlatoonRates()->count()!= 0 );

    // 3. Check probabilities for velocities
    sum = 0;
    for( auto item : traffic->getVelocities()->values() )
        sum += item->getProbability();

    trafficView->setVelocities_ProbabilityError(sum!=1.0 && traffic->getVelocities()->count()!= 0 );

    // 4. Check probabilities for homogeneities
    sum = 0;
    for( auto item : traffic->getHomogeneities()->values() )
        sum += item->getProbability();

    trafficView->setHomogeneities_ProbabilityError(sum!=1.0 && traffic->getHomogeneities()->count()!= 0 );


    // 5. Check probabilities for regular lane agents
    sum = 0;
    for( auto item : traffic->getAgentsRegularLane()->values() )
        sum += item->getProbability();

    trafficView->setRegularLane_ProbabilityError(sum!=1.0 && traffic->getAgentsRegularLane()->count()!= 0 );

    // 6. Check probabilities for rightmost lane agents
    sum = 0;
    for( auto item : traffic->getAgentsRightMostLane()->values() )
        sum += item->getProbability();

    trafficView->setRightmostLane_ProbabilityError(sum!=1.0 && traffic->getAgentsRightMostLane()->count()!= 0 );
}


void TrafficPresenter::setName(const QString &name)
{
    if(traffic->getName() != name)
        undoStack->push(new EditTrafficName(traffic, trafficView, name));
}

void TrafficPresenter::addVolume()
{
    undoStack->push(new AddTrafficItem(volumes, default_volume, default_probability, "traffic volumes table"));
}

void TrafficPresenter::removeVolume(const int &row)
{
    if(volumes->rowCount() != 0)
        undoStack->push(new RemoveTrafficItem(volumes, row, "traffic volumes table"));
}

void TrafficPresenter::addPlatoonRate()
{
    undoStack->push(new AddTrafficItem(platoonRates, default_platoonRate, default_probability, "platoon rates table"));
}

void TrafficPresenter::removePlatoonRate(const int &row)
{
    if(platoonRates->rowCount() != 0)
        undoStack->push(new RemoveTrafficItem(platoonRates, row, "platoon rates table"));
}

void TrafficPresenter::addVelocity()
{
    undoStack->push(new AddTrafficItem(velocities, default_velocity, default_probability, "velocities table"));
}

void TrafficPresenter::removeVelocity(const int &row)
{
    if(velocities->rowCount() != 0)
        undoStack->push(new RemoveTrafficItem(velocities, row, "velocities table"));
}

void TrafficPresenter::addHomogeneity()
{
    undoStack->push(new AddTrafficItem(homogeneities, default_homogeneity, default_probability, "homogeneities table"));
}

void TrafficPresenter::removeHomogeneity(const int &row)
{
    if(homogeneities->rowCount() != 0)
        undoStack->push(new RemoveTrafficItem(homogeneities, row, "homogeneities table"));
}

void TrafficPresenter::addRegularLaneAgent()
{
    if( !agentList->isEmpty() )
        undoStack->push(new AddTrafficItem(regularLaneAgents, agentList->first(), default_probability, "regular lane agents table"));
}

void TrafficPresenter::removeRegularLaneAgent(const int &row)
{
    if(regularLaneAgents->rowCount() != 0)
        undoStack->push(new RemoveTrafficItem(regularLaneAgents, row, "regular lane agents table"));
}

void TrafficPresenter::addRightmostLaneAgent()
{
    if( !agentList->isEmpty() )
        undoStack->push(new AddTrafficItem(rightmostLaneAgents, agentList->first(), default_probability, "rightmost lane agents table"));
}

void TrafficPresenter::removeRightmostLaneAgent(const int &row)
{
    if(rightmostLaneAgents->rowCount() != 0)
        undoStack->push(new RemoveTrafficItem(rightmostLaneAgents, row, "rightmost lane agents table"));
}

void TrafficPresenter::setupVolumes()
{
    QTableView *volumesView = trafficView->getVolumesView();
    volumesView->setModel(volumes);
    volumes->setObjectName("volumesTable");
    volumesView->setItemDelegateForColumn(0, new DelegateDoubleView(0, 3000, 100, 0,"traffic volume's", undoStack, this));
    volumesView->setItemDelegateForColumn(1, new DelegateDoubleView(0, 1, 0.1, 2, "traffic volume's probability",undoStack, this));

    connect(trafficView, &TrafficView::addVolume, this, &TrafficPresenter::addVolume);
    connect(trafficView, &TrafficView::removeVolume, this, &TrafficPresenter::removeVolume);
}

void TrafficPresenter::setupPlatoonRates()
{
    QTableView *platoonRatesView = trafficView->getPlatoonRatesView();
    platoonRatesView->setModel(platoonRates);
    platoonRates->setObjectName("platoonRatesTable");
    platoonRatesView->setItemDelegateForColumn(0, new DelegateDoubleView(0, 1, 0.1, 2,"platoon rate's", undoStack, this));
    platoonRatesView->setItemDelegateForColumn(1, new DelegateDoubleView(0, 1, 0.1, 2, "platoon rate's probability", undoStack, this));

    connect(trafficView, &TrafficView::addPlatoonRate, this, &TrafficPresenter::addPlatoonRate);
    connect(trafficView, &TrafficView::removePlatoonRate, this, &TrafficPresenter::removePlatoonRate);
}

void TrafficPresenter::setupVelocities()
{
    QTableView *velicitiesView = trafficView->getVelocitiesView();
    velicitiesView->setModel(velocities);
    velocities->setObjectName("velocitiesTable");
    velicitiesView->setItemDelegateForColumn(0, new DelegateDoubleView(0, 250, 10, 0,"velocity's", undoStack, this));
    velicitiesView->setItemDelegateForColumn(1, new DelegateDoubleView(0, 1, 0.1, 2, "velocity's probability", undoStack, this));

    connect(trafficView, &TrafficView::addVelocity, this, &TrafficPresenter::addVelocity);
    connect(trafficView, &TrafficView::removeVelocity, this, &TrafficPresenter::removeVelocity);
}

void TrafficPresenter::setupHomogeneities()
{
    QTableView *homogeneitiesView = trafficView->getHomogeneitiesView();
    homogeneitiesView->setModel(homogeneities);
    homogeneities->setObjectName("homogeneitiesTable");
    homogeneitiesView->setItemDelegateForColumn(0, new DelegateDoubleView(0, 1, 0.1, 2, "homogeneity's", undoStack,this));
    homogeneitiesView->setItemDelegateForColumn(1, new DelegateDoubleView(0, 1, 0.1, 2, "homogeneity's probability", undoStack, this));

    connect(trafficView, &TrafficView::addHomogeneity, this, &TrafficPresenter::addHomogeneity);
    connect(trafficView, &TrafficView::removeHomogeneity, this, &TrafficPresenter::removeHomogeneity);
}

void TrafficPresenter::setupRegularLaneAgents()
{
    QTableView *regularLaneAgentsView = trafficView->getRegularLaneAgentsView();
    regularLaneAgentsView->setModel(regularLaneAgents);
    regularLaneAgents->setObjectName("regularLaneAgentsTable");
    regularLaneAgentsView->setItemDelegateForColumn(0, new DelegateComboBoxView(agentList, "regular lane agent's", undoStack, this) );
    regularLaneAgentsView->setItemDelegateForColumn(1, new DelegateDoubleView(0, 1, 0.1, 2, "regular lane agent's probability", undoStack, this));

    connect(trafficView, &TrafficView::addRegularLaneAgent, this, &TrafficPresenter::addRegularLaneAgent);
    connect(trafficView, &TrafficView::removeRegularLaneAgent, this, &TrafficPresenter::removeRegularLaneAgent);
}

void TrafficPresenter::setupRightmostLaneAgents()
{
    QTableView *rightmostLaneAgentsView = trafficView->getRightmostLaneAgentsView();
    rightmostLaneAgentsView->setModel(rightmostLaneAgents);
    rightmostLaneAgents->setObjectName("rightmostLaneAgentsTable");
    rightmostLaneAgentsView->setItemDelegateForColumn(0, new DelegateComboBoxView(agentList, "rightmost lane agent's", undoStack, this) );
    rightmostLaneAgentsView->setItemDelegateForColumn(1, new DelegateDoubleView(0, 1, 0.1, 2, "rightmost lane agent's probability", undoStack, this));

    connect(trafficView, &TrafficView::addRightmostLaneAgent, this, &TrafficPresenter::addRightmostLaneAgent);
    connect(trafficView, &TrafficView::removeRightmostLaneAgent, this, &TrafficPresenter::removeRightmostLaneAgent);
}

void TrafficPresenter::checkAgents()
{
    // determine list of all used agent profiles
    QList<TrafficItemInterface*> allAgents = traffic->getAgentsRegularLane()->values()
                                            + traffic->getAgentsRightMostLane()->values();

    trafficView->setAgentProfilesError("");

    // check whether used agent profiles are contained in the agentList!
    // (agentList contains all defined agent profiles in the profiles catalogue)
    for(auto agent : allAgents)
    {
        TrafficItemInterface::Value name = agent->getValue();
        if(!agentList->contains(name))
        {
            trafficView->setAgentProfilesError("Agent Profiles inconsistent with Profiles Catalogue!");
            break;
        }
    }
}

void TrafficPresenter::updateView()
{
    trafficView->setName(traffic->getName());
    volumes->update();
    platoonRates->update();
    velocities->update();
    homogeneities->update();

    regularLaneAgents->update();
    rightmostLaneAgents->update();

    checkAgents();
}

AddTrafficItem::AddTrafficItem(TrafficItemMapPresenter * const table,
                               QString const default_value,
                               const double default_probability,
                               QString tableName,
                               QUndoCommand * parent)
    : QUndoCommand (parent)
    , table(table)
    , default_value(default_value)
    , default_probability(default_probability)
{
     setText(QString("added entry to %1").arg(tableName));
}


void AddTrafficItem::redo()
{
    table->addRow(table->rowCount(), default_value, default_probability);
}

void AddTrafficItem::undo()
{
    table->removeRow(table->rowCount()-1);
}

RemoveTrafficItem::RemoveTrafficItem(TrafficItemMapPresenter * const table,
                                     int const row,
                                     QString tableName,
                                     QUndoCommand * parent)
    : QUndoCommand (parent)
    , table(table)
    , row(row)
    , value(table->data( table->index(row, 0) ).toString())
    , probability(table->data( table->index(row, 1) ).toDouble())
{
    setText(QString("removed entry from %1").arg(tableName));
}

void RemoveTrafficItem::redo()
{
    table->removeRow(row);
}

void RemoveTrafficItem::undo()
{
    table->addRow(row, value, probability);
}

EditTrafficName::EditTrafficName(TrafficInterface * const traffic,
                                 TrafficView * const trafficView,
                                 TrafficInterface::Name const &newName,
                                 QUndoCommand * parent)
    : QUndoCommand (parent)
    , traffic(traffic)
    , trafficView(trafficView)
    , newName(newName)
    , oldName(traffic->getName())
{
    setText("edited environment name");
}

void EditTrafficName::redo()
{
    traffic->setName(newName);
    trafficView->setName(newName);
}

void EditTrafficName::undo()
{
    traffic->setName(oldName);
    trafficView->setName(oldName);
}
