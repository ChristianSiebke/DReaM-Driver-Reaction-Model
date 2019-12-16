/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef VEHICLEPROFILESVIEW_H
#define VEHICLEPROFILESVIEW_H

#include "Views/WidgetView.h"
#include "Views/SystemProfileView.h"

#include <QString>
#include <QStringList>

namespace Ui {
class VehicleProfilesView;
}

class VehicleProfilesView : public WidgetView
{
    Q_OBJECT

public:
    explicit VehicleProfilesView(QWidget *parent = nullptr);
    ~VehicleProfilesView();

Q_SIGNALS:
    void addVehicleProfile(QString const &name);
    void removeVehicleProfile(QString const &name);
    void modifiedName(QString const & oldName, QString const &newName);
    void selectedVehicleProfile(QString const &name);

Q_SIGNALS:
    void refreshModelCatalogue();
    void changeModelCatalogue();
    void modifiedModel(QString const &ModelName);

Q_SIGNALS:
    void changeSystemConfig();
    void refreshSystemConfig();
    void addSystemProfile(QString const &type);
    void removeSystemProfile(QString const &type);

private Q_SLOTS:
    void on_Name_activated(const QString &name);
    void on_add_clicked();
    void on_remove_clicked();
    void on_edit_clicked();

private Q_SLOTS:
    void on_modelCatalogue_Refresh_clicked();
    void on_modelCatalogue_Browse_clicked();
    void on_Model_activated(const QString &model);

private Q_SLOTS:
    void on_systemConfigRefresh_clicked();
    void on_systemConfigBrowse_clicked();
    void on_addSystemProfile_clicked();
    void on_removeSystemProfile_clicked();

public:
    void setCurrentVehicle(QString const &name);
    void addVehicleProfileSelectionItem(QString const &name);
    void removeVehicleProfileSelectionItem(QString const &name);
    void setVehicleProfileSelectionItems(QStringList const &names);

public:
    void setModelCatalogue(QString const &filepath);
    void setModel(QString const &model);
    void setModelSelectionItems(QStringList const &models);
    void setModelCatalogueError(QString const &message);
    void enableModelView(bool enable);

public:
    void enableConfiguration(bool enable);

public:
    void setSystemConfig(QString const &systemConfig);
    void setSystemProfileSelection(QList<QString> profiles);
    void setSystemConfigError(QString const &message);
    void setSystemProfilesError(QString const &message);

public:
    SystemProfileView * addSystemProfileView( QString const &type );
    void removeSystemProfileView( QString const &type );
    void clearSystemProfileViews();


private:
    Ui::VehicleProfilesView *ui;
};

#endif // VEHICLEPROFILESVIEW_H
