/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "ScenarioDisplayPresenter.h"

#include "Models/TrafficSimulationXMLLoadModel.h"
#include <QFile>

ScenarioDisplayPresenter::ScenarioDisplayPresenter(ScenarioInterface *scenario,
                                                   ScenarioDisplayView *scenarioDisplayView,
                                                   ProjectInterface * const project,
                                                   QObject *parent)
    : QObject(parent)
    , scenario(scenario)
    , scenarioDisplayView(scenarioDisplayView)
    , project(project)
{

    connect(scenario, &ScenarioInterface::modifiedName, this, &ScenarioDisplayPresenter::setScenarioNameView);
    connect(scenario, &ScenarioInterface::modifiedScenarioFile, this, &ScenarioDisplayPresenter::setScenarioFileView);
    connect(scenario, &ScenarioInterface::setEntities, this, &ScenarioDisplayPresenter::setScenarioAgentsView);

    connect(project, &ProjectInterface::update, this, &ScenarioDisplayPresenter::setScenarioFileView);

    updateView();
}


void ScenarioDisplayPresenter::setScenarioNameView()
{
    scenarioDisplayView->setScenarioName(scenario->getName());

    setStatus();
}


void ScenarioDisplayPresenter::setScenarioFileView()
{
    QFile file(project->absoluteToConfigPath(scenario->getScenarioFile()));

    if(scenario->getScenarioFile().isEmpty())
        scenarioDisplayView->setScenarioFile("-");
    else if(file.exists())
        scenarioDisplayView->setScenarioFile("existing");
    else
        scenarioDisplayView->setScenarioFile("not existing");

    setStatus();
}

void ScenarioDisplayPresenter::setScenarioAgentsView()
{
    int num_agents = scenario->getEntities()->count();

    scenarioDisplayView->setScenarioAgents(num_agents);

    setStatus();
}

void ScenarioDisplayPresenter::setStatus()
{
    bool ok = true;

    QFile scenarioFile(project->absoluteToConfigPath(scenario->getScenarioFile()));
    if(scenario->getEntities()->count() == 0)
        ok = false;

    else if( !scenarioFile.exists())
        ok = false;

    else if( !scenarioAgentsDefined() )
        ok = false;



    if(ok && scenario->getName().isEmpty())
        scenarioDisplayView->setStatus(ScenarioDisplayView::StatusType::Warning, QString("No name specified"));

    else if(ok)
        scenarioDisplayView->setStatus(ScenarioDisplayView::StatusType::OK, QString("OK"));

    else
        scenarioDisplayView->setStatus(ScenarioDisplayView::StatusType::Error, QString("Error"));

    scenario->setStatus(ok);
}


void ScenarioDisplayPresenter::updateView()
{
    setScenarioNameView();
    setScenarioFileView();
    setScenarioAgentsView();

    setStatus();
}

void ScenarioDisplayPresenter::setScenarioAgentsList()
{
    scenarioAgentsList.clear();

    for(auto catalogRef : scenario->getEntities()->values())
    {
        if(!QFileInfo::exists(project->absoluteToConfigPath(catalogRef.first))
                || scenarioAgentsList.contains(catalogRef.first))
            break;

        // load agents into list
        TrafficSimulationXMLLoadModel::loadAgentProfiles(project->absoluteToConfigPath(catalogRef.first),
                                                         scenarioAgentsList[catalogRef.first]);
    }
}


bool ScenarioDisplayPresenter::scenarioAgentsDefined()
{
    setScenarioAgentsList();

    for(auto entity : scenario->getEntities()->values())
    {
        QString agentProfile = entity.second;
        QString catalogue = entity.first;

        if( !scenarioAgentsList.contains(catalogue) )
            return false;
        else if( !scenarioAgentsList.value(catalogue).contains(agentProfile) )
            return false;
    }

    return true;
}
