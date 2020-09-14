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
//! @file  EnvironmentView.h
//! @ingroup trafficSimulationPlugin
//! @brief This class provides an editor for configuring the environment setup, i.e.
//!        it is filled with the content stored in an EnvironmentInterface instance.
//!
//!        It contains an #ui object describing the views appearance (defined in EnvironmentView.ui)
//!        and GUI elements. Particularly, it comprises table views (see Qt's QTableView class)
//!        for editing and displaying the maps of environment parameters. User action will lead to
//!        signal emissions which are noticed and managed by the corresponding presenter class,
//!        i.e. EnvironmentPresenter.
//-----------------------------------------------------------------------------
#ifndef ENVIRONMENTVIEW_H
#define ENVIRONMENTVIEW_H

#include <QTableView>
#include <QWidget>

namespace Ui {
class EnvironmentView;
}

//-----------------------------------------------------------------------------
//! @brief This class provides an editor for configuring the environment setup, i.e.
//!        it is filled with the content stored in an EnvironmentInterface instance.
//!
//!        It contains an #ui object describing the views appearance (defined in Environment.ui).
//!        and GUI elements. Particularly, it comprises table views (see Qt's QTableView class)
//!        for editing and displaying the maps of environment parameters. User action will lead to
//!        signal emissions which are noticed and managed by the corresponding presenter class,
//!        i.e. EnvironmentPresenter.
//-----------------------------------------------------------------------------
class EnvironmentView : public QWidget
{
    Q_OBJECT

public:
    explicit EnvironmentView(QWidget *parent = nullptr);
    ~EnvironmentView();

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted when the user edits the name of the environment configuration.
    //! Transmits the new name the user has typed in.
    //!
    //! @param[in]      name        the new name, the user has typed in the line edit
    //-----------------------------------------------------------------------------
    void modifiedName(const QString &name);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the user clicks the add-button of the time of day table.
    //-----------------------------------------------------------------------------
    void addTimeOfDay();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the user clicks the remove-button of the time of day
    //! table. Transmits the row index of the table cell being currently active.
    //!
    //! @param[in]      row        the row index of the table cell being currently
    //!                            active
    //-----------------------------------------------------------------------------
    void removeTimeOfDay(const int &row);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the user clicks the add-button of the table of visibility
    //! distances.
    //-----------------------------------------------------------------------------
    void addVisibilityDistance();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the user clicks the remove-button of the table of
    //! visibility distances. Transmits the row index of the table cell being
    //! currently active.
    //!
    //! @param[in]      row        the row index of the table cell being currently
    //!                            active
    //-----------------------------------------------------------------------------
    void removeVisibilityDistance(const int &row);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the user clicks the add-button of the table of friction
    //! coefficients.
    //-----------------------------------------------------------------------------
    void addFriction();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the user clicks the remove-button of the table of
    //! friction coefficients. Transmits the row index of the table cell being
    //! currently active.
    //!
    //! @param[in]      row        the row index of the table cell being currently
    //!                            active
    //-----------------------------------------------------------------------------
    void removeFriction(const int &row);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the user clicks the add-button of the table of weather
    //! parameters.
    //-----------------------------------------------------------------------------
    void addWeather();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the user clicks the remove-button of the table of
    //! friction coefficients. Transmits the row index of the table cell being
    //! currently active.
    //!
    //! @param[in]      row        the row index of the table cell being currently
    //!                            active
    //-----------------------------------------------------------------------------
    void removeWeather(const int &row);

public:
    //-----------------------------------------------------------------------------
    //! Method which provides access to the table of time of day parameters.
    //!
    //! @return             a pointer to the table of time of day parameters
    //-----------------------------------------------------------------------------
    QTableView *getTimeOfDayView() const;

    //-----------------------------------------------------------------------------
    //! Method which provides access to the table of visibility distances.
    //!
    //! @return             a pointer to the table of visibility distances
    //-----------------------------------------------------------------------------
    QTableView *getVisibilityDistanceView() const;

    //-----------------------------------------------------------------------------
    //! Method which provides access to the table of friction coefficients.
    //!
    //! @return             a pointer to the table of friction coefficients
    //-----------------------------------------------------------------------------
    QTableView *getFrictionView() const;

    //-----------------------------------------------------------------------------
    //! Method which provides access to the table of weather parameters.
    //!
    //! @return             a pointer to the table of weather parameters.
    //-----------------------------------------------------------------------------
    QTableView *getWeatherView() const;

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Qt Designer-created slot which emits the #addTimeOfDay signal when the user
    //! clicks the add_TimeOfDay button.
    //-----------------------------------------------------------------------------
    void on_add_TimeOfDay_clicked();

    //-----------------------------------------------------------------------------
    //! Qt Designer-created slot which emits the #removeTimeOfDay signal when the user
    //! clicks the remove_TimeOfDay button.
    //-----------------------------------------------------------------------------
    void on_remove_TimeOfDay_clicked();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Qt Designer-created slot which emits the #addVisibilityDistance signal when
    //! the user clicks the add_VisibilityDistance button.
    //-----------------------------------------------------------------------------
    void on_add_VisibilityDistance_clicked();

    //-----------------------------------------------------------------------------
    //! Qt Designer-created slot which emits the #removeVisibilityDistance signal when
    //! the user clicks the remove_VisibilityDistance button.
    //-----------------------------------------------------------------------------
    void on_remove_VisibilityDistance_clicked();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Qt Designer-created slot which emits the #addFriction signal when
    //! the user clicks the add_Friction button.
    //-----------------------------------------------------------------------------
    void on_add_Friction_clicked();

    //-----------------------------------------------------------------------------
    //! Qt Designer-created slot which emits the #removeFriction signal when
    //! the user clicks the remove_Friction button.
    //-----------------------------------------------------------------------------
    void on_remove_Friction_clicked();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Qt Designer-created slot which emits the #addWeather signal when
    //! the user clicks the add_Weather button.
    //-----------------------------------------------------------------------------
    void on_add_Weather_clicked();

    //-----------------------------------------------------------------------------
    //! Qt Designer-created slot which emits the #removeWeather signal when
    //! the user clicks the remove_Weather button.
    //-----------------------------------------------------------------------------
    void on_remove_Weather_clicked();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Qt Designer-created slot which emits the #modifiedName signal when
    //! finishes editing the line edit for the environment configuration's name.
    //-----------------------------------------------------------------------------
    void on_Name_editingFinished();

public:
    //-----------------------------------------------------------------------------
    //! Method which sets the current text of the environment name's line edit.
    //!
    //! @param[in]      name        the new name to be displayed by the line edit
    //-----------------------------------------------------------------------------
    void setName(QString const &name);

public:
    //-----------------------------------------------------------------------------
    //! Method which displays a probability error message for the table of time
    //! of day parameters if the boolian argument is "true".
    //!
    //! @param[in]      error       the status whether the probabilities are
    //!                             erroneous (true) or not (false)
    //-----------------------------------------------------------------------------
    void setTimeOfDay_ProbabilityError(bool error);

    //-----------------------------------------------------------------------------
    //! Method which displays a probability error message for the table of visibility
    //! distances if the boolian argument is "true".
    //!
    //! @param[in]      error       the status whether the probabilities are
    //!                             erroneous (true) or not (false)
    //-----------------------------------------------------------------------------
    void setVisibilityDistance_ProbabilityError(bool error);

    //-----------------------------------------------------------------------------
    //! Method which displays a probability error message for the table of friction
    //! coefficients if the boolian argument is "true".
    //!
    //! @param[in]      error       the status whether the probabilities are
    //!                             erroneous (true) or not (false)
    //-----------------------------------------------------------------------------
    void setFriction_ProbabilityError(bool error);

    //-----------------------------------------------------------------------------
    //! Method which displays a probability error message for the table of weather
    //! parameters if the boolian argument is "true".
    //!
    //! @param[in]      error       the status whether the probabilities are
    //!                             erroneous (true) or not (false)
    //-----------------------------------------------------------------------------
    void setWeather_ProbabilityError(bool error);

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
    Ui::EnvironmentView *ui; //!< the ui object which contains the GUI elements
};

#endif // ENVIRONMENTVIEW_H
