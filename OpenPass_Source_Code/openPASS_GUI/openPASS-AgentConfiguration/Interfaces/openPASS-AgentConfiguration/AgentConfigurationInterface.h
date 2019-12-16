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
//! @file  AgentConfigurationInterface.h
//! @ingroup agentConfigurationPlugin
//! @brief This class represents the root element of both, the GUI-specific
//!        *.ui as well as the simulation core-relevant ProfilesCatalog.xml
//!        files.
//!
//!        It provides access to the maps (container classes) of agent profiles,
//!        vehicle profiles and driver profiles. Additionally, in the
//!        ProfilesCatalog.xml vehicle component profiles (representing
//!        algorithms of ADAS) and sensor profiles occur which are also referenced
//!        by this class.
//-----------------------------------------------------------------------------

#ifndef AGENTCONFIGURATIONINTERFACE_H
#define AGENTCONFIGURATIONINTERFACE_H

#include "openPASS/ServiceInterface.h"

#include "openPASS-AgentConfiguration/AgentMapInterface.h"
#include "openPASS-AgentConfiguration/VehicleProfileMapInterface.h"
#include "openPASS-AgentConfiguration/DriverProfileMapInterface.h"
#include "openPASS-AgentConfiguration/VehicleComponentProfileMapInterface.h"
#include "openPASS-AgentConfiguration/SensorProfileMapInterface.h"

#include <QObject>

static ServiceManagerInterface::ID const AgentConfigurationInterfaceID =
        ServiceManagerInterfaceID("openPASS.AgentConfiguration.AgentConfigurationInterface");

//-----------------------------------------------------------------------------
//! @brief This class represents the root element of both, the GUI-specific
//!        *.ui as well as the simulation core-relevant ProfilesCatalog.xml
//!        files.
//!
//!        It provides access to the maps (container classes) of agent profiles,
//!        vehicle profiles and driver profiles. Additionally, in the
//!        ProfilesCatalog.xml vehicle component profiles (representing
//!        algorithms of ADAS) and sensor profiles occur which are also referenced
//!        by this class.
//-----------------------------------------------------------------------------
class AgentConfigurationInterface : public QObject,
        public ServiceInterface<AgentConfigurationInterface, AgentConfigurationInterfaceID>
{
    Q_OBJECT

public:
    explicit AgentConfigurationInterface(QObject * const parent = nullptr)
        : QObject(parent), ServiceInterface(this) {}
    virtual ~AgentConfigurationInterface() = default;

public:
    //-----------------------------------------------------------------------------
    //! Method which provides access to the map of agent profiles.
    //!
    //! @return                 the pointer to the map of agent profiles
    //-----------------------------------------------------------------------------
    virtual AgentMapInterface * getAgents() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which provides access to the map of vehicle profiles.
    //!
    //! @return                 the pointer to the map of vehicle profiles
    //-----------------------------------------------------------------------------
    virtual VehicleProfileMapInterface * getVehicleProfiles() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which provides access to the map of driver profiles.
    //!
    //! @return                 the pointer to the map of driver profiles
    //-----------------------------------------------------------------------------
    virtual DriverProfileMapInterface * getDriverProfiles() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which provides access to the map of vehicle component profiles
    //! (in fact, ADAS algorithms).
    //!
    //! @return                 the pointer to the map of vehicle component profiles
    //-----------------------------------------------------------------------------
    virtual VehicleComponentProfileMapInterface * getVehicleComponentProfiles() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which provides access to the map of sensor profiles.
    //!
    //! @return                 the pointer to the map of sensor profiles
    //-----------------------------------------------------------------------------
    virtual SensorProfileMapInterface * getSensorProfiles() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which clears all profile maps and deletes their entries.
    //-----------------------------------------------------------------------------
    virtual void clear() = 0;

};

#endif // AGENTCONFIGURATIONINTERFACE_H
