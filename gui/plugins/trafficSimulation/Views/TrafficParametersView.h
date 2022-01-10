/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
//-----------------------------------------------------------------------------
//! @file  TrafficParametersView.h
//! @ingroup trafficSimulationPlugin
//! @brief This class defines the editor widget for traffic parameters and their probabilities.
//!
//!        It consists of an ui element (TrafficParametersView.ui) with four list sections for
//!        traffic volumes, platoon rates, velocities and homogeneities. In these list sections,
//!        the editor views for the particular traffic items (instances of TrafficItemView) can be
//!        inserted upon pressing the corresponding add buttons.
//-----------------------------------------------------------------------------
#ifndef TRAFFICPARAMETERSVIEW_H
#define TRAFFICPARAMETERSVIEW_H

#include "TrafficItemView.h"
#include "WidgetView.h"

namespace Ui {
class TrafficParametersView;
}

//-----------------------------------------------------------------------------
//! @brief This class defines the editor widget for traffic parameters and their probabilities.
//!
//!        It constitutes an ui element (TrafficParametersView.ui) with four list sections for
//!        traffic volumes, platoon rates, velocities and homogeneities. In these list sections,
//!        the visualization of the particular traffic items (instances of TrafficItemView) can be
//!        inserted upon pressing the corresponding add buttons.
//-----------------------------------------------------------------------------
class TrafficParametersView : public WidgetView
{
    Q_OBJECT

public:
    explicit TrafficParametersView(QWidget *parent = 0);
    ~TrafficParametersView();

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted when this view has been resized.
    //-----------------------------------------------------------------------------
    void resized();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the add-button in the traffic volumes section has been
    //! pressed.
    //-----------------------------------------------------------------------------
    void addVolume();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the add-button in the platoon rates section has been
    //! pressed.
    //-----------------------------------------------------------------------------
    void addPlatoonRate();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the add-button in the velocities section has been
    //! pressed.
    //-----------------------------------------------------------------------------
    void addVelocity();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the add-button in the homogeneities section has been
    //! pressed.
    //-----------------------------------------------------------------------------
    void addHomogeneity();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the probability of a traffic item has been modified.
    //! Transmits the ID, the new probability value and the type of the
    //! traffic item.
    //-----------------------------------------------------------------------------
    void probabilityChanged(unsigned int ID, double value, TrafficItemView::Type type);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the value of a traffic item has been modified.
    //! Transmits the ID, the new value and the type of the
    //! traffic item.
    //-----------------------------------------------------------------------------
    void valueChanged(unsigned int ID, QString value, TrafficItemView::Type type);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the remove button of a traffic parameter has been clicked.
    //! Transmits the ID and the type of the traffic item.
    //-----------------------------------------------------------------------------
    void remove(unsigned int ID, TrafficItemView::Type type);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the add-button in the traffic volumes
    //! section has been clicked. Will emit the #addVolume signal.
    //-----------------------------------------------------------------------------
    void on_addVolume_clicked();

    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the add-button in the platoon rates
    //! section has been clicked. Will emit the #addPlatoonRate signal.
    //-----------------------------------------------------------------------------
    void on_addPlatoonRate_clicked();

    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the add-button in the velocities
    //! section has been clicked. Will emit the #addVelocity signal.
    //-----------------------------------------------------------------------------
    void on_addVelocity_clicked();

    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the add-button in the homogeneities
    //! section has been clicked. Will emit the #addHomogeneity signal.
    //-----------------------------------------------------------------------------
    void on_addHomogeneity_clicked();

public:
    //-----------------------------------------------------------------------------
    //! Method which creates a view of a traffic volume using the given ID and inserts
    //! the view into the Volumes section. Sets the display values of value and probability
    //! to the second and third arguments, respectively. Also connects the remove,
    //! valueChanged and probabilityChanged signals of TrafficItemView to the signals
    //! within this class such that they can be forwarded to the presenter. The whole
    //! editor widget is resized according to the required size of the item view.
    //!
    //! @param[in]      id              the ID of the view
    //! @param[in]      value           the value to be displayed (as a string)
    //! @param[in]      probability     the probability to be displayed
    //-----------------------------------------------------------------------------
    void addVolumeView(unsigned int id, QString value, double probability);

    //-----------------------------------------------------------------------------
    //! Method which creates a view of a platoon rate using the given ID and inserts
    //! the view into the Platoon Rates section. Sets the display values of value and probability
    //! to the second and third arguments, respectively. Also connects the remove,
    //! valueChanged and probabilityChanged signals of TrafficItemView to the signals
    //! within this class such that they can be forwarded to the presenter. The whole
    //! editor widget is resized according to the required size of the item view.
    //!
    //! @param[in]      id              the ID of the view
    //! @param[in]      value           the value to be displayed (as a string)
    //! @param[in]      probability     the probability to be displayed
    //-----------------------------------------------------------------------------
    void addPlatoonRateView(unsigned int id, QString value, double probability);

    //-----------------------------------------------------------------------------
    //! Method which creates a view of a velocity using the given ID and inserts
    //! the view into the Velocities section. Sets the display values of value and probability
    //! to the second and third arguments, respectively. Also connects the remove,
    //! valueChanged and probabilityChanged signals of TrafficItemView to the signals
    //! within this class such that they can be forwarded to the presenter. The whole
    //! editor widget is resized according to the required size of the item view.
    //!
    //! @param[in]      id              the ID of the view
    //! @param[in]      value           the value to be displayed (as a string)
    //! @param[in]      probability     the probability to be displayed
    //-----------------------------------------------------------------------------
    void addVelocityView(unsigned int id, QString value, double probability);

    //-----------------------------------------------------------------------------
    //! Method which creates a view of a homogeneity using the given ID and inserts
    //! the view into the Homogeneities section. Sets the display values of value and probability
    //! to the second and third arguments, respectively. Also connects the remove,
    //! valueChanged and probabilityChanged signals of TrafficItemView to the signals
    //! within this class such that they can be forwarded to the presenter. The whole
    //! editor widget is resized according to the required size of the item view.
    //!
    //! @param[in]      id              the ID of the view
    //! @param[in]      value           the value to be displayed (as a string)
    //! @param[in]      probability     the probability to be displayed
    //-----------------------------------------------------------------------------
    void addHomogeneityView(unsigned int id, QString value, double probability);

    //-----------------------------------------------------------------------------
    //! Method which removes the view of a traffic item with the given ID and type.
    //! All item views lying below the removed one will be moved on place up in order
    //! to fill the gap. Afterwards, the whole editor widget is resized.
    //!
    //! @param[in]      id              the ID of the view
    //! @param[in]      value           the value to be displayed (as a string)
    //! @param[in]      probability     the probability to be displayed
    //-----------------------------------------------------------------------------
    void removeTrafficItemView(unsigned int id, TrafficItemView::Type type);

    //-----------------------------------------------------------------------------
    //! Method which deletes all registered traffic parameter views and resizes
    //! the this editor widget to its initial size.
    //-----------------------------------------------------------------------------
    void clearTrafficParametersView();

public:
    //-----------------------------------------------------------------------------
    //! Method which sets the displayed probability value of the traffic parameter
    //! with the given id and type to the value passed as a third argument.
    //!
    //! @param[in]      id      the ID of the traffic parameter
    //! @param[in]      type    the type of the traffic parameter
    //! @param[in]      value   the probability value to be displayed
    //-----------------------------------------------------------------------------
    void setProbability(unsigned int id, TrafficItemView::Type type, double value);

    //-----------------------------------------------------------------------------
    //! Method which sets the displayed value of the traffic parameter
    //! with the given id and type to the value passed as a third argument.
    //!
    //! @param[in]      id      the ID of the traffic parameter
    //! @param[in]      type    the type of the traffic parameter
    //! @param[in]      value   the value to be displayed
    //-----------------------------------------------------------------------------
    void setValue(unsigned int id, TrafficItemView::Type type, QString value);

private:
    //-----------------------------------------------------------------------------
    //! Reimplementation of the resize event. Emits the #resized() signal.
    //!
    //! @param[in]      event       the resize event (Qt's QResizeEvent class)
    //-----------------------------------------------------------------------------
    void resizeEvent(QResizeEvent *event);

private:
    Ui::TrafficParametersView *ui; //!< the ui object containing all visualization elements of this editor widget
    int initialHeight;             //!< the initial height of this editor widget
};

#endif // TRAFFICPARAMETERSVIEW_H
