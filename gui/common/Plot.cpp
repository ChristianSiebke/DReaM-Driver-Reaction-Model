/*********************************************************************
* Copyright (c) 2017, 2018, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "Plot.h"

Plot::Plot() :
    QGraphicsWidget()
{
    QTransform matrix = QTransform::fromScale(1.0, -1.0);
    this->setTransform(matrix);

    colors.push_back(Qt::blue);
    colors.push_back(Qt::green);
    colors.push_back(Qt::red);
    colors.push_back(Qt::darkBlue);
    colors.push_back(Qt::darkGreen);
    colors.push_back(Qt::darkRed);
    colors.push_back(Qt::darkYellow);
    colors.push_back(Qt::darkCyan);
    colors.push_back(Qt::darkMagenta);
    colors.push_back(Qt::cyan);
    colors.push_back(Qt::magenta);

    leftAxisGraph = new Graph(originX, originY, graphWidth, graphHeight, this);
    rightAxisGraph = new Graph(originX, originY, graphWidth, graphHeight, this);

    xLegend = new CenteredTextItem(originX + graphWidth / 2, 25, "", this);
    yLegendLeft = new CenteredTextItem(20, originY + graphHeight / 2, "", this, true);
    yLegendRight = new CenteredTextItem(640, originY + graphHeight / 2, "", this, true);

    axes = new PlotAxes(originX, originY, graphWidth, graphHeight, this);

    leftTick1 = new QGraphicsLineItem(originX - tick, originY + graphHeight * 0.75, originX, originY + graphHeight * 0.75, this);
    leftTick2 = new QGraphicsLineItem(originX - tick, originY + graphHeight * 0.5, originX, originY + graphHeight * 0.5, this);
    leftTick3 = new QGraphicsLineItem(originX - tick, originY + graphHeight * 0.25, originX, originY + graphHeight * 0.25, this);

    rightTick1 = new QGraphicsLineItem(originX + graphWidth, originY + graphHeight * 0.75, originX + graphWidth + tick, originY + graphHeight * 0.75, this);
    rightTick2 = new QGraphicsLineItem(originX + graphWidth, originY + graphHeight * 0.5, originX + graphWidth + tick, originY + graphHeight * 0.5, this);
    rightTick3 = new QGraphicsLineItem(originX + graphWidth, originY + graphHeight * 0.25, originX + graphWidth + tick, originY + graphHeight * 0.25, this);

    xTick1 = new QGraphicsLineItem(originX + graphWidth * 0.25, originY, originX + graphWidth * 0.25, originY - tick, this);
    xTick2 = new QGraphicsLineItem(originX + graphWidth * 0.5, originY, originX + graphWidth * 0.5, originY - tick, this);
    xTick3 = new QGraphicsLineItem(originX + graphWidth * 0.75, originY, originX + graphWidth * 0.75, originY - tick, this);
}

void Plot::RemoveAllLines()
{
    yScaleLeft = std::numeric_limits<double>::max();
    yMinLeft = std::numeric_limits<double>::max();
    yMaxLeft = std::numeric_limits<double>::min();
    yScaleRight = std::numeric_limits<double>::max();
    yMinRight = std::numeric_limits<double>::max();
    yMaxRight = std::numeric_limits<double>::min();
    xScale = std::numeric_limits<double>::max();
    xMin = std::numeric_limits<double>::max();
    xMax = std::numeric_limits<double>::min();

    xLegend->SetText("");
    yLegendLeft->SetText("");
    yLegendRight->SetText("");

    for (auto &l : leftAxisGraph->childItems())
    {
        delete l;
    }
    for (auto &r : rightAxisGraph->childItems())
    {
        delete r;
    }
    if (axes != nullptr)
    {
        delete axes;
        axes = new PlotAxes(originX, originY, graphWidth, graphHeight, this);
    }
    count = 0;
}

void Plot::AddBins(const std::vector<double> &bins, double yScaleMax)
{
    double binSizePx = static_cast<double>(graphWidth) / bins.size();
    int bin = 0;
    //    std::vector<double>::const_iterator it = std::max_element(bins.begin(), bins.end());
    for (auto &height : bins)
    {
        QGraphicsRectItem *item = new QGraphicsRectItem(0.0 + bin * binSizePx, 0.0, binSizePx, height / yScaleMax * graphHeight, leftAxisGraph);
        item->setBrush(Qt::green);
        bin++;
    }
    axes->SetYLeft(0.0, yScaleMax);
}

void Plot::AddLine(QVector<QPointF> *lineVec, QString labelX, QString labelY)
{
    QPen linePen(Qt::GlobalColor(colors.at(++count % colors.size())));
    linePen.setWidth(0);

    xLegend->SetText(labelX);
    yLegendLeft->SetText(labelY);

    PlotGraphicsItem *line = new PlotGraphicsItem(lineVec, linePen, leftAxisGraph);

    xMin = std::min(line->GetXMin(), xMin);
    xMax = std::max(line->GetXMax(), xMax);
    xScale = std::min(graphWidth / std::abs(xMax - xMin), xScale);

    yMinLeft = std::min(line->GetYMin(), yMinLeft);
    yMaxLeft = std::max(line->GetYMax(), yMaxLeft);
    yScaleLeft = std::min(graphHeight / std::abs(yMaxLeft - yMinLeft), yScaleLeft);

    axes->SetX(xMin, xMax);
    axes->SetYLeft(yMinLeft, yMaxLeft);

    QTransform matrixTrajectory = QTransform::fromScale(xScale, yScaleLeft);
    matrixTrajectory.translate(-xMin, -yMinLeft);

    for (auto t : leftAxisGraph->childItems())
    {
        t->setTransform(matrixTrajectory);
    }
}

void Plot::AddLineRight(QVector<QPointF> *lineVec, QString labelY)
{
    QPen linePen(Qt::GlobalColor(colors.at(count % colors.size())));
    linePen.setStyle(Qt::DotLine);
    linePen.setWidth(0);

    yLegendRight->SetText(labelY);

    PlotGraphicsItem *line = new PlotGraphicsItem(lineVec, linePen, rightAxisGraph);

    yMinRight = std::min(line->GetYMin(), yMinRight);
    yMaxRight = std::max(line->GetYMax(), yMaxRight);
    yScaleRight = std::min(graphHeight / std::abs(yMaxRight - yMinRight), yScaleRight);

    axes->SetYRight(yMinRight, yMaxRight);

    QTransform matrixTrajectory = QTransform::fromScale(xScale, yScaleRight);
    matrixTrajectory.translate(-xMin, -yMinRight);

    for (auto t : rightAxisGraph->childItems())
    {
        t->setTransform(matrixTrajectory);
    }
}

void Plot::SetAxisX(QString label)
{
    xLegend->SetText(label);
}

void Plot::SetAxisY(QString label)
{
    yLegendLeft->SetText(label);
}

void Plot::SetBinCenters(QStringList binCenters)
{
    xTick1->hide();
    xTick2->hide();
    xTick3->hide();
    axes->SetBins(binCenters);
}
