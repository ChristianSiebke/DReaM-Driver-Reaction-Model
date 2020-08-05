/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/ 

#include "Views/WindowView.h"
#include "ui_WindowView.h"

#include "Presenters/WindowPresenter.h"
#include "Views/LogoDelegate.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QPixmap>
#include <QRect>
#include <QMessageBox>

WindowView::WindowView(WindowPresenter * const windowPresenter,
                       QWidget * const parent) :
    QMainWindow(parent),
    windowPresenter(windowPresenter),
    ui(new Ui::WindowView)
{
    // Setup UI
    ui->setupUi(this);
    setWindowTitle(QString(QStringLiteral("%1 %2")).
                   arg(QApplication::applicationName(),
                       QApplication::applicationVersion()));

    // Update size
    QRect const desktop = QApplication::desktop()->availableGeometry();
    resize(desktop.width() * .75, desktop.height() * .65);

    // Update UI
    updateViews();

    // Display the application logo
    ui->logo->setPixmap(LogoDelegate::logoRequest());

    // Connect WindowPresenter to WindowView
    connect(windowPresenter, &WindowPresenter::modified,
            this, &WindowView::updateViews);

    show();
}

WindowView::~WindowView()
{
    // Clear UI
    clearViews();

    // Destroy UI
    delete ui;
}

void WindowView::initialize_view()
{
    // Open UI on the first registred widget without menu
    show();

    // Display menu
    QList<QPushButton *> buttons = ui->sidebar->findChildren<QPushButton *>();
    if (!buttons.isEmpty())
    {
        // Activate the first button in the sidebar
        QPushButton * button = buttons.first();
        button->clicked();

        // Update menu
        connect(button, &QPushButton::clicked, this, &WindowView::sidebar_clicked);
    }
}

void WindowView::sidebar_clicked()
{
    // Clear menu
    clearMenu();
    ui->menuLayout->update();

    // Update menu
    for (QPushButton * const button : ui->sidebar->findChildren<QPushButton *>())
    {
        button->setChecked(false);
        if ((QObject::sender() == button) && (views.contains(button)))
        {
            button->setChecked(true);
            for (QWidget * const item : views[button].second)
            {
                ui->menuLayout->addWidget(item ? item : new QLabel(this));
                QPushButton * const bitem = qobject_cast<QPushButton *>(item);
                if (bitem && bitem->menu())
                {
                    bitem->menu()->setStyleSheet(styleSheet());
                }
            }
            ui->views->setCurrentWidget(views[button].first);
        }
    }
}

void WindowView::clearViews()
{
    // Clear internal views
    views.clear();

    // Clear menu
    clearMenu();

    // Clear sidebar
    while (ui->sidebarLayout->count() > 0)
    {
        QLayoutItem * const item = ui->sidebarLayout->takeAt(0);
        disconnect(qobject_cast<QPushButton *>(item->widget()), &QPushButton::clicked,
                   this, &WindowView::sidebar_clicked);
        item->widget()->setParent(nullptr);
        delete item;
    }

    // Clear views
    while (ui->views->count() > 0)
    {
        QWidget * const widget = ui->views->widget(0);
        ui->views->removeWidget(widget);
        widget->setParent(nullptr);
    }
}

void WindowView::clearMenu()
{
    // Clear menu
    while (ui->menuLayout->count() > 0)
    {
        QLayoutItem * const item = ui->menuLayout->takeAt(0);
        item->widget()->setParent(nullptr);
        delete item;
    }
}

void WindowView::updateViews()
{
    // Clear views
    clearViews();

    // Add registered views
    for (WindowModel::ViewModel const & view : windowPresenter->list())
    {
        ui->sidebarLayout->addWidget(view.button);
        if (view.widget != nullptr)
        {
            views.insert(view.button, {view.widget, view.menu});
            view.button->setCheckable(true);
            view.button->setChecked(ui->views->count() == 0);
            ui->views->addWidget(view.widget);
            connect(view.button, &QPushButton::clicked,
                    this, &WindowView::sidebar_clicked);
        }
    }
}

void WindowView::closeEvent(QCloseEvent * event)
{
    if (QMessageBox::Yes != QMessageBox::question(this, "Close OpenPASS?",
            "All unsaved settings will be lost! Are you sure you want to exit?", QMessageBox::Yes | QMessageBox::No))
        {
            event->ignore();
        }

    else
    {
        event->accept();
        QCoreApplication::quit();
    }
}
