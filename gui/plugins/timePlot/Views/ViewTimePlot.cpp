/*********************************************************************
* Copyright (c) 2019, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "ViewTimePlot.h"

#include "ui_ViewTimePlot.h"

const WindowInterface::ID ViewTimePlot::ViewID = QStringLiteral("openPASS.TimePlot");

ViewTimePlot::ViewTimePlot(WindowInterface *const window,
                           QWidget *parent) :
    QWidget(parent), window(window), ui(new Ui::ViewTimePlot), actionMenuBrowse(WindowInterface::createAction(tr("Select result folder..."), this, &ViewTimePlot::ActionBrowse))

{
    ui->setupUi(this);

    //    ui->treeViewResults->horizontalScrollBar()->setEnabled(true);
    ui->treeViewResults->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    actionMenuBrowse->setEnabled(true);

    tabwidget = ui->tabWidget;
    tabwidget->setTabsClosable(false);

    connect(tabwidget, &QTabWidget::tabCloseRequested,
            this, &ViewTimePlot::OnTabClose);

    connect(tabwidget, &QTabWidget::tabBarClicked,
            this, &ViewTimePlot::SetTableModel);

    selectionTableModel = new QItemSelectionModel();

    plotScene = new QGraphicsScene(this);
    ui->graphicsView->setScene(plotScene);

    plot = new Plot();
    plot->setMinimumSize(660, 380);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::white);
    plot->setAutoFillBackground(true);
    plot->setPalette(pal);

    plotScene->setBackgroundBrush(Qt::lightGray);

    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    plotScene->addItem(plot);

    plotScene->setSceneRect(plotScene->itemsBoundingRect());

    window->add(ViewID, WindowInterface::createButton(tr("Time Plot"), 0, 00100), this,
                {actionMenuBrowse});
}

ViewTimePlot::~ViewTimePlot()
{
    for (auto widget : tabwidget->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly))
    {
        delete widget;
    }
    delete tabwidget;

    delete plot;
    delete plotScene;

    window->remove(ViewID);

    delete ui;
}

void ViewTimePlot::AddTableModel(std::unordered_map<std::string, QAbstractTableModel *> *tableMap, const QString &tableName)
{
    QAbstractTableModel *tableModel = tableMap->find(tableName.toStdString())->second;
    QTableView *tableView = new QTableView(this);
    tableView->setAccessibleName(tableName);
    tableView->setModel(tableModel);

    tabwidget->addTab(tableView, tableName);

    selectedHeaders.clear();

    selectionTableModel->disconnect();
    selectionTableModel = static_cast<QTableView *>(tabwidget->currentWidget())->selectionModel();

    connect(selectionTableModel, &QItemSelectionModel::selectionChanged,
            this, &ViewTimePlot::OnColumnChanged);
}

void ViewTimePlot::PlotColumns(QAbstractItemModel *tableModel)
{
    if (selectedHeaders.size() >= 2)
    {
        QVector<QPointF> vec1{};
        int column1 = selectedHeaders.first().column();
        int selectedColumn = selectedHeaders.at(1).column();
        for (int i = 0; i < tableModel->rowCount(); ++i)
        {
            QModelIndex index1 = tableModel->index(i, column1);
            QModelIndex index2 = tableModel->index(i, selectedColumn);
            QPointF point = QPointF(tableModel->data(index1).toDouble(), tableModel->data(index2).toDouble());
            vec1.push_back(point);
        }
        plot->AddLine(&vec1,
                      tableModel->headerData(column1, Qt::Orientation::Horizontal).toString(),
                      tableModel->headerData(selectedColumn, Qt::Orientation::Horizontal).toString());
    }

    if (selectedHeaders.size() > 2)
    {
        QVector<QPointF> vec2{};
        int column1 = selectedHeaders.first().column();
        int selectedColumn2 = selectedHeaders.at(2).column();
        for (int i = 0; i < tableModel->rowCount(); ++i)
        {
            QModelIndex index1 = tableModel->index(i, column1);
            QModelIndex index2 = tableModel->index(i, selectedColumn2);
            QPointF point = QPointF(tableModel->data(index1).toDouble(), tableModel->data(index2).toDouble());
            vec2.push_back(point);
        }
        plot->AddLineRight(&vec2,
                           tableModel->headerData(selectedColumn2, Qt::Orientation::Horizontal).toString());
    }
}

void ViewTimePlot::SetTableModel(int index)
{
    selectedHeaders.clear();

    selectionTableModel->disconnect();
    selectionTableModel->clear();
    selectionTableModel = static_cast<QTableView *>(tabwidget->widget(index))->selectionModel();

    connect(selectionTableModel, &QItemSelectionModel::selectionChanged,
            this, &ViewTimePlot::OnColumnChanged);
}

void ViewTimePlot::SetModelHighD(QAbstractItemModel *model)
{
    ui->treeViewResults->setModel(model);
}

QItemSelectionModel *ViewTimePlot::GetSelectionModelHighD()
{
    return ui->treeViewResults->selectionModel();
}

void ViewTimePlot::OnClear()
{
    selectedHeaders.clear();
    tabwidget->clear();
    plot->RemoveAllLines();
}

void ViewTimePlot::OnColumnChanged(const QItemSelection &selected,
                                   const QItemSelection &deselected)
{
    plot->RemoveAllLines();

    for (QModelIndex index : deselected.indexes())
    {
        if (index.row() == 0)
        {
            selectedHeaders.removeOne(index);
        }
    }

    for (QModelIndex index : selected.indexes())
    {
        if (index.row() == 0)
        {
            if (selectedHeaders.size() > 2)
            {
                selectionTableModel->select(index, QItemSelectionModel::Deselect | QItemSelectionModel::Columns);
            }
            else if (selectedHeaders.size() <= 2)
            {
                selectedHeaders.append(index);
            }
        }
    }

    for (int index = 0; index < tabwidget->count(); ++index)
    {
        QWidget *widget = tabwidget->widget(index);
        static_cast<QTableView *>(widget)->viewport()->update();

        QString tableName = widget->accessibleName();
        Q_EMIT SendTableName(tableName);
    }
}

void ViewTimePlot::ActionBrowse()
{
    QDir const root = QDir(QCoreApplication::applicationDirPath());
    QString const resultPath = QFileDialog::getExistingDirectory(
        this, tr("Open Result Folder"), root.canonicalPath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!resultPath.isNull())
    {
        Q_EMIT ResultPathChanged(resultPath);
    }

    ui->treeViewResults->expandAll();
    ui->treeViewResults->resizeColumnToContents(0);
}

void ViewTimePlot::OnTabClose(int index)
{
    tabwidget->removeTab(index);
}

void ViewTimePlot::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    ui->graphicsView->fitInView(plotScene->sceneRect(), Qt::KeepAspectRatio);
}

void ViewTimePlot::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    ui->graphicsView->fitInView(plotScene->sceneRect(), Qt::KeepAspectRatio);
}
