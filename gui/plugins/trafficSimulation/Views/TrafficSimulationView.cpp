/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "TrafficSimulationView.h"

#include <iostream>

#include <QCoreApplication>
#include <QFileDialog>
#include <QMessageBox>

#include "ui_TrafficSimulationView.h"

const WindowInterface::ID TrafficSimulationView::ViewID = QStringLiteral("openPASS.TrafficSimulation");

TrafficSimulationView::TrafficSimulationView(WindowInterface *const window,
                                             QWidget *parent) :
    QWidget(parent), window(window), undoView(new QUndoView(this)), actionMenuNew(WindowInterface::createAction(tr("Clear"), this, &TrafficSimulationView::actionNew)), actionMenuLoad(WindowInterface::createAction(tr("Load"), this, &TrafficSimulationView::actionLoad)), actionMenuSave(WindowInterface::createAction(tr("Save"), this, &TrafficSimulationView::actionSave)), actionMenuUndo(WindowInterface::createAction(tr(""), this, &TrafficSimulationView::actionUndo)), actionMenuRedo(WindowInterface::createAction(tr(""), this, &TrafficSimulationView::actionRedo)), actionMenuHistory(WindowInterface::createAction(tr("Command History"), this, &TrafficSimulationView::actionHistory)), ui(new Ui::TrafficSimulationView), statusbar(new StatusBarView(this))
{
    ui->setupUi(this);

    // Create the views

    // 1. Experiment Base View
    experimentBaseView = new ExperimentBaseView(this);
    delete ui->experimentScrollArea->widget();
    ui->experimentScrollArea->setWidget(experimentBaseView);

    // 2. Environment View
    environmentView = new EnvironmentView(this);
    delete ui->environmentScrollArea->widget();
    ui->environmentScrollArea->setWidget(environmentView);

    // 3. Scenario View
    scenarioView = new ScenarioView(this);
    delete ui->scenarioScrollArea->widget();
    ui->scenarioScrollArea->setWidget(scenarioView);

    // 4. Traffic View
    trafficView = new TrafficView(this);
    delete ui->trafficScrollArea->widget();
    ui->trafficScrollArea->setWidget(trafficView);

    // 5. Insert status bar and hide it
    ui->layout->insertWidget(2, statusbar, 0);
    statusbar->hide();

    // Register view in window interface
    window->add(ViewID, WindowInterface::createButton(tr("Scenario-based\nSimulation"), 0, 06000), this, {actionMenuNew, actionMenuLoad, actionMenuSave, actionMenuUndo, actionMenuRedo, actionMenuHistory});

    // prepare undo view and undo/redo buttons
    actionMenuRedo->setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_ArrowRight));
    actionMenuUndo->setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_ArrowLeft));
    actionMenuRedo->setToolTip("redo previous action");
    actionMenuUndo->setToolTip("undo last action");
    undoView->raise();
    undoView->hide();

    // adjust the positioning of the widgets according to the size of this view
    adjustWidgets();
}

TrafficSimulationView::~TrafficSimulationView()
{
    //Deregister view from window interface
    window->remove(ViewID);

    delete ui;
}

void TrafficSimulationView::on_tabWidget_currentChanged(int index)
{
    Q_EMIT currentTabChanged(index);
}

void TrafficSimulationView::actionNew()
{
    QMessageBox warning;

    int ret;

    // spell out a warning that everything will be deleted
    warning.setText("The whole worksheet will be deleted!");
    warning.setInformativeText("Do you want to continue?");
    warning.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    ret = warning.exec();

    if (ret == QMessageBox::Yes)
    {
        Q_EMIT NewSetup();
    }
}

void TrafficSimulationView::actionHistory()
{
    if (undoView->isVisible())
    {
        undoView->hide();
    }
    else
    {
        undoView->show();
        undoView->move(actionMenuHistory->x(), 0);
        undoView->resize(2 * width() / 5, 3 * height() / 5);
    }
}

void TrafficSimulationView::actionRedo()
{
    Q_EMIT Redo();
}

void TrafficSimulationView::actionUndo()
{
    Q_EMIT Undo();
}

void TrafficSimulationView::actionLoad()
{
    Q_EMIT LoadSetup();
}

void TrafficSimulationView::actionSave()
{
    Q_EMIT SaveSetup();
}

void TrafficSimulationView::adjustWidgets()
{
    experimentBaseView->adjustWidgets(this);
}

QUndoView *TrafficSimulationView::getUndoView()
{
    return undoView;
}

ExperimentView *TrafficSimulationView::getExperimentView()
{
    return experimentBaseView->getExperimentView();
}

EnvironmentView *TrafficSimulationView::getEnvironmentView()
{
    return environmentView;
}

EnvironmentDisplayView *TrafficSimulationView::getEnvironmentDisplayView()
{
    return experimentBaseView->getEnvironmentDisplayView();
}

ScenarioDisplayView *TrafficSimulationView::getScenarioDisplayView()
{
    return experimentBaseView->getScenarioDisplayView();
}

ScenarioView *TrafficSimulationView::getScenarioView()
{
    return scenarioView;
}

TrafficView *TrafficSimulationView::getTrafficView()
{
    return trafficView;
}

TrafficDisplayView *TrafficSimulationView::getTrafficDisplayView()
{
    return experimentBaseView->getTrafficDisplayView();
}

void TrafficSimulationView::resizeEvent(QResizeEvent *event)
{
    event->ignore();
    adjustWidgets();
}

void TrafficSimulationView::showStatusBarText(QString text, StatusBarView::Color color)
{
    statusbar->showText(text, color);
}

void TrafficSimulationView::hideStatusBar()
{
    statusbar->hide();
}

void TrafficSimulationView::setProfileCatalogueView(QString const &filename)
{
    ui->ProfileCatalogue->setText(filename);
}

void TrafficSimulationView::on_CatalogueBrowse_clicked()
{
    Q_EMIT profileCatalogueChanged();
}

void TrafficSimulationView::on_CatalogueRefresh_clicked()
{
    Q_EMIT refreshCatalogue();
}

void TrafficSimulationView::setProfileCatalogueError(const QString &message)
{
    ui->CatalogueError->setText(message);
}
