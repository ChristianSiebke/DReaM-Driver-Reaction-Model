/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  DelegateTimeView.h
//! @ingroup trafficSimulationPlugin
//! @brief This class defines time delegates to be used within Qt's model/
//!        view framework.
//!
//!        Several functions of QStyledItemDelegate are reimplemented in
//!        order to enable editing of time data (QTime) within model-based
//!        item views (e.g. QTableView or QListView). As an editor, a a
//!        time editor (QTimeEdit) is used.
//-----------------------------------------------------------------------------

#ifndef DELEGATETIMEVIEW_H
#define DELEGATETIMEVIEW_H

#include <QStyledItemDelegate>
#include <QUndoStack>

//-----------------------------------------------------------------------------
//! @brief This class defines time delegates to be used within Qt's model/
//!        view framework.
//!
//!        Several functions of QStyledItemDelegate are reimplemented in
//!        order to enable editing of time data (QTime) within model-based
//!        item views (e.g. QTableView or QListView). As an editor, a a
//!        time editor (QTimeEdit) is used.
//-----------------------------------------------------------------------------
class DelegateTimeView : public QStyledItemDelegate
{
public:
    explicit DelegateTimeView(QString const &variableName, QUndoStack *const undoStack,
                              QObject *parent = nullptr);
    virtual ~DelegateTimeView() = default;

public:
    //-----------------------------------------------------------------------------
    //! Reimplementation of createEditor. Creates and returns a time editor
    //! (QTimeEdit).
    //!
    //! @param[in]      parent      The parent widget in which the spinbox should
    //!                             appear
    //! @param[in]      option      an option item
    //! @param[in]      index       The model index of the data item which should be
    //!                             addressed by this delegate
    //!
    //! @return                     The pointer to the spinbox created as an editor.
    //-----------------------------------------------------------------------------
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const override;

public:
    //-----------------------------------------------------------------------------
    //! Reimplementation of setEditorData. Sets the number being shown by the time editor
    //! to the current value in the model.
    //!
    //! @param[in]      editor      a widget which should be casted on a spinbox
    //! @param[in]      index       The model index of the data item which should be
    //!                             addressed by this delegate
    //-----------------------------------------------------------------------------
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    //-----------------------------------------------------------------------------
    //! Reimplementation of setModelData. Sets the model value to the current
    //! value shown in the time editor.
    //!
    //! @param[in]      editor      a widget which should be casted on a time editor
    //! @param[in]      model       The pointer to the model whose value should be
    //!                             edited
    //! @param[in]      index       The model index of the data item which should be
    //!                             addressed by this delegate
    //-----------------------------------------------------------------------------
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

public:
    //-----------------------------------------------------------------------------
    //! Reimplementation of updateEditorGeometry. Sets the geometry of the editor
    //! according to the passed option.
    //!
    //! @param[in]      editor      a widget referring to the editor
    //! @param[in]      option      an option item
    //! @param[in]      index       The model index of the data item which should be
    //!                             addressed by this delegate
    //-----------------------------------------------------------------------------
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;

private:
    QString const variableName;  //!< the name of the variable edited by this delegate
    QUndoStack *const undoStack; //!< a pointer to the undo stack
};

#endif // DELEGATETIMEVIEW_H
