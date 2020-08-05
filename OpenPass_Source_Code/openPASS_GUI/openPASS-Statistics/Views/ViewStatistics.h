/*********************************************************************
* Copyright (c) 2019 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef VIEWHISTOGRAMS_H
#define VIEWHISTOGRAMS_H

#include "openPASS-Window/WindowInterface.h"

#include <QWidget>
#include <QDir>
#include <QFileDialog>
#include <QAbstractItemModel>
#include <QTabWidget>
#include <QTableView>
#include <QMessageBox>
#include <QGraphicsView>
#include "Common_GUI/RowHistograms.h"
#include "Common_GUI/Plot.h"

namespace Ui {
class ViewHistograms;
}

class ViewStatistics : public QWidget
{
    Q_OBJECT

public:
    explicit ViewStatistics(WindowInterface *const window,
                            QWidget *parent = nullptr);
    virtual ~ViewStatistics() override;

    void SetModelHistograms(QAbstractItemModel *model);

    QItemSelectionModel *GetSelectionModelHistograms();

    static QString resultParentFolder;

public Q_SLOTS:
    void OnClear();
    void PlotHistograms(RowHistograms *histoModel, int row, QLabel *folderLabel, std::vector<double> *yMaxVec);
    void ShowMessage(QString title, QString message);

public:
Q_SIGNALS:
    void ResultPathChanged(const QString &newResultPath) const;

private Q_SLOTS:
    void ActionBrowse();

private:
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;

    static WindowInterface::ID const ViewID;
    WindowInterface *const window;
    Ui::ViewHistograms *ui;
    WindowInterface::Action *const actionMenuBrowse;

    QList<QGraphicsView *> views;
    QList<QLabel *> labels;
};

#endif // VIEWHISTOGRAMS_H
