/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "ScenarioDisplayView.h"
#include "ui_ScenarioDisplayView.h"

ScenarioDisplayView::ScenarioDisplayView(QWidget *parent) :
    WidgetView(parent),
    ui(new Ui::ScenarioDisplayView)
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

ScenarioDisplayView::~ScenarioDisplayView()
{
    delete ui;
}

void ScenarioDisplayView::setScenarioName(QString name)
{
    ui->ScenarioName->setText(name);
}

void ScenarioDisplayView::setScenarioFile(QString scenarioFile)
{
    ui->ScenarioFile->setText(scenarioFile);
}

void ScenarioDisplayView::setScenarioAgents(int scenarioAgents)
{
    ui->ScenarioAgents->setText(QString::number(scenarioAgents));
}

void ScenarioDisplayView::setStatus(StatusType status, QString message)
{
    // display message on status
    ui->Status->setText(message);

    // coloring of the frame
    setStyleSheet(color[status]);
}
