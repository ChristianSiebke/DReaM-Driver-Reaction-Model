/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "SystemProfileView.h"

#include "ui_SystemProfileView.h"

SystemProfileView::SystemProfileView(QString AlgoType, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemProfileView)
{
    ui->setupUi(this);

    QFont headerFont;
    headerFont.setBold(true);

    ui->AlgoType_label->setText(QString("%1:").arg(AlgoType));
    ui->SystemView->installEventFilter(this);
    ui->SystemView->horizontalHeader()->setFont(headerFont);

    ui->gridLayout->setColumnStretch(1, 1);

    setObjectName(AlgoType);
}

SystemProfileView::~SystemProfileView()
{
    delete ui;
}

QTableView *SystemProfileView::getSystemView() const
{
    return ui->SystemView;
}

void SystemProfileView::on_addSystem_clicked()
{
    Q_EMIT addSystem(objectName(), ui->SystemSelection->currentText());
}

void SystemProfileView::on_removeSystem_clicked()
{
    Q_EMIT removeSystem(objectName(), ui->SystemView->currentIndex().row());
}

void SystemProfileView::setSelectionItems(QList<QString> const &items)
{
    ui->SystemSelection->clear();
    ui->SystemSelection->addItems(items);
}

bool SystemProfileView::eventFilter(QObject *watched, QEvent *event)
{
    QTableView *table = qobject_cast<QTableView *>(watched);

    if (table && event->type() == QEvent::Resize)
    {
        int cell = (table->width()) / 5;
        table->setColumnWidth(0, cell);
        table->setColumnWidth(1, 2 * cell);
    }

    return false;
}
