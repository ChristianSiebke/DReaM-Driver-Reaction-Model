/********************************************************************************
 * Copyright (c) 2020 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <QGraphicsWidget>

#include "CenteredTextItem.h"

class PlotAxes : public QGraphicsWidget
{
public:
    QRectF boundingRect() const;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    PlotAxes(double originX, double originY, double graphWidth, double graphHeight, QGraphicsWidget *parent = nullptr);

    void SetYLeft(double yMinLeft, double yMaxLeft);
    void SetYRight(double yMinLeft, double yMaxLeft);
    void SetX(double yMinLeft, double yMaxLeft);
    void SetBins(QStringList binCenters);

private:
    CenteredTextItem *yLeftMin{nullptr};
    CenteredTextItem *yLeft1{nullptr};
    CenteredTextItem *yLeft2{nullptr};
    CenteredTextItem *yLeft3{nullptr};
    CenteredTextItem *yLeftMax{nullptr};

    CenteredTextItem *yRightMin{nullptr};
    CenteredTextItem *yRight1{nullptr};
    CenteredTextItem *yRight2{nullptr};
    CenteredTextItem *yRight3{nullptr};
    CenteredTextItem *yRightMax{nullptr};

    CenteredTextItem *xMin{nullptr};
    CenteredTextItem *x1{nullptr};
    CenteredTextItem *x2{nullptr};
    CenteredTextItem *x3{nullptr};
    CenteredTextItem *xMax{nullptr};

    float dist = 35;
    float originX = 0.0;
    float originY = 0.0;
    float graphWidth = 0.0;
    float graphHeight = 0.0;
};
