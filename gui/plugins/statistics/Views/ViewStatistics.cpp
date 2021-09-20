/*********************************************************************
* Copyright (c) 2019, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "ViewStatistics.h"

#include "ui_ViewStatistics.h"

const WindowInterface::ID ViewStatistics::ViewID = QStringLiteral("openPASS.Statistics");

ViewStatistics::ViewStatistics(WindowInterface *const window,
                               QWidget *parent) :
    QWidget(parent), window(window), ui(new Ui::ViewHistograms), actionMenuBrowse(WindowInterface::createAction(tr("Add result folder..."), this, &ViewStatistics::ActionBrowse))

{
    // Create UI
    ui->setupUi(this);

    // Customize the menu
    actionMenuBrowse->setEnabled(true);

    // Register view
    window->add(ViewID, WindowInterface::createButton(tr("Statistics"), 0, 00100), this,
                {actionMenuBrowse});
}

ViewStatistics::~ViewStatistics()
{
    OnClear();

    // Deregister view
    window->remove(ViewID);

    // Destroy UI
    delete ui;
}

void ViewStatistics::ShowMessage(QString title, QString message)
{
    QMessageBox::information(this, title, message);
}

void ViewStatistics::PlotHistograms(RowHistograms *histoModel, int row, QLabel *folderLabel, std::vector<double> *yMaxVec)
{
    QFont font("Arial", 16, QFont::Bold);
    folderLabel->setFont(font);
    ui->gridLayout->addWidget(folderLabel, row, 1);
    labels << folderLabel;

    for (int column = 2; column < histoModel->columnCount(); ++column)
    {
        Plot *plot = new Plot();
        plot->setMinimumSize(610, 370);
        plot->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        plot->SetAxisX(histoModel->headerData(column, Qt::Orientation::Horizontal, Qt::DisplayRole).toString());
        plot->SetAxisY("Frequency");

        QGraphicsScene *scene = new QGraphicsScene();
        scene->addItem(plot); // scene takes ownership of item

        QGraphicsView *plotView = new QGraphicsView(scene); // view does not take ownership of scene
        plotView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        ui->gridLayout->addWidget(plotView, row, column);
        ui->gridLayout->setColumnMinimumWidth(column, 660);
        views << plotView;

        QStringList categories;
        int numBins = histoModel->GetHistogram(column)->GetData().size();
        double binSize = histoModel->GetHistogram(column)->GetBinSize();
        double minHisto = histoModel->GetHistogram(column)->GetMin();
        double maxHisto = histoModel->GetHistogram(column)->GetMax();

        plot->AddBins(histoModel->GetHistogram(column)->GetData(), yMaxVec->at(column));

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

        for (int i = 0; i < numBins; i++)
        {
            double tick = minHisto + binSize * i;
            QString bin = QString::number(tick, 'g', 3);
            categories.push_back(bin);
        }
        plot->SetBinCenters(categories);
    }
}

void ViewStatistics::SetModelHistograms(QAbstractItemModel *model)
{
    ui->treeViewResults->setModel(model);
}

QItemSelectionModel *ViewStatistics::GetSelectionModelHistograms()
{
    return ui->treeViewResults->selectionModel();
}

void ViewStatistics::OnClear()
{
    if (!ui->gridLayout->isEmpty())
    {
        for (QGraphicsView *view : views)
        {
            delete view->scene();
            delete view;
        }
        views.clear();
    }
    if (!labels.isEmpty())
    {
        for (QLabel *label : labels)
        {
            ui->gridLayout->removeWidget(label);
            delete label;
        }
        labels.clear();
    }
}

void ViewStatistics::ActionBrowse()
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
}

void ViewStatistics::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
}

void ViewStatistics::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
}
