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
//! @file  VehicleProfileItemInterface.h
//! @ingroup agentConfigurationPlugin
//! @brief  This class defines vehicle profiles which unite all data necessary
//!         for instantiating a vehicle in the simulation.
//!
//!         This particularly comprises the vehicle model and optional driver
//!         assistance systems (ADAS) and sensors. In the profile catalogue (see
//!         documentation of the simulation core), ADAS and sensors are defined
//!         by the XML elements <components> and <sensors>, respectively. In this
//!         catalogue, it is also defined how vehicle components (ADAS) are
//!         connected to sensors via the so-called <sensorLinks> elements.
//!         Furthermore, the (stochastic) parameterizations of sensors and
//!         vehicle components is established via sensor profiles and vehicle
//!         component profiles, respectively. However, the GUI offers the System
//!         Editor Plugin where these pieces of information on ADAS and sensors
//!         are edited by the user. Therefore, a system config XML file exported
//!         from the System Editor Plugin is referenced here in order
//!         to extract this information. While for the GUI, the reference to specific
//!         systems of a system config XML file is sufficient, the profile catalogue
//!         needs these information to be transferred to the corresponding profiles
//!         which are, thus,  also part of this class. Note, that a special feature
//!         OpenPASS that ADAS and their parameterization (profiles) can be equipped
//!         stochastically. Hence, the systems defined in the system config are
//!         assigned to the corresponding probabilities.
//-----------------------------------------------------------------------------

#ifndef VEHICLEPROFILEITEMINTERFACE_H
#define VEHICLEPROFILEITEMINTERFACE_H

#include <QMap>
#include <QObject>

#include "openPASS-AgentConfiguration/SensorMapInterface.h"
#include "openPASS-AgentConfiguration/VehicleComponentMapInterface.h"

//-----------------------------------------------------------------------------
//! @brief  This class defines vehicle profiles which unite all data necessary
//!         for instantiating a vehicle in the simulation.
//!
//!         This particularly comprises the vehicle model and optional driver
//!         assistance systems (ADAS) and sensors. In the profile catalogue (see
//!         documentation of the simulation core), ADAS and sensors are defined
//!         by the XML elements <components> and <sensors>, respectively. In this
//!         catalogue, it is also defined how vehicle components (ADAS) are
//!         connected to sensors via the so-called <sensorLinks> elements.
//!         Furthermore, the (stochastic) parameterizations of sensors and
//!         vehicle components is established via sensor profiles and vehicle
//!         component profiles, respectively. However, the GUI offers the System
//!         Editor Plugin where these pieces of information on ADAS and sensors
//!         are edited by the user. Therefore, a system config XML file exported
//!         from the System Editor Plugin is referenced here in order
//!         to extract this information. While for the GUI, the reference to specific
//!         systems of a system config XML file is sufficient, the profile catalogue
//!         needs these information to be transferred to the corresponding profiles
//!         which are, thus,  also part of this class. Note, that a special feature
//!         OpenPASS that ADAS and their parameterization (profiles) can be equipped
//!         stochastically. Hence, the systems defined in the system config are
//!         assigned to the corresponding probabilities.
//-----------------------------------------------------------------------------
class VehicleProfileItemInterface : public QObject
{
    Q_OBJECT

public:
    using Name = QString;       //!< typedef for the name of the vehicle profile
    using Probability = double; //!< typedef for a probability

    using Model = QString;          //!< typedef for the vehicle model
    using ModelCatalogue = QString; //!< typedef for the vehicle model catalogue

    using SystemConfig = QString;  //!< typedef for the system config XML file
    using SystemID = unsigned int; //!< typedef for a system Id (valid in corresponding system config XML file)
    using SystemTitle = QString;   //!< typedef for the system title
    using AlgoType = QString;      //!< typedef for the algorithm type (in fact, the implementation of the algorithm)

    using System = QPair<SystemID, SystemTitle>;      //!< typedef for a system (here, the system Id and title are sufficient)
    using Systems = QMap<System, Probability>;        //!< typedef for the map assigning probabilities to systems
    using SystemProfiles = QMap<AlgoType, Systems *>; //!< typedef for the map sorting the probability-assigned systems by the algorithm type

public:
    using VehicleComponents = VehicleComponentMapInterface; //!< typedef for the vehicle component (-> to be exported from systems in system config)
    using Sensors = SensorMapInterface;                     //!< typedef for sensors (-> to be exported from systems in system config)

public:
    VehicleProfileItemInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~VehicleProfileItemInterface() = default;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the name of the vehicle profile.
    //!
    //! @return                     the name of the vehicle profile
    //-----------------------------------------------------------------------------
    virtual Name getName() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the current vehicle model catalogue.
    //!
    //! @return                     the name of the current vehicle model
    //!                             catalogue
    //-----------------------------------------------------------------------------
    virtual ModelCatalogue getModelCatalogue() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which sets the current vehicle model catalogue to the one passed as
    //! an argument
    //!
    //! @param[in]      _modelCatalogue     the new vehicle model catalogue
    //-----------------------------------------------------------------------------
    virtual void setModelCatalogue(ModelCatalogue const &_modelCatalogue) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the currently selected vehicle model.
    //!
    //! @return                     the currently selected vehicle model
    //-----------------------------------------------------------------------------
    virtual Model getModel() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which sets the current vehicle model to the one passed as an argument.
    //!
    //! @param[in]      _model    the new vehicle model
    //-----------------------------------------------------------------------------
    virtual void setModel(Model const &_model) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the name of the current system config XML file.
    //!
    //! @return                     the current system config XML filename
    //-----------------------------------------------------------------------------
    virtual SystemConfig getSystemConfig() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which sets the current system config XML file to the one passed as
    //! an argument.
    //!
    //! @param[in]      _systemConfig       the new system config XML file to be
    //!                                     used
    //-----------------------------------------------------------------------------
    virtual void setSystemConfig(SystemConfig const &_systemConfig) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns all mappings of systems to probabilities sorted by
    //! their algorithm type. The corresponding data structure is defined by the
    //! typedef SystemProfiles.
    //!
    //! @return                             the system profiles map (see corresponding
    //!                                     typedef SystemProfiles)
    //-----------------------------------------------------------------------------
    virtual SystemProfiles *getSystemProfiles() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the mapping of all systems of the given algorithm type to
    //! their equipment probabilities.
    //!
    //! @param[in]      _type        the requested algorithm type
    //! @return                      the mapping of all systems to equipment probabilities
    //!                              being of type _type.
    //-----------------------------------------------------------------------------
    virtual Systems *getSystems(AlgoType const &_type) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the map of vehicle components, i.e. the container class
    //! for instances of the VehicleComponentItemInterface class.
    //!
    //! @note this getter method is only used when exporting the definition of a
    //! system in thesystem config XML file to the format needed by the profiles
    //! catalogue.
    //!
    //! @return                       the map of vehicle components
    //!
    //-----------------------------------------------------------------------------
    virtual VehicleComponents *getVehicleComponents() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns the map of sensors, i.e. the container class
    //! for instances of the SensorItemInterface class.
    //!
    //! @note this getter method is only used when exporting the definition of a
    //! system in the system config XML file to the format needed by the profiles
    //! catalogue.
    //!
    //! @return                       the map of sensors
    //-----------------------------------------------------------------------------
    virtual Sensors *getSensors() const = 0;
};

#endif // VEHICLEPROFILEITEMINTERFACE_H
