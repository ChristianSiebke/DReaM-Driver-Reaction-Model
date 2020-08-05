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
#include <QGraphicsWidget>
#include <QAbstractTableModel>
#include <QGraphicsLineItem>
#include "Common/globalDefinitions.h"
#include "CenteredTextItem.h"
#include "PlotGraphicsItem.h"
#include "Graph.h"
#include "PlotAxes.h"

class Plot : public QGraphicsWidget
{
public:
    Plot();

    void RemoveAllLines();
    void AddLine(QVector<QPointF> *lineVec, QString labelX, QString yLegendLeft);
    void AddLineRight(QVector<QPointF> *lineVec, QString yLegendLeft);
    void AddBins(const std::vector<double>& bins, double yScaleMax);
    void SetAxisX(QString label);
    void SetAxisY(QString label);
    void SetBinCenters(QStringList binCenters);

private:
    unsigned long long count = 0;

    int graphWidth = 460;
    int graphHeight = 260;
    int originX = 100;
    int originY = 80;
    int tick = 5;

    QGraphicsRectItem* leftAxisGraph {nullptr};
    QGraphicsRectItem* rightAxisGraph {nullptr};

    std::vector<int> colors;

    CenteredTextItem* xLegend {nullptr};
    CenteredTextItem* yLegendRight {nullptr};
    CenteredTextItem* yLegendLeft {nullptr};

    QGraphicsLineItem* leftTick1 {nullptr};
    QGraphicsLineItem* leftTick2 {nullptr};
    QGraphicsLineItem* leftTick3 {nullptr};

    QGraphicsLineItem* rightTick1 {nullptr};
    QGraphicsLineItem* rightTick2 {nullptr};
    QGraphicsLineItem* rightTick3 {nullptr};

    QGraphicsLineItem* xTick1 {nullptr};
    QGraphicsLineItem* xTick2 {nullptr};
    QGraphicsLineItem* xTick3 {nullptr};

    PlotAxes* axes {nullptr};

    double yScaleLeft = std::numeric_limits<double>::max();
    double yMinLeft = std::numeric_limits<double>::max();
    double yMaxLeft = std::numeric_limits<double>::min();
    double yScaleRight = std::numeric_limits<double>::max();
    double yMinRight = std::numeric_limits<double>::max();
    double yMaxRight = std::numeric_limits<double>::min();
    double xScale = std::numeric_limits<double>::max();
    double xMin = std::numeric_limits<double>::max();
    double xMax = std::numeric_limits<double>::min();
};
