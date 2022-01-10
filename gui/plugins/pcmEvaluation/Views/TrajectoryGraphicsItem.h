/********************************************************************************
 * Copyright (c) 2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef TRAJECTORYGRAPHICSITEM_H
#define TRAJECTORYGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QPainter>

class TrajectoryGraphicsItem : public QGraphicsItem
{
public:
    TrajectoryGraphicsItem(QVector<QPointF> *trajectoryData, QPen pen, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);

private:
    QPen pen;

    double minX = std::numeric_limits<double>::max();
    double minY = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double maxY = std::numeric_limits<double>::lowest();
};

#endif // TRAJECTORYGRAPHICSITEM_H
