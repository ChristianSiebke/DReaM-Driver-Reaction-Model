/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "TrafficView.h"

#include "ui_TrafficView.h"

TrafficView::TrafficView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrafficView)
{
    ui->setupUi(this);

    QFont headerFont;
    headerFont.setBold(true);

    ui->VolumesView->horizontalHeader()->setFont(headerFont);
    ui->PlatoonRatesView->horizontalHeader()->setFont(headerFont);
    ui->VelocitiesView->horizontalHeader()->setFont(headerFont);
    ui->HomogeneitiesView->horizontalHeader()->setFont(headerFont);

    ui->RegularLaneAgentsView->horizontalHeader()->setFont(headerFont);
    ui->RightmostLaneAgentsView->horizontalHeader()->setFont(headerFont);

    // Event filters for resizing the table columns
    ui->VolumesView->installEventFilter(this);
    ui->PlatoonRatesView->installEventFilter(this);
    ui->VelocitiesView->installEventFilter(this);
    ui->HomogeneitiesView->installEventFilter(this);

    ui->RegularLaneAgentsView->installEventFilter(this);
    ui->RightmostLaneAgentsView->installEventFilter(this);
}

TrafficView::~TrafficView()
{
    delete ui;
}

void TrafficView::setName(const QString &_name)
{
    ui->NameEdit->setText(_name);
}

void TrafficView::on_NameEdit_editingFinished()
{
    Q_EMIT modifiedName(ui->NameEdit->text());
}

QTableView *TrafficView::getVolumesView() const
{
    return ui->VolumesView;
}

QTableView *TrafficView::getVelocitiesView() const
{
    return ui->VelocitiesView;
}

QTableView *TrafficView::getPlatoonRatesView() const
{
    return ui->PlatoonRatesView;
}

QTableView *TrafficView::getHomogeneitiesView() const
{
    return ui->HomogeneitiesView;
}

QTableView *TrafficView::getRegularLaneAgentsView() const
{
    return ui->RegularLaneAgentsView;
}

QTableView *TrafficView::getRightmostLaneAgentsView() const
{
    return ui->RightmostLaneAgentsView;
}

void TrafficView::on_addVolume_clicked()
{
    Q_EMIT addVolume();
}

void TrafficView::on_removeVolume_clicked()
{
    Q_EMIT removeVolume(ui->VolumesView->currentIndex().row());
}

void TrafficView::on_addPlatoonRate_clicked()
{
    Q_EMIT addPlatoonRate();
}

void TrafficView::on_removePlatoonRate_clicked()
{
    Q_EMIT removePlatoonRate(ui->PlatoonRatesView->currentIndex().row());
}

void TrafficView::on_addVelocity_clicked()
{
    Q_EMIT addVelocity();
}

void TrafficView::on_removeVelocity_clicked()
{
    Q_EMIT removeVelocity(ui->VelocitiesView->currentIndex().row());
}

void TrafficView::on_addHomogeneity_clicked()
{
    Q_EMIT addHomogeneity();
}

void TrafficView::on_removeHomogeneity_clicked()
{
    Q_EMIT removeHomogeneity(ui->HomogeneitiesView->currentIndex().row());
}

void TrafficView::on_addRegularLaneAgent_clicked()
{
    Q_EMIT addRegularLaneAgent();
}

void TrafficView::on_removeRegularLaneAgent_clicked()
{
    Q_EMIT removeRegularLaneAgent(ui->RegularLaneAgentsView->currentIndex().row());
}

void TrafficView::on_addRightmostLaneAgent_clicked()
{
    Q_EMIT addRightmostLaneAgent();
}

void TrafficView::on_removeRightmostLaneAgent_clicked()
{
    Q_EMIT removeRightmostLaneAgent(ui->RightmostLaneAgentsView->currentIndex().row());
}

bool TrafficView::eventFilter(QObject *watched, QEvent *event)
{
    QTableView *table = qobject_cast<QTableView *>(watched);

    if (table && event->type() == QEvent::Resize)
    {
        int cell = (table->width()) / 2;
        table->setColumnWidth(0, cell);
    }

    return false;
}

void TrafficView::setVelocities_ProbabilityError(bool error)
{
    if (error)
        ui->Velocity_ProbabilityError->setText("probability sum invalid!");
    else
        ui->Velocity_ProbabilityError->setText("");
}

void TrafficView::setVolumes_ProbabilityError(bool error)
{
    if (error)
        ui->Volume_ProbabilityError->setText("probability sum invalid!");
    else
        ui->Volume_ProbabilityError->setText("");
}

void TrafficView::setPlatoonRates_ProbabilityError(bool error)
{
    if (error)
        ui->PlatoonRate_ProbabilityError->setText("probability sum invalid!");
    else
        ui->PlatoonRate_ProbabilityError->setText("");
}

void TrafficView::setHomogeneities_ProbabilityError(bool error)
{
    if (error)
        ui->Homogeneity_ProbabilityError->setText("probability sum invalid!");
    else
        ui->Homogeneity_ProbabilityError->setText("");
}

void TrafficView::setRegularLane_ProbabilityError(bool error)
{
    if (error)
        ui->RegularLane_ProbabilityError->setText("probability sum invalid!");
    else
        ui->RegularLane_ProbabilityError->setText("");
}

void TrafficView::setRightmostLane_ProbabilityError(bool error)
{
    if (error)
        ui->RightmostLane_ProbabilityError->setText("probability sum invalid!");
    else
        ui->RightmostLane_ProbabilityError->setText("");
}

void TrafficView::setAgentProfilesError(const QString &message)
{
    ui->agentProfilesError->setText(message);
}
