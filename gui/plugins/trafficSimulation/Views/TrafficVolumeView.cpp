/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "TrafficVolumeView.h"

#include "ui_TrafficVolumeView.h"

TrafficVolumeView::TrafficVolumeView(unsigned int ID,
                                     int index,
                                     QWidget *parent) :
    QWidget(parent),
    ID(ID),
    index(index),
    ui(new Ui::TrafficVolumeView)
{
    ui->setupUi(this);
}

TrafficVolumeView::~TrafficVolumeView()
{
    delete ui;
}

void TrafficVolumeView::on_remove_clicked()
{
    Q_EMIT remove(ID);
}

void TrafficVolumeView::on_probabilityValue_editingFinished()
{
    Q_EMIT probabilityChanged(ID, ui->probabilityValue->value());
}

void TrafficVolumeView::on_VolumeValue_editingFinished()
{
    Q_EMIT valueChanged(ID, ui->VolumeValue->value());
}

unsigned int TrafficVolumeView::getID() const
{
}
