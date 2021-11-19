/********************************************************************************
 * Copyright (c) 2017-2020 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "PlotGraphicsItem.h"

PlotGraphicsItem::PlotGraphicsItem(QVector<QPointF> *trajectoryData,
                                   QPen pen,
                                   QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    if (trajectoryData == nullptr)
    {
        return;
    }
    for (int i = 1; i < trajectoryData->size(); i++)
    {
        QPointF firstPoint = trajectoryData->at(i - 1);
        QPointF secondPoint = trajectoryData->at(i);
        QGraphicsLineItem *line = new QGraphicsLineItem(QLineF(firstPoint,
                                                               secondPoint),
                                                        this);
        line->setPen(pen);

        minX = std::min(minX, std::min(firstPoint.x(), secondPoint.x()));
        minY = std::min(minY, std::min(firstPoint.y(), secondPoint.y()));
        maxX = std::max(maxX, std::max(firstPoint.x(), secondPoint.x()));
        maxY = std::max(maxY, std::max(firstPoint.y(), secondPoint.y()));
    }
    if (std::abs(minX - maxX) < std::numeric_limits<double>::epsilon())
    {
        if (minX > 0.5)
        {
            minX = 0.0;
            maxX = 2 * maxX;
        }
        else if (minX < -0.5)
        {
            minX = 2 * minX;
            maxX = 0.0;
        }
        else
        {
            minX = -1.0;
            maxX = 1.0;
        }
    }
    if (std::abs(minY - maxY) < std::numeric_limits<double>::epsilon())
    {
        if (minY > 0.5)
        {
            minY = 0.0;
            maxY = 2 * maxY;
        }
        else if (minY < -0.5)
        {
            minY = 2 * minY;
            maxY = 0.0;
        }
        else
        {
            minY = -1.0;
            maxY = 1.0;
        }
    }
}

double PlotGraphicsItem::GetYMin()
{
    return minY;
}

double PlotGraphicsItem::GetYMax()
{
    return maxY;
}

double PlotGraphicsItem::GetXMin()
{
    return minX;
}

double PlotGraphicsItem::GetXMax()
{
    return maxX;
}

QRectF PlotGraphicsItem::boundingRect() const
{
    return QRectF(QPointF(minX, minY), QPointF(maxX, maxY));
}

void PlotGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                             QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}
