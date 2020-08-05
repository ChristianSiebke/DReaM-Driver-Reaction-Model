/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "VehicleProfilesView.h"
#include "ui_VehicleProfilesView.h"

#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>

VehicleProfilesView::VehicleProfilesView(QWidget *parent) :
    WidgetView(parent),
    ui(new Ui::VehicleProfilesView)
{
    QFont font;

    ui->setupUi(this);
    enableConfiguration(false);

    font.setBold(true);
    ui->systemConfigError->setFont(font);
    ui->modelCatalogueError->setFont(font);
    ui->systemProfilesError->setFont(font);
}

VehicleProfilesView::~VehicleProfilesView()
{
    delete ui;
}

void VehicleProfilesView::on_add_clicked()
{
    QString newName = QInputDialog::getText (
                 this, tr ("Add Entry"),
                 tr ("Insert a Name"),
                 QLineEdit::Normal);

    if(!newName.isEmpty())
    {
        Q_EMIT addVehicleProfile(newName);
    }
}

void VehicleProfilesView::on_remove_clicked()
{
    QMessageBox warning;
    int ret;

    // spell out a warning that everything will be deleted
    warning.setText("The whole configuration of the vehicle profile will be deleted!");
    warning.setInformativeText("Do you want to continue?");
    warning.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    ret=warning.exec();

   if(ret==QMessageBox::Yes)
   {
        Q_EMIT removeVehicleProfile(ui->Name->currentText());
   }
}

void VehicleProfilesView::on_edit_clicked()
{
    QString newName = QInputDialog::getText (
                 this, tr ("Edit Entry"),
                 tr ("Insert a new Name"),
                 QLineEdit::Normal);

    if(!newName.isEmpty() )
    {
        Q_EMIT modifiedName(ui->Name->currentText(), newName);
        ui->Name->setCurrentText(newName);
    }

}

void VehicleProfilesView::on_systemConfigBrowse_clicked()
{
    Q_EMIT changeSystemConfig();
}

void VehicleProfilesView::on_systemConfigRefresh_clicked()
{
    Q_EMIT refreshSystemConfig();
}

void VehicleProfilesView::setCurrentVehicle(const QString &name)
{
    if(ui->Name->findText(name) != -1)
        ui->Name->setCurrentText(name);
}

void VehicleProfilesView::addVehicleProfileSelectionItem(QString const &name)
{
    ui->Name->addItem(name);

    enableConfiguration(true);
}

void VehicleProfilesView::removeVehicleProfileSelectionItem(QString const &name)
{
    int index = ui->Name->findText(name);
    ui->Name->removeItem(index);
}

void VehicleProfilesView::setVehicleProfileSelectionItems(const QStringList &names)
{
    ui->Name->clear();
    ui->Name->addItems(names);
}

void VehicleProfilesView::on_removeSystemProfile_clicked()
{
    Q_EMIT removeSystemProfile(ui->SystemProfiles->currentText());
}

void VehicleProfilesView::on_addSystemProfile_clicked()
{
    Q_EMIT addSystemProfile(ui->SystemProfiles->currentText());
}

void VehicleProfilesView::on_Name_activated(const QString &name)
{
    Q_EMIT selectedVehicleProfile(name);
}

void VehicleProfilesView::on_modelCatalogue_Browse_clicked()
{
    Q_EMIT changeModelCatalogue();
}

void VehicleProfilesView::on_modelCatalogue_Refresh_clicked()
{
    Q_EMIT refreshModelCatalogue();
}

void VehicleProfilesView::on_Model_activated(const QString &model)
{
    Q_EMIT modifiedModel(model);
}

void VehicleProfilesView::setModelSelectionItems(const QStringList &models)
{
    ui->Model->clear();
    ui->Model->addItems(models);
}

void VehicleProfilesView::enableModelView(bool enable)
{
    ui->Model->setEnabled(enable);
}

void VehicleProfilesView::enableConfiguration(bool enable)
{
    ui->remove->setEnabled(enable);
    ui->edit->setEnabled(enable);
    ui->systemConfigBrowse->setEnabled(enable);
    ui->systemConfigRefresh->setEnabled(enable);
    ui->systemConfig->setEnabled(enable);
    ui->modelCatalogue->setEnabled(enable);
    ui->modelCatalogue_Browse->setEnabled(enable);
    ui->modelCatalogue_Refresh->setEnabled(enable);
    ui->Model->setEnabled(enable);
    ui->SystemProfiles->setEnabled(enable);
    ui->addSystemProfile->setEnabled(enable);
    ui->removeSystemProfile->setEnabled(enable);
}

void VehicleProfilesView::setSystemConfig(const QString &systemConfig)
{
    ui->systemConfig->setText(systemConfig);
}

void VehicleProfilesView::setModel(const QString &model)
{
    ui->Model->setCurrentText(model);
}

void VehicleProfilesView::setModelCatalogue(const QString &filepath)
{
    ui->modelCatalogue->setText(filepath);
}

SystemProfileView * VehicleProfilesView::addSystemProfileView(const QString &type)
{
        int rows = ui->vehicleProfilesLayout->rowCount();
        int columns = ui->vehicleProfilesLayout->columnCount();
        SystemProfileView * systemProfileView = new SystemProfileView(type, this);

        ui->vehicleProfilesLayout->addWidget(systemProfileView, rows, 0, 1, columns );

        return systemProfileView;
}

void VehicleProfilesView::removeSystemProfileView(const QString &type)
{
    SystemProfileView * systemProfileView = findChild<SystemProfileView*>(type);

    if(!type.isEmpty() && systemProfileView)
    {
        ui->vehicleProfilesLayout->removeWidget(systemProfileView);
        delete systemProfileView;
    }
}

void VehicleProfilesView::setSystemProfileSelection(QList<QString> profiles)
{
    ui->SystemProfiles->clear();
    ui->SystemProfiles->addItems(profiles);
}

void VehicleProfilesView::clearSystemProfileViews()
{
    for(auto view : findChildren<SystemProfileView*>())
    {
        ui->vehicleProfilesLayout->removeWidget(view);
        delete view;
    }
}

void VehicleProfilesView::setSystemConfigError(QString const &message)
{
    ui->systemConfigError->setText(message);
}

void VehicleProfilesView::setModelCatalogueError(QString const &message)
{
    ui->modelCatalogueError->setText(message);
}

void VehicleProfilesView::setSystemProfilesError(QString const &message)
{
    ui->systemProfilesError->setText(message);
}
