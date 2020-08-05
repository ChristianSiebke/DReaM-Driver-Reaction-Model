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
//! @file  ExperimentView.h
//! @ingroup trafficSimulationPlugin
//! @brief This class provides an editor for configuring the experiment setup, i.e.
//!        it is filled with the content stored in an ExperimentInterface instance.
//!
//!        It contains an #ui object describing the views appearance (defined in ExperimentView.ui).
//!        This ui object comprises a form layout with labels and editing elements for the different
//!        attributes of the experiment configuration. User action will lead to signal emissions
//!        which are noticed and managed by the corresponding presenter class, i.e.
//!        ExperimentPresenter.
//-----------------------------------------------------------------------------

#ifndef EXPERIMENTVIEW_H
#define EXPERIMENTVIEW_H

#include "WidgetView.h"

#include <QWidget>
#include <QListView>
#include <QTableView>

namespace Ui {
class ExperimentView;
}

//-----------------------------------------------------------------------------
//! @brief This class provides an editor for configuring the experiment setup, i.e.
//!        it is filled with the content stored in an ExperimentInterface instance.
//!
//!        It contains an #ui object describing the views appearance (defined in Experiment.ui).
//!        This ui object comprises a form layout with labels and editing elements for the different
//!        attributes of the experiment configuration. User action will lead to signal emissions
//!        which are noticed and managed by the corresponding presenter class,
//!        i.e. ExperimentPresenter.
//-----------------------------------------------------------------------------
class ExperimentView : public WidgetView
{
    Q_OBJECT

public:
    explicit ExperimentView(QWidget *parent = nullptr);
    ~ExperimentView();

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted when the number of invocations has been modified by the user.
    //-----------------------------------------------------------------------------
    void Invocations_valueChanged(int value);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the random seed value has been modified by the user.
    //-----------------------------------------------------------------------------
    void Seed_valueChanged(int value);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the add-button in the library section has
    //! been pressed by the user.
    //-----------------------------------------------------------------------------
    void addLibrary();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the remove-button in the library section has
    //! been pressed by the user.
    //-----------------------------------------------------------------------------
    void removeLibrary(int const & row);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the add-button in the logging groups section has
    //! been pressed by the user.
    //-----------------------------------------------------------------------------
    void addLoggingGroup();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the remove-button in the logging groups section has
    //! been pressed by the user.
    //-----------------------------------------------------------------------------
    void removeLoggingGroup(int const & row);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when this view has been resized. Will be connected to the
    //! slot ExperimentBaseView::updateSize in order to resize the container widget
    //! according to the new size of this view.
    //-----------------------------------------------------------------------------
    void resized();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the user clicks the Start Simulation Button. Will trigger
    //! a status check of the slave Config settings (edited this plugin) and the
    //! creation of the simulation slave process.
    //-----------------------------------------------------------------------------
    void startSimulation();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the spinbox for the number of invocations
    //! has been edited. Will emit the signal #Invocations_valueChanged.
    //-----------------------------------------------------------------------------
    void on_InvocationsValue_editingFinished();

    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the spinbox value for the random seed
    //! has been edited. Will emit the signal #Seed_valueChanged.
    //-----------------------------------------------------------------------------
    void on_SeedValue_editingFinished();

    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the add-button in the Library section
    //! has been pressed. Emits the #addLibrary signal.
    //-----------------------------------------------------------------------------
    void on_Library_add_clicked();

    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the remove-button in the Library section
    //! has been pressed. Emits the #removeLibrary signal.
    //-----------------------------------------------------------------------------
    void on_Library_remove_clicked();

    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the add-button in the logging groups section
    //! has been pressed. Emits the #addLoggingGroup signal.
    //-----------------------------------------------------------------------------
    void on_LoggingGroup_add_clicked();

    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the remove-button in the logging groups section
    //! has been pressed. Emits the #removeLoggingGroup signal.
    //-----------------------------------------------------------------------------
    void on_LoggingGroup_remove_clicked();

    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the Start SImulation button has
    //! been clicked. Emits the #startSimulation signal.
    //-----------------------------------------------------------------------------
    void on_StartSimulation_clicked();

public:
    //-----------------------------------------------------------------------------
    //! Method for setting the spinbox value for the number of invocations to the value
    //! passed as an argument.
    //!
    //! @param[in]      value       the value to be displayed by the spinbox
    //-----------------------------------------------------------------------------
    void setInvocationsValue(int value);

    //-----------------------------------------------------------------------------
    //! Method for setting the spinbox value for the random seed to the value passed as an argument.
    //!
    //! @param[in]      value       the value to be displayed by the spinbox
    //-----------------------------------------------------------------------------
    void setSeedValue(int value);

    //-----------------------------------------------------------------------------
    //! Method which gives access to the QListView instance listing the logging groups.
    //! Will be used by the presenter to connect it to the corresponding model.
    //!
    //! @return              the pointer to the List view of the logging groups
    //-----------------------------------------------------------------------------
    QListView * getLoggingGroupsView() const;

    //-----------------------------------------------------------------------------
    //! Method which gives access to the QTableView instance holding the entries of
    //! the libraries map.
    //! Will be used by the presenter to connect it to the corresponding model.
    //!
    //! @return                the pointer to the table of libraries
    //-----------------------------------------------------------------------------
    QTableView * getLibrariesView() const;

    //-----------------------------------------------------------------------------
    //! Method which enables the start simulation button.
    //-----------------------------------------------------------------------------
    void enableSimulation(bool enable) ;

private:
    //-----------------------------------------------------------------------------
    //! Reimplementation of the resize event. Will emit the signal #resized when this
    //! view is resized.
    //!
    //! @param[in]      event       the resize event (Qt's QResizeEvent class)
    //-----------------------------------------------------------------------------
    void resizeEvent(QResizeEvent * event);

    //-----------------------------------------------------------------------------
    //! Reimplementation of the event filter. This filter catches the resize events
    //! of the table view (libraries table) in order to resize their columns properly.
    //! (Doing it in the resize event of the table view itself did not yield the
    //! desired behaviour).
    //!
    //! @param[in]      watched     the object whose events should be catched
    //! @param[in]      event       the event to be catched
    //!
    //! @return                     always "false" since the event should never
    //!                             be blocked
    //-----------------------------------------------------------------------------
    bool eventFilter(QObject *watched, QEvent *event) ;

private:
    Ui::ExperimentView *ui;     //!< the ui object containing the GUI elements of this editor
};

#endif // EXPERIMENTVIEW_H
