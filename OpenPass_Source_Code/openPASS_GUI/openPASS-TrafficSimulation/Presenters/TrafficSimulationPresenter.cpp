/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "TrafficSimulationPresenter.h"
#include "ExperimentPresenter.h"
#include "EnvironmentPresenter.h"
#include "EnvironmentDisplayPresenter.h"
#include "ScenarioPresenter.h"
#include "ScenarioDisplayPresenter.h"
#include "TrafficPresenter.h"
#include "TrafficDisplayPresenter.h"

#include "EnvironmentMapPresenter.h"

#include "Models/TrafficSimulationModel.h"
#include "Models/TrafficSimulationXMLSaveModel.h"
#include "Models/TrafficSimulationXMLLoadModel.h"

#include "Views/TrafficSimulationView.h"


#include "DelegateDoubleView.h"
#include "DelegateTimeView.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include <QFileSystemWatcher>

TrafficSimulationPresenter::TrafficSimulationPresenter(TrafficSimulationInterface *trafficSimulation,
                                                       TrafficSimulationView *trafficSimulationView,
                                                       ProjectInterface * const project,
                                                       QObject *parent)
    : QObject(parent)
    , trafficSimulation(trafficSimulation)
    , trafficSimulationView(trafficSimulationView)
    , project(project)
    , undoStack(new QUndoStack(this))
    , experimentPresenter(new ExperimentPresenter(trafficSimulation->getExperiment(),
                                                  trafficSimulationView->getExperimentView(),
                                                  project,
                                                  undoStack,
                                                  this))
    , environmentPresenter(new EnvironmentPresenter(trafficSimulation->getEnvironment(),
                                                    trafficSimulationView->getEnvironmentView(),
                                                    undoStack,
                                                    this))
    , environmentDisplayPresenter(new EnvironmentDisplayPresenter(trafficSimulation->getEnvironment(),
                                                                  trafficSimulationView->getEnvironmentDisplayView(),
                                                                  this))
    , scenarioPresenter(new ScenarioPresenter(trafficSimulation->getScenario(),
                                              trafficSimulationView->getScenarioView(),
                                              project,
                                              undoStack,
                                              this))
    , scenarioDisplayPresenter(new ScenarioDisplayPresenter(trafficSimulation->getScenario(),
                                                            trafficSimulationView->getScenarioDisplayView(),
                                                            project,
                                                            this))
    , trafficPresenter(new TrafficPresenter(trafficSimulation->getTraffic(),
                                            trafficSimulationView->getTrafficView(),
                                            &agentProfiles,
                                            undoStack,
                                            this))
    , trafficDisplayPresenter( new TrafficDisplayPresenter(trafficSimulation->getTraffic(),
                                                           trafficSimulationView->getTrafficDisplayView(),
                                                           &agentProfiles,
                                                           this))
    , redo(new QShortcut(QKeySequence(tr("Ctrl+Y", "redo")),trafficSimulationView))
    , undo (new QShortcut(QKeySequence(tr("Ctrl+Z", "undo")),trafficSimulationView))
{

    connect(trafficSimulationView, &TrafficSimulationView::SaveSetup, this, &TrafficSimulationPresenter::save);
    connect(trafficSimulationView, &TrafficSimulationView::LoadSetup, this, &TrafficSimulationPresenter::load);
    connect(trafficSimulationView,&TrafficSimulationView::NewSetup,this,&TrafficSimulationPresenter::clear);
    connect(trafficSimulationView, &TrafficSimulationView::currentTabChanged, this, &TrafficSimulationPresenter::updateStatusBar);

    connect(experimentPresenter, &ExperimentPresenter::startSimulation, this, &TrafficSimulationPresenter::startSimulation);

    connect(trafficSimulationView, &TrafficSimulationView::profileCatalogueChanged, this,  &TrafficSimulationPresenter::setProfileCatalogue);
    connect(trafficSimulationView, &TrafficSimulationView::refreshCatalogue, this, &TrafficSimulationPresenter::updateCatalogueView);

    connect(trafficSimulation->getEnvironment(), &EnvironmentInterface::modifiedStatus, this, &TrafficSimulationPresenter::checkStatus);
    connect(trafficSimulation->getScenario(), &ScenarioInterface::modifiedStatus, this, &TrafficSimulationPresenter::checkStatus);
    connect(trafficSimulation->getTraffic(), &TrafficInterface::modifiedStatus, this, &TrafficSimulationPresenter::checkStatus);

    connect(project, &ProjectInterface::update, this, &TrafficSimulationPresenter::updateCatalogueView);

    connect(redo, &QShortcut::activated, undoStack, &QUndoStack::redo);
    connect(undo, &QShortcut::activated, undoStack, &QUndoStack::undo);
    connect(trafficSimulationView, &TrafficSimulationView::Redo, undoStack, &QUndoStack::redo);
    connect(trafficSimulationView, &TrafficSimulationView::Undo, undoStack, &QUndoStack::undo);

    // connect undoStack to undoView
    trafficSimulationView->getUndoView()->setStack(undoStack);
    updateStatusBar(0);
}

void TrafficSimulationPresenter::checkStatus()
{
    bool status = trafficSimulation->getEnvironment()->getStatus() &&
                  trafficSimulation->getTraffic()->getStatus() &&
                  trafficSimulation->getScenario()->getStatus() &&
                  !trafficSimulation->getProfilesCatalogue().isEmpty();

    trafficSimulationView->getExperimentView()->enableSimulation(status);
}

void TrafficSimulationPresenter::simulationFinished(int exitCode)
{
    QMessageBox message;
    QString text;

    if(exitCode == 0)
       text = QString("Simulation slave has finished successfully! Results have been stored in %1").arg(project->getResultPath());
    else
        text = QString("Simulation slave has finished with exit code %1! See log file %2 for details.").arg(exitCode).arg(project->getLogSlave());


    message.setText(text);
    message.exec();

    QProcess * slave = qobject_cast<QProcess*>(sender());
    if(slave)
        delete slave;

    checkStatus();
}

void TrafficSimulationPresenter::startSimulation()
{
    QMessageBox warning;
    int ret;
    QString slaveConfig (project->absoluteToConfigPath("slaveConfig.xml"));

    // save the slaveConfig explicitly under the name "slave config"
    if(QFileInfo::exists(slaveConfig))
    {
        // spell out a warning that everything will be deleted
        warning.setText("There already exists a file named 'slaveConfig.xml' in the current configuration directory (see Master Configuration)!");
        warning.setInformativeText("Do you want to overwrite it with the current settings?");
        warning.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        ret=warning.exec();

        if(ret == QMessageBox::No)
            return;
    }

    TrafficSimulationXMLSaveModel::save(slaveConfig, trafficSimulation);

    // Prepare slave arguments
    QStringList arguments;

    arguments << QString("--logLevel") << QString::number(project->getLogLevel())
              << QString("--logFile") << project->getLogSlave()
              << QString("--lib") << project->getLibraryPath()
              << QString("--configs") << project->getConfigPath()
              << QString("--results") << project->getResultPath();

    QProcess *slave = new QProcess;
    connect(slave, static_cast<void (QProcess::*)(int)>(&QProcess::finished) , this, &TrafficSimulationPresenter::simulationFinished);

    if(QFileInfo::exists(project->getSlaveExe()))
    {
        slave->setProgram(project->getSlaveExe());
        slave->setArguments(arguments);
        slave->start();
        trafficSimulationView->getExperimentView()->enableSimulation(false);
    }
    else
    {
        // tell user that slave exe does not exist
        warning.setText("Slave exe not found! Check slave path settings in Master Configuration!");
        warning.setInformativeText("");
        warning.setStandardButtons(QMessageBox::Ok);
        ret=warning.exec();
    }
}

void TrafficSimulationPresenter::save()
{

    QString const filepath = QFileDialog::getSaveFileName(
                   trafficSimulationView, QObject::tr("openPASS / Save slave config"),
                   project->getConfigPath(), QStringLiteral("slaveConfig (*.xml)"));

    TrafficSimulationXMLSaveModel::save(filepath,trafficSimulation);
}

void TrafficSimulationPresenter::updateStatusBar(int index)
{
    QString info;

    switch(index)
    {

    // Experiment Configuration
    case 0:
    {
        info = QString("This tab contains the overall experiment setup. Experiments can be configured and started here.");
        trafficSimulationView->showStatusBarText(info, StatusBarView::Color::Message);
        break;
    }
    // Environment Configuration
    case 1:
    {
        info = QString("This tab contains information about the world and the general environment inside the simulation. "
                       "Every invocation re-rolls the environment parameters. Probabilities need to add up to 1.0 for each parameter. Note that, in "
                       "the current implementation, the environment parameters 'time of day' as well as 'weather' are not used yet.");
        trafficSimulationView->showStatusBarText(info, StatusBarView::Color::Message);
        break;
    }
    case 2:
    {
        info = QString("This tab contains information about the scenario and scenery setup for the experiment. This information does not change between invocations. "
                       "Here the scenario and scenery files as well as the AgentProfiles for all agents are specified. For every agent listed in the scenario file one AgentProfile is required");
        trafficSimulationView->showStatusBarText(info, StatusBarView::Color::Message);
        break;
    }
    case 3:
    {
        info = QString("This tab contains parameters regarding the traffic and distribution of agent profiles for all surrounding vehicles. "
                       "Every invocation re-rolls the traffic parameters and agents. Probabilities need to add up to 1.0 for each parameter.");
        trafficSimulationView->showStatusBarText(info, StatusBarView::Color::Message);
        break;
    }
    default:
        break;

    }
}

void TrafficSimulationPresenter::load()
{
    bool success;
    QString const filepath = QFileDialog::getOpenFileName(
                   trafficSimulationView, QObject::tr("openPASS / Load slave config"),
                   project->getConfigPath(), QStringLiteral("slaveConfig (*.xml)"));

    TrafficSimulationInterface * loadedXML = new TrafficSimulationModel;
    success = TrafficSimulationXMLLoadModel::load(filepath, loadedXML);

    if(success)
    {
        undoStack->beginMacro(QString("loaded slaveConfig %1").arg(filepath));

        // 0. clear everything first
        clearExperimentConfig();
        clearEnvironmentConfig();
        clearScenarioConfig();
        clearTrafficConfig();

        // 1. set the profiles catalogue
        undoStack->push(new EditProfilesCatalogue(trafficSimulation, trafficSimulationView, loadedXML->getProfilesCatalogue()));

        //2. Experiment Settings
        copyExperimentConfig(loadedXML);

        //3. Environment Settings
        copyEnvironmentConfig(loadedXML);

        //4. Scenario Settings
        copyScenarioConfig(loadedXML);

        //5. Traffic Settings
        copyTrafficConfig(loadedXML);

        undoStack->endMacro();
    }

    delete loadedXML;
}

void TrafficSimulationPresenter::copyExperimentConfig(TrafficSimulationInterface const * const from)
{
    int row;

    undoStack->push(new EditNumberInvocations(trafficSimulation->getExperiment(),
                                              trafficSimulationView->getExperimentView(),
                                              from->getExperiment()->getNumberInvocations()));

    undoStack->push(new EditRandomSeed(trafficSimulation->getExperiment(),
                                       trafficSimulationView->getExperimentView(),
                                       from->getExperiment()->getSeed()));

    LibrariesPresenter * const libPresenter = findChild<LibrariesPresenter * const>();
    ExperimentInterface::Libraries * const loadedLibs = from->getExperiment()->getLibraries();
    row = 0;
    for(auto libType : loadedLibs->keys())
    {
        undoStack->push(new AddLibrary(libPresenter, libType));
        undoStack->push(new EditData(libPresenter, libPresenter->index(row,1), loadedLibs->value(libType), ""));
        ++row;
    }

    LoggingGroupsPresenter * const loggingGroupsPresenter = findChild<LoggingGroupsPresenter * const>();
    ExperimentInterface::LoggingGroups * const loadedLoggingGroups = from->getExperiment()->getLoggingGroups();
    for(auto loggingGroup : *loadedLoggingGroups)
    {
        undoStack->push(new AddLoggingGroup(loggingGroupsPresenter, loggingGroup));
    }
}

void TrafficSimulationPresenter::copyEnvironmentConfig(const TrafficSimulationInterface *const from)
{
    EnvironmentMapPresenter * table;
    QMap<EnvironmentInterface::ID, EnvironmentInterface::Item *> * map;

    // 0. edit name
    undoStack->push(new EditEnvironmentName(trafficSimulation->getEnvironment(),
                                            trafficSimulationView->getEnvironmentView(),
                                            from->getEnvironment()->getName()));

    // 1. copy time of day table
    table = findChild<EnvironmentMapPresenter *>("timeOfDayTable");
    map =  from->getEnvironment()->getTimeOfDayMap();
    for(auto item : *map)
        undoStack->push(new AddEnvironmentItem(table, item->getValue(), item->getProbability()));

    // 2. copy visibility distance table
    table = findChild<EnvironmentMapPresenter *>("visibilityDistanceTable");
    map =  from->getEnvironment()->getVisibilityDistanceMap();
    for(auto item : *map)
        undoStack->push(new AddEnvironmentItem(table, item->getValue(), item->getProbability()));

    // 3. copy friction table
    table = findChild<EnvironmentMapPresenter *>("frictionTable");
    map =  from->getEnvironment()->getFrictionMap();
    for(auto item : *map)
        undoStack->push(new AddEnvironmentItem(table, item->getValue(), item->getProbability()));

    // 4. copy weather table
    table = findChild<EnvironmentMapPresenter *>("weatherTable");
    map =  from->getEnvironment()->getWeatherMap();
    for(auto item : *map)
        undoStack->push(new AddEnvironmentItem(table, item->getValue(), item->getProbability()));
}

void TrafficSimulationPresenter::copyScenarioConfig(const TrafficSimulationInterface *const from)
{
    // 0. edit name
    undoStack->push(new EditScenarioName(trafficSimulation->getScenario(),
                                         trafficSimulationView->getScenarioView(),
                                         from->getScenario()->getName()));

    // 1. edit scenario file
    undoStack->push(new EditScenarioFile(trafficSimulation->getScenario(),
                                         trafficSimulationView->getScenarioView(),
                                         from->getScenario()->getScenarioFile()));
}

void TrafficSimulationPresenter::copyTrafficConfig(const TrafficSimulationInterface *const from)
{
    TrafficItemMapPresenter * table;
    TrafficItemMapInterface * map;

    // 0. edit name
    undoStack->push(new EditTrafficName(trafficSimulation->getTraffic(),
                                        trafficSimulationView->getTrafficView(),
                                        from->getTraffic()->getName()));

    // 1. copy traffic volume table
    table = findChild<TrafficItemMapPresenter *>("volumesTable");
    map = from->getTraffic()->getVolumes();
    for(auto item: *map)
        undoStack->push(new AddTrafficItem(table, item->getValue(), item->getProbability()));

    // 2. copy velocities table
    table = findChild<TrafficItemMapPresenter *>("velocitiesTable");
    map = from->getTraffic()->getVelocities();
    for(auto item: *map)
        undoStack->push(new AddTrafficItem(table, item->getValue(), item->getProbability()));

    // 3. copy platoon Rates table
    table = findChild<TrafficItemMapPresenter *>("platoonRatesTable");
    map = from->getTraffic()->getPlatoonRates();
    for(auto item: *map)
        undoStack->push(new AddTrafficItem(table, item->getValue(), item->getProbability()));

    // 4. copy homogeneities table
    table = findChild<TrafficItemMapPresenter *>("homogeneitiesTable");
    map = from->getTraffic()->getHomogeneities();
    for(auto item: *map)
        undoStack->push(new AddTrafficItem(table, item->getValue(), item->getProbability()));

    // 5. copy regular lange agents table
    table = findChild<TrafficItemMapPresenter *>("regularLaneAgentsTable");
    map = from->getTraffic()->getAgentsRegularLane();
    for(auto item: *map)
        undoStack->push(new AddTrafficItem(table, item->getValue(), item->getProbability()));

    // 6. copy rightmost lange agents table
    table = findChild<TrafficItemMapPresenter *>("rightmostLaneAgentsTable");
    map = from->getTraffic()->getAgentsRightMostLane();
    for(auto item: *map)
        undoStack->push(new AddTrafficItem(table, item->getValue(), item->getProbability()));
}

void TrafficSimulationPresenter::clearExperimentConfig()
{

    // reset number of invocations
    undoStack->push(new EditNumberInvocations(trafficSimulation->getExperiment(),
                                              trafficSimulationView->getExperimentView(),
                                              ExperimentModel::numberInvocations_default));
    // reset randomn seed
    undoStack->push(new EditRandomSeed(trafficSimulation->getExperiment(),
                                       trafficSimulationView->getExperimentView(),
                                       ExperimentModel::seed_default));

    // empty libraries table
    LibrariesPresenter * const libPresenter = findChild<LibrariesPresenter * const>();
    while(libPresenter->rowCount() > 0)
        undoStack->push(new RemoveLibrary(libPresenter, 0));

    // empty logging groups list
    LoggingGroupsPresenter * const loggingGroupsPresenter = findChild<LoggingGroupsPresenter * const>();
    while(loggingGroupsPresenter->rowCount() > 0)
        undoStack->push(new RemoveLoggingGroup(loggingGroupsPresenter, 0));

}

void TrafficSimulationPresenter::clearEnvironmentConfig()
{
    // reset name
    undoStack->push(new EditEnvironmentName(trafficSimulation->getEnvironment(),
                                            trafficSimulationView->getEnvironmentView(),
                                            ""));

    // remove all table entries
    for(auto table : findChildren<EnvironmentMapPresenter*>())
        while(table->rowCount()>0)
            undoStack->push(new RemoveEnvironmentItem(table,0));
}

void TrafficSimulationPresenter::clearTrafficConfig()
{
    // reset name
    undoStack->push(new EditTrafficName(trafficSimulation->getTraffic(),
                                        trafficSimulationView->getTrafficView(),
                                        ""));

    // remove all table entries
    for(auto table : findChildren<TrafficItemMapPresenter*>())
        while(table->rowCount()>0)
            undoStack->push(new RemoveTrafficItem(table,0));
}

void TrafficSimulationPresenter::clearScenarioConfig()
{
    // reset name
    undoStack->push(new EditScenarioName(trafficSimulation->getScenario(),
                                         trafficSimulationView->getScenarioView(),
                                         ""));

    // reset scenario file
    undoStack->push(new EditScenarioFile(trafficSimulation->getScenario(),
                                         trafficSimulationView->getScenarioView(),
                                         ""));
}

void TrafficSimulationPresenter::clear()
{

    undoStack->beginMacro("cleared worksheet");

    undoStack->push(new EditProfilesCatalogue(trafficSimulation, trafficSimulationView,""));

    clearExperimentConfig();
    clearEnvironmentConfig();
    clearScenarioConfig();
    clearTrafficConfig();

    undoStack->endMacro();
}


void TrafficSimulationPresenter::setProfileCatalogue()
{
    QString const filename = QFileDialog::getOpenFileName(
                   trafficSimulationView, QObject::tr("openPASS / Select profiles catalogue"),
                   project->getConfigPath(), QStringLiteral("ProfilesCatalog (*.xml)"));

    QString Catalogue= project->relativeToConfigPath(filename);

    if(trafficSimulation->getProfilesCatalogue()!= Catalogue)
        undoStack->push(new EditProfilesCatalogue(trafficSimulation, trafficSimulationView,Catalogue));
}

void TrafficSimulationPresenter::updateCatalogueView()
{
    QString profilesCatalogue = trafficSimulation->getProfilesCatalogue();
    TrafficSimulationXMLLoadModel::loadAgentProfiles(project->absoluteToConfigPath(profilesCatalogue),
                                                     agentProfiles);

    trafficSimulationView->setProfileCatalogueView(profilesCatalogue);
    trafficSimulationView->setProfileCatalogueError("");
    trafficPresenter->checkAgents();

    if(profilesCatalogue.isEmpty())
        return;

    if(!QFileInfo::exists(project->absoluteToConfigPath(profilesCatalogue)))
    {
        trafficSimulationView->setProfileCatalogueError("Profiles Catalogue not accessible! Check configuration path in Project plugin!");
    }

    trafficDisplayPresenter->setStatus();
}

EditProfilesCatalogue::EditProfilesCatalogue(TrafficSimulationInterface * const trafficSimulation,
                                             TrafficSimulationView * const trafficSimulationView,
                                             TrafficSimulationInterface::ProfilesCatalogue const &newCatalogue,
                                             QUndoCommand *parent)
    : QUndoCommand (parent)
    , trafficSimulation(trafficSimulation)
    , trafficSimulationView(trafficSimulationView)
    , newCatalogue(newCatalogue)
    , oldCatalogue(trafficSimulation->getProfilesCatalogue())
{
    setText("edited profiles catalogue");
}

void EditProfilesCatalogue::redo()
{
    trafficSimulation->setProfilesCatalogue(newCatalogue);
    trafficSimulationView->setProfileCatalogueView(newCatalogue);
    Q_EMIT trafficSimulationView->refreshCatalogue();
}

void EditProfilesCatalogue::undo()
{
    trafficSimulation->setProfilesCatalogue(oldCatalogue);
    trafficSimulationView->setProfileCatalogueView(oldCatalogue);
    Q_EMIT trafficSimulationView->refreshCatalogue();
}
