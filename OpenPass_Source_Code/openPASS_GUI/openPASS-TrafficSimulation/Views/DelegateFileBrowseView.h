/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef DELEGATEFILEBROWSEVIEW_H
#define DELEGATEFILEBROWSEVIEW_H


#include "openPASS-Project/ProjectInterface.h"
#include "FileBrowseView.h"

#include "EditDataCommand.h"

#include <QStyledItemDelegate>
#include <QUndoStack>


class DelegateFileBrowseView : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit DelegateFileBrowseView(ProjectInterface const * const project, QString const &variableName, QUndoStack * const undoStack, QObject *parent = nullptr);
    virtual ~DelegateFileBrowseView() override = default;

public:
    //-----------------------------------------------------------------------------
    //! Reimplementation of createEditor. Creates an ui-object defined in the
    //! Ui::DelegateFileBrowseView class. This is based on the xml file in
    //! DelegateFileBrowseView.ui.
    //!
    //! @param[in]      parent      The parent widget in which the spinbox should appear
    //! @param[in]      option      an option item
    //! @param[in]      index       The model index of the data item which should be
    //!                             addressed by this delegate
    //!
    //! @return                     The pointer to the file browse ui object
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
private Q_SLOT:
    void editData(QWidget *editor);

private:
    ProjectInterface const * const project;     //!< a pointer to the project plugin (required to access the file structure of the master config)
    QString const variableName;                 //!< the name of the variable edited by this delegate
    QUndoStack * const undoStack;               //!< a pointer to the undo stack
};

#endif // DELEGATEFILEBROWSEVIEW_H
