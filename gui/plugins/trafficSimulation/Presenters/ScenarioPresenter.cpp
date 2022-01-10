/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "ScenarioPresenter.h"

#include <QFileDialog>

#include "Models/TrafficSimulationXMLLoadModel.h"

ScenarioPresenter::ScenarioPresenter(ScenarioInterface *scenario,
                                     ScenarioView *scenarioView,
                                     ProjectInterface *const project,
                                     QUndoStack *const undoStack,
                                     QObject *parent) :
    QObject(parent), scenario(scenario), scenarioView(scenarioView), project(project), undoStack(undoStack)
{
    scenarioAgentsPresenter = new ScenarioAgentsPresenter(scenario->getEntities(), scenarioAgentsList, this);
    scenarioView->getScenarioAgentsView()->setModel(scenarioAgentsPresenter);

    connect(scenarioView, &ScenarioView::modifiedName, this, &ScenarioPresenter::setName);
    connect(scenarioView, &ScenarioView::modifiedScenarioFile, this, &ScenarioPresenter::setScenarioFile);
    connect(scenarioView, &ScenarioView::refreshScenarioFile, this, &ScenarioPresenter::refreshScenarioFile);

    connect(project, &ProjectInterface::update, this, &ScenarioPresenter::refreshScenarioFile);
}

void ScenarioPresenter::setName(const QString &name)
{
    if (scenario->getName() != name)
        undoStack->push(new EditScenarioName(scenario, scenarioView, name));
}

void ScenarioPresenter::setScenarioFile()
{
    QString const filename = QFileDialog::getOpenFileName(
        scenarioView, QObject::tr("openPASS / Choose OpenScenario File"),
        project->getConfigPath(), QStringLiteral("OpenScenario File (*.xosc)"));

    if (filename.isEmpty())
        return;

    else
    {
        QString relativePath = project->relativeToConfigPath(filename);

        if (scenario->getScenarioFile() != relativePath)
            undoStack->push(new EditScenarioFile(scenario, scenarioView, relativePath));
    }
}

void ScenarioPresenter::refreshScenarioFile()
{
    QString filename = project->absoluteToConfigPath(scenario->getScenarioFile());

    // load entities defined in the open scenario file
    scenario->getEntities()->clear();
    TrafficSimulationXMLLoadModel::loadEntities(scenario, filename);

    setScenarioAgentsList();
    scenarioAgentsPresenter->update();
    scenarioView->setScenarioFileError("");

    if (scenario->getScenarioFile().isEmpty())
        return;

    if (!QFileInfo::exists(project->absoluteToConfigPath(scenario->getScenarioFile())))
    {
        scenarioView->setScenarioFileError("Open Scenario File not accessible! Check configuration path in Project plugin.");
        return;
    }
}

void ScenarioPresenter::updateView()
{
    scenarioView->setName(scenario->getName());
    scenarioView->setScenarioFile(scenario->getScenarioFile());
    refreshScenarioFile();
}

void ScenarioPresenter::setScenarioAgentsList()
{
    scenarioAgentsList.clear();

    for (auto catalogRef : scenario->getEntities()->values())
    {
        if (!QFileInfo::exists(project->absoluteToConfigPath(catalogRef.first)) || scenarioAgentsList.contains(catalogRef.first))
            break;

        // load agents into list
        TrafficSimulationXMLLoadModel::loadAgentProfiles(project->absoluteToConfigPath(catalogRef.first),
                                                         scenarioAgentsList[catalogRef.first]);
    }
}

EditScenarioName::EditScenarioName(ScenarioInterface *const scenario,
                                   ScenarioView *const scenarioView,
                                   ScenarioInterface::Name const &newName,
                                   QUndoCommand *parent) :
    QUndoCommand(parent), scenario(scenario), scenarioView(scenarioView), newName(newName), oldName(scenario->getName())
{
    setText("edited scenario name");
}

void EditScenarioName::redo()
{
    scenario->setName(newName);
    scenarioView->setName(newName);
}

void EditScenarioName::undo()
{
    scenario->setName(oldName);
    scenarioView->setName(oldName);
}

EditScenarioFile::EditScenarioFile(ScenarioInterface *const scenario,
                                   ScenarioView *const scenarioView,
                                   ScenarioInterface::ScenarioFile const &newFile,
                                   QUndoCommand *parent) :
    QUndoCommand(parent), scenario(scenario), scenarioView(scenarioView), newFile(newFile), oldFile(scenario->getScenarioFile())
{
    setText("edited OpenSCENARIO file");
}

void EditScenarioFile::redo()
{
    scenario->setScenarioFile(newFile);
    scenarioView->setScenarioFile(newFile);
    Q_EMIT scenarioView->refreshScenarioFile();
}

void EditScenarioFile::undo()
{
    scenario->setScenarioFile(oldFile);
    scenarioView->setScenarioFile(oldFile);
    Q_EMIT scenarioView->refreshScenarioFile();
}
