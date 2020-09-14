/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/

#include "Views/SystemView.h"

#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QMouseEvent>
#include <QRadioButton>

#include "Views/SystemBaseView.h"
#include "Views/SystemComponentManagerView.h"
#include "Views/SystemItemView.h"
#include "Views/SystemTabView.h"
#include "openPASS-System/SystemInterface.h"
#include "ui_SystemView.h"

WindowInterface::ID const SystemView::ViewID = QStringLiteral("openPASS.System");

SystemView::SystemView(WindowInterface *const window,
                       SystemInterface *const system,
                       ProjectInterface *const project,
                       QWidget *const parent) :
    WidgetView(parent), window(window), directory(project->getConfigPath()), system(system), dynamicMode(new bool(false)), ui(new Ui::SystemView)
{
    // Setup UI
    ui->setupUi(this);
    ui->layout->insertWidget(0, new SystemTabView(system->getSystems(), system->getComponents(), dynamicMode, this), 1);
    ui->layout->insertWidget(1, new SystemComponentManagerView(system->getComponents(), dynamicMode, this), 0);

    // create radio buttons for activating/deactivating dynamic mode
    QRadioButton *dynamicMode_on = new QRadioButton("Dynamic Mode", this);
    QRadioButton *staticMode_on = new QRadioButton("Static Mode", this);

    dynamicMode_on->setObjectName("dynamic");
    staticMode_on->setObjectName("static");

    connect(dynamicMode_on, &QRadioButton::toggled, this, &SystemView::activateDynamicMode);
    dynamicMode_on->installEventFilter(this);
    staticMode_on->toggle();
    staticMode_on->installEventFilter(this);

    // Register view
    window->add(ViewID, WindowInterface::createButton(tr("System Editor"), 0, 04000), this, {
                                                                                                WindowInterface::createAction(tr("Clear"), this, &SystemView::actionClear_clicked),
                                                                                                WindowInterface::createAction(tr("Load"), this, &SystemView::actionLoadSystem_clicked),
                                                                                                WindowInterface::createAction(tr("Save"), this, &SystemView::actionSaveSystem_clicked),
                                                                                                WindowInterface::createAction(tr("Save Screenshot"), this, &SystemView::actionSaveScreenshot_clicked),
                                                                                                WindowInterface::createAction(tr("New System"), this, &SystemView::actionNewSystem_clicked),
                                                                                                dynamicMode_on,
                                                                                                staticMode_on,
                                                                                            });

    // create the default main system and its view
    system->getSystems()->add(0);
    system->getSystems()->getItem(0)->setTitle("System 1");
    Q_EMIT system->getSystems()->modifiedTitle(0);
}

SystemView::~SystemView()
{
    // Deregister view
    window->remove(ViewID);

    // Destroy UI
    delete ui;
}

void SystemView::actionClear_clicked()
{
    QMessageBox warning;

    int ret;

    // spell out a warning that everything will be deleted
    warning.setText("Clearing will delete the whole worksheet!");
    warning.setInformativeText("Do you want to continue?");
    warning.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    ret = warning.exec();

    if (ret == QMessageBox::Yes)
    {
        // clear all system and subsystem maps
        system->clear();

        // create the default main system and its view
        system->getSystems()->add(0);
        system->getSystems()->getItem(0)->setTitle("System 1");
        Q_EMIT system->getSystems()->modifiedTitle(0);

        // Update the component manager
        findChild<SystemComponentManagerView *>(QString(), Qt::FindDirectChildrenOnly)->updateManagerView();
        findChild<SystemComponentManagerView *>(QString(), Qt::FindDirectChildrenOnly)->show();
    }
}

// should create a new tab with a subsystem
void SystemView::actionNewSystem_clicked()
{
    // create the system
    SystemMapInterface::ID id = system->getSystems()->generateID();
    SystemItemInterface::Title name = system->getSystems()->generateTitle();
    system->getSystems()->add(id);

    system->getSystems()->getItem(id)->setTitle(name);
    Q_EMIT system->getSystems()->modifiedTitle(id);
}

void SystemView::actionLoadSystem_clicked()
{
    // Prompt file dialog to load system
    QString const filepath = QFileDialog::getOpenFileName(
        this, QObject::tr("openPASS / Load System"),
        directory, QStringLiteral("System (*.xml)"));
    if (!filepath.isNull())
    {
        // Update project directory
        directory = QFileInfo(filepath).absolutePath();

        // Load system from file
        system->load(filepath, dynamicMode);

        // Hide component list
        findChild<SystemComponentManagerView *>(QString(), Qt::FindDirectChildrenOnly)->hide();
    }
}

void SystemView::actionSaveSystem_clicked()
{
    // Prompt file dialog to save system
    QString const filepath = QFileDialog::getSaveFileName(
        this, QObject::tr("openPASS / Save System"),
        directory, QStringLiteral("System (*.xml)"));
    if (!filepath.isNull())
    {
        // Update project directory
        directory = QFileInfo(filepath).absolutePath();

        // Save system to file
        system->save(filepath, dynamicMode);
    }
}

void SystemView::actionSaveScreenshot_clicked()
{
    // Prompt file dialog to save screenshot
    QString const filepath = QFileDialog::getSaveFileName(
        this, QObject::tr("openPASS / Save Screenshot"),
        directory, QStringLiteral("Image (*.png)"));
    if (!filepath.isNull())
    {
        // Update project directory
        directory = QFileInfo(filepath).absolutePath();

        // Save screenshot to file
        findChild<SystemBaseView *>()->grab().save(filepath);
    }
}

bool SystemView::eventFilter(QObject *watched, QEvent *event)
{
    QMessageBox warning;
    int ret;
    QRadioButton *radioButton = qobject_cast<QRadioButton *>(watched);
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

    if (radioButton && mouseEvent && radioButton->objectName() == "dynamic" &&
        !radioButton->isChecked() && (mouseEvent->type() == QMouseEvent::MouseButtonPress) && (mouseEvent->button() == Qt::LeftButton))
    {
        warning.setText("You are going to activate the dynamic mode. In this mode, you can only build systems compatible with the profile-based agent configuration ansatz. "
                        "This means that you are going to configure the sensors + vehicle component (algorithm) part of a vehicle profile. "
                        "The connections you draw between algorithm and sensor thereby correspond to the sensorLinks of the vehicle profile. "
                        "Finally, not all components will be supported. Hence, the entire worksheet will be cleared before continuing.");
        warning.setInformativeText("Do you want to continue?");
        warning.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        ret = warning.exec();

        if (ret == QMessageBox::Yes)
        {
            // clear all system and subsystem maps
            system->clear();

            // create the default main system and its view
            system->getSystems()->add(0);
            system->getSystems()->getItem(0)->setTitle("System 1");
            Q_EMIT system->getSystems()->modifiedTitle(0);

            radioButton->toggle();
        }
    }
    else if (radioButton && mouseEvent && radioButton->objectName() == "static" &&
             !radioButton->isChecked() && (mouseEvent->type() == QMouseEvent::MouseButtonPress) && (mouseEvent->button() == Qt::LeftButton))
    {
        warning.setText("You are going to return to the static mode.  "
                        "There will be no restrictions anymore, but you will loose the 'dynamic=true' identifier in the XML file. "
                        "Hence, the XML file will not be accepted by those plugins which require systems configured in the dynamic mode.");
        warning.setInformativeText("Do you want to return to the static mode?");
        warning.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        ret = warning.exec();

        if (ret == QMessageBox::Yes)
            radioButton->toggle();
    }

    return QWidget::eventFilter(watched, event);
}

void SystemView::activateDynamicMode(bool checked)
{
    *dynamicMode = checked;

    // Update the component manager
    findChild<SystemComponentManagerView *>(QString(), Qt::FindDirectChildrenOnly)->updateManagerView();
    findChild<SystemComponentManagerView *>(QString(), Qt::FindDirectChildrenOnly)->show();

    Q_EMIT dynamicModeActicated(*dynamicMode);
}

bool SystemView::isDynamicMode() const
{
    return *dynamicMode;
}
