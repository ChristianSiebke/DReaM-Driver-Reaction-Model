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
//! @file  SensorItemInterface.h
//! @ingroup agentConfigurationPlugin
//! @brief  This file contains the class declaration of a sensor data object as
//!         well as a structure for a sensor position.
//!
//!         The SensorItemInterface class combines a sensor profile
//!         (SensorProfileItemInterface, containing parameters) and the sensor
//!         position to one sensor data object. The sensor position
//!         strucutre gathers all variables which specify a sensor position. This
//!         includes three spatial coordinates (vehicle reference frame) as well as three
//!         angles (Euler angles) for the orientation. Additionally, a name for
//!         identifying/characterizing the sensor position is provided.
//-----------------------------------------------------------------------------

#ifndef SENSORITEMINTERFACE_H
#define SENSORITEMINTERFACE_H

#include <QObject>

#include "openPASS-AgentConfiguration/SensorProfileItemInterface.h"

//-----------------------------------------------------------------------------
//! This structure gathers all variables which specify a sensor position. This
//! includes three spatial coordinates (vehicle reference frame) as well as three
//! angles (Euler angles) for the orientation. Additionally, a name for
//! identifying/characterizing the sensor position is provided.
//-----------------------------------------------------------------------------
struct SensorPosition
{
    QString Name;

    double roll;
    double yaw;
    double pitch;

    double height;
    double lateral;
    double longitudinal;
};

//-----------------------------------------------------------------------------
//! The SensorItemInterface class combines a sensor profile (SensorProfileItemInterface,
//! containing parameters) and the sensor position to a sensor data object.
//-----------------------------------------------------------------------------
class SensorItemInterface : public QObject
{
    Q_OBJECT

public:
    using ID = unsigned int;                    //!< typedef for the sensor's key identifier
    using Profile = SensorProfileItemInterface; //!< tpedef for the sensor profile

public:
    SensorItemInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~SensorItemInterface() = default;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the position of the sensor.
    //!
    //! @return             the sensor's position structure
    //-----------------------------------------------------------------------------
    virtual SensorPosition getSensorPosition() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which sets the position of the sensor to the one passed as an
    //! argument.
    //!
    //! @param[in]             the new sensor position structure
    //-----------------------------------------------------------------------------
    virtual void setSensorPosition(SensorPosition const &position) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the sensor profile which is currently used.
    //!
    //! @return             the sensor profile in use
    //-----------------------------------------------------------------------------
    virtual Profile *getSensorProfile() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which sets the sensor profile to the one passed as an argument.
    //!
    //! @param[in]             the new sensor profile
    //-----------------------------------------------------------------------------
    virtual void setSensorProfile(Profile *const _profile) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the id of this sensor, if this sensor is registered
    //! in a map (SensorMapInterface). Otherwise, an empty id is returned.
    //!
    //! @return             the id of this sensor
    //-----------------------------------------------------------------------------
    virtual ID getID() const = 0;
};

#endif // SENSORITEMINTERFACE_H
