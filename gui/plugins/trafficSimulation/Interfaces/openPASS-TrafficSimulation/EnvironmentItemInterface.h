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
//! @file  EnvironmentItemInterface.h
//! @ingroup trafficSimulationPlugin
//! @brief The EnvironmentItemInterface class provides methods for reading and editing
//!        environment parameters.
//!
//!        An environment parameter is generally described by a value and a selection
//!        probability. The latter defines the probability to be selected as an
//!        environment parameter in a simulation run. Environment parameters are stored
//!        in the simulationConfig XML file under the <EnvironmentConfig> element.
//-----------------------------------------------------------------------------
#ifndef ENVIRONMENTITEMINTERFACE_H
#define ENVIRONMENTITEMINTERFACE_H

#include <QObject>
#include <QString>

//-----------------------------------------------------------------------------
//! @brief The EnvironmentItemInterface class provides methods for reading and editing
//!        environment parameters.
//!
//!        An environment parameter is generally described by a value and a selection
//!        probability. The latter defines the probability to be selected as an
//!        environment parameter in a simulation run. Environment parameters are stored
//!        in the simulationConfig XML file under the <EnvironmentConfig> element.
//-----------------------------------------------------------------------------
class EnvironmentItemInterface : public QObject
{
    Q_OBJECT

public:
    using Value = QString;      //!< a general string datatype for the environment parameter's value.
    using Probability = double; //!< the selection probability's datatype.

public:
    explicit EnvironmentItemInterface(QObject *parent = nullptr) :
        QObject(parent)
    {
    }

    virtual ~EnvironmentItemInterface() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, which is emitted when the environment parameter's attributes have
    //! been modified. Will be forwarded by the EnvironmentInterface instance.
    //-----------------------------------------------------------------------------
    void edited();

public:
    //-----------------------------------------------------------------------------
    //! Method which sets the value of the environment parameter to the one passed
    //! as an argument.
    //!
    //! @param[in]      _value      the new value to be adopted
    //-----------------------------------------------------------------------------
    virtual void setValue(const Value &_value) = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns the current value of the environment parameter.
    //!
    //! @return               the current value of the environment parameter
    //-----------------------------------------------------------------------------
    virtual Value getValue() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which sets the selection probability of the environment parameter
    //! to the one passed as an argument.
    //!
    //! @param[in]      _value      the new probability to be adopted
    //-----------------------------------------------------------------------------
    virtual void setProbability(const Probability &_probability) = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns the current selection probability of the environment
    //! parameter.
    //!
    //! @return               the current selection probability of the environment
    //!                       parameter
    //-----------------------------------------------------------------------------
    virtual Probability getProbability() const = 0;
};

#endif // ENVIRONMENTITEMINTERFACE_H
