/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "DelegateDoubleView.h"

#include <QDoubleSpinBox>

DelegateDoubleView::DelegateDoubleView(double min, double max, double increment, int decimals, const QString &variableName, QUndoStack * const undoStack, QObject *parent)
    : QStyledItemDelegate(parent)
    , min(min)
    , max(max)
    , increment(increment)
    , decimals(decimals)
    , variableName(variableName)
    , undoStack(undoStack)
{

}

QWidget * DelegateDoubleView::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    QDoubleSpinBox * editor = new QDoubleSpinBox(parent);
    editor->setMinimum(min);
    editor->setMaximum(max);
    editor->setDecimals(decimals);
    editor->setSingleStep(increment);
    editor->setFrame(false);
    editor->setAlignment(Qt::AlignCenter);

    return editor;
}

void DelegateDoubleView::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    double value = index.model()->data(index, Qt::DisplayRole).toDouble();
    QDoubleSpinBox *doubleSpinBox = static_cast<QDoubleSpinBox*>(editor);
    doubleSpinBox->setValue(value);
}

void DelegateDoubleView::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDoubleSpinBox *doubleSpinBox = static_cast<QDoubleSpinBox*>(editor);
    undoStack->push(new EditData(model, index, QString::number(doubleSpinBox->value(),'f',decimals), variableName));
}

void DelegateDoubleView::updateEditorGeometry(QWidget *editor,
                                              const QStyleOptionViewItem &option,
                                              const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
