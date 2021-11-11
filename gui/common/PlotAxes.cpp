/********************************************************************************
 * Copyright (c) 2020 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "PlotAxes.h"

PlotAxes::PlotAxes(double originX, double originY, double graphWidth, double graphHeight, QGraphicsWidget *parent) :
    QGraphicsWidget(parent), originX(static_cast<float>(originX)), originY(static_cast<float>(originY)), graphWidth(static_cast<float>(graphWidth)), graphHeight(static_cast<float>(graphHeight))
{
    yLeftMin = new CenteredTextItem(originX - dist, originY, "", this);
    yLeft1 = new CenteredTextItem(originX - dist, originY + graphHeight * 0.25, "", this);
    yLeft2 = new CenteredTextItem(originX - dist, originY + graphHeight * 0.5, "", this);
    yLeft3 = new CenteredTextItem(originX - dist, originY + graphHeight * 0.75, "", this);
    yLeftMax = new CenteredTextItem(originX - dist, originY + graphHeight, "", this);

    yRightMin = new CenteredTextItem(originX + graphWidth + dist, originY, "", this);
    yRight1 = new CenteredTextItem(originX + graphWidth + dist, originY + graphHeight * 0.25, "", this);
    yRight2 = new CenteredTextItem(originX + graphWidth + dist, originY + graphHeight * 0.5, "", this);
    yRight3 = new CenteredTextItem(originX + graphWidth + dist, originY + graphHeight * 0.75, "", this);
    yRightMax = new CenteredTextItem(originX + graphWidth + dist, originY + graphHeight, "", this);

    xMin = new CenteredTextItem(originX, originY - 20, "", this);
    x1 = new CenteredTextItem(originX + graphWidth * 0.25, originY - 20, "", this);
    x2 = new CenteredTextItem(originX + graphWidth * 0.5, originY - 20, "", this);
    x3 = new CenteredTextItem(originX + graphWidth * 0.75, originY - 20, "", this);
    xMax = new CenteredTextItem(originX + graphWidth, originY - 20, "", this);
}

void PlotAxes::SetYLeft(double yMinLeft, double yMaxLeft)
{
    yLeftMin->SetText(QString::number(static_cast<float>(yMinLeft), 'g', 4));
    yLeft1->SetText(QString::number(static_cast<float>(yMinLeft + std::abs(yMaxLeft - yMinLeft) * 0.25), 'g', 4));
    yLeft2->SetText(QString::number(static_cast<float>(yMinLeft + std::abs(yMaxLeft - yMinLeft) * 0.5), 'g', 4));
    yLeft3->SetText(QString::number(static_cast<float>(yMinLeft + std::abs(yMaxLeft - yMinLeft) * 0.75), 'g', 4));
    yLeftMax->SetText(QString::number(static_cast<float>(yMinLeft + std::abs(yMaxLeft - yMinLeft)), 'g', 4));
}

void PlotAxes::SetYRight(double yMinLeft, double yMaxLeft)
{
    yRightMin->SetText(QString::number(static_cast<float>(yMinLeft), 'g', 4));
    yRight1->SetText(QString::number(static_cast<float>(yMinLeft + std::abs(yMaxLeft - yMinLeft) * 0.25), 'g', 4));
    yRight2->SetText(QString::number(static_cast<float>(yMinLeft + std::abs(yMaxLeft - yMinLeft) * 0.5), 'g', 4));
    yRight3->SetText(QString::number(static_cast<float>(yMinLeft + std::abs(yMaxLeft - yMinLeft) * 0.75), 'g', 4));
    yRightMax->SetText(QString::number(static_cast<float>(yMinLeft + std::abs(yMaxLeft - yMinLeft)), 'g', 4));
}

void PlotAxes::SetX(double yMinLeft, double yMaxLeft)
{
    xMin->SetText(QString::number(static_cast<float>(yMinLeft), 'g', 5));
    x1->SetText(QString::number(static_cast<float>(yMinLeft + std::abs(yMaxLeft - yMinLeft) * 0.25), 'g', 5));
    x2->SetText(QString::number(static_cast<float>(yMinLeft + std::abs(yMaxLeft - yMinLeft) * 0.5), 'g', 5));
    x3->SetText(QString::number(static_cast<float>(yMinLeft + std::abs(yMaxLeft - yMinLeft) * 0.75), 'g', 5));
    xMax->SetText(QString::number(static_cast<float>(yMinLeft + std::abs(yMaxLeft - yMinLeft)), 'g', 5));
}

void PlotAxes::SetBins(QStringList binCenters)
{
    int numBins = binCenters.count();
    float binPixWidth = graphWidth / numBins;
    int i = 0;
    for (auto &elem : binCenters)
    {
        CenteredTextItem *item = new CenteredTextItem(originX + (i + 0.5) * binPixWidth, originY - 20, elem, this);
        ++i;
    }
}

QRectF PlotAxes::boundingRect() const
{
    return yLeftMin->boundingRect().united(yLeftMax->boundingRect());
}

void PlotAxes::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}
