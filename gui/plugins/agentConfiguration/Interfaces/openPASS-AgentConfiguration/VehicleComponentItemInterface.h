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
//! @file  VehicleComponentItemInterface.h
//! @ingroup agentConfigurationPlugin
//! @brief  The VehicleComponentItemInterface class defines data on
//!         vehicle components which can be thought of a algorithms that will
//!         be built into a vehicle with a certain probability.
//!
//!         The stored data comprise the algorithm of the component (type),
//!         a profile containing the parameterization together with probabilities
//!         for the profile to be instantiated and, finally, sensor links
//!         if the component requires sensor data.
//-----------------------------------------------------------------------------

#ifndef VEHICLECOMPONENTITEMINTERFACE_H
#define VEHICLECOMPONENTITEMINTERFACE_H

#include <QMap>
#include <QObject>

#include "openPASS-AgentConfiguration/VehicleComponentProfileItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief  The VehicleComponentItemInterface class defines data on
//!         vehicle components which can be thought of a algorithms that will
//!         be built into a vehicle with a certain probability.
//!
//!         The stored data comprise the algorithm of the component (type),
//!         a profile containing the parameterization together with probabilities
//!         for the profile to be instantiated and, finally, sensor links
//!         if the component requires sensor data.
//-----------------------------------------------------------------------------
class VehicleComponentItemInterface : public QObject
{
    Q_OBJECT

public:
    using Type = QString;                                 //!< typedef for the component type (= algorithm)
    using Profile = VehicleComponentProfileItemInterface; //!< typedef for the profile containing the parameterization
    using Probability = double;                           //!< typedef for a probability

public:
    using Profiles = QMap<Profile *, Probability>;   //!< typedef for the map which assigns probabilities to profiles
    using SensorLinks = QMap<unsigned int, QString>; //!< typedef for the map describing sensor links

public:
    VehicleComponentItemInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~VehicleComponentItemInterface() = default;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the map of profiles assigned to probabilites.
    //!
    //! @return             the pointer to the map of profiles assigned to probabilities
    //-----------------------------------------------------------------------------
    virtual Profiles *getProfiles() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns the map of sensor links.
    //!
    //! @return             the pointer to the sensor links
    //-----------------------------------------------------------------------------
    virtual SensorLinks *getSensorLinks() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns type (algorithm) of the vehicle component.
    //!
    //! @return             the type (algorithm) of the vehicle component
    //-----------------------------------------------------------------------------
    virtual Type getType() const = 0;
};

#endif // VEHICLECOMPONENTITEMINTERFACE_H
