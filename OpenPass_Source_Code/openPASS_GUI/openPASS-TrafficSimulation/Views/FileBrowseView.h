/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef FILEBROWSEVIEW_H
#define FILEBROWSEVIEW_H


#include "openPASS-Project/ProjectInterface.h"

#include <QWidget>
#include <QString>
#include <QStyledItemDelegate>

namespace Ui{
class FileBrowseView;
}

class FileBrowseView : public QWidget
{
    Q_OBJECT

public:
    explicit FileBrowseView(ProjectInterface const * const project, QWidget *parent = nullptr);
    ~FileBrowseView();

Q_SIGNALS:
    void editingFinished(QWidget * editor);

private Q_SLOTS:
    void on_Browse_clicked();

public:
    void setFile(QString const &file);
    QString getFile() const;

private:
    Ui::FileBrowseView *ui;
    ProjectInterface const * const project;
};

#endif // FILEBROWSEVIEW_H
