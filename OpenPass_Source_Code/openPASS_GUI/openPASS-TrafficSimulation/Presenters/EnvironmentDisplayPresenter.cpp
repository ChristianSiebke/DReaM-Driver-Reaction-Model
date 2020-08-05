/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "EnvironmentDisplayPresenter.h"

EnvironmentDisplayPresenter::EnvironmentDisplayPresenter(EnvironmentInterface * environment,
                                                         EnvironmentDisplayView * environmentDisplayView,
                                                         QObject *parent)
    : QObject(parent)
    , environment(environment)
    , environmentDisplayView(environmentDisplayView)
{
    connect(environment, &EnvironmentInterface::modifiedName, this, &EnvironmentDisplayPresenter::setEnvironmentNameView);
    connect(environment, &EnvironmentInterface::edited, this,&EnvironmentDisplayPresenter::updateView );

    updateView();
    setStatus();
}


void EnvironmentDisplayPresenter::setEnvironmentNameView()
{
    environmentDisplayView->setEnvironmentName(environment->getName());

    setStatus();
}

void EnvironmentDisplayPresenter::setTimeOfDayView()
{
    environmentDisplayView->setTimeOfDay(environment->getTimeOfDayMap()->count());

    setStatus();
}

void EnvironmentDisplayPresenter::setVisibilityDistanceView()
{
    environmentDisplayView->setVisibilityDistance(environment->getVisibilityDistanceMap()->count());

    setStatus();
}

void EnvironmentDisplayPresenter::setFrictionView()
{
   environmentDisplayView->setFriction(environment->getFrictionMap()->count());

    setStatus();
}

void EnvironmentDisplayPresenter::setWeatherView()
{
    environmentDisplayView->setWeather(environment->getWeatherMap()->count());

    setStatus();
}

void EnvironmentDisplayPresenter::updateView()
{
    setEnvironmentNameView();

    setTimeOfDayView();
    setVisibilityDistanceView();
    setFrictionView();
    setWeatherView();

    setStatus();
}

void EnvironmentDisplayPresenter::setStatus()
{
    bool ok = true;

    if(environment->getTimeOfDayMap()->count() == 0 || !checkProbabilities(environment->getTimeOfDayMap()))
        ok = false;

    else if(environment->getVisibilityDistanceMap()->count() == 0 || !checkProbabilities(environment->getVisibilityDistanceMap()))
        ok = false;

    else if(environment->getFrictionMap()->count() == 0 || !checkProbabilities(environment->getFrictionMap()))
        ok = false;

    else if(environment->getWeatherMap()->count() == 0 || !checkProbabilities(environment->getWeatherMap()))
        ok = false;


    if( ok && environment->getName().isEmpty() )
        environmentDisplayView->setStatus(EnvironmentDisplayView::StatusType::Warning, QString("No name specified"));

    else if(ok)
        environmentDisplayView->setStatus(EnvironmentDisplayView::StatusType::OK, QString("OK"));

    else
        environmentDisplayView->setStatus(EnvironmentDisplayView::StatusType::Error, QString("Error"));

    environment->setStatus(ok);
}

bool EnvironmentDisplayPresenter::checkProbabilities(QMap<EnvironmentInterface::ID, EnvironmentInterface::Item*> * const items)
{
    double sum=0.0;

    for(auto item : items->values())
        sum += item->getProbability();

    return sum == 1.0;
}
