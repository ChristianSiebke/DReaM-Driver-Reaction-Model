/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef DRIVERPROFILESVIEW_H
#define DRIVERPROFILESVIEW_H

#include "WidgetView.h"

#include <QTableView>

namespace Ui {
class DriverProfilesView;
}

class DriverProfilesView : public WidgetView
{
    Q_OBJECT

public:
    explicit DriverProfilesView(QWidget *parent = 0);
    ~DriverProfilesView();

Q_SIGNALS:
    void addDriverProfile(QString const &name);
    void removeDriverProfile(QString const &name);
    void editedName(QString const & oldName, QString const &newName);
    void selectionChanged(QString const &name);
    void selectedDriverConfig(QString const &filepath);

private Q_SLOTS:
    void on_Name_activated(const QString &name);
    void on_add_clicked();
    void on_remove_clicked();
    void on_edit_clicked();

private Q_SLOTS:
    void on_Browse_clicked();
    void on_driverConfig_editingFinished();

public:
    QTableView * getParameterView() const;

public:
    void setCurrentDriver(QString const &name);
    void addDriverView(QString const &name);
    void removeDriverView(QString const &name);
    void setDriverSelectionItems(QStringList const &names);

public:
    void setConfigView(QString const &filepath);

public:
    void enableConfiguration(bool enable);

private:
    bool eventFilter(QObject *watched, QEvent *event);


private:
    Ui::DriverProfilesView *ui;
};

#endif // DRIVERPROFILESVIEW_H
