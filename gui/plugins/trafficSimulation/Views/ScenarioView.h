/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ScenarioView.h
//! @ingroup trafficSimulationPlugin
//! @brief This class provides an editor for configuring the scenario setup, i.e.
//!        it is filled with the content stored in an ScenarioInterface instance.
//!
//!        It contains an #ui object describing the views appearance (defined in ScenarioView.ui).
//!        This ui object comprises a form layout with labels and editing elements for the different
//!        attributes of the scenario configuration. User action will lead to signal emissions
//!        which are noticed and managed by the corresponding presenter class.
//-----------------------------------------------------------------------------

#ifndef SCENARIOVIEW_H
#define SCENARIOVIEW_H

#include <QTableView>
#include <QWidget>

namespace Ui {
class ScenarioView;
}

//-----------------------------------------------------------------------------
//! @brief This class provides an editor for configuring the scenario setup, i.e.
//!        it is filled with the content stored in an ScenarioInterface instance.
//!
//!        It contains an #ui object describing the views appearance (defined in ScenarioVIew.ui).
//!        This ui object comprises a form layout with labels and editing elements for the different
//!        attributes of the scenario configuration. User action will lead to signal emissions
//!        which are noticed and managed by the corresponding presenter class.
//-----------------------------------------------------------------------------
class ScenarioView : public QWidget
{
    Q_OBJECT

public:
    explicit ScenarioView(QWidget *parent = nullptr);
    ~ScenarioView();

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal emitted when the name of the scenario has been modified by the user.
    //! Transmits the new name to the presenter.
    //-----------------------------------------------------------------------------
    void modifiedName(QString const &name);

    //-----------------------------------------------------------------------------
    //! Signal emitted when the open scenario file shall be modified.
    //-----------------------------------------------------------------------------
    void modifiedScenarioFile();

    //-----------------------------------------------------------------------------
    //! Signal emitted when the open scenario file shall be refreshed.
    //-----------------------------------------------------------------------------
    void refreshScenarioFile();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the line edit for the scenario name
    //! has been edited. Will emit the #modifiedName signal.
    //-----------------------------------------------------------------------------
    void on_NameEdit_editingFinished();

    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the Scenario File Browse button
    //! is clicked by the user. Will emit the #modifiedScenarioFile signal.
    //-----------------------------------------------------------------------------
    void on_ScenarioFileBrowse_clicked();

    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the Scenario File Refresh button
    //! is clicked by the user. Will emit the #refreshScenarioFile signal.
    //-----------------------------------------------------------------------------
    void on_ScenarioFileRefresh_clicked();

public:
    //-----------------------------------------------------------------------------
    //! Method which sets the line edit text for the scenario name to the name passed
    //! as an argument.
    //!
    //! @param[in]      name        the name which should be displayed
    //-----------------------------------------------------------------------------
    void setName(QString const &name);

    //-----------------------------------------------------------------------------
    //! Method which sets the line edit text for the open scenario file to the name passed
    //! as an argument.
    //!
    //! @param[in]      filename        the filename which should be displayed
    //-----------------------------------------------------------------------------
    void setScenarioFile(QString const &filename);

    //-----------------------------------------------------------------------------
    //! Method which sets an error message below the scenario file's line edit.
    //!
    //! @param[in]      message     the error message to be displayed
    //-----------------------------------------------------------------------------
    void setScenarioFileError(QString const &message);

    //-----------------------------------------------------------------------------
    //! Method for returning the #ui 's table widget. Will be passed to the corresponding
    //! presenter object containing the QAbstractTableModel.
    //!
    //! @return             the table view for agent profiles
    //-----------------------------------------------------------------------------
    QTableView *getScenarioAgentsView() const;

private:
    //-----------------------------------------------------------------------------
    //! Reimplementation of the event filter. This filter catches the resize events
    //! of the table view in order to resize their columns properly.
    //! (Doing it in the resize event of the table view itself did not yield the
    //! desired behaviour).
    //!
    //! @param[in]      watched     the object whose events should be catched
    //! @param[in]      event       the event to be catched
    //!
    //! @return                     always "false" since the event should never
    //!                             be blocked
    //-----------------------------------------------------------------------------
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::ScenarioView *ui; //!< the ui object containing the GUI elements of the view
};

#endif // SCENARIOVIEW_H
