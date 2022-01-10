/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef TRAFFICSCENE_H
#define TRAFFICSCENE_H

#include <QAbstractTableModel>
#include <QGraphicsLineItem>
#include <QGraphicsScene>

#include "pcm_definitions.h"
#include "TrajectoryGraphicsItem.h"

class TrafficScene : public QGraphicsScene
{
public:
    TrafficScene(QObject *parent = 0);

    void AddTrajectory(QVector<QPointF> *trajectoryData);
    void AddMark(QVector<QPointF> *line, MarkType markType);
    void AddObject(QVector<QPointF> *line, ObjectType objectType);
    void RemoveTrajectory(int index);

private:
    void AddLine(QVector<QPointF> *line, QPen pen);
    QList<TrajectoryGraphicsItem *> trajectories;

    std::vector<int> colors;
};

#endif // TRAFFICSCENE_H
