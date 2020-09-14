/*********************************************************************
* Copyright (c) 2017, 2018, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#pragma once

#include <iostream>

#include <QGraphicsItem>
#include <QPainter>

class PlotGraphicsItem : public QGraphicsItem
{
public:
    PlotGraphicsItem(QVector<QPointF> *trajectoryData, QPen pen, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    double GetXRange();
    double GetYRange();
    double GetYMin();
    double GetYMax();
    double GetXMin();
    double GetXMax();

private:
    QPen pen;

    double minX = std::numeric_limits<double>::max();
    double minY = std::numeric_limits<double>::max();
    double maxX = -std::numeric_limits<double>::max();
    double maxY = -std::numeric_limits<double>::max();
};
