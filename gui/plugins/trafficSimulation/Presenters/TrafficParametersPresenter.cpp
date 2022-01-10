/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "TrafficParametersPresenter.h"

#include <iostream>

TrafficParametersPresenter::TrafficParametersPresenter(TrafficInterface *traffic,
                                                       TrafficParametersView *trafficParametersView,
                                                       QObject *parent) :
    QObject(parent), traffic(traffic), trafficParametersView(trafficParametersView)
{
    connect(trafficParametersView, &TrafficParametersView::addVolume, this, &TrafficParametersPresenter::addVolume);
    connect(trafficParametersView, &TrafficParametersView::addPlatoonRate, this, &TrafficParametersPresenter::addPlatoonRate);
    connect(trafficParametersView, &TrafficParametersView::addVelocity, this, &TrafficParametersPresenter::addVelocity);
    connect(trafficParametersView, &TrafficParametersView::addHomogeneity, this, &TrafficParametersPresenter::addHomogeneity);

    connect(trafficParametersView, &TrafficParametersView::remove, this, &TrafficParametersPresenter::removeTrafficItem);
    connect(trafficParametersView, &TrafficParametersView::probabilityChanged, this, &TrafficParametersPresenter::setProbability);
    connect(trafficParametersView, &TrafficParametersView::valueChanged, this, &TrafficParametersPresenter::setValue);
}

void TrafficParametersPresenter::addVolume()
{
    unsigned int id = traffic->getVolumes()->generateID();
    traffic->getVolumes()->add(id, default_volume);
    trafficParametersView->addVolumeView(id, QString("%1").arg(default_volume), default_probability);

    Q_EMIT volume_added();
}

void TrafficParametersPresenter::addPlatoonRate()
{
    unsigned int id = traffic->getPlatoonRates()->generateID();
    traffic->getPlatoonRates()->add(id, default_platoonRate);
    trafficParametersView->addPlatoonRateView(id, QString("%1").arg(default_platoonRate), default_probability);

    Q_EMIT platoonRate_added();
}

void TrafficParametersPresenter::addVelocity()
{
    unsigned int id = traffic->getVelocities()->generateID();
    traffic->getVelocities()->add(id, default_velocity);
    trafficParametersView->addVelocityView(id, QString("%1").arg(default_velocity), default_probability);

    Q_EMIT velocity_added();
}

void TrafficParametersPresenter::addHomogeneity()
{
    unsigned int id = traffic->getHomogeneities()->generateID();
    traffic->getHomogeneities()->add(id, default_homogeneity);
    trafficParametersView->addHomogeneityView(id, QString("%1").arg(default_homogeneity), default_probability);

    Q_EMIT homogeneity_added();
}

void TrafficParametersPresenter::removeTrafficItem(unsigned int id, TrafficItemView::Type type)
{
    switch (type)
    {
    case TrafficItemView::Type::Volume: {
        if (traffic->getVolumes()->remove(id))
        {
            trafficParametersView->removeTrafficItemView(id, type);
            Q_EMIT volume_removed();
        }

        break;
    }
    case TrafficItemView::Type::PlatoonRate: {
        if (traffic->getPlatoonRates()->remove(id))
        {
            trafficParametersView->removeTrafficItemView(id, type);
            Q_EMIT platoonRate_removed();
        }

        break;
    }
    case TrafficItemView::Type::Velocity: {
        if (traffic->getVelocities()->remove(id))
        {
            trafficParametersView->removeTrafficItemView(id, type);
            Q_EMIT velocity_removed();
        }

        break;
    }
    case TrafficItemView::Type::Homogeneity: {
        if (traffic->getHomogeneities()->remove(id))
        {
            trafficParametersView->removeTrafficItemView(id, type);
            Q_EMIT homogeneity_removed();
        }

        break;
    }
    default:
        break;
    }
}

void TrafficParametersPresenter::setProbability(unsigned int id, double value, TrafficItemView::Type type)
{
    switch (type)
    {
    case TrafficItemView::Type::Volume: {
        traffic->getVolumes()->getItem(id)->setProbability(value);
        Q_EMIT volume_probabilityChanged();
        break;
    }
    case TrafficItemView::Type::PlatoonRate: {
        traffic->getPlatoonRates()->getItem(id)->setProbability(value);
        Q_EMIT platoonRate_probabilityChanged();
        break;
    }
    case TrafficItemView::Type::Velocity: {
        traffic->getVelocities()->getItem(id)->setProbability(value);
        Q_EMIT velocity_probabilityChanged();
        break;
    }
    case TrafficItemView::Type::Homogeneity: {
        traffic->getHomogeneities()->getItem(id)->setProbability(value);
        Q_EMIT homogeneity_probabilityChanged();
        break;
    }
    default:
        break;
    }
}

void TrafficParametersPresenter::setValue(unsigned int id, QString value, TrafficItemView::Type type)
{
    switch (type)
    {
    case TrafficItemView::Type::Volume: {
        traffic->getVolumes()->getItem(id)->setValue(value.toInt());
        break;
    }
    case TrafficItemView::Type::PlatoonRate: {
        traffic->getPlatoonRates()->getItem(id)->setValue(value.toDouble());
        break;
    }
    case TrafficItemView::Type::Velocity: {
        traffic->getVelocities()->getItem(id)->setValue(value.toDouble());
        break;
    }
    case TrafficItemView::Type::Homogeneity: {
        traffic->getHomogeneities()->getItem(id)->setValue(value.toDouble());
        break;
    }
    default:
        break;
    }
}

void TrafficParametersPresenter::updateView()
{
    trafficParametersView->clearTrafficParametersView();
    Q_EMIT cleared();

    // Setup all volumes
    TrafficItemMapInterface<int> *volumes = traffic->getVolumes();

    for (TrafficItemInterface<int> *volume : volumes->values())
    {
        TrafficItemMapInterface<int>::ID id = volumes->getID(volume);
        int value = volume->getValue();
        double probability = volume->getProbability();

        trafficParametersView->addVolumeView(id, QString("%1").arg(value), probability);
        Q_EMIT volume_added();
    }

    // Setup all platoon rates
    TrafficItemMapInterface<double> *platoonRates = traffic->getPlatoonRates();

    for (TrafficItemInterface<double> *platoonRate : platoonRates->values())
    {
        TrafficItemMapInterface<double>::ID id = platoonRates->getID(platoonRate);
        double value = platoonRate->getValue();
        double probability = platoonRate->getProbability();

        trafficParametersView->addPlatoonRateView(id, QString("%1").arg(value), probability);
        Q_EMIT platoonRate_added();
    }

    // Setup all velocities
    TrafficItemMapInterface<double> *velocities = traffic->getVelocities();

    for (TrafficItemInterface<double> *velocity : velocities->values())
    {
        TrafficItemMapInterface<double>::ID id = velocities->getID(velocity);
        double value = velocity->getValue();
        double probability = velocity->getProbability();

        trafficParametersView->addVelocityView(id, QString("%1").arg(value), probability);
        Q_EMIT velocity_added();
    }

    // Setup all homogeneities
    TrafficItemMapInterface<double> *homogeneities = traffic->getHomogeneities();

    for (TrafficItemInterface<double> *homogeneity : homogeneities->values())
    {
        TrafficItemMapInterface<double>::ID id = homogeneities->getID(homogeneity);
        double value = homogeneity->getValue();
        double probability = homogeneity->getProbability();

        trafficParametersView->addHomogeneityView(id, QString("%1").arg(value), probability);
        Q_EMIT homogeneity_added();
    }
}

void TrafficParametersPresenter::clearTrafficParameters()
{
    trafficParametersView->clearTrafficParametersView();
    Q_EMIT cleared();
}
