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
//! @file  DriverProfileItemInterface.h
//! @ingroup agentConfigurationPlugin
//! @brief  This class defines objects containing all relevant information on
//!         driver profiles.
//!
//!         In particular, the name, some config file and all parameters are part
//!         of driver profiles. The driver config refers to some XML
//!         metadata describing a driver model algorithm. Note that such an XML
//!         file has not been developed for driver models yet and, thus, driver
//!         configs are not yet in use.
//-----------------------------------------------------------------------------

#ifndef DRIVERPROFILEITEMINTERFACE_H
#define DRIVERPROFILEITEMINTERFACE_H

#include <QMap>
#include <QObject>
#include <QString>

#include "openPASS-AgentConfiguration/ParameterMapInterface.h"

//-----------------------------------------------------------------------------
//! @brief  This class defines objects containing all relevant information on
//!         driver profiles.
//!
//!         In particular, the name, some config file and all parameters are part
//!         of driver profiles. The driver config refers to some XML
//!         metadata describing a driver model algorithm. Note that such an XML
//!         file has not been developed for driver models yet and, thus, driver
//!         configs are not yet in use.
//-----------------------------------------------------------------------------
class DriverProfileItemInterface : public QObject
{
    Q_OBJECT

public:
    using Name = QString;                     //!< typedef for the driver profile's name
    using DriverConfig = QString;             //!< typedef for the driver config file
    using Parameters = ParameterMapInterface; //!< typedef for the parameters of the driver profile

public:
    DriverProfileItemInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    ~DriverProfileItemInterface() = default;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the name of this driver profile. In fact, this is the
    //! key identifier under which the profile is registered in the map, i.e. an
    //! instance of DriverProfileMapInterface / DriverProfileMapModel.
    //!
    //! @return             the name of this driver profile
    //-----------------------------------------------------------------------------
    virtual Name getName() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the driver configuration file containing XML metadata.
    //! @note   This method is not yet in use as the XML metadata file needs to be still
    //!         developed.
    //!
    //! @return             the driver configuration file
    //-----------------------------------------------------------------------------
    virtual DriverConfig getConfig() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which sets the driver configuration file containing XML metadata to
    //! the one passed as an argument.
    //! @note   This method is not yet in use as the XML metadata file needs to be still
    //!         developed.
    //!
    //! @param[in]             the driver configuration file to be used for this profile
    //-----------------------------------------------------------------------------
    virtual void setConfig(DriverConfig const &_driverConfig) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the parameter map (ParameterMapInterface) of this driver
    //! profile.
    //!
    //! @return         the parameter map of this driver profile
    //-----------------------------------------------------------------------------
    virtual Parameters *getParameters() const = 0;
};

#endif // DRIVERPROFILEITEMINTERFACE_H
