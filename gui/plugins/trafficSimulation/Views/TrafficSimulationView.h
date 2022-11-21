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
//! @file  TrafficSimulationView.h
//! @ingroup trafficSimulationPlugin
//! @brief This class constitutes the main view of the Traffic Simulation Plugin and
//!        instantiates all other configuration views as child widgets.
//!
//!        It contains an #ui object describing the views appearence (defined in TrafficSimulation.ui).
//!        This ui object comprises a tab widget for the experiment, scenario, environments and traffic
//!        views. These tabs contain scroll areas in which a resizable container widget will be put for the
//!        particular configuration widget. Further, the action menu and info bar are instantiated.
//-----------------------------------------------------------------------------
#ifndef TRAFFICSIMULATIONVIEW_H
#define TRAFFICSIMULATIONVIEW_H

#include <QColor>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPoint>
#include <QResizeEvent>
#include <QTableView>
#include <QUndoView>
#include <QWidget>

#include "Views/EnvironmentView.h"
#include "Views/ExperimentBaseView.h"
#include "Views/ScenarioView.h"
#include "Views/StatusBarView.h"
#include "Views/TrafficView.h"
#include "openPASS-Window/WindowInterface.h"

namespace Ui {
class TrafficSimulationView;
}

//-----------------------------------------------------------------------------
//! @brief This class constitutes the main view of the Traffic Simulation Plugin and
//!        instantiates all other configuration views as child widgets.
//!
//!        It contains an #ui object describing the views appearence (defined in TrafficSimulation.ui).
//!        This ui object comprises a tab widget for the experiment, scenario, environments and traffic
//!        views. These tabs contain scroll areas in which a resizable container widget will be put for the
//!        particular configuration widget. Further, the action menu and info bar are instantiated.
//-----------------------------------------------------------------------------
class TrafficSimulationView : public QWidget
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! Constructor which initialises all attributes, creates the #ui object as well
    //! as the actions for the action menu and the status bar. Further, all the tab widget
    //! contents for editing the experiment, scenario and traffic are created.
    //!
    //! @param[in]      window      pointer to the window interface in order to register the view
    //! @param[in]      parent      a parent widget
    //-----------------------------------------------------------------------------
    explicit TrafficSimulationView(WindowInterface *const window,
                                   QWidget *parent = nullptr);
    ~TrafficSimulationView() override;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted when the current tab has been changed. Transmits the
    //! index of the tab.
    //-----------------------------------------------------------------------------
    void currentTabChanged(int index);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the new-button in the action menu is clicked.
    //-----------------------------------------------------------------------------
    void NewSetup();

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the save-button in the action menu has been clicked.
    //-----------------------------------------------------------------------------
    void SaveSetup();

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the load-button in the action menu has been clicked.
    //-----------------------------------------------------------------------------
    void LoadSetup();

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the undo-button in the action menu has been clicked.
    //-----------------------------------------------------------------------------
    void Undo();

    //-----------------------------------------------------------------------------
    //! Signal, emitted after the redo-button in the action menu has been clicked.
    //-----------------------------------------------------------------------------
    void Redo();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the profiles catalogue has been modified by the user.
    //!
    //! @param[in]      filename        the new filename of the profiles catalogue
    //-----------------------------------------------------------------------------
    void profileCatalogueChanged();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the refresh button of the profile catalogue has been
    //! clicked by the user. Will lead to an update of the list of available agent
    //! profiles according to the currently displayed profile catalogue.
    //-----------------------------------------------------------------------------
    void refreshCatalogue();

public Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot, activated when the current tab has changed. Will emit the
    //! #currentTabChanged signal transmitting the new tab index.
    //!
    //! @param[in]      index        the new tab index
    //-----------------------------------------------------------------------------
    void on_tabWidget_currentChanged(int index);

public:
    //-----------------------------------------------------------------------------
    //! Method for adjusting the geometry and positioning of child widgets according
    //! to the geometry of this view.
    //-----------------------------------------------------------------------------
    void adjustWidgets();

public:
    //-----------------------------------------------------------------------------
    //! Method that returns the experiment configuration widget (instance of
    //! ExperimentView).
    //!
    //! @return             the experiment configuration widget
    //-----------------------------------------------------------------------------
    ExperimentView *getExperimentView();

    //-----------------------------------------------------------------------------
    //! Method that returns the environment configuration widget (instance of
    //! EnvironmentView).
    //!
    //! @return             the environment configuration widget
    //-----------------------------------------------------------------------------
    EnvironmentView *getEnvironmentView();

    //-----------------------------------------------------------------------------
    //! Method that returns the scenario configuration widget (instance of
    //! ScenarioView).
    //!
    //! @return             the scenario configuration widget
    //-----------------------------------------------------------------------------
    ScenarioView *getScenarioView();

    //-----------------------------------------------------------------------------
    //! Method that returns the traffic configuration widget (instance of
    //! TrafficView).
    //!
    //! @return             the traffic configuration widget
    //-----------------------------------------------------------------------------
    TrafficView *getTrafficView();

    //-----------------------------------------------------------------------------
    //! Method that returns the undo view.
    //!
    //! @return             the undo view
    //-----------------------------------------------------------------------------
    QUndoView *getUndoView();

public:
    //-----------------------------------------------------------------------------
    //! Method that returns the environment display widget (instance of
    //! EnvironmentDisplayView).
    //!
    //! @return             the environment display widget
    //-----------------------------------------------------------------------------
    EnvironmentDisplayView *getEnvironmentDisplayView();

    //-----------------------------------------------------------------------------
    //! Method that returns the scenario display widget (instance of
    //! ScenarioDisplayView).
    //!
    //! @return             the scenario display widget
    //-----------------------------------------------------------------------------
    ScenarioDisplayView *getScenarioDisplayView();

    //-----------------------------------------------------------------------------
    //! Method that returns the traffic display widget (instance of
    //! TrafficDisplayView).
    //!
    //! @return             the traffic display widget
    //-----------------------------------------------------------------------------
    TrafficDisplayView *getTrafficDisplayView();

protected:
    //-----------------------------------------------------------------------------
    //! Reimplementation of the resize event. Triggers #adjustWidgets in order to
    //! adjust the geometry of the child widgets when this view has been resized.
    //!
    //! @param[in]      event       the resize event (instance of Qt's QResizeEvent class)
    //-----------------------------------------------------------------------------
    virtual void resizeEvent(QResizeEvent *event) override;

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the new-button of the action menu has been clicked.
    //! Shows a warning dialog box making the user aware of the fact that the worksheet
    //! will be deleted when continuing. When the user clickes "yes", the NewSetup signal
    //! is emitted.
    //-----------------------------------------------------------------------------
    void actionNew();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the save-button of the action menu has been clicked.
    //! Prompts a file dialog box to let the user select a filepath. Afterwards the signal
    //! SaveSetup is emitted transmitting the selected filepath.
    //-----------------------------------------------------------------------------
    void actionSave();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the load-button of the action menu has been clicked.
    //! Prompts a file dialog box to let the user select a filepath. Afterwards the signal
    //! LoadSetup is emitted transmitting the selected filepath.
    //-----------------------------------------------------------------------------
    void actionLoad();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the undo-button of the action menu has been clicked.
    //! Will trigger the undo of the previous action.
    //-----------------------------------------------------------------------------
    void actionUndo();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the redo-button of the action menu has been clicked.
    //! Will trigger the redo of the previously undone action.
    //-----------------------------------------------------------------------------
    void actionRedo();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the command history button is clicked. Will
    //! show or hide the command history.
    //-----------------------------------------------------------------------------
    void actionHistory();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when Browse-button next to the profile catalogue's
    //! line edit is clicked. It will open a dialog box in which the user can select a
    //! profile catalogue from the file system. If the user has made a valid
    //! selection, the filename will be transmitted to the presenter object
    //! (TrafficSimulationPresenter) via the signal #profileCatalogueChanged.
    //-----------------------------------------------------------------------------
    void on_CatalogueBrowse_clicked();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the Refresh button of the profiles catalogue
    //! section is clicked. Will emit the #refreshCatalogue signal.
    //-----------------------------------------------------------------------------
    void on_CatalogueRefresh_clicked();

public:
    //-----------------------------------------------------------------------------
    //! Method which shows the text message passed as a first argument in the info bar
    //! in the color code specified as a second argument.
    //-----------------------------------------------------------------------------
    void showStatusBarText(QString text, StatusBarView::Color color);

    //-----------------------------------------------------------------------------
    //! Method which hides the info bar.
    //-----------------------------------------------------------------------------
    void hideStatusBar();

    //-----------------------------------------------------------------------------
    //! Method which sets the filename displayed in the profile catalogue's line
    //! edit to the one passed as an argument.
    //!
    //! @param[in]      filename        the filename to be displayed for the profile
    //!                                 catalogue
    //-----------------------------------------------------------------------------
    void setProfileCatalogueView(QString const &filename);

    //-----------------------------------------------------------------------------
    //! Method which sets an error message next the profile catalogue's line edit.
    //!
    //! @param[in]      message        the error message to be displayed
    //-----------------------------------------------------------------------------
    void setProfileCatalogueError(QString const &message);

private:
    WindowInterface *const window;           //!< pointer to the window interface for registering this view
    static const WindowInterface::ID ViewID; //!< the view id under which this view is registered in the window interface
    QUndoView *const undoView;               //!< the view of the undo stack

private:
    WindowInterface::Action *const actionMenuNew;     //!< The action menu object for the creating a new worksheet
    WindowInterface::Action *const actionMenuLoad;    //!< The action menu object for loading a simulationConfig file
    WindowInterface::Action *const actionMenuSave;    //!< The action menu object for saving a simulationConfig file
    WindowInterface::Action *const actionMenuUndo;    //!< The action menu object for undoing actions
    WindowInterface::Action *const actionMenuRedo;    //!< The action menu object for redoing actions
    WindowInterface::Action *const actionMenuHistory; //!< The action menu object for showing the command history

private:
    Ui::TrafficSimulationView *ui; //!< The ui object defining the components of the view
    StatusBarView *statusbar;      //!< the info bar object

private:
    EnvironmentView *environmentView; //!< the view of the environment configuration
    ScenarioView *scenarioView;       //!< the view of the scenario settings
    TrafficView *trafficView;         //!< the view of the traffic configuration

private:
    ExperimentBaseView *experimentBaseView; //!< The container widget for the experiment configuration (to be put into the scroll area)
};

#endif // TRAFFICSIMULATIONVIEW_H
