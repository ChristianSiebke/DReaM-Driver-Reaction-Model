/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "EnvironmentModel.h"

EnvironmentModel::EnvironmentModel(QObject *parent) :
    EnvironmentInterface(parent), name(""), timeOfDayMap(new TimeOfDayMap), visibilityDistanceMap(new VisibilityDistanceMap), frictionMap(new FrictionMap), weatherMap(new WeatherMap)
{
}

EnvironmentInterface::Name EnvironmentModel::getName() const
{
    return name;
}

void EnvironmentModel::setName(const Name &_name)
{
    name = _name;
    Q_EMIT modifiedName();
}

EnvironmentModel::~EnvironmentModel()
{
    delete timeOfDayMap;
    delete visibilityDistanceMap;
    delete frictionMap;
    delete weatherMap;
}

EnvironmentInterface::TimeOfDayMap *EnvironmentModel::getTimeOfDayMap() const
{
    return timeOfDayMap;
}

EnvironmentInterface::VisibilityDistanceMap *EnvironmentModel::getVisibilityDistanceMap() const
{
    return visibilityDistanceMap;
}

EnvironmentInterface::FrictionMap *EnvironmentModel::getFrictionMap() const
{
    return frictionMap;
}

EnvironmentInterface::WeatherMap *EnvironmentModel::getWeatherMap() const
{
    return weatherMap;
}

EnvironmentInterface::Item *EnvironmentModel::createItem(const EnvironmentItemInterface::Value &value,
                                                         const EnvironmentItemInterface::Probability &probability)
{
    EnvironmentItemInterface *item = new EnvironmentItemModel(value, probability, this);
    connect(item, &EnvironmentItemInterface::edited, this, &EnvironmentInterface::edited);

    return item;
}

void EnvironmentModel::reset()
{
    name = QString("");

    qDeleteAll(*timeOfDayMap);
    timeOfDayMap->clear();

    qDeleteAll(*visibilityDistanceMap);
    visibilityDistanceMap->clear();

    qDeleteAll(*frictionMap);
    frictionMap->clear();

    qDeleteAll(*weatherMap);
    weatherMap->clear();

    Q_EMIT edited();
}

void EnvironmentModel::setStatus(const Status &_status)
{
    status = _status;
    Q_EMIT modifiedStatus();
}

EnvironmentInterface::Status EnvironmentModel::getStatus() const
{
    return status;
}
