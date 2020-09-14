//-----------------------------------------------------------------------------
//! @file  TrafficLanesView.h
//! @ingroup trafficSimulationPlugin
//! @brief This class defines the editor widget for assigning agent profiles to
//!        regular and rightmost lanes.
//!
//!        It consists of an ui element (TrafficLanesView.ui) with two sections, one
//!        for listing agent profiles assigned to regular lanes and one for the
//!        rightmost lane. In these sections, the editor views for the agent profiles
//!        and their probabilities of occurence can be inserted upon pressing the
//!        corresponding add-buttons.
//-----------------------------------------------------------------------------

#ifndef TRAFFICLANESVIEW_H
#define TRAFFICLANESVIEW_H

#include "WidgetView.h"

namespace Ui {
class TrafficLanesView;
}

//-----------------------------------------------------------------------------
//! @brief This class defines the editor widget for assigning agent profiles to
//!        regular and rightmost lanes.
//!
//!        It consists of an ui element (TrafficLanesView.ui) with two sections, one
//!        for listing agent profiles assigned to regular lanes and one for the
//!        rightmost lane. In these sections, the editor views for the agent profiles
//!        and their probabilities of occurence can be inserted upon pressing the
//!        corresponding add-buttons.
//-----------------------------------------------------------------------------
class TrafficLanesView : public WidgetView
{
    Q_OBJECT

public:
    explicit TrafficLanesView(QWidget *parent = 0);
    ~TrafficLanesView();

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted when this view is resized.
    //-----------------------------------------------------------------------------
    void resized();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the add button in the rightmost lane agents section
    //! is clicked. Transmits the name of the agent profile currently selected in
    //! the combobox.
    //-----------------------------------------------------------------------------
    void addRightmostLaneAgent(QString const &name);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the add button in the regula lane agents section
    //! is clicked. Transmits the name of the agent profile currently selected in
    //! the combobox.
    //-----------------------------------------------------------------------------
    void addRegularLaneAgent(QString const &name);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the remove button of a particular rightmost lane agent
    //! profile is clicked. Transmits the ID of the profile.
    //-----------------------------------------------------------------------------
    void removeRightmostLaneAgent(unsigned int id);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the remove button of a particular regular lane agent
    //! profile is clicked. Transmits the ID of the profile.
    //-----------------------------------------------------------------------------
    void removeRegularLaneAgent(unsigned int id);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the probability of a particular regular lane agent
    //! is modified by the user. Transmits the ID and the new probability displayed in
    //! the spinbox.
    //-----------------------------------------------------------------------------
    void RegularLaneAgent_probabilityChanged(unsigned int id, double value);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the probability of a particular rightmost lane agent
    //! is modified by the user. Transmits the ID and the new probability displayed in
    //! the spinbox.
    //-----------------------------------------------------------------------------
    void RightmostLaneAgent_probabilityChanged(unsigned int id, double value);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the add-button in the rightmost lane
    //! section is clicked. Emits the #addRightmostLaneAgent signal.
    //-----------------------------------------------------------------------------
    void on_addRightmostLaneAgent_clicked();

    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the add-button in the regula lanes
    //! section is clicked. Emits the #addRegularLaneAgent signal.
    //-----------------------------------------------------------------------------
    void on_addRegularLaneAgent_clicked();

public:
    //-----------------------------------------------------------------------------
    //! Method which fills the comboboxes for agent profile selections with the string
    //! list passed as an argument.
    //!
    //! @param[in]      items       the string list of selectable items to be offered
    //!                             by the comboboxes
    //-----------------------------------------------------------------------------
    void setAgentSelectionItems(const QStringList &items);

public:
    //-----------------------------------------------------------------------------
    //! Method which creates an editor view for a regular lane agent profile and
    //! inserts it into the corresponding regular lane section using the ID passed
    //! as a first argument. The name and the probability value to be displayed are
    //! passed as second and third arguments, respectively. The entire view (this class)
    //! is resized accordingly.
    //!
    //! @param[in]      id              the ID of the view
    //! @param[in]      name            the displayed name of the profile
    //! @param[in]      probability     the probability value to be displayed
    //-----------------------------------------------------------------------------
    void addRegularLaneAgentView(unsigned int id, QString name, double probability);

    //-----------------------------------------------------------------------------
    //! Method which creates an editor view for a rightmost lane agent profile and
    //! inserts it into the corresponding rightmost lane section using the ID passed
    //! as a first argument. The name and the probability value to be displayed are
    //! passed as second and third arguments, respectively. The entire view (this class)
    //! is resized accordingly.
    //!
    //! @param[in]      id              the ID of the view
    //! @param[in]      name            the displayed name of the profile
    //! @param[in]      probability     the probability value to be displayed
    //-----------------------------------------------------------------------------
    void addRightmostLaneAgentView(unsigned int id, QString name, double probability);

    //-----------------------------------------------------------------------------
    //! Method which removes the editor view for a regular lane agent profile specified
    //! by the ID passed as an argument. The entire view (this class) is resized accordingly
    //! and the enumeration of the remaining views is adjusted.
    //!
    //! @param[in]      id              the ID of the view to be removed
    //-----------------------------------------------------------------------------
    void removeRegularLaneAgentView(unsigned int id);

    //-----------------------------------------------------------------------------
    //! Method which removes the editor view for a rightmost lane agent profile specified
    //! by the ID passed as an argument. The entire view (this class) is resized accordingly
    //! and the enumeration of the remaining views is adjusted.
    //!
    //! @param[in]      id              the ID of the view to be removed
    //-----------------------------------------------------------------------------
    void removeRightmostLaneAgentView(unsigned int id);

    //-----------------------------------------------------------------------------
    //! Method which deletes all existing editor views for both, regular and rightmost lanes.
    //! Resizes the entire view (this class) to its initial size.
    //-----------------------------------------------------------------------------
    void clearTrafficLanesView();

public:
    //-----------------------------------------------------------------------------
    //! Method which sets the display value of the probability for the regular lane
    //! agent profile specified by the ID passed as a first argument. The new value
    //! is passed as a second argument.
    //!
    //! @param[in]      id      the ID of the regular lane agent editor view
    //! @param[in]      value   the probability value to be displayed
    //-----------------------------------------------------------------------------
    void setRegularLaneAgentProbability(unsigned int id, double value);

    //-----------------------------------------------------------------------------
    //! Method which sets the display value of the probability for the rightmost lane
    //! agent profile specified by the ID passed as a first argument. The new value
    //! is passed as a second argument.
    //!
    //! @param[in]      id      the ID of the rightmost lane agent editor view
    //! @param[in]      value   the probability value to be displayed
    //-----------------------------------------------------------------------------
    void setRightmostLaneAgentProbability(unsigned int id, double value);

private:
    //-----------------------------------------------------------------------------
    //! Reimplementation of the resize event. Emits the #resized signal when this
    //! view is resized.
    //-----------------------------------------------------------------------------
    void resizeEvent(QResizeEvent *event);

private:
    Ui::TrafficLanesView *ui; //!< The ui object containing all visualization elements
    int initialHeight;        //!< The initial height (empty form) of this view
};

#endif // TRAFFICLANESVIEW_H
