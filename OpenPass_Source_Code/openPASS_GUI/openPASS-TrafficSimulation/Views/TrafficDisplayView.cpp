/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "TrafficDisplayView.h"
#include "ui_TrafficDisplayView.h"

TrafficDisplayView::TrafficDisplayView(QWidget *parent) :
    WidgetView(parent),
    ui(new Ui::TrafficDisplayView)
{
    ui->setupUi(this);

    color = { {StatusType::OK, QString("QGroupBox{border:2px solid green;"
               "border-radius:5px;margin-top: 4ex;} "
               "QGroupBox::title{subcontrol-origin: margin;"
               "subcontrol-position:top center;padding:0 3px;}")},
              {StatusType::Error, QString("QGroupBox{border:2px solid red;"
               "border-radius:5px;margin-top: 4ex;} "
               "QGroupBox::title{subcontrol-origin: margin;"
               "subcontrol-position:top center;padding:0 3px;}")},
              {StatusType::Warning, QString("QGroupBox{border:2px solid orange;border-radius:5px;margin-top: 4ex;} "
               "QGroupBox::title{subcontrol-origin: margin;"
               "subcontrol-position:top center;padding:0 3px;}")}
            };
}

TrafficDisplayView::~TrafficDisplayView()
{
    delete ui;
}


void TrafficDisplayView::setTrafficName(QString name)
{
    ui->TrafficName->setText(name);
}


void TrafficDisplayView::setVolumes(int number)
{
    ui->Volumes->setText(QString::number(number));
}

void TrafficDisplayView::setPlatoonRates(int number)
{
    ui->PlatoonRates->setText(QString::number(number));
}

void TrafficDisplayView::setVelocities(int number)
{
    ui->Velocities->setText(QString::number(number));
}

void TrafficDisplayView::setHomogeneities(int number)
{
    ui->Homogeneities->setText(QString::number(number));
}

void TrafficDisplayView::setRegularLaneAgents(int number)
{
    ui->RegularLane->setText(QString::number(number) );
}

void TrafficDisplayView::setRightmostLaneAgents(int number)
{
    ui->RightmostLane->setText(QString::number(number));
}

void TrafficDisplayView::setStatus(StatusType status, QString message)
{
    ui->Status->setText(message);

    setStyleSheet(color[status]);
}
