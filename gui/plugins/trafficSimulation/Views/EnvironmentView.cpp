/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "EnvironmentView.h"

#include "ui_EnvironmentView.h"

EnvironmentView::EnvironmentView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EnvironmentView)
{
    ui->setupUi(this);

    QFont headerFont;
    headerFont.setBold(true);

    ui->TimeOfDay->horizontalHeader()->setFont(headerFont);
    ui->VisibilityDistance->horizontalHeader()->setFont(headerFont);
    ui->Friction->horizontalHeader()->setFont(headerFont);
    ui->Weather->horizontalHeader()->setFont(headerFont);

    // install event filters for proper sizing of table columns
    ui->TimeOfDay->installEventFilter(this);
    ui->VisibilityDistance->installEventFilter(this);
    ui->Friction->installEventFilter(this);
    ui->Weather->installEventFilter(this);
}

EnvironmentView::~EnvironmentView()
{
    delete ui;
}

void EnvironmentView::on_add_TimeOfDay_clicked()
{
    Q_EMIT addTimeOfDay();
}

void EnvironmentView::on_remove_TimeOfDay_clicked()
{
    Q_EMIT removeTimeOfDay(ui->TimeOfDay->currentIndex().row());
}

void EnvironmentView::on_add_VisibilityDistance_clicked()
{
    Q_EMIT addVisibilityDistance();
}

void EnvironmentView::on_remove_VisibilityDistance_clicked()
{
    Q_EMIT removeVisibilityDistance(ui->VisibilityDistance->currentIndex().row());
}

void EnvironmentView::on_add_Friction_clicked()
{
    Q_EMIT addFriction();
}

void EnvironmentView::on_remove_Friction_clicked()
{
    Q_EMIT removeFriction(ui->Friction->currentIndex().row());
}

void EnvironmentView::on_add_Weather_clicked()
{
    Q_EMIT addWeather();
}

void EnvironmentView::on_remove_Weather_clicked()
{
    Q_EMIT removeWeather(ui->Weather->currentIndex().row());
}

QTableView *EnvironmentView::getTimeOfDayView() const
{
    return ui->TimeOfDay;
}

QTableView *EnvironmentView::getWeatherView() const
{
    return ui->Weather;
}

QTableView *EnvironmentView::getFrictionView() const
{
    return ui->Friction;
}

QTableView *EnvironmentView::getVisibilityDistanceView() const
{
    return ui->VisibilityDistance;
}

void EnvironmentView::on_Name_editingFinished()
{
    Q_EMIT modifiedName(ui->Name->text());
}

void EnvironmentView::setName(const QString &name)
{
    ui->Name->setText(name);
}

bool EnvironmentView::eventFilter(QObject *watched, QEvent *event)
{
    QTableView *table = qobject_cast<QTableView *>(watched);

    if (table && event->type() == QEvent::Resize)
    {
        int cell = (table->width()) / 2;
        table->setColumnWidth(0, cell);
    }

    return false;
}

void EnvironmentView::setTimeOfDay_ProbabilityError(bool error)
{
    if (error)
        ui->TimeOfDay_ProbabilityError->setText("probability sum invalid!");
    else
        ui->TimeOfDay_ProbabilityError->setText("");
}

void EnvironmentView::setVisibilityDistance_ProbabilityError(bool error)
{
    if (error)
        ui->VisibilityDistance_ProbabilityError->setText("probability sum invalid!");
    else
        ui->VisibilityDistance_ProbabilityError->setText("");
}

void EnvironmentView::setFriction_ProbabilityError(bool error)
{
    if (error)
        ui->Friction_ProbabilityError->setText("probability sum invalid!");
    else
        ui->Friction_ProbabilityError->setText("");
}

void EnvironmentView::setWeather_ProbabilityError(bool error)
{
    if (error)
        ui->Weather_ProbabilityError->setText("probability sum invalid!");
    else
        ui->Weather_ProbabilityError->setText("");
}
