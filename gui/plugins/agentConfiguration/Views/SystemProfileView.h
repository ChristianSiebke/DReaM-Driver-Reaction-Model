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
//! @file  SystemProfileView.h
//! @ingroup agentConfigurationPlugin
//! @brief This class defines the view of system profiles, i.e. the mapping of
//!        systems with a certain algorithm type onto probabilities.
//!
//!        The view constitutes a table view for assigning systems to probabilities,
//!        buttons for adding and removing systems and a combobox to select
//!        systems for adding.
//-----------------------------------------------------------------------------

#ifndef SYSTEMPROFILEVIEW_H
#define SYSTEMPROFILEVIEW_H

#include <QTableView>
#include <QWidget>

namespace Ui {
class SystemProfileView;
}

//-----------------------------------------------------------------------------
//! @brief This class defines the view of system profiles, i.e. the mapping of
//!        systems with a certain algorithm type onto probabilities.
//!
//!        The view constitutes a table view for assigning systems to probabilities,
//!        buttons for adding and removing systems and a combobox to select
//!        systems for adding.
//-----------------------------------------------------------------------------
class SystemProfileView : public QWidget
{
    Q_OBJECT

public:
    explicit SystemProfileView(QString AlgoType,
                               QWidget *parent = nullptr);
    ~SystemProfileView();

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted when the add-button for systems has been clicked. Transmits
    //! the algorithm type and the system currently selected in the corresponding
    //! combo box (i.e. the one to be added to the table).
    //!
    //! @param[in]      AlgoType        the algorithm type of the systems
    //! @param[in]      selectionItem   the system currently selected (i.e. to be
    //!                                 added)
    //-----------------------------------------------------------------------------
    void addSystem(QString AlgoType, QString const &selectionItem);

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the remove-button for systems has been clicked.
    //! Transmits the algorithm type and the row index of the system to be removed
    //! from the table (i.e. the row which has focus).
    //!
    //! @param[in]      AlgoType        the algorithm type of the systems
    //! @param[in]      row             the row index of the system to be removed
    //-----------------------------------------------------------------------------
    void removeSystem(QString AlgoType, const int &row);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot triggered when the add-button for systems is clicked. Emits the
    //! #addSystem signal transmitting the system currently displayed by the
    //! combo box.
    //-----------------------------------------------------------------------------
    void on_addSystem_clicked();

    //-----------------------------------------------------------------------------
    //! Slot triggered when the remove-button for systems is clicked. Emits the
    //! #removeSystem signal transmitting the system name currently displayed by the
    //! combo box.
    //-----------------------------------------------------------------------------
    void on_removeSystem_clicked();

public:
    //-----------------------------------------------------------------------------
    //! Method which clears the selection list for the systems' combo box
    //! and inserts the given list.
    //!
    //! @param[in]      items       the list of systems to be listed in
    //!                             the corresponding combo box
    //-----------------------------------------------------------------------------
    void setSelectionItems(const QList<QString> &items);

    //-----------------------------------------------------------------------------
    //! Method which provides access to the table view for systems.
    //!
    //! @return                     the table view (QTableView) for systems
    //-----------------------------------------------------------------------------
    QTableView *getSystemView() const;

private:
    //-----------------------------------------------------------------------------
    //! Reimplementation of the event filter for catching the resize-events of the
    //! systems' table view. It takes care of the proper column width ratio after
    //! resizing.
    //!
    //! @param[in]      watched     the watched object
    //! @param[in]      event       the event to be catched
    //!
    //! @return                     always "false" as the event should be catched but
    //!                             not filtered
    //-----------------------------------------------------------------------------
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::SystemProfileView *ui; //!< the ui object containing all GUI elements of this view
};

#endif // SYSTEMPROFILEVIEW_H
