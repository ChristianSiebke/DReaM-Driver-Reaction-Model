/*********************************************************************
* Copyright (c) 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "CenteredTextItem.h"

CenteredTextItem::CenteredTextItem(double centerX, double centerY, QString text, QGraphicsWidget *parent, bool rotate) : QGraphicsWidget(parent), centerX(centerX), centerY(centerY), rotate(rotate)
{
    centered = new QGraphicsSimpleTextItem(text, this);

    QTransform matrix = QTransform::fromScale(1.0, -1.0);
    if (rotate)
        matrix.rotate(-90);
    centered->setTransform(matrix);

    QRectF rect = centered->sceneBoundingRect();
    if (rotate)
        centered->setPos( centerX - rect.width() / 2, centerY - rect.height() / 2 );
    else
        centered->setPos( centerX - rect.width() / 2, centerY + rect.height() / 2 );
}

void CenteredTextItem::SetText(QString text)
{
    centered->setText(text);
    QRectF rect = centered->sceneBoundingRect();
    if (rotate)
        centered->setPos( centerX - rect.width() / 2, centerY - rect.height() / 2 );
    else
        centered->setPos( centerX - rect.width() / 2, centerY + rect.height() / 2 );
}

QRectF CenteredTextItem::boundingRect() const
{
    return centered->boundingRect();
}

void CenteredTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                                   QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}
