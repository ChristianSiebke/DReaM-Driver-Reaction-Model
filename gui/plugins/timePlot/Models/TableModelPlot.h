/********************************************************************************
 * Copyright (c) 2019-2020 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef TABLEMODELCSV_H
#define TABLEMODELCSV_H

#include <set>

#include <QAbstractTableModel>
#include <QFile>
#include <QPointF>
#include <QTextStream>

class TableModelPlot : public QAbstractTableModel
{
    Q_OBJECT
public:
    TableModelPlot(QObject *parent, const QString &fileName);
    TableModelPlot(QObject *parent, const QString &fileName, int trackId);
    virtual ~TableModelPlot();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QStringList GetHeaderData() const;

    void Clear();
    bool ReadCsv(const QString &fileName);
    bool ReadTrack(const QString &fileName, int trackId);
    std::set<int> GetTrackIds();

private:
    QList<QStringList> dataCsv;
    QStringList headerDataCsv;

    QVector<QPointF> trajectoryPoints;
    std::set<int> trackIds = {};
};

#endif // TABLEMODELCSV_H
