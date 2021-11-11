/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef EDITDATACOMMAND_H
#define EDITDATACOMMAND_H

#include <QAbstractItemModel>
#include <QUndoCommand>

//-----------------------------------------------------------------------------
//! @brief This class defines the command for an undoable edit-operation on a
//! general abstract item model.
//-----------------------------------------------------------------------------
class EditData : public QUndoCommand
{
public:
    EditData(QAbstractItemModel *const model, const QModelIndex &index, const QVariant &value, const QString variableName,
             QUndoCommand *parent = nullptr) :
        QUndoCommand(parent), model(model), index(index), newValue(value), oldValue(model->data(index))
    {
        setText(QString("edited %1 value").arg(variableName));
    }

public:
    void redo() override
    {
        model->setData(index, newValue);
    }
    void undo() override
    {
        model->setData(index, oldValue);
    }

private:
    QAbstractItemModel *const model;
    QModelIndex const index;
    QVariant const newValue;
    QVariant const oldValue;
};

#endif // EDITDATACOMMAND_H
