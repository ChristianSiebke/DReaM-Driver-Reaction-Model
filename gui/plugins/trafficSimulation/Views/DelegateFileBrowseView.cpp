/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "DelegateFileBrowseView.h"

DelegateFileBrowseView::DelegateFileBrowseView(const ProjectInterface *const project, const QString &variableName, QUndoStack *const undoStack, QObject *parent) :
    QStyledItemDelegate(parent), project(project), variableName(variableName), undoStack(undoStack)
{
}

QWidget *DelegateFileBrowseView::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    FileBrowseView *filebrowse = new FileBrowseView(project, parent);
    connect(filebrowse, &FileBrowseView::editingFinished, this, &DelegateFileBrowseView::editData);
    return filebrowse;
}

void DelegateFileBrowseView::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString const filename = index.model()->data(index, Qt::DisplayRole).toString();
    qobject_cast<FileBrowseView *>(editor)->setFile(filename);
}

void DelegateFileBrowseView::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    FileBrowseView *filebrowse = qobject_cast<FileBrowseView *>(editor);
    undoStack->push(new EditData(model, index, filebrowse->getFile(), variableName));
}

void DelegateFileBrowseView::updateEditorGeometry(QWidget *editor,
                                                  const QStyleOptionViewItem &option,
                                                  const QModelIndex & /* index */) const
{
    editor->setGeometry(option.rect);
}

void DelegateFileBrowseView::editData(QWidget *editor)
{
    Q_EMIT commitData(editor);
    Q_EMIT closeEditor(editor, QAbstractItemDelegate::EndEditHint::NoHint);
}
