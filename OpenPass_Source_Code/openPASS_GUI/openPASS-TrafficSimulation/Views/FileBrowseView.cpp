/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "FileBrowseView.h"
#include "ui_FileBrowseView.h"

#include <QFileDialog>

FileBrowseView::FileBrowseView(const ProjectInterface * const project, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FileBrowseView)
    , project(project)
{
    ui->setupUi(this);
}

FileBrowseView::~FileBrowseView()
{
    delete ui;
}


void FileBrowseView::setFile(QString const &file)
{
    ui->File->setText(file);
}

QString FileBrowseView::getFile() const
{
    return ui->File->text();
}

void FileBrowseView::on_Browse_clicked()
{
    QString filename = QFileDialog::getOpenFileName(
                this, QObject::tr("openPASS / Choose a library file"),
                project->getLibraryPath(), QStringLiteral("Dynamic library file(*.dll)"));

    // chop the .dll suffix
    filename.chop(4);
    ui->File->setText(project->getLibrary().relativeFilePath(filename));

    Q_EMIT editingFinished(this);
}
