/********************************************************************************
 * Copyright (c) 2020 ITK Engineering GmbH
 *               2017 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "Views/ProjectView.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QKeyEvent>
#include <QStandardPaths>
#include <QString>

#include "Models/ProjectModel.h"
#include "Presenters/ProjectPresenter.h"
#include "ui_ProjectView.h"

const WindowInterface::ID ProjectView::ViewID = QStringLiteral("openPASS.Project");

ProjectView::ProjectView(WindowInterface *const window,
                         ProjectPresenter *const projectPresenter,
                         QWidget *parent) :
    QWidget(parent), window(window), projectPresenter(projectPresenter), ui(new Ui::ProjectView), actionMenuNew(WindowInterface::createAction(tr("Clear"), this, &ProjectView::actionProjectNew)), actionMenuLoad(WindowInterface::createAction(tr("Load"), this, &ProjectView::actionProjectLoad)), actionMenuSave(WindowInterface::createAction(tr("Save"), this, &ProjectView::actionProjectSave)), actionMenuStart(WindowInterface::createAction(tr("Start"), this, &ProjectView::actionProjectStart))
{
    // Create UI
    ui->setupUi(this);

    // Update the view
    updateView();

    // Signals-Slots connection declaration for the Menu-bar Actions
    connect(projectPresenter, &ProjectPresenter::loaded,
            this, &ProjectView::updateView);
    connect(projectPresenter, &ProjectPresenter::saved,
            this, &ProjectView::updateView);

    // Signals-Slots connection declaration for actions from the Browser Buttons
    connect(projectPresenter, &ProjectPresenter::updateView,
            this, &ProjectView::updateView);

    // Register view
    window->add(ViewID, WindowInterface::createButton(tr("Project"), 0, 01000), this,
                {actionMenuNew, actionMenuLoad, actionMenuSave, actionMenuStart});
}

ProjectView::~ProjectView()
{
    // Deregister view
    window->remove(ViewID);

    // Destroy UI
    delete ui;
}

// Set the Menu-bar action NEW
void ProjectView::actionProjectNew()
{
    projectPresenter->createNewProject();
    ui->titleEdit->setText("");
}

// Set the Menu-bar action LOAD
void ProjectView::actionProjectLoad()
{
    QDir const root = QDir(QCoreApplication::applicationDirPath());
    QString const filepath = QFileDialog::getOpenFileName(
        this, tr("openPASS / Load Project"), root.canonicalPath(),
        QStringLiteral("Project (*.xml);;All files (*)"));
    if (!filepath.isNull())
    {
        QString filename = QFileInfo(filepath).fileName();
        filename.chop(4);

        ui->titleEdit->setText(filename);
        projectPresenter->loadProject(filepath);
    }
}

// Set the Menu-bar action SAVE
void ProjectView::actionProjectSave()
{
    QDir const root = QDir(QCoreApplication::applicationDirPath());

    QString default_file;

    if (!ui->titleEdit->text().isEmpty())
        default_file = root.canonicalPath() + "/" + ui->titleEdit->text();
    else
        default_file = root.canonicalPath();

    QString const filepath = QFileDialog::getSaveFileName(
        this, tr("openPASS / Save Project"), default_file,
        QStringLiteral("XML File (*.xml)"));
    if (!filepath.isNull())
    {
        projectPresenter->saveProject(filepath);
    }
}

// Set the Menu-bar action SAVE
void ProjectView::actionProjectStart()
{
    projectPresenter->simulate();
}

// Import the Component Library (load path for opSimulationManager)
void ProjectView::on_libraryBrowseButton_clicked()
{
    QDir const root = QDir(QCoreApplication::applicationDirPath());
    QString const filepath = QFileDialog::getExistingDirectory(
        this, tr("openPASS / Directory for simulation libraries"),
        root.canonicalPath());
    if (!filepath.isNull())
    {
        projectPresenter->setLibraryPath(filepath);
    }
}

// Set a Log Level for opSimulationManager
void ProjectView::on_logLevelSpinBox_valueChanged(int level)
{
    projectPresenter->setLogLevel(static_cast<ProjectInterface::LogLevel>(level));
}

// Select path to save the Log opSimulationManager File
void ProjectView::on_logOpSimulationManagerBrowseButton_clicked()
{
    QDir const root = QDir(QCoreApplication::applicationDirPath());
    QString const filepath = QFileDialog::getSaveFileName(
        this, tr("openPASS / Select a opSimulationManager log file"), root.canonicalPath(),
        QStringLiteral("opSimulationManager log file (*.log);;All files (*)"));
    if (!filepath.isNull())
    {
        projectPresenter->setLogOpSimulationManager(filepath);
    }
}

// Select path to save the Log Simulation File
void ProjectView::on_logSimulationBrowseButton_clicked()
{
    QDir const root = QDir(QCoreApplication::applicationDirPath());
    QString const filepath = QFileDialog::getSaveFileName(
        this, tr("openPASS / Select a Simulation log file"), root.canonicalPath(),
        QStringLiteral("Simulation log file (*.log);;All files (*)"));
    if (!filepath.isNull())
    {
        projectPresenter->setLogSimulation(filepath);
    }
}

// Select path to save the results of the simulation(s)
void ProjectView::on_outputBrowseButton_clicked()
{
    QDir const root = QDir(QCoreApplication::applicationDirPath());
    QString const filepath = QFileDialog::getExistingDirectory(
        this, tr("openPASS / Directory for simulation results"),
        root.canonicalPath());
    if (!filepath.isNull())
    {
        projectPresenter->setResultPath(filepath);
    }
}

// Select path to save the Log Simulation File
void ProjectView::on_simulationBrowseButton_clicked()
{
    QDir const root = QDir(QCoreApplication::applicationDirPath());
    QString const filepath = QFileDialog::getOpenFileName(
        this, tr("openPASS / Select the openPASS simulation"), root.canonicalPath(),
        QStringLiteral("Simulation (*.exe)"));
    if (!filepath.isNull())
    {
        projectPresenter->setSimulation(filepath);
    }
}

void ProjectView::on_configBrowseButton_clicked()
{
    QDir const root = QDir(QCoreApplication::applicationDirPath());
    QString const filepath = QFileDialog::getExistingDirectory(
        this, tr("openPASS / Directory for configuration files"),
        root.canonicalPath());
    if (!filepath.isNull())
    {
        projectPresenter->setConfigPath(filepath);
    }
}

// update the information displayed in the View
void ProjectView::updateView()
{
    ui->libraryPathEdit->setText(projectPresenter->getLibraryPath());
    ui->logLevelSpinBox->setValue(projectPresenter->getLogLevel());
    ui->logOpSimulationManagerPathEdit->setText(projectPresenter->getLogOpSimulationManager());
    ui->logSimulationPathEdit->setText(projectPresenter->getLogSimulation());
    ui->outputPathEdit->setText(projectPresenter->getResultPath());
    ui->simulationEdit->setText(projectPresenter->getSimulation());
    ui->configPathEdit->setText(projectPresenter->getConfigPath());
}
