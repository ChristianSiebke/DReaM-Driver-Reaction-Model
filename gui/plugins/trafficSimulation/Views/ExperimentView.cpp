/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "ExperimentView.h"

#include <QResizeEvent>

#include "ui_ExperimentView.h"

ExperimentView::ExperimentView(QWidget *parent) :
    WidgetView(parent),
    ui(new Ui::ExperimentView)
{
    ui->setupUi(this);

    QFont headerFont;
    headerFont.setBold(true);

    ui->LibrariesView->horizontalHeader()->setFont(headerFont);
    ui->LibrariesView->installEventFilter(this);

    enableSimulation(false);
}

ExperimentView::~ExperimentView()
{
    delete ui;
}

void ExperimentView::on_InvocationsValue_editingFinished()
{
    Q_EMIT Invocations_valueChanged(ui->InvocationsValue->value());
}

void ExperimentView::setInvocationsValue(int value)
{
    ui->InvocationsValue->setValue(value);
}

void ExperimentView::on_SeedValue_editingFinished()
{
    Q_EMIT Seed_valueChanged(ui->SeedValue->value());
}

void ExperimentView::on_Library_add_clicked()
{
    Q_EMIT addLibrary();
}

void ExperimentView::on_Library_remove_clicked()
{
    Q_EMIT removeLibrary(ui->LibrariesView->currentIndex().row());
}

void ExperimentView::on_LoggingGroup_add_clicked()
{
    Q_EMIT addLoggingGroup();
}

void ExperimentView::on_LoggingGroup_remove_clicked()
{
    Q_EMIT removeLoggingGroup(ui->LoggingGroupsView->currentIndex().row());
}

void ExperimentView::on_StartSimulation_clicked()
{
    Q_EMIT startSimulation();
}

QListView *ExperimentView::getLoggingGroupsView() const
{
    return ui->LoggingGroupsView;
}

QTableView *ExperimentView::getLibrariesView() const
{
    return ui->LibrariesView;
}

void ExperimentView::setSeedValue(int value)
{
    ui->SeedValue->setValue(value);
}

void ExperimentView::resizeEvent(QResizeEvent *event)
{
    event->ignore();
    Q_EMIT resized();
}

bool ExperimentView::eventFilter(QObject *watched, QEvent *event)
{
    QTableView *table = qobject_cast<QTableView *>(watched);

    if (table && event->type() == QEvent::Resize)
    {
        int cell = (table->width()) / 2;
        table->setColumnWidth(0, cell);
    }

    return false;
}

void ExperimentView::enableSimulation(bool enable)
{
    ui->StartSimulation->setEnabled(enable);
}
