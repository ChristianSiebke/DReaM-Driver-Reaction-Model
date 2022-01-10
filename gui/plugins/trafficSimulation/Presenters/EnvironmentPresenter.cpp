/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "EnvironmentPresenter.h"

#include <QTableView>

#include "DelegateComboBoxView.h"
#include "DelegateDoubleView.h"
#include "DelegateTimeView.h"
#include "Models/EnvironmentItemModel.h"

double const EnvironmentPresenter::default_probability = 0.5;
QString const EnvironmentPresenter::default_timeOfDay = "15:00";
QString const EnvironmentPresenter::default_visibilityDistance = "1000.0";
QString const EnvironmentPresenter::default_friction = "1.00";
QString const EnvironmentPresenter::default_weather = "Sunny";
QStringList const EnvironmentPresenter::weatherList =
    {
        "Sunny",
        "Rainy",
        "Cloudy",
        "Snowy",
        "Foggy",
        "Stormy"};

EnvironmentPresenter::EnvironmentPresenter(EnvironmentInterface *const environment,
                                           EnvironmentView *const environmentView,
                                           QUndoStack *const undoStack,
                                           QObject *parent) :
    QObject(parent), environment(environment), environmentView(environmentView), undoStack(undoStack), timeOfDayPresenter(new EnvironmentMapPresenter(environment, environment->getTimeOfDayMap(), undoStack, this)), visibilityDistancePresenter(new EnvironmentMapPresenter(environment, environment->getVisibilityDistanceMap(), undoStack, this)), frictionPresenter(new EnvironmentMapPresenter(environment, environment->getFrictionMap(), undoStack, this)), weatherPresenter(new EnvironmentMapPresenter(environment, environment->getWeatherMap(), undoStack, this))
{
    SetupTimeOfDay();
    SetupVisibilityDistance();
    SetupFriction();
    SetupWeather();

    connect(environmentView, &EnvironmentView::modifiedName, this, &EnvironmentPresenter::setName);
    connect(environment, &EnvironmentInterface::edited, this, &EnvironmentPresenter::checkProbabilities);
}

void EnvironmentPresenter::checkProbabilities()
{
    // 1. Check probabilities for time of day map
    double sum = 0;
    for (auto item : environment->getTimeOfDayMap()->values())
        sum += item->getProbability();

    environmentView->setTimeOfDay_ProbabilityError(sum != 1.0 && !environment->getTimeOfDayMap()->isEmpty());

    // 2. Check probabilities for visibility distance map
    sum = 0;
    for (auto item : environment->getVisibilityDistanceMap()->values())
        sum += item->getProbability();

    environmentView->setVisibilityDistance_ProbabilityError(sum != 1.0 && !environment->getVisibilityDistanceMap()->isEmpty());

    // 3. Check probabilities for friction map
    sum = 0;
    for (auto item : environment->getFrictionMap()->values())
        sum += item->getProbability();

    environmentView->setFriction_ProbabilityError(sum != 1.0 && !environment->getFrictionMap()->isEmpty());

    // 4. Check probabilities for weather map
    sum = 0;
    for (auto item : environment->getWeatherMap()->values())
        sum += item->getProbability();

    environmentView->setWeather_ProbabilityError(sum != 1.0 && !environment->getWeatherMap()->isEmpty());
}

void EnvironmentPresenter::setName(const QString &name)
{
    if (environment->getName() != name)
        undoStack->push(new EditEnvironmentName(environment, environmentView, name));
}

void EnvironmentPresenter::SetupTimeOfDay()
{
    QTableView *timeOfDayView = environmentView->getTimeOfDayView();

    timeOfDayView->setItemDelegateForColumn(1, new DelegateDoubleView(0, 1, 0.1, 2, "time of day probability", undoStack, this));
    timeOfDayView->setItemDelegateForColumn(0, new DelegateTimeView("time of day", undoStack, this));
    timeOfDayView->setModel(timeOfDayPresenter);

    timeOfDayPresenter->setObjectName("timeOfDayTable");

    connect(environmentView, &EnvironmentView::addTimeOfDay, this, &EnvironmentPresenter::addTimeOfDay);
    connect(environmentView, &EnvironmentView::removeTimeOfDay, this, &EnvironmentPresenter::removeTimeOfDay);
}

void EnvironmentPresenter::SetupVisibilityDistance()
{
    QTableView *visibilityDistanceView = environmentView->getVisibilityDistanceView();

    visibilityDistanceView->setItemDelegateForColumn(1, new DelegateDoubleView(0, 1, 0.1, 2, "visibility distance's probability", undoStack, this));
    visibilityDistanceView->setItemDelegateForColumn(0, new DelegateDoubleView(0, 5000.0, 10.0, 1, "visibility distance's", undoStack, this));
    visibilityDistanceView->setModel(visibilityDistancePresenter);

    visibilityDistancePresenter->setObjectName("visibilityDistanceTable");

    connect(environmentView, &EnvironmentView::addVisibilityDistance, this, &EnvironmentPresenter::addVisibilityDistance);
    connect(environmentView, &EnvironmentView::removeVisibilityDistance, this, &EnvironmentPresenter::removeVisibilityDistance);
}

void EnvironmentPresenter::SetupFriction()
{
    QTableView *frictionView = environmentView->getFrictionView();

    frictionView->setItemDelegateForColumn(1, new DelegateDoubleView(0, 1, 0.1, 2, "friction coefficient's probability", undoStack, this));
    frictionView->setItemDelegateForColumn(0, new DelegateDoubleView(0, 10.0, 0.1, 2, "friction coefficient's", undoStack, this));
    frictionView->setModel(frictionPresenter);

    frictionPresenter->setObjectName("frictionTable");

    connect(environmentView, &EnvironmentView::addFriction, this, &EnvironmentPresenter::addFriction);
    connect(environmentView, &EnvironmentView::removeFriction, this, &EnvironmentPresenter::removeFriction);
}

void EnvironmentPresenter::SetupWeather()
{
    QTableView *weatherView = environmentView->getWeatherView();

    weatherView->setItemDelegateForColumn(1, new DelegateDoubleView(0, 1, 0.1, 2, "weather probability", undoStack, this));
    weatherView->setItemDelegateForColumn(0, new DelegateComboBoxView(&weatherList, "weather", undoStack, this));
    weatherView->setModel(weatherPresenter);

    weatherPresenter->setObjectName("weatherTable");

    connect(environmentView, &EnvironmentView::addWeather, this, &EnvironmentPresenter::addWeather);
    connect(environmentView, &EnvironmentView::removeWeather, this, &EnvironmentPresenter::removeWeather);
}

void EnvironmentPresenter::addTimeOfDay()
{
    undoStack->push(new AddEnvironmentItem(timeOfDayPresenter, default_timeOfDay, default_probability, "time of day table"));
}

void EnvironmentPresenter::removeTimeOfDay(const int &row)
{
    if (timeOfDayPresenter->rowCount() != 0)
        undoStack->push(new RemoveEnvironmentItem(timeOfDayPresenter, row, "time of day table"));
}

void EnvironmentPresenter::addVisibilityDistance()
{
    undoStack->push(new AddEnvironmentItem(visibilityDistancePresenter, default_visibilityDistance, default_probability, "visibility distances table"));
}

void EnvironmentPresenter::removeVisibilityDistance(const int &row)
{
    if (visibilityDistancePresenter->rowCount() != 0)
        undoStack->push(new RemoveEnvironmentItem(visibilityDistancePresenter, row, "visibility distances table"));
}

void EnvironmentPresenter::addFriction()
{
    undoStack->push(new AddEnvironmentItem(frictionPresenter, default_friction, default_probability, "friction coefficients table"));
}

void EnvironmentPresenter::removeFriction(const int &row)
{
    if (frictionPresenter->rowCount() != 0)
        undoStack->push(new RemoveEnvironmentItem(frictionPresenter, row, "friction coefficients table"));
}

void EnvironmentPresenter::addWeather()
{
    undoStack->push(new AddEnvironmentItem(weatherPresenter, default_weather, default_probability, "weather table"));
}

void EnvironmentPresenter::removeWeather(const int &row)
{
    if (weatherPresenter->rowCount() != 0)
        undoStack->push(new RemoveEnvironmentItem(weatherPresenter, row, "weather table"));
}

void EnvironmentPresenter::updateView()
{
    environmentView->setName(environment->getName());

    timeOfDayPresenter->update();
    visibilityDistancePresenter->update();
    frictionPresenter->update();
    weatherPresenter->update();
}

AddEnvironmentItem::AddEnvironmentItem(EnvironmentMapPresenter *const table,
                                       const QString default_value,
                                       const double default_probability,
                                       QString tableName,
                                       QUndoCommand *parent) :
    QUndoCommand(parent), table(table), default_value(default_value), default_probability(default_probability)
{
    setText(QString("added entry to %1").arg(tableName));
}

void AddEnvironmentItem::redo()
{
    table->addRow(table->rowCount(), default_value, default_probability);
}

void AddEnvironmentItem::undo()
{
    table->removeRow(table->rowCount() - 1);
}

RemoveEnvironmentItem::RemoveEnvironmentItem(EnvironmentMapPresenter *const table,
                                             int const row,
                                             QString tableName,
                                             QUndoCommand *parent) :
    QUndoCommand(parent), table(table), row(row), value(table->data(table->index(row, 0)).toString()), probability(table->data(table->index(row, 1)).toDouble())
{
    setText(QString("removed entry from %1").arg(tableName));
}

void RemoveEnvironmentItem::redo()
{
    table->removeRow(row);
}

void RemoveEnvironmentItem::undo()
{
    table->addRow(row, value, probability);
}

EditEnvironmentName::EditEnvironmentName(EnvironmentInterface *const environment,
                                         EnvironmentView *const environmentView,
                                         EnvironmentInterface::Name const &newName,
                                         QUndoCommand *parent) :
    QUndoCommand(parent), environment(environment), environmentView(environmentView), newName(newName), oldName(environment->getName())
{
    setText("edited environment name");
}

void EditEnvironmentName::redo()
{
    environment->setName(newName);
    environmentView->setName(newName);
}

void EditEnvironmentName::undo()
{
    environment->setName(oldName);
    environmentView->setName(oldName);
}
