/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "DriverProfilesView.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "ui_DriverProfilesView.h"

DriverProfilesView::DriverProfilesView(QWidget *parent) :
    WidgetView(parent),
    ui(new Ui::DriverProfilesView)
{
    ui->setupUi(this);

    QFont headerFont;
    headerFont.setBold(true);

    ui->ParametersView->installEventFilter(this);
    ui->ParametersView->horizontalHeader()->setFont(headerFont);
}

DriverProfilesView::~DriverProfilesView()
{
    delete ui;
}

QTableView *DriverProfilesView::getParameterView() const
{
    return ui->ParametersView;
}

void DriverProfilesView::on_Name_activated(const QString &name)
{
    Q_EMIT selectionChanged(name);
}

void DriverProfilesView::on_add_clicked()
{
    QString newName = QInputDialog::getText(
        this, tr("Add Entry"),
        tr("Insert a Name"),
        QLineEdit::Normal);

    if (!newName.isEmpty())
    {
        Q_EMIT addDriverProfile(newName);
    }
}

void DriverProfilesView::on_remove_clicked()
{
    QMessageBox warning;
    int ret;

    // spell out a warning that everything will be deleted
    warning.setText("The whole configuration of the driver profile will be deleted!");
    warning.setInformativeText("Do you want to continue?");
    warning.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    ret = warning.exec();

    if (ret == QMessageBox::Yes)
    {
        Q_EMIT removeDriverProfile(ui->Name->currentText());
    }
}

void DriverProfilesView::on_edit_clicked()
{
    QString newName = QInputDialog::getText(
        this, tr("Edit Entry"),
        tr("Insert a new Name"),
        QLineEdit::Normal);

    if (!newName.isEmpty())
    {
        Q_EMIT editedName(ui->Name->currentText(), newName);
        ui->Name->setCurrentText(newName);
    }
}

void DriverProfilesView::on_Browse_clicked()
{
    // Prompt file dialog to load setup
    QString const filepath = QFileDialog::getOpenFileName(
        this, QObject::tr("openPASS / Choose DriverConfig"),
        QCoreApplication::applicationDirPath(), QStringLiteral("DriverConfig (*.xml)"));

    if (!filepath.isNull())
    {
        ui->driverConfig->setText(filepath);
        Q_EMIT selectedDriverConfig(filepath);
    }
}

void DriverProfilesView::setCurrentDriver(const QString &name)
{
    if (ui->Name->findText(name) != -1)
        ui->Name->setCurrentText(name);
}

void DriverProfilesView::setDriverSelectionItems(const QStringList &names)
{
    ui->Name->clear();
    ui->Name->addItems(names);
}

void DriverProfilesView::setConfigView(const QString &filepath)
{
    ui->driverConfig->setText(filepath);
}

void DriverProfilesView::addDriverView(const QString &name)
{
    ui->Name->addItem(name);
}

void DriverProfilesView::removeDriverView(const QString &name)
{
    int index = ui->Name->findText(name);
    ui->Name->removeItem(index);
}

void DriverProfilesView::enableConfiguration(bool enable)
{
    ui->remove->setEnabled(enable);
    ui->edit->setEnabled(enable);

    // currently this is not utilised as there is only one fixed driver model
    ui->Browse->setEnabled(false);
    ui->driverConfig->setEnabled(false);
}

bool DriverProfilesView::eventFilter(QObject *watched, QEvent *event)
{
    QTableView *table = qobject_cast<QTableView *>(watched);

    if (table && event->type() == QEvent::Resize)
    {
        int cell = (table->width()) / 12;
        table->setColumnWidth(0, 2 * cell);
        table->setColumnWidth(1, 4 * cell);
        table->setColumnWidth(2, cell);
    }

    return false;
}
