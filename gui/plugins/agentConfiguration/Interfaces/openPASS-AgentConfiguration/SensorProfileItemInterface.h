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
//! @file  SensorProfileItemInterface.h
//! @ingroup agentConfigurationPlugin
//! @brief  This class defines objects containing all relevant information on
//!         sensor profiles.
//!
//!         In particular, the name, the type descibing the sensor's implementation
//!         and all parameters constitute a sensor profile addressed by this class.
//-----------------------------------------------------------------------------

#ifndef SENSORPROFILEITEMINTERFACE_H
#define SENSORPROFILEITEMINTERFACE_H

#include <QObject>
#include <QPair>

#include "openPASS-AgentConfiguration/ParameterMapInterface.h"

//-----------------------------------------------------------------------------
//! @brief  This class defines objects containing all relevant information on
//!         sensor profiles.
//!
//!         In particular, the name, the type descibing the sensor's implementation
//!         and all parameters constitute a sensor profile addressed by this class.
//-----------------------------------------------------------------------------
class SensorProfileItemInterface : public QObject
{
    Q_OBJECT

public:
    using Name = QString;                     //!< typedef for the sensor profile's name
    using Type = QString;                     //!< typedef for the sensor's type (implementation)
    using ID = QPair<Name, Type>;             //!< typedef for the identifier of a sensor profile, i.e. its name and type
    using Parameters = ParameterMapInterface; //!< typedef for the sensor profile's parameters

public:
    SensorProfileItemInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~SensorProfileItemInterface() = default;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the parameter map of this sensor profile (instance of
    //! ParameterMapInterface).
    //!
    //! @return             the parameter map of this sensor profile
    //-----------------------------------------------------------------------------
    virtual Parameters *getParameters() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the identifier of the sensor profile, i.e. the pair of
    //! name and type.
    //!
    //! @return             the identifier of the sensor profile
    //-----------------------------------------------------------------------------
    virtual ID getID() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns the name of the sensor profile.
    //!
    //! @return             the name of the sensor profile
    //-----------------------------------------------------------------------------
    virtual Name getName() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns the type (implementation) of the sensor adressed by
    //! this profile.
    //!
    //! @return             the type of the sensor
    //-----------------------------------------------------------------------------
    virtual Type getType() const = 0;
};

#endif // SENSORPROFILEITEMINTERFACE_H
