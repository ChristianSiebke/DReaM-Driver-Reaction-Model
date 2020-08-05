/*********************************************************************
* Copyright (c) 2019, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#pragma once

#include "openPASS-Window/WindowInterface.h"

#include <QWidget>
#include <QDir>
#include <QFileDialog>
#include <QAbstractItemModel>
#include <QTabWidget>
#include <QTableView>
#include <QGraphicsScene>
#include "Common_GUI/Plot.h"

namespace Ui {
class ViewTimePlot;
}

class ViewTimePlot : public QWidget
{
    Q_OBJECT

public:
    explicit ViewTimePlot(WindowInterface *const window,
                          QWidget *parent = nullptr);
    virtual ~ViewTimePlot();

    void SetModelHighD(QAbstractItemModel *model);

    QItemSelectionModel *GetSelectionModelHighD();

    static QString resultParentFolder;

public Q_SLOTS:
    void AddTableModel(std::unordered_map<std::string, QAbstractTableModel*> *tableMap, const QString &tableName);
    void OnClear();
    void SetTableModel(int index);
    void OnColumnChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void PlotColumns(QAbstractItemModel *tableModel);

public:
Q_SIGNALS:
    void ResultPathChanged(const QString &newResultPath) const;
    void SendTableName(const QString &tableName) const;

private Q_SLOTS:
    void ActionBrowse();
    void OnTabClose(int index);

private:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void showEvent(QShowEvent *event);

    static WindowInterface::ID const ViewID;
    WindowInterface *const window {nullptr};
    Ui::ViewTimePlot *ui {nullptr};
    WindowInterface::Action *const actionMenuBrowse {nullptr};

    QTabWidget *tabwidget {nullptr};
    QGraphicsScene *plotScene {nullptr};
    QTableView *currentTableView {nullptr};
    QItemSelectionModel *selectionTableModel {nullptr};

    QModelIndexList selectedHeaders;

    Plot *plot {nullptr};
};
