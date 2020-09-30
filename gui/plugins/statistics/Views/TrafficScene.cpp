/*********************************************************************
* Copyright (c) 2017, 2018, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "TrafficScene.h"

TrafficScene::TrafficScene(QObject *parent) :
    QGraphicsScene(parent)
{
    colors.push_back(Qt::blue);
    colors.push_back(Qt::green);
    colors.push_back(Qt::red);
    colors.push_back(Qt::yellow);
    colors.push_back(Qt::cyan);
    colors.push_back(Qt::magenta);
    colors.push_back(Qt::darkBlue);
    colors.push_back(Qt::darkGreen);
    colors.push_back(Qt::darkRed);
    colors.push_back(Qt::darkYellow);
    colors.push_back(Qt::darkCyan);
    colors.push_back(Qt::darkMagenta);
}

void TrafficScene::AddTrajectory(QVector<QPointF> *trajectoryData)
{
    QPen linePen(Qt::GlobalColor(colors.at(trajectories.size() % colors.size())));
    linePen.setWidth(0);

    AddLine(trajectoryData, linePen);
}

void TrafficScene::RemoveTrajectory(int index)
{
    removeItem(trajectories.at(index));
    trajectories.removeAt(index);
}

void TrafficScene::AddLine(QVector<QPointF> *line, QPen pen)
{
    TrajectoryGraphicsItem *trajectory = new TrajectoryGraphicsItem(line, pen);

    addItem(trajectory);

    trajectories.push_back(trajectory);
}
