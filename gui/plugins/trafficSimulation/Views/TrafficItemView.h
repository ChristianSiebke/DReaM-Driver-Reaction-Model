/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
//-----------------------------------------------------------------------------
//! @file  TrafficItemView.h
//! @ingroup trafficSimulationPlugin
//! @brief This class defines a general view for a traffic item, which can be
//!        a volume, platoon rate, velocity, homogeneity or an agent profile.
//!
//!        A traffic item generally has a value (depending on the type) and a
//!        probability view. The latter is always a double spinbox, whereas the former
//!        constitutes a spinbox if the value is an integer or a double (volumes, platoon
//!        rates, velocites, homogeneities) or a label if the value is a string (agent profile).
//-----------------------------------------------------------------------------
#ifndef TRAFFICITEMVIEW_H
#define TRAFFICITEMVIEW_H

#include "WidgetView.h"

namespace Ui {
class TrafficItemView;
}

//-----------------------------------------------------------------------------
//! @brief This class defines a general view for a traffic item, which can be
//!        a volume, platoon rate, velocity, homogeneity or an agent profile.
//!
//!        A traffic item generally has a value (depending on the type) and a
//!        probability view. The latter is always a double spinbox, whereas the former
//!        constitutes a spinbox if the value is an integer or a double (volumes, platoon
//!        rates, velocites, homogeneities) or a label if the value is a string (agent profile).
//-----------------------------------------------------------------------------
class TrafficItemView : public WidgetView
{
    Q_OBJECT

public:
    enum class Type
    { //!< An enum class defining the type of the traffic item
        Volume = 1,
        PlatoonRate = 2,
        Velocity = 3,
        Homogeneity = 4,
        Agent = 5,
    };

public:
    explicit TrafficItemView(Type type,
                             unsigned int ID,
                             int index,
                             QWidget *parent = 0);

    ~TrafficItemView();

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted when the value of the traffic item has been changed. Transmits
    //! the ID, value and type of the item.
    //-----------------------------------------------------------------------------
    void valueChanged(unsigned int ID, QString value, Type type);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when remove button has been clicked. Transmits the ID and type
    //! of the traffic item.
    //-----------------------------------------------------------------------------
    void remove(unsigned int ID, Type type);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when probability of the traffic item has been modified by
    //! the user. Transmits the ID, probability value and type of the item.
    //-----------------------------------------------------------------------------
    void probabilityChanged(unsigned int ID, double value, Type type);

protected Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the remove button has been clicked.
    //! Emits the #remove signal.
    //-----------------------------------------------------------------------------
    void on_remove_clicked();

    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the probability value of the double
    //! spinbox has been edited by the user. Emits the #probabilityChanged signal.
    //-----------------------------------------------------------------------------
    void on_probabilityValue_editingFinished();

    //-----------------------------------------------------------------------------
    //! A slot which is activated when the value of the traffic item
    //! has been edited by the user. Emits the #valueChanged signal.
    //!
    //! @param[in]      value       the new value of the traffic item
    //-----------------------------------------------------------------------------
    void valueView_valueChanged(QString const &value);

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the ID of the traffic item.
    //!
    //! @return             the ID of the traffic item
    //-----------------------------------------------------------------------------
    unsigned int getID() const;

    //-----------------------------------------------------------------------------
    //! Method which returns the index of the traffic item. Items will be numerated
    //! in the order they appear in the TrafficParametersView or TrafficLanesView.
    //!
    //! @return             the index of the traffic item
    //-----------------------------------------------------------------------------
    int getIndex() const;

    //-----------------------------------------------------------------------------
    //! Method which returns the type of the traffic item. The types are defined by
    //! the enum class #Type.
    //!
    //! @return             the type of the traffic item
    //-----------------------------------------------------------------------------
    Type getType() const;

    //-----------------------------------------------------------------------------
    //! Method which returns the value of the traffic item.
    //!
    //! @return             the value of the traffic item
    //-----------------------------------------------------------------------------
    QString getValue() const;

    //-----------------------------------------------------------------------------
    //! Method which returns the probability of the traffic item.
    //!
    //! @return             the probability of the traffic item
    //-----------------------------------------------------------------------------
    double getProbability() const;

public:
    //-----------------------------------------------------------------------------
    //! Method which sets the displayed value of the traffic item to the one passed as
    //! an argument.
    //!
    //! @param[in]      the new value to be displayed
    //-----------------------------------------------------------------------------
    void setValue(QString value);

    //-----------------------------------------------------------------------------
    //! Method which sets the displayed probability of the traffic item to the one passed as
    //! an argument.
    //!
    //! @param[in]      the new probability to be displayed
    //-----------------------------------------------------------------------------
    void setProbability(double value);

    //-----------------------------------------------------------------------------
    //! Method which sets the displayed index of the traffic item to the one passed as
    //! an argument.
    //!
    //! @param[in]      the new index to be displayed
    //-----------------------------------------------------------------------------
    void setIndex(int newIndex);

private:
    unsigned int ID;         //!< the ID of the traffic item
    int index;               //!< the numeration index of the traffic item (changes, e.g., if traffic items in the list are removed)
    Type type;               //!< the type of the traffic item
    Ui::TrafficItemView *ui; //!< the ui object containing all visualization elements of the traffic item view
};

#endif // TRAFFICITEMVIEW_H
