/********************************************************************************
 * Copyright (c) 2019-2020 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef MODELHISTOGRAMS_H
#define MODELHISTOGRAMS_H

#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QItemSelectionModel>
#include <QLabel>
#include <QObject>
#include <QStandardItemModel>
#include <QStyle>

#include "RowHistograms.h"
#include "TableModel.h"

class ModelStatistics : public QObject
{
    Q_OBJECT

public:
    explicit ModelStatistics(QObject *parent = nullptr);
    virtual ~ModelStatistics() override;

    QAbstractItemModel *GetItemModelHistograms() const;
    void SetSelectionModelHistograms(QItemSelectionModel *selectionModel);

public:
Q_SIGNALS:
    void HistogramAdded(RowHistograms *histoModel, int row, QLabel *folderLabel, std::vector<double> *yMaxVec);
    void Clear();
    void ShowMessage(QString title, QString message);

public Q_SLOTS:
    bool AddTableModel(const QString &resultPath, const QString &folderName);
    void LoadPathData(const QString &pathName);

private Q_SLOTS:
    void OnFolderSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    bool ContainsHighDFormat(const QString &fileName);
    void CalculateHistograms(QString folderName, const TableModel &tableModel);
    void FillMinMax(const TableModel &folderTable);

    // for each added result directory an entry is added to the map with all highD tables
    std::map<QString, TableModel *> folderTables;
    // for each selected result directory an entry is added to the map with all histograms
    std::map<QLabel *, RowHistograms *> histoTables;
    std::vector<double> minVec{};
    std::vector<double> maxVec{};
    std::vector<double> yMaxVec{};
    QStringList headers{};

    QItemSelectionModel *selectionFolderModel = nullptr;
    QModelIndexList selectedIndexList;

    QStandardItemModel *listFolderModel = nullptr;

    QString rootPath = "";

    // odd  number
    qreal numberOfBins = 7;
};

#endif // MODELHISTOGRAMS_H
