/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef SYSTEMPROFILEVIEW_H
#define SYSTEMPROFILEVIEW_H

#include <QWidget>
#include <QTableView>

namespace Ui {
class SystemProfileView;
}

class SystemProfileView : public QWidget
{
    Q_OBJECT

public:
    explicit SystemProfileView(QString AlgoType,
                               QWidget *parent = 0);
    ~SystemProfileView();

Q_SIGNALS:
    void addSystem(QString AlgoType, QString const & selectionItem);
    void removeSystem(QString AlgoType, const int &row);

private Q_SLOTS:
    void on_addSystem_clicked();
    void on_removeSystem_clicked();

public:
    void setSelectionItems(const QList<QString> &items );
    QTableView * getSystemView() const;

private:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::SystemProfileView *ui;
};

#endif // SYSTEMPROFILEVIEW_H
