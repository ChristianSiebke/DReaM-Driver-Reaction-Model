/*********************************************************************
* Copyright (c) 2017, 2018, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef TABLEMODELCSV_H
#define TABLEMODELCSV_H

#include <QAbstractTableModel>
#include <QFile>
#include <QPointF>
#include <QTextStream>
#include "pcm_definitions.h"

class TableModelCsv : public QAbstractTableModel
{
    Q_OBJECT
public:
    TableModelCsv(QObject *parent, const QString &fileName, unsigned indexAgent);
    virtual ~TableModelCsv();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void Clear();
    bool ReadCsv(const QString &fileName, unsigned indexAgent);
    QVector<QPointF> *GetTrajectoryData();

private:
    void CreateTrajectory();
    void AddPoint(QString xPos, QString yPos);

    QList<QStringList> dataCsv;
    QStringList headerDataCsv;

    QVector<QPointF> trajectoryPoints;
};

#endif // TABLEMODELCSV_H
