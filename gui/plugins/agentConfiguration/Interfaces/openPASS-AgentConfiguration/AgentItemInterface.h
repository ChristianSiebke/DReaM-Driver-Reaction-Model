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
//! @file  AgentItemInterface.h
//! @ingroup agentConfigurationPlugin
//! @brief This class defines agent profiles defined in both, the GUI-specific *.ui and
//!        the simulation core-relevant profiles cataloge (*.xml) under <AgentProfiles>.
//!
//!        In particular, this class gains accesss to the maps of vehicle and driver
//!        profiles for dynamic agent profiles. For static agent profiles, this class
//!        defines setter and getter methods for vehicle model catalogue and system
//!        config as well as models and systems defined therein.
//-----------------------------------------------------------------------------

#ifndef AGENTITEMINTERFACE_H
#define AGENTITEMINTERFACE_H

#include <QMap>
#include <QObject>

//-----------------------------------------------------------------------------
//! @brief This class defines agent profiles defined in both, the GUI-specific *.ui and
//!        the simulation core-relevant profiles cataloge (*.xml) under <AgentProfiles>.
//!
//!        In particular, this class gains accesss to the maps of vehicle and driver
//!        profiles for dynamic agent profiles. For static agent profiles, this class
//!        defines setter and getter methods for vehicle model catalogue and system
//!        config as well as models and systems defined therein.
//-----------------------------------------------------------------------------
class AgentItemInterface : public QObject
{
    Q_OBJECT

public:
    using Name = QString;       //!< the name of the agent profiles
    using Type = QString;       //!< the type of the agent profile (dynamic or static)
    using Probability = double; //!< a probability value (e.g. of a driver or a vehicle profile)

public:
    using SystemConfig = QString;                //!< the system config file
    using SystemID = unsigned int;               //!< the id of a system defined in a system config
    using SystemTitle = QString;                 //!< the title of a system defined in a system config
    using System = QPair<SystemID, SystemTitle>; //!< the pair of id and title of a system in system config (used as system reference)
    using VehicleModelCatalogue = QString;       //!< the vehicle model catalogue file
    using VehicleModel = QString;                //!< a vehicle model defined in a vehicle model catalogue file

public:
    using VehicleProfile = QString;                            //!< the name of a vehicle profile
    using VehicleProfiles = QMap<VehicleProfile, Probability>; //!< the mapping of a vehicle profile to the instantiation probability

public:
    using DriverProfile = QString;                           //!< the nome of  a driver profile
    using DriverProfiles = QMap<DriverProfile, Probability>; //!< the mapping of a driver profile to the instantiation probability

public:
    explicit AgentItemInterface(QObject *parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~AgentItemInterface() = default;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the name of this agent profile, i.e. the key identifier
    //! name of this agent profile if registered in a map (AgentMapInterface).
    //!
    //! @return                 the name of this agent profile
    //-----------------------------------------------------------------------------
    virtual Name getName() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the type of this agent profile.
    //!
    //! @return                 the type of this agent profile
    //-----------------------------------------------------------------------------
    virtual Type getType() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which sets the type of this agent profile to the one passed as an
    //! argument.
    //!
    //! @param[in]      _type              the new type of this agent profile
    //-----------------------------------------------------------------------------
    virtual void setType(const Type &_type) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns a pointer to the map of vehicle profiles, i.e. the mapping
    //! of vehicle profiles to instantiation probabilities (for dynamic agent profiles).
    //!
    //! @return                             the pointer to the map of vehicle profiles
    //-----------------------------------------------------------------------------
    virtual VehicleProfiles *getVehicleProfiles() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns a pointer to the map of driver profiles, i.e. the mapping
    //! of driver profiles to instantiation probabilities (for dynamic agent profiles).
    //!
    //! @return                             the pointer to the map of driver profiles
    //-----------------------------------------------------------------------------
    virtual DriverProfiles *getDriverProfiles() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the currently selected vehicle profile (for static agent
    //! profiles).
    //!
    //! @return                             the currently selected vehicle model
    //-----------------------------------------------------------------------------
    virtual VehicleModel getVehicleModel() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which sets the currently selected vehicle profile to the one passed
    //! as an argument (for static agent profiles).
    //!
    //! @param[in]      _vehicleModel       the new vehicle model
    //-----------------------------------------------------------------------------
    virtual void setVehicleModel(VehicleModel const &_vehicleModel) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the current vehicle model catalogue (for static agent
    //! profiles).
    //!
    //! @return                             the currently selected vehicle model
    //-----------------------------------------------------------------------------
    virtual VehicleModelCatalogue getVehicleModelCatalogue() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which sets the current vehicle model catalogue to the one passed
    //! as an argument (for static agent profiles).
    //!
    //! @param[in]      _vehicleModelCatalogue       the new vehicle model catalogue
    //-----------------------------------------------------------------------------
    virtual void setVehicleModelCatalogue(VehicleModelCatalogue const &_vehicleModelCatalogue) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the id of the currently selected system (for static agent
    //! profiles).
    //!
    //! @return                            the id of the currently selected system
    //-----------------------------------------------------------------------------
    virtual SystemID getSystemID() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which sets the currently selected system id to the one passed
    //! as an argument (for static agent profiles).
    //!
    //! @param[in]      _systemID       the new system id
    //-----------------------------------------------------------------------------
    virtual void setSystemID(SystemID const &_systemID) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the title of the currently selected system (for static agent
    //! profiles).
    //!
    //! @return                            the title of the currently selected system
    //-----------------------------------------------------------------------------
    virtual SystemTitle getSystemTitle() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which sets the currently selected system title to the one passed
    //! as an argument (for static agent profiles).
    //!
    //! @param[in]      _systemTitle       the new system title
    //-----------------------------------------------------------------------------
    virtual void setSystemTitle(SystemTitle const &_systemTitle) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the current system config file (for static agent
    //! profiles).
    //!
    //! @return                             the current system config file
    //-----------------------------------------------------------------------------
    virtual SystemConfig getSystemConfig() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which sets the current system config file to the one passed
    //! as an argument (for static agent profiles).
    //!
    //! @param[in]      _systemConfig      the new system config file
    //-----------------------------------------------------------------------------
    virtual void setSystemConfig(SystemConfig const &_systemConfig) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns a pointer to the system (i.e. the pair of system id and
    //! title) currently selected in this agent profile (for static agent profiles
    //! only).
    //!
    //! @return                             a pointer to the pair of system id and
    //!                                     title
    //-----------------------------------------------------------------------------
    virtual System *getSystem() = 0;
};

#endif // AGENTITEMINTERFACE_H
