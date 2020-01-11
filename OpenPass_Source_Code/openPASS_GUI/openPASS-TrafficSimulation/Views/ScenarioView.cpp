/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "ScenarioView.h"
#include "ui_ScenarioView.h"

#include <QDir>
#include <QFileDialog>

ScenarioView::ScenarioView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScenarioView)
{
    ui->setupUi(this);

    QFont headerFont;
    headerFont.setBold(true);
    ui->ScenarioAgentsView->horizontalHeader()->setFont(headerFont);
    ui->ScenarioAgentsView->installEventFilter(this);
}

ScenarioView::~ScenarioView()
{
    delete ui;
}

void ScenarioView::on_NameEdit_editingFinished()
{
    Q_EMIT modifiedName(ui->NameEdit->text());
}

void ScenarioView::on_ScenarioFileBrowse_clicked()
{
    Q_EMIT modifiedScenarioFile();
}

void ScenarioView::on_ScenarioFileRefresh_clicked()
{
    Q_EMIT refreshScenarioFile();
}

void ScenarioView::setName(const QString &name)
{
    ui->NameEdit->setText(name);
}

void ScenarioView::setScenarioFile(const QString &filename)
{
    ui->ScenarioFileEdit->setText(filename);
}

QTableView * ScenarioView::getScenarioAgentsView() const
{
    return ui->ScenarioAgentsView;
}

bool ScenarioView::eventFilter(QObject *watched, QEvent *event)
{
    QTableView* table = qobject_cast<QTableView*> (watched);

    if( table && event->type() == QEvent::Resize )
    {
        int cell = (table->width())/3;
        table->setColumnWidth(0, cell);
        table->setColumnWidth(1, cell);
    }

    return false;
}

void ScenarioView::setScenarioFileError(QString const &message)
{
    ui->ScenarioFileError->setText(message);
}
