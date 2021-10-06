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

#include <QGraphicsSimpleTextItem>
#include <QGraphicsWidget>

class CenteredTextItem : public QGraphicsWidget
{
public:
    QRectF boundingRect() const;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    CenteredTextItem(double centerX, double centerY, QString text = "", QGraphicsWidget *parent = nullptr, bool rotate = false);

    void SetText(QString text);

private:
    QGraphicsSimpleTextItem *centered{nullptr};
    double centerX = 0.0;
    double centerY = 0.0;
    bool rotate = false;
};
