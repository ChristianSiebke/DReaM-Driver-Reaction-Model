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
//! @file  DelegateDoubleView.h
//! @ingroup trafficSimulationPlugin
//! @brief This class defines spinbox delegates to be used within Qt's model/
//!        view framework for editing numbers.
//!
//!        Several functions of QStyledItemDelegate are reimplemented in
//!        order to enable editing of double precision numbers within model-based
//!        item views (e.g. QTableView or QListView). As an editor, a spinbox
//!        (QDoubleSpinBox) is used. The values for the minimum, maximum, number
//!        of decimals and increment are to be passed when instantiating this
//!        class.
//-----------------------------------------------------------------------------
#ifndef DELEGATEDOUBLEVIEW_H
#define DELEGATEDOUBLEVIEW_H

#include <QStyledItemDelegate>

#include "EditDataCommand.h"

//-----------------------------------------------------------------------------
//! @brief This class defines spinbox delegates to be used within Qt's model/
//!        view framework for editing numbers.
//!
//!        Several functions of QStyledItemDelegate are reimplemented in
//!        order to enable editing of double precision numbers within model-based
//!        item views (e.g. QTableView or QListView). As an editor, a spinbox
//!        (QDoubleSpinBox) is used. The values for the minimum, maximum, number
//!        of decimals and increment are to be passed when instantiating this
//!        class.
//-----------------------------------------------------------------------------
class DelegateDoubleView : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit DelegateDoubleView(double min,
                                double max,
                                double increment,
                                int decimals,
                                QString const &variableName,
                                QUndoStack *const undoStack,
                                QObject *parent = nullptr);

    virtual ~DelegateDoubleView() = default;

public:
    //-----------------------------------------------------------------------------
    //! Reimplementation of createEditor. Creates a frameless QDoubleSpinBox object, sets its minimum
    //! and maximum values and returns it as an editor.
    //!
    //! @param[in]      parent      The parent widget in which the spinbox should appear
    //! @param[in]      option      an option item
    //! @param[in]      index       The model index of the data item which should be
    //!                             addressed by this delegate
    //!
    //! @return                     The pointer to the spinbox created as an editor.
    //-----------------------------------------------------------------------------
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const override;

public:
    //-----------------------------------------------------------------------------
    //! Reimplementation of setEditorData. Sets the number being shown by the spinbox
    //! to the current value in the model.
    //!
    //! @param[in]      editor      a widget which should be casted on a spinbox
    //! @param[in]      index       The model index of the data item which should be
    //!                             addressed by this delegate
    //-----------------------------------------------------------------------------
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    //-----------------------------------------------------------------------------
    //! Reimplementation of setModelData. Sets the model value to the current
    //! value shown in the spinbox.
    //!
    //! @param[in]      editor      a widget which should be casted on a spinbox
    //! @param[in]      model       The pointer to the model whose value should be edited
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
    double const min;            //!< the minimum value
    double const max;            //!< the maximum value
    double const increment;      //!< the increment
    int const decimals;          //!< the number of decimals
    QString const variableName;  //!< the name of the variable edited by this delegate
    QUndoStack *const undoStack; //!< the pointer to the undo stack
};

#endif // DELEGATEDOUBLEVIEW_H
