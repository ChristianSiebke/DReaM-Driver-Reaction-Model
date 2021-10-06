/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "ProfileItemView.h"

#include "ui_ProfileItemView.h"

ProfileItemView::ProfileItemView(QString const &label,
                                 QString const &name,
                                 double probability,
                                 QWidget *parent) :
    WidgetView(parent),
    name(name),
    ui(new Ui::ProfileItemView)
{
    ui->setupUi(this);
    ui->Label->setText(label);

    ui->ItemTitle->setText(name);

    ui->probability->setValue(probability);
}

ProfileItemView::~ProfileItemView()
{
    delete ui;
}

void ProfileItemView::on_probability_editingFinished()
{
    Q_EMIT probabilityChanged(name, ui->probability->value());
}

void ProfileItemView::on_remove_clicked()
{
    Q_EMIT remove(name);
}

void ProfileItemView::setLabel(QString const &label)
{
    ui->Label->setText(label);
}

void ProfileItemView::setName(const QString &_name)
{
    name = _name;
    ui->ItemTitle->setText(name);
}
