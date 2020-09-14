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
//! @file  TrafficView.h
//! @ingroup trafficSimulationPlugin
//! @brief This class provides an editor for configuring the traffic setup, i.e.
//!        it is filled with the content stored in an TrafficInterface instance.
//!
//!        It contains an #ui object describing the views appearance (defined in TrafficView.ui)
//!        and GUI elements. Particularly, it comprises table views (see Qt's QTableView class)
//!        for editing and displaying the maps of traffic parameters. User action will lead to
//!        signal emissions which are noticed and managed by the corresponding presenter class,
//!        i.e. TrafficPresenter.
//-----------------------------------------------------------------------------
#ifndef TRAFFICVIEW_H
#define TRAFFICVIEW_H

#include <QTableView>
#include <QWidget>

namespace Ui {
class TrafficView;
}

//-----------------------------------------------------------------------------
//! @brief This class provides an editor for configuring the traffic setup, i.e.
//!        it is filled with the content stored in an TrafficInterface instance.
//!
//!        It contains an #ui object describing the views appearance (defined in TrafficView.ui)
//!        and GUI elements. Particularly, it comprises table views (see Qt's QTableView class)
//!        for editing and displaying the maps of traffic parameters. User action will lead to
//!        signal emissions which are noticed and managed by the corresponding presenter class,
//!        i.e. TrafficPresenter.
//-----------------------------------------------------------------------------
class TrafficView : public QWidget
{
    Q_OBJECT

public:
    explicit TrafficView(QWidget *parent = nullptr);
    ~TrafficView();

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal emitted when the name of the traffic has been modified by the user.
    //! Transmits the new name to the presenter.
    //-----------------------------------------------------------------------------
    void modifiedName(QString const &_name);

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal emitted when add-button above the traffic volume table is clicked.
    //-----------------------------------------------------------------------------
    void addVolume();

    //-----------------------------------------------------------------------------
    //! Signal emitted when remove-button above the traffic volume table is clicked.
    //! Transmits the currently selected row index to the presenter (TrafficPresenter).
    //!
    //! @param[in]      row     the currently selected row
    //-----------------------------------------------------------------------------
    void removeVolume(const int &row);

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal emitted when add-button above the platoon rates table is clicked.
    //-----------------------------------------------------------------------------
    void addPlatoonRate();

    //-----------------------------------------------------------------------------
    //! Signal emitted when remove-button above the platoon rates table is clicked.
    //! Transmits the currently selected row index to the presenter (TrafficPresenter).
    //!
    //! @param[in]      row     the currently selected row
    //-----------------------------------------------------------------------------
    void removePlatoonRate(const int &row);

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal emitted when add-button above the velocities table is clicked.
    //-----------------------------------------------------------------------------
    void addVelocity();

    //-----------------------------------------------------------------------------
    //! Signal emitted when remove-button above the velocities table is clicked.
    //! Transmits the currently selected row index to the presenter (TrafficPresenter).
    //!
    //! @param[in]      row     the currently selected row
    //-----------------------------------------------------------------------------
    void removeVelocity(const int &row);

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal emitted when add-button above the homogeneities table is clicked.
    //-----------------------------------------------------------------------------
    void addHomogeneity();

    //-----------------------------------------------------------------------------
    //! Signal emitted when remove-button above the homogeneities table is clicked.
    //! Transmits the currently selected row index to the presenter (TrafficPresenter).
    //!
    //! @param[in]      row     the currently selected row
    //-----------------------------------------------------------------------------
    void removeHomogeneity(const int &row);

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal emitted when add-button above the regular lane agents table is clicked.
    //-----------------------------------------------------------------------------
    void addRegularLaneAgent();

    //-----------------------------------------------------------------------------
    //! Signal emitted when remove-button above the regular lane agents table is clicked.
    //! Transmits the currently selected row index to the presenter (TrafficPresenter).
    //!
    //! @param[in]      row     the currently selected row
    //-----------------------------------------------------------------------------
    void removeRegularLaneAgent(const int &row);

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal emitted when add-button above the rigthmost lane agents table is
    //! clicked.
    //-----------------------------------------------------------------------------
    void addRightmostLaneAgent();

    //-----------------------------------------------------------------------------
    //! Signal emitted when remove-button above the rightmost lane agents table is
    //! clicked. Transmits the currently selected row index to the presenter
    //! (TrafficPresenter).
    //!
    //! @param[in]      row     the currently selected row
    //-----------------------------------------------------------------------------
    void removeRightmostLaneAgent(const int &row);

public:
    //-----------------------------------------------------------------------------
    //! Method which sets the traffic configuration's name to the one passed as an
    //! argument.
    //!
    //! @param[in]      _name       the new name for the traffic configuration
    //-----------------------------------------------------------------------------
    void setName(QString const &_name);

public:
    //-----------------------------------------------------------------------------
    //! Method which provides access to the table view displaying traffic volumes.
    //!
    //! @return             a pointer to the traffic volumes table view
    //-----------------------------------------------------------------------------
    QTableView *getVolumesView() const;

    //-----------------------------------------------------------------------------
    //! Method which provides access to the table view displaying platoon rates.
    //!
    //! @return             a pointer to the platoon rates table view
    //-----------------------------------------------------------------------------
    QTableView *getPlatoonRatesView() const;

    //-----------------------------------------------------------------------------
    //! Method which provides access to the table view displaying velocities.
    //!
    //! @return             a pointer to the velocities table view
    //-----------------------------------------------------------------------------
    QTableView *getVelocitiesView() const;

    //-----------------------------------------------------------------------------
    //! Method which provides access to the table view displaying homogeneities.
    //!
    //! @return             a pointer to the homogeneities table view
    //-----------------------------------------------------------------------------
    QTableView *getHomogeneitiesView() const;

public:
    //-----------------------------------------------------------------------------
    //! Method which provides access to the table view displaying regular lane
    //! agents.
    //!
    //! @return             a pointer to the regular lane agents table view
    //-----------------------------------------------------------------------------
    QTableView *getRegularLaneAgentsView() const;

    //-----------------------------------------------------------------------------
    //! Method which provides access to the table view displaying rightmost lane
    //! agents.
    //!
    //! @return             a pointer to the rightmost lane agents table view
    //-----------------------------------------------------------------------------
    QTableView *getRightmostLaneAgentsView() const;

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the user has finished editing the traffic
    //! configuration's name. Will emit the #modifiedName signal transmitting
    //! the new name to the corresponding presenter object.
    //-----------------------------------------------------------------------------
    void on_NameEdit_editingFinished();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the user clicks on the add-button above the
    //! traffic volumes table. Will emit the #addVolume signal.
    //-----------------------------------------------------------------------------
    void on_addVolume_clicked();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the user clicks the remove-button of the traffic volumes
    //! table. Will emit the #removeVolume signal transmitting the currently selected
    //! row index of the table.
    //-----------------------------------------------------------------------------
    void on_removeVolume_clicked();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the user clicks on the add-button above the
    //! platoon rate table. Will emit the #addVolume signal.
    //-----------------------------------------------------------------------------
    void on_addPlatoonRate_clicked();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the user clicks the remove-button of the platoon rates
    //! table. Will emit the #removePlatoonRate signal transmitting the currently selected
    //! row index of the table.
    //-----------------------------------------------------------------------------
    void on_removePlatoonRate_clicked();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the user clicks on the add-button above the
    //! velocities table. Will emit the #addVelocity signal.
    //-----------------------------------------------------------------------------
    void on_addVelocity_clicked();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the user clicks the remove-button of the velocities
    //! table. Will emit the #removeVelocity signal transmitting the currently
    //! selected row index of the table.
    //-----------------------------------------------------------------------------
    void on_removeVelocity_clicked();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the user clicks on the add-button above the
    //! homogeneities table. Will emit the #addHomogeneity signal.
    //-----------------------------------------------------------------------------
    void on_addHomogeneity_clicked();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the user clicks the remove-button of the homogeneities
    //! table. Will emit the #removeHomogeneity signal transmitting the currently
    //! selected row index of the table.
    //-----------------------------------------------------------------------------
    void on_removeHomogeneity_clicked();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the user clicks on the add-button above the
    //! regular lane agents table. Will emit the #addRegularLaneAgent signal.
    //-----------------------------------------------------------------------------
    void on_addRegularLaneAgent_clicked();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the user clicks the remove-button of the regular
    //! lane agents table. Will emit the #removeRegularLaneAgent signal transmitting
    //! the currently selected row index of the table.
    //-----------------------------------------------------------------------------
    void on_removeRegularLaneAgent_clicked();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the user clicks on the add-button above the
    //! rightmost lane agents table. Will emit the #addRightmostLaneAgent signal.
    //-----------------------------------------------------------------------------
    void on_addRightmostLaneAgent_clicked();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the user clicks the remove-button of the rightmost
    //! lane agents table. Will emit the #removeRightmostLaneAgent signal
    //! transmitting the currently selected row index of the table.
    //-----------------------------------------------------------------------------
    void on_removeRightmostLaneAgent_clicked();

public:
    //-----------------------------------------------------------------------------
    //! Method which displays a probability error message for the table of traffic
    //! volumes if the boolian argument is "true".
    //!
    //! @param[in]      error       the status whether the probabilities are
    //!                             erroneous (true) or not (false)
    //-----------------------------------------------------------------------------
    void setVolumes_ProbabilityError(bool error);

    //-----------------------------------------------------------------------------
    //! Method which displays a probability error message for the table of platoon
    //! rates if the boolian argument is "true".
    //!
    //! @param[in]      error       the status whether the probabilities are
    //!                             erroneous (true) or not (false)
    //-----------------------------------------------------------------------------
    void setPlatoonRates_ProbabilityError(bool error);

    //-----------------------------------------------------------------------------
    //! Method which displays a probability error message for the table of
    //! velocities if the boolian argument is "true".
    //!
    //! @param[in]      error       the status whether the probabilities are
    //!                             erroneous (true) or not (false)
    //-----------------------------------------------------------------------------
    void setVelocities_ProbabilityError(bool error);

    //-----------------------------------------------------------------------------
    //! Method which displays a probability error message for the table of
    //! homogeneities if the boolian argument is "true".
    //!
    //! @param[in]      error       the status whether the probabilities are
    //!                             erroneous (true) or not (false)
    //-----------------------------------------------------------------------------
    void setHomogeneities_ProbabilityError(bool error);

public:
    //-----------------------------------------------------------------------------
    //! Method which displays a an error message concerning agent profiles.
    //!
    //! @param[in]      message       the error message to be displayed
    //-----------------------------------------------------------------------------
    void setAgentProfilesError(QString const &message);

    //-----------------------------------------------------------------------------
    //! Method which displays a probability error message for the table of
    //! regular lane agents if the boolian argument is "true".
    //!
    //! @param[in]      error       the status whether the probabilities are
    //!                             erroneous (true) or not (false)
    //-----------------------------------------------------------------------------
    void setRegularLane_ProbabilityError(bool error);

    //-----------------------------------------------------------------------------
    //! Method which displays a probability error message for the table of
    //! rightmost lane agents if the boolian argument is "true".
    //!
    //! @param[in]      error       the status whether the probabilities are
    //!                             erroneous (true) or not (false)
    //-----------------------------------------------------------------------------
    void setRightmostLane_ProbabilityError(bool error);

private:
    //-----------------------------------------------------------------------------
    //! Reimplementation of the event filter. This filter catches the resize events
    //! of the table views in order to resize their columns properly.
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
    Ui::TrafficView *ui; //!< the ui object containing the GUI elements of this view
};

#endif // TRAFFICVIEW_H
