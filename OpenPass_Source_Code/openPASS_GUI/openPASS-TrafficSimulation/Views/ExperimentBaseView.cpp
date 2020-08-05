/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "ExperimentBaseView.h"

#include <QPaintEvent>
#include <QPainter>


ExperimentBaseView::ExperimentBaseView(QWidget *parent)
    : WidgetView(parent)
{
    experimentView = new ExperimentView(this);
    scenarioDisplayView = new ScenarioDisplayView(this);
    trafficDisplayView = new TrafficDisplayView(this);
    environmentDisplayView = new EnvironmentDisplayView(this);

    connect(experimentView, &ExperimentView::resized, this, &ExperimentBaseView::updateSize);

    adjustWidgets(parent);
}

ExperimentView* ExperimentBaseView::getExperimentView() const
{
    return experimentView;
}

ScenarioDisplayView * ExperimentBaseView::getScenarioDisplayView() const
{
    return scenarioDisplayView;
}

EnvironmentDisplayView * ExperimentBaseView::getEnvironmentDisplayView() const
{
    return environmentDisplayView;
}

TrafficDisplayView * ExperimentBaseView::getTrafficDisplayView() const
{
    return trafficDisplayView;
}

void ExperimentBaseView::updateSize()
{
    QRect frame(0,0,1,1);

    for(auto child : findChildren<WidgetView*>())
        frame = frame.united(child->geometry());

    setGeometry(frame);
}

void ExperimentBaseView::adjustWidgets(QWidget *mainWindow)
{
    int x,y;

    // ajust the geometry of Experiment View:
    x = ( mainWindow->width() - experimentView->width())/2;
    experimentView->move(x,50);

    // adjust the geometry of Environment Display View:
    x = experimentView->x() - environmentDisplayView->width()/2 -130;
    y = experimentView->y() + experimentView->height()+100;
    environmentDisplayView->move(x,y);

    // adjust the geometry of Scenario Display View:
    x = experimentView->x() + experimentView->width()/2 - scenarioDisplayView->width()/2;
    scenarioDisplayView->move(x,y);

    // adjust the geometry of Traffic Display View:
    x = experimentView->x() + experimentView->width() - trafficDisplayView->width()/2 +130;
    trafficDisplayView->move(x,y);

    updateSize();

}

void ExperimentBaseView::paintEvent(QPaintEvent *event)
{
    event->ignore();
    QPainter painter(this);

    QPoint P1(experimentView->x() + experimentView->width()/2, experimentView->y() + experimentView->height()) ;
    QPoint P2(scenarioDisplayView->x() + scenarioDisplayView->width()/2, scenarioDisplayView->y());
    QPoint P3(trafficDisplayView->x() + trafficDisplayView->width()/2, trafficDisplayView->y());
    QPoint P4( environmentDisplayView->x() + environmentDisplayView->width()/2, environmentDisplayView->y() );

    painter.setPen(QColor(QStringLiteral("#777777")));
    QPainterPath path(P1);

    path.lineTo(P2);
    painter.drawPath(path);

    path.lineTo(P1);
    path.lineTo(P3);

    painter.drawPath(path);

    path.lineTo(P1);
    path.lineTo(P4);

    painter.drawPath(path);

}
