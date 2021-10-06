/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#ifndef DELEGATECOMBOBOXVIEW_H
#define DELEGATECOMBOBOXVIEW_H

#include <QComboBox>
#include <QStyledItemDelegate>

class DelegateComboBoxView : public QStyledItemDelegate
{
    Q_OBJECT

public:
    DelegateComboBoxView(QList<QString> *const selectionList,
                         QObject *parent = nullptr);
    ~DelegateComboBoxView() = default;

    //-----------------------------------------------------------------------------
    //! Reimplementation of createEditor. A QComboBox is created as an editor.
    //!
    //! @param[in]      parent      The parent widget in which the combobox should appear
    //! @param[in]      option      an option item
    //! @param[in]      index       The model index of the data item which should be
    //!                             addressed by this delegate
    //!
    //! @return                     The pointer to the Combobox created as an editor.
    //-----------------------------------------------------------------------------
    QWidget *createEditor(QWidget *parent,
                          QStyleOptionViewItem const &option,
                          QModelIndex const &index) const override;

    //-----------------------------------------------------------------------------
    //! Reimplementation of setEditorData. Sets the data being shown by the combobox.
    //! The currently displayed value is determined according to the current value of
    //! the model data. Further, #selectionList will be shown when expanding the combobox.
    //!
    //! @param[in]      editor      a widget which should be casted on a combobox
    //! @param[in]      index       The model index of the data item which should be
    //!                             addressed by this delegate
    //-----------------------------------------------------------------------------
    void setEditorData(QWidget *editor,
                       QModelIndex const &index) const override;

    //-----------------------------------------------------------------------------
    //! Reimplementation of setModelData. Sets the model data according to the current
    //! value shown in the combobox.
    //!
    //! @param[in]      editor      a widget which should be casted on a combobox
    //! @param[in]      model       The pointer to the model whose data should be edited
    //! @param[in]      index       The model index of the data item which should be
    //!                             addressed by this delegate
    //-----------------------------------------------------------------------------
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      QModelIndex const &index) const override;

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
                              QStyleOptionViewItem const &option,
                              QModelIndex const &index) const override;

private:
    QList<QString> *const selectionList;
};

#endif // DELEGATECOMBOBOXVIEW_H
