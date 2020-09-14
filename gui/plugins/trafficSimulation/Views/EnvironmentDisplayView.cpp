/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "EnvironmentDisplayView.h"

#include "ui_EnvironmentDisplayView.h"

EnvironmentDisplayView::EnvironmentDisplayView(QWidget *parent) :
    WidgetView(parent),
    ui(new Ui::EnvironmentDisplayView)
{
    ui->setupUi(this);

    color = {{StatusType::OK, QString("QGroupBox{border:2px solid green;"
                                      "border-radius:5px;margin-top: 4ex;} "
                                      "QGroupBox::title{subcontrol-origin: margin;"
                                      "subcontrol-position:top center;padding:0 3px;}")},
             {StatusType::Error, QString("QGroupBox{border:2px solid red;"
                                         "border-radius:5px;margin-top: 4ex;} "
                                         "QGroupBox::title{subcontrol-origin: margin;"
                                         "subcontrol-position:top center;padding:0 3px;}")},
             {StatusType::Warning, QString("QGroupBox{border:2px solid orange;border-radius:5px;margin-top: 4ex;} "
                                           "QGroupBox::title{subcontrol-origin: margin;"
                                           "subcontrol-position:top center;padding:0 3px;}")}};
}

EnvironmentDisplayView::~EnvironmentDisplayView()
{
    delete ui;
}

void EnvironmentDisplayView::setEnvironmentName(QString name)
{
    ui->EnvironmentName->setText(name);
}

void EnvironmentDisplayView::setTimeOfDay(int number)
{
    ui->TimeOfDay->setText(QString::number(number));
}

void EnvironmentDisplayView::setVisibilityDistance(int number)
{
    ui->Visibility->setText(QString::number(number));
}

void EnvironmentDisplayView::setFriction(int number)
{
    ui->Friction->setText(QString::number(number));
}

void EnvironmentDisplayView::setWeather(int number)
{
    ui->Weather->setText(QString::number(number));
}

void EnvironmentDisplayView::setStatus(StatusType status, QString message)
{
    ui->Status->setText(message);

    setStyleSheet(color[status]);
}
