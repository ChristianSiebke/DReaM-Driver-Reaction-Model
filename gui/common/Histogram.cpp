/********************************************************************************
 * Copyright (c) 2019 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "Histogram.h"

Histogram::Histogram(int numBins, const std::vector<double> &tableColumn, double minHisto,
                     double maxHisto) :
    minHisto(minHisto), maxHisto(maxHisto)
{
    for (int n = 0; n < numBins; ++n)
    {
        histoVector.push_back(0);
    }
    histoSum = tableColumn.size();
    double diff = std::abs(maxHisto - minHisto);
    if (diff <= std::numeric_limits<double>::epsilon())
    {
        minHisto -= (numBins / 2);
        binSize = 1;
    }
    else
    {
        binSize = diff / (numBins - 1);
    }
    for (auto item : tableColumn)
    {
        for (int i = 0; i < numBins; ++i)
        {
            if (item >= (minHisto + (i - 0.5) * binSize) && item < (minHisto + (i + 0.5) * binSize))
            {
                histoVector.at(i) += 1;
            }
        }
    }
    // normalize to 'frequencies'
    for (auto &item : histoVector)
    {
        item = item / histoSum;
        yScaleMax = std::max(item, yScaleMax);
    }
}

std::vector<double> Histogram::GetData() const
{
    return histoVector;
}

double Histogram::GetMin() const
{
    return minHisto;
}

double Histogram::GetMax() const
{
    return maxHisto;
}

double Histogram::GetBinSize() const
{
    return binSize;
}

double Histogram::GetHistoSum() const
{
    return histoSum;
}

double Histogram::GetYScaleMax() const
{
    return yScaleMax;
}
