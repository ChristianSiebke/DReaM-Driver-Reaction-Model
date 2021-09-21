/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* Copyright (c) 2020 ITK Engineering GmbH.
*
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/

#ifndef PROJECTVIEW_H
#define PROJECTVIEW_H

#include <QString>
#include <QWidget>

#include "openPASS-Window/WindowInterface.h"

namespace Ui {
class ProjectView;
}

class ProjectPresenter;

class ProjectView : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectView(WindowInterface *const window,
                         ProjectPresenter *const ProjectPresenter,
                         QWidget *parent = nullptr);
    virtual ~ProjectView();

private:
    static WindowInterface::ID const ViewID;

private:
    WindowInterface *const window;
    ProjectPresenter *const projectPresenter;

private:
    Ui::ProjectView *ui;

private:
    WindowInterface::Action *const actionMenuNew;
    WindowInterface::Action *const actionMenuLoad;
    WindowInterface::Action *const actionMenuSave;
    WindowInterface::Action *const actionMenuStart;

private Q_SLOTS: // Define actions in the Menu-bar
    void actionProjectNew();
    void actionProjectLoad();
    void actionProjectSave();
    void actionProjectStart();

private Q_SLOTS: // Define actions for the Browse Buttons
    void on_libraryBrowseButton_clicked();
    void on_configBrowseButton_clicked();
    void on_logLevelSpinBox_valueChanged(int level);
    void on_logOpSimulationManagerBrowseButton_clicked();
    void on_logSimulationBrowseButton_clicked();
    void on_outputBrowseButton_clicked();
    void on_simulationBrowseButton_clicked();

private Q_SLOTS: // Update actions from the Browser Buttons and also the load action
    void updateView();
};

#endif // PROJECTVIEW_H
