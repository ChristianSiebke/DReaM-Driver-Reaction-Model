/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "TrafficParametersView.h"

#include <iostream>

#include <QDoubleSpinBox>
#include <QResizeEvent>

#include "ui_TrafficParametersView.h"

TrafficParametersView::TrafficParametersView(QWidget *parent) :
    WidgetView(parent),
    ui(new Ui::TrafficParametersView)
{
    ui->setupUi(this);
    initialHeight = height();
}

TrafficParametersView::~TrafficParametersView()
{
    delete ui;
}

void TrafficParametersView::on_addVolume_clicked()
{
    Q_EMIT addVolume();
}

void TrafficParametersView::on_addPlatoonRate_clicked()
{
    Q_EMIT addPlatoonRate();
}

void TrafficParametersView::on_addVelocity_clicked()
{
    Q_EMIT addVelocity();
}

void TrafficParametersView::on_addHomogeneity_clicked()
{
    Q_EMIT addHomogeneity();
}

void TrafficParametersView::addVolumeView(unsigned int id, QString value, double probability)
{
    int index = ui->volumesLayout->count() + 1;
    TrafficItemView *volumeView = new TrafficItemView(TrafficItemView::Type::Volume, id, index, this);
    volumeView->setValue(value);
    volumeView->setProbability(probability);

    connect(volumeView, &TrafficItemView::remove, this, &TrafficParametersView::remove);
    connect(volumeView, &TrafficItemView::valueChanged, this, &TrafficParametersView::valueChanged);
    connect(volumeView, &TrafficItemView::probabilityChanged, this, &TrafficParametersView::probabilityChanged);

    resize(width(), height() + volumeView->height());
    ui->volumesLayout->insertWidget(index - 1, volumeView);
}

void TrafficParametersView::addPlatoonRateView(unsigned int id, QString value, double probability)
{
    int index = ui->platoonRatesLayout->count() + 1;
    TrafficItemView *platoonRateView = new TrafficItemView(TrafficItemView::Type::PlatoonRate, id, index, this);
    platoonRateView->setValue(value);
    platoonRateView->setProbability(probability);

    connect(platoonRateView, &TrafficItemView::remove, this, &TrafficParametersView::remove);
    connect(platoonRateView, &TrafficItemView::valueChanged, this, &TrafficParametersView::valueChanged);
    connect(platoonRateView, &TrafficItemView::probabilityChanged, this, &TrafficParametersView::probabilityChanged);

    resize(width(), height() + platoonRateView->height());
    ui->platoonRatesLayout->insertWidget(index, platoonRateView);
}

void TrafficParametersView::addVelocityView(unsigned int id, QString value, double probability)
{
    int index = ui->velocitiesLayout->count() + 1;
    TrafficItemView *velocityView = new TrafficItemView(TrafficItemView::Type::Velocity, id, index, this);
    velocityView->setValue(value);
    velocityView->setProbability(probability);

    connect(velocityView, &TrafficItemView::remove, this, &TrafficParametersView::remove);
    connect(velocityView, &TrafficItemView::valueChanged, this, &TrafficParametersView::valueChanged);
    connect(velocityView, &TrafficItemView::probabilityChanged, this, &TrafficParametersView::probabilityChanged);

    ui->velocitiesLayout->insertWidget(index, velocityView);
    resize(width(), height() + velocityView->height());
}

void TrafficParametersView::addHomogeneityView(unsigned int id, QString value, double probability)
{
    int index = ui->homogeneitiesLayout->count() + 1;
    TrafficItemView *homogeneityView = new TrafficItemView(TrafficItemView::Type::Homogeneity, id, index, this);
    homogeneityView->setValue(value);
    homogeneityView->setProbability(probability);

    connect(homogeneityView, &TrafficItemView::remove, this, &TrafficParametersView::remove);
    connect(homogeneityView, &TrafficItemView::valueChanged, this, &TrafficParametersView::valueChanged);
    connect(homogeneityView, &TrafficItemView::probabilityChanged, this, &TrafficParametersView::probabilityChanged);

    ui->homogeneitiesLayout->insertWidget(index, homogeneityView);
    resize(width(), height() + homogeneityView->height());
}

void TrafficParametersView::removeTrafficItemView(unsigned int id, TrafficItemView::Type type)
{
    bool shift = false;

    for (auto trafficItemView : findChildren<TrafficItemView *>())
    {
        if (trafficItemView->getType() == type)
        {
            if (shift)
                trafficItemView->setIndex(trafficItemView->getIndex() - 1);

            if (trafficItemView->getID() == id)
            {
                resize(width(), height() - trafficItemView->height());
                delete trafficItemView;
                shift = true;
            }
        }
    }
}

void TrafficParametersView::resizeEvent(QResizeEvent *event)
{
    event->ignore();
    Q_EMIT resized();
}

void TrafficParametersView::setProbability(unsigned int id, TrafficItemView::Type type, double value)
{
    for (auto trafficItemView : findChildren<TrafficItemView *>())
    {
        if (trafficItemView->getType() == type && trafficItemView->getID() == id)
            trafficItemView->setProbability(value);
    }
}

void TrafficParametersView::setValue(unsigned int id, TrafficItemView::Type type, QString value)
{
    for (auto trafficItemView : findChildren<TrafficItemView *>())
    {
        if (trafficItemView->getType() == type && trafficItemView->getID() == id)
            trafficItemView->setValue(value);
    }
}

void TrafficParametersView::clearTrafficParametersView()
{
    qDeleteAll(findChildren<TrafficItemView *>());
    resize(width(), initialHeight);
}
