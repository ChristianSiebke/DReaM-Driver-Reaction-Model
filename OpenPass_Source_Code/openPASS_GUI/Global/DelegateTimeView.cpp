/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "DelegateTimeView.h"

#include "EditDataCommand.h"
#include <QTimeEdit>

DelegateTimeView::DelegateTimeView(const QString &variableName, QUndoStack * const undoStack,
                                   QObject *parent)
    : QStyledItemDelegate(parent)
    , variableName(variableName)
    , undoStack(undoStack)
{

}

QWidget * DelegateTimeView::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
    QTimeEdit * editor= new QTimeEdit(parent);
    editor->setAlignment(Qt::AlignCenter);

    return editor;
}
void DelegateTimeView::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QTime time = index.model()->data(index, Qt::DisplayRole).toTime();
    QTimeEdit * timeEdit = static_cast<QTimeEdit*>(editor);
    timeEdit->setTime( time );

}

void DelegateTimeView::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QTimeEdit * timeEdit = static_cast<QTimeEdit*>(editor);
    undoStack->push(new EditData(model, index, timeEdit->time().toString(QString("hh:mm")), variableName));
}

void DelegateTimeView::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}


