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
//! @file  VehicleComponentProfileItemInterface.h
//! @ingroup agentConfigurationPlugin
//! @brief  This class defines vehicle component profiles which contain the
//!         parameterization of a vehicle component, i.e. an instance of the
//!         VehicleComponentItemInterface class.
//!
//!         The profiles are characterized by a name and the type of the vehicle
//!         component (algorithm) they correspond to. Further, the profiles
//!         access the parameter map.
//-----------------------------------------------------------------------------

#ifndef VEHICLECOMPONENTPROFILEITEMINTERFACE_H
#define VEHICLECOMPONENTPROFILEITEMINTERFACE_H

#include "openPASS-AgentConfiguration/ParameterMapInterface.h"

#include <QObject>
#include <QMap>

//-----------------------------------------------------------------------------
//! @brief  This class defines vehicle component profiles which contain the
//!         parameterization of a vehicle component, i.e. an instance of the
//!         VehicleComponentItemInterface class.
//!
//!         The profiles are characterized by a name and the type of the vehicle
//!         component (algorithm) they correspond to. Further, the profiles
//!         access the parameter map.
//-----------------------------------------------------------------------------
class VehicleComponentProfileItemInterface: public QObject
{

    Q_OBJECT

public:
    using Name = QString;                       //!< typedef for the name of the profile
    using Type = QString;                       //!< typedef for the type (algorithm) of the corresponding vehicle component
    using Parameters = ParameterMapInterface;   //!< typedef for the parameter map

public:
    VehicleComponentProfileItemInterface(QObject * const parent = nullptr) : QObject(parent){}
    virtual ~VehicleComponentProfileItemInterface() = default;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the parameter map of the vehicle component profile.
    //!
    //! @return             the vehicle component profile's parameter map
    //-----------------------------------------------------------------------------
    virtual Parameters * getParameters() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the name of the vehicle component profile.
    //!
    //! @return             the vehicle component profile's name
    //-----------------------------------------------------------------------------
    virtual Name getName() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns the type of the corresponding vehicle component.
    //!
    //! @return             the type of the corresponding vehicle component
    //-----------------------------------------------------------------------------
    virtual Type getType() const = 0;
};

#endif // VEHICLECOMPONENTPROFILEITEMINTERFACE_H
