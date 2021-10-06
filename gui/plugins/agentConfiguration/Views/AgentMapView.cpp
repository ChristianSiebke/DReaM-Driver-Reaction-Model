/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "AgentMapView.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "ui_AgentMapView.h"

AgentMapView::AgentMapView(QWidget *parent) :
    WidgetView(parent), ui(new Ui::AgentMapView)
{
    ui->setupUi(this);

    QFont headerFont;
    headerFont.setBold(true);

    enableConfiguration(false);

    ui->VehicleView->installEventFilter(this);
    ui->VehicleView->horizontalHeader()->setFont(headerFont);

    ui->DriverView->installEventFilter(this);
    ui->DriverView->horizontalHeader()->setFont(headerFont);
}

AgentMapView::~AgentMapView()
{
    delete ui;
}

void AgentMapView::enableModelView(bool enable)
{
    ui->Model->setEnabled(enable);
}

void AgentMapView::enableSystemView(bool enable)
{
    ui->system->setEnabled(enable);
}

void AgentMapView::enableConfiguration(bool enable)
{
    ui->remove->setEnabled(enable);
    ui->edit->setEnabled(enable);
    ui->Type->setEnabled(enable);

    ui->addVehicle->setEnabled(enable);
    ui->removeVehicle->setEnabled(enable);

    ui->addDriver->setEnabled(enable);
    ui->removeDriver->setEnabled(enable);

    ui->system->setEnabled(enable);
    ui->systemConfig->setEnabled(enable);
    ui->systemConfigBrowse->setEnabled(enable);
    ui->systemConfigRefresh->setEnabled(enable);

    ui->Model->setEnabled(enable);
    ui->ModelCatalogue->setEnabled(enable);
    ui->ModelCatalogueBrowse->setEnabled(enable);
    ui->ModelCatalogueRefresh->setEnabled(enable);
}

void AgentMapView::on_add_clicked()
{
    QString newName = QInputDialog::getText(
        this, tr("Add Entry"),
        tr("Insert an agent profile name"),
        QLineEdit::Normal);

    if (!newName.isEmpty())
    {
        Q_EMIT add(newName);
    }
}

void AgentMapView::on_remove_clicked()
{
    QMessageBox warning;
    int ret;

    warning.setText(QString("The entry and corresponding data will be deleted!"));
    warning.setInformativeText("Do you want to continue?");
    warning.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    ret = warning.exec();

    if (ret == QMessageBox::Yes)
        Q_EMIT remove(ui->AgentProfileSelection->currentText());
}

void AgentMapView::on_edit_clicked()
{
    QString newName = QInputDialog::getText(
        this, tr("Edit Entry"),
        tr("Insert a new name"),
        QLineEdit::Normal);

    if (!newName.isEmpty())
        Q_EMIT modifiedName(ui->AgentProfileSelection->currentText(), newName);
}

void AgentMapView::setCurrentAgent(const QString &name)
{
    if (ui->AgentProfileSelection->findText(name) != -1)
        ui->AgentProfileSelection->setCurrentText(name);
}

void AgentMapView::addAgent(const QString &name)
{
    ui->AgentProfileSelection->addItem(name);
}

void AgentMapView::setAgentSelectionItems(QStringList const &names)
{
    ui->AgentProfileSelection->clear();
    ui->AgentProfileSelection->addItems(names);
}

void AgentMapView::removeAgent(const QString &name)
{
    int index = ui->AgentProfileSelection->findText(name);
    ui->AgentProfileSelection->removeItem(index);
}

void AgentMapView::on_AgentProfileSelection_activated(const QString &name)
{
    Q_EMIT selectionChanged(name);
}

void AgentMapView::clear()
{
    ui->systemConfig->clear();
    ui->system->clear();
    ui->ModelCatalogue->clear();
    ui->Model->clear();

    ui->Type->setCurrentText("Dynamic");
    ui->TypeSpecificElements->setCurrentIndex(0);
}

void AgentMapView::on_addVehicle_clicked()
{
    Q_EMIT addVehicleProfile();
}

void AgentMapView::on_removeVehicle_clicked()
{
    Q_EMIT removeVehicleProfile(ui->VehicleView->currentIndex().row());
}

void AgentMapView::on_addDriver_clicked()
{
    Q_EMIT addDriverProfile();
}

void AgentMapView::on_removeDriver_clicked()
{
    Q_EMIT removeDriverProfile(ui->DriverView->currentIndex().row());
}

QTableView *AgentMapView::getVehicleProfilesView() const
{
    return ui->VehicleView;
}

QTableView *AgentMapView::getDriverProfilesView() const
{
    return ui->DriverView;
}

void AgentMapView::setCurrentType(const QString &type)
{
    if (type == "Dynamic")
    {
        ui->Type->setCurrentText(type);
        ui->TypeSpecificElements->setCurrentWidget(ui->Dynamic);
    }
    if (type == "Static")
    {
        ui->Type->setCurrentText(type);
        ui->TypeSpecificElements->setCurrentWidget(ui->Static);
    }
}

void AgentMapView::on_Type_activated(const QString &type)
{
    Q_EMIT typeChanged(type);
}

void AgentMapView::on_system_activated(const QString &system)
{
    Q_EMIT selectedSystem(system);
}

void AgentMapView::on_systemConfigBrowse_clicked()
{
    Q_EMIT setSystemConfig();
}

void AgentMapView::on_systemConfigRefresh_clicked()
{
    Q_EMIT refreshSystemConfig();
}

void AgentMapView::setCurrentSystemConfig(const QString &systemConfig)
{
    ui->systemConfig->setText(systemConfig);
}

void AgentMapView::setCurrentSystem(const QString &system)
{
    if (ui->system->findText(system) != -1)
        ui->system->setCurrentText(system);
}

void AgentMapView::setSystemSelectionItems(const QStringList &systemItems)
{
    ui->system->clear();

    if (!systemItems.isEmpty())
        ui->system->addItems(systemItems);
}

void AgentMapView::on_ModelCatalogueBrowse_clicked()
{
    Q_EMIT setModelCatalogue();
}

void AgentMapView::on_ModelCatalogueRefresh_clicked()
{
    Q_EMIT refreshModelCatalogue();
}

void AgentMapView::on_Model_activated(const QString &model)
{
    Q_EMIT setModel(model);
}

void AgentMapView::setCurrentModelCatalogue(const QString &modelCatalogue)
{
    ui->ModelCatalogue->setText(modelCatalogue);
}

void AgentMapView::setCurrentModel(const QString &model)
{
    if (ui->Model->findText(model) != -1)
        ui->Model->setCurrentText(model);
}

void AgentMapView::setModelSelectionItems(const QStringList &modelItems)
{
    ui->Model->clear();
    ui->Model->addItems(modelItems);
}

bool AgentMapView::eventFilter(QObject *watched, QEvent *event)
{
    QTableView *table = qobject_cast<QTableView *>(watched);

    if (table && event->type() == QEvent::Resize)
    {
        int cell = (table->width()) / 2;
        table->setColumnWidth(0, cell);
    }

    return false;
}

void AgentMapView::setSystemConfigError(const QString &message)
{
    ui->systemConfigError->setText(message);
}

void AgentMapView::setModelCatalogueError(const QString &message)
{
    ui->modelCatalogueError->setText(message);
}

void AgentMapView::setDriverProbabilityError(bool error)
{
    if (error)
        ui->DriverProbabilityError->setText("probability sum invalid!");
    else
        ui->DriverProbabilityError->setText("");
}

void AgentMapView::setVehicleProbabilityError(bool error)
{
    if (error)
        ui->VehicleProbabilityError->setText("probability sum invalid!");
    else
        ui->VehicleProbabilityError->setText("");
}
