/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef AGENTMAPVIEW_H
#define AGENTMAPVIEW_H

#include "WidgetView.h"

#include <QWidget>
#include <QTableView>

namespace Ui {
class AgentMapView;
}

class AgentMapView : public WidgetView
{
    Q_OBJECT

public:
    explicit AgentMapView(QWidget *parent = nullptr);
    ~AgentMapView();

Q_SIGNALS:
    void add(QString const & name);
    void remove(QString const & name);
    void selectionChanged(QString const & name);
    void typeChanged(QString const &type);
    void modifiedName(QString const & oldName, QString const & newName);

Q_SIGNALS:
    void addVehicleProfile();
    void removeVehicleProfile(const int &row);

Q_SIGNALS:
    void addDriverProfile();
    void removeDriverProfile(const int &row);

Q_SIGNALS:
    void refreshSystemConfig();
    void setSystemConfig();
    void selectedSystem(QString const &system);
    void refreshModelCatalogue();
    void setModelCatalogue();
    void setModel(QString const &model);


private Q_SLOTS:
    void on_add_clicked();
    void on_remove_clicked();
    void on_edit_clicked();
    void on_AgentProfileSelection_activated(const QString &name);
    void on_Type_activated(const QString &type);

private Q_SLOTS:
    void on_addVehicle_clicked();
    void on_removeVehicle_clicked();

private Q_SLOTS:
    void on_addDriver_clicked();
    void on_removeDriver_clicked();

private Q_SLOTS:
    void on_System_activated(const QString &system);
    void on_systemConfigBrowse_clicked();
    void on_systemConfigRefresh_clicked();

private Q_SLOTS:
    void on_ModelCatalogueBrowse_clicked();
    void on_ModelCatalogueRefresh_clicked();
    void on_Model_activated(const QString &model);

public:
    QTableView * getVehicleProfilesView() const;
    QTableView * getDriverProfilesView() const;

public:
    void addAgent(QString const & name);
    void removeAgent(QString const & name);
    void setAgentSelectionItems(QStringList const &names);
    void setCurrentAgent(QString const & name);
    void setCurrentType(QString const &type);
    void clear();

public:
    void setCurrentSystemConfig(const QString &systemConfig);
    void setSystemSelectionItems(const QStringList &systemItems);
    void setCurrentSystem(const QString &system);
    void setCurrentModelCatalogue(const QString &modelCatalogue);
    void setModelSelectionItems(const QStringList &modelItems);
    void setCurrentModel(const QString &model);

public:
    void enableConfiguration(bool enable);
    void enableModelView(bool enable);
    void enableSystemView(bool enable);

public:
    void setSystemConfigError(QString const &message);
    void setModelCatalogueError(QString const &message);
    void setDriverProbabilityError(bool error);
    void setVehicleProbabilityError(bool error);

private:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::AgentMapView *ui;
};

#endif // AGENTMAPVIEW_H
