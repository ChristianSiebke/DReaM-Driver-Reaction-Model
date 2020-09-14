/*********************************************************************
* Copyright (c) 2019 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef HISTOGRAMS_H
#define HISTOGRAMS_H

#include <algorithm>
#include <string>
#include <vector>

#include <QAbstractTableModel>

#include "Histogram.h"
#include "TableModel.h"

class RowHistograms : public QAbstractTableModel
{
public:
    RowHistograms(QObject *parent, const TableModel &tableModel, int numberOfBins,
                  const std::vector<double> &minVec, const std::vector<double> &maxVec);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void Clear();
    const Histogram *GetHistogram(int section) const;

private:
    std::vector<Histogram> dataHisto; // one histogram per column
    QStringList headerHisto;
};

#endif // HISTOGRAMS_H
