/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "DelegateComboBoxView.h"

#include <QLineEdit>

DelegateComboBoxView::DelegateComboBoxView(const QList<QString> *const selectionList, const QString &variableName, QUndoStack *const undoStack, QObject *parent) :
    QStyledItemDelegate(parent), selectionList(selectionList), variableName(variableName), undoStack(undoStack)
{
}

QWidget *DelegateComboBoxView::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    QComboBox *combobox = new QComboBox(parent);
    combobox->setEditable(true);
    combobox->lineEdit()->setReadOnly(true);
    combobox->lineEdit()->setAlignment(Qt::AlignCenter);

    return combobox;
}

void DelegateComboBoxView::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *combobox = static_cast<QComboBox *>(editor);

    if (combobox)
    {
        combobox->addItems(*selectionList);
        QString currentText = index.data(Qt::EditRole).toString();
        int currentIndex = combobox->findText(currentText);

        if (currentIndex >= 0)
            combobox->setCurrentIndex(currentIndex);
    }
    else
        QStyledItemDelegate::setEditorData(editor, index);
}

void DelegateComboBoxView::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *combobox = static_cast<QComboBox *>(editor);

    if (combobox)
    {
        undoStack->push(new EditData(model, index, combobox->currentText(), variableName));
    }
    else
        QStyledItemDelegate::setModelData(editor, model, index);
}

void DelegateComboBoxView::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);

    editor->setGeometry(option.rect);
}
