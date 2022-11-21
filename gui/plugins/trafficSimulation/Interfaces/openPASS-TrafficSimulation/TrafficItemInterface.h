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
//! @file  TrafficItemInterface.h
//! @ingroup trafficSimulationPlugin
//! @brief The Traffic Item Interface defines a general traffic parameter which
//!        can be incorporated into the traffic configuration setup.
//!
//!        A traffic parameter is defined by a value (generally a string)
//!        as well as a probability to be selected in a simulation run. This class
//!        provides setter and getter method for manipulating these properties.
//-----------------------------------------------------------------------------

#ifndef TRAFFICITEMINTERFACE_H
#define TRAFFICITEMINTERFACE_H

#include <QObject>
#include <QString>

//-----------------------------------------------------------------------------
//! @brief The Traffic Item Interface defines a general traffic parameter which
//!        can be incorporated into the traffic configuration setup.
//!
//!        A traffic parameter is defined by a value (generally a string)
//!        as well as a probability to be selected in a simulation run. This class
//!        provides setter and getter method for manipulating these properties.
//-----------------------------------------------------------------------------
class TrafficItemInterface : public QObject
{
    Q_OBJECT

public:
    using Value = QString;      //!< the datatype for the traffic parameter's value (generally a string)
    using Probability = double; //!< the selection probability's datatype

public:
    explicit TrafficItemInterface(QObject *parent = 0) :
        QObject(parent)
    {
    }
    virtual ~TrafficItemInterface() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal emitted whenever an attribute of the traffic parameter has changed.
    //-----------------------------------------------------------------------------
    void edited();

public:
    //-----------------------------------------------------------------------------
    //! Sets the traffic item's value to the value passed as an argument.
    //!
    //! @param[in]      _value      the new value to be adopted
    //-----------------------------------------------------------------------------
    virtual void setValue(Value const &_value) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the current value of the traffic item.
    //!
    //! @return                     the current value of the traffic item
    //-----------------------------------------------------------------------------
    virtual Value getValue() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the probability of the traffic item's occurence.
    //!
    //! @param[in]      _probability      the new probability to be adopted
    //-----------------------------------------------------------------------------
    virtual void setProbability(Probability const &_probability) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the current probability of the traffic item's occurence.
    //!
    //! @return                             the current probability of the traffic item's
    //!                                     occurence
    //-----------------------------------------------------------------------------
    virtual Probability getProbability() const = 0;
};

#endif // TRAFFICITEMINTERFACE_H
