/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "AgentConfigurationView.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include "ui_AgentConfigurationView.h"

const WindowInterface::ID AgentConfigurationView::ViewID = QStringLiteral("openPASS.AgentConfiguration");

AgentConfigurationView::AgentConfigurationView(WindowInterface *const window,
                                               QWidget *parent) :
    QWidget(parent), window(window), undoView(new QUndoView(this)), actionMenuSave(WindowInterface::createAction(tr("Save"), this, &AgentConfigurationView::actionSave)), actionMenuExport(WindowInterface::createAction(tr("Export"), this, &AgentConfigurationView::actionExport)), actionMenuLoad(WindowInterface::createAction(tr("Load"), this, &AgentConfigurationView::actionLoad)), actionMenuClear(WindowInterface::createAction(tr("Clear"), this, &AgentConfigurationView::actionClear)), actionMenuUndo(WindowInterface::createAction(tr(""), this, &AgentConfigurationView::actionUndo)), actionMenuRedo(WindowInterface::createAction(tr(""), this, &AgentConfigurationView::actionRedo)), actionMenuHistory(WindowInterface::createAction(tr("Command History"), this, &AgentConfigurationView::actionHistory)), ui(new Ui::AgentConfigurationView)
{
    // Create UI
    ui->setupUi(this);

    delete ui->agentContents;
    agentsView = new AgentMapView;
    ui->agentScrollArea->setWidget(agentsView);

    delete ui->vehicleContents;
    vehicleProfilesView = new VehicleProfilesView;
    ui->vehicleScrollArea->setWidget(vehicleProfilesView);

    delete ui->driverContents;
    driverProfilesView = new DriverProfilesView;
    ui->driverScrollArea->setWidget(driverProfilesView);

    // Register view
    window->add(ViewID, WindowInterface::createButton(tr("Agent Configuration"), 0, 05000), this,
                {actionMenuClear, actionMenuLoad, actionMenuSave, actionMenuExport, actionMenuUndo, actionMenuRedo, actionMenuHistory});

    // prepare undo view and undo/redo buttons
    actionMenuRedo->setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_ArrowRight));
    actionMenuUndo->setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_ArrowLeft));
    actionMenuRedo->setToolTip("redo previous action");
    actionMenuUndo->setToolTip("undo last action");

    undoView->raise();
    undoView->hide();
}

AgentConfigurationView::~AgentConfigurationView()
{
    // Deregister view
    window->remove(ViewID);

    // Destroy UI
    delete ui;
}

void AgentConfigurationView::actionHistory()
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

void AgentConfigurationView::actionSave()
{
    Q_EMIT SaveSetup();
}

void AgentConfigurationView::actionExport()
{
    Q_EMIT ExportSetup();
}

void AgentConfigurationView::actionLoad()
{
    Q_EMIT LoadSetup();
}

void AgentConfigurationView::actionRedo()
{
    Q_EMIT Redo();
}

void AgentConfigurationView::actionUndo()
{
    Q_EMIT Undo();
}

void AgentConfigurationView::actionClear()
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
        Q_EMIT ClearSetup();
    }
}

AgentMapView *AgentConfigurationView::getAgentsView() const
{
    return agentsView;
}

VehicleProfilesView *AgentConfigurationView::getVehicleProfilesView() const
{
    return vehicleProfilesView;
}

DriverProfilesView *AgentConfigurationView::getDriverProfilesView() const
{
    return driverProfilesView;
}

QUndoView *AgentConfigurationView::getUndoView() const
{
    return undoView;
}
