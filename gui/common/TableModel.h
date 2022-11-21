/********************************************************************************
 * Copyright (c) 2019 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QFile>
#include <QRegExp>
#include <QTextStream>

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    TableModel(QObject *parent, const QString &fileName);
    virtual ~TableModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void Clear();
    bool ReadCsv(const QString &fileName);
    bool AppendCsv(const QString &fileName);
    const std::vector<double> *GetColumnVector(int section) const;
    QStringList GetHeader() const;

private:
    void FillInData(QTextStream *csvStream);
    std::vector<std::vector<double>> dataCsv;
    QStringList headerDataCsv;
};

#endif // TABLEMODEL_H
