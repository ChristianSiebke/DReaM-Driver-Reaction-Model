/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "StatusBarView.h"
#include "ui_StatusBarView.h"

QMap<StatusBarView::Color, QColor> StatusBarView::colorMap = { {StatusBarView::Color::Error, QColor(189,79,79)},
                                                       {StatusBarView::Color::Note, QColor(116,189,79)},
                                                       {StatusBarView::Color::Warning, QColor(255, 204, 0)},
                                                       {StatusBarView::Color::Message, QColor("black")} };

StatusBarView::StatusBarView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatusBarView)
{
    ui->setupUi(this);
    ui->StatusText->setText("");
}

StatusBarView::~StatusBarView()
{
    delete ui;
}


void StatusBarView::on_button_clicked()
{
    ui->StatusText->setVisible(!ui->StatusText->isVisible());
}


void StatusBarView::showText(QString text, Color color)
{
    ui->StatusText->setTextColor(colorMap[color]);
    ui->StatusText->setText(text);
}

void StatusBarView::show()
{
    ui->button->setChecked(true);
    ui->StatusText->setVisible(true);
}

void StatusBarView::hide()
{
    ui->button->setChecked(false);
    ui->StatusText->setVisible(false);
}
