/*********************************************************************
* Copyright (c) 2019 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <vector>
#include <algorithm>
#include <QDebug>
#include <QString>

class Histogram
{
public:
    Histogram(int numBins, const std::vector<double> &tableColumn, double minHisto, double maxHisto);

    std::vector<double> GetData() const;
    double GetMin() const;
    double GetMax() const;
    double GetBinSize() const;
    double GetHistoSum() const;
    double GetYScaleMax() const;

private:
    std::vector<double> histoVector{};
    double minHisto;
    double maxHisto;
    double binSize;
    double histoSum;
    double yScaleMax {0.0};
};

#endif // HISTOGRAM_H
