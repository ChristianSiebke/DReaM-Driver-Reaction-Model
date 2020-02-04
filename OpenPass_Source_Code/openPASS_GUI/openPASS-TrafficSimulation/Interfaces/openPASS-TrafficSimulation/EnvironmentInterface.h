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
//! @file  EnvironmentInterface.h
//! @ingroup trafficSimulationPlugin
//! @brief The EnvironmentInterface class provides setter and getter methods for the maps of
//!        environmental parameters, listed under <EnvironmentConfig> in the slaveConfig
//!        XML file.
//!
//!        These comprise the time of day, visibility distance, friction coefficient
//!        and the wheather. Different parameters of the same kind can be listed in the slaveConfig
//!        giving a probability to be selected in a simulation run. Environmental parameters
//!        are instances of the EnvironmentItemInterface class. The container class for storing
//!        environmental parameters is Qt't QMap class.
//-----------------------------------------------------------------------------

#ifndef ENVIRONMENTINTERFACE_H
#define ENVIRONMENTINTERFACE_H

#include "openPASS-TrafficSimulation/EnvironmentItemInterface.h"

#include <QObject>
#include <QString>
#include <QTime>
#include <QMap>

//-----------------------------------------------------------------------------
//! @brief The EnvironmentInterface class provides setter and getter methods for the maps of
//!        environmental parameters, listed under <EnvironmentConfig> in the slaveConfig
//!        XML file.
//!
//!        These comprise the time of day, visibility distance, friction coefficient
//!        and the wheather. Different parameters of the same kind can be listed in the slaveConfig
//!        giving a probability to be selected in a simulation run. Environmental parameters
//!        are instances of the EnvironmentItemInterface class. The container class for storing
//!        environmental parameters is Qt't QMap class.
//-----------------------------------------------------------------------------
class EnvironmentInterface : public QObject
{
    Q_OBJECT

public:
    using Name = QString;                           //!< The name of the environment configuration

public:
    using ID = unsigned int;                        //!< A key identifier for the map of environment items
    using Item = EnvironmentItemInterface;          //!< The class describing one environmental parameter

    using TimeOfDayMap = QMap<ID, Item *>;          //!< The container class for storing time of day parameters
    using VisibilityDistanceMap = QMap<ID, Item *>; //!< The container class for storing visitbility distances
    using FrictionMap = QMap<ID, Item *>;           //!< The container class for storing friction coefficients
    using WeatherMap = QMap<ID, Item *>;            //!< The container class for storing wheather parameters
    using Status = bool;                            //!< Type characterizing the status of the environment settings

public:
    explicit EnvironmentInterface(QObject *parent = nullptr)
        : QObject(parent) {}
    virtual ~EnvironmentInterface() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted when the name of the environment setting has been changed using
    //! the #setName method. Will inform EnvironmentDisplayPresenter to display the
    //! new name.
    //-----------------------------------------------------------------------------
    void modifiedName();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the environment settings have changed (e.g. specific
    //! parameters). Will be noticed by the EnvironmentDisplayPresenter to update
    //! the displayed info on the environment setting.
    //-----------------------------------------------------------------------------
    void edited();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the environment's status has changed.
    //-----------------------------------------------------------------------------
    void modifiedStatus();

public:
    //-----------------------------------------------------------------------------
    //! Sets the name of the environment configuration to the one passed as an
    //! argument.
    //!
    //! @param[in]      _name       the new name
    //-----------------------------------------------------------------------------
    virtual void setName(Name const & _name) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the current name of the environment configuration.
    //!
    //! @return             the current name
    //-----------------------------------------------------------------------------
    virtual Name getName() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Returns a pointer to the time of day parameter map.
    //!
    //! @return             the time of day parameter map.
    //-----------------------------------------------------------------------------
    virtual TimeOfDayMap * getTimeOfDayMap() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns a pointer to the map of visibility distances.
    //!
    //! @return             the map of visibility distances
    //-----------------------------------------------------------------------------
    virtual VisibilityDistanceMap * getVisibilityDistanceMap() const =0;

    //-----------------------------------------------------------------------------
    //! Returns a pointer to the map of friction coefficients.
    //!
    //! @return             the map of friction coefficients
    //-----------------------------------------------------------------------------
    virtual FrictionMap * getFrictionMap() const =0;

    //-----------------------------------------------------------------------------
    //! Returns a pointer to the map of whether parameters
    //!
    //! @return             the map of whether parameters
    //-----------------------------------------------------------------------------
    virtual WeatherMap * getWeatherMap() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method for setting the status of the environment configuration to the
    //! value passed as an argument.
    //!
    //! @param[in]      _status     the status
    //-----------------------------------------------------------------------------
    virtual void setStatus(Status const &_status) = 0;

    //-----------------------------------------------------------------------------
    //! Method  which returns the current status of the environment settings.
    //!
    //! @return                 the current status of the environment configuration
    //-----------------------------------------------------------------------------
    virtual Status getStatus() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Creates an environmental parameter (i.e. an instance of EnvironmentItemInterface)
    //! with the given value and probability and returns a pointer to it.
    //!
    //! @param[in]      value           the value of the new parameter
    //! @param[in]      probability     the selection probability of the new parameter
    //!
    //! @return                         a pointer to the newly created environment parameter
    //-----------------------------------------------------------------------------
    virtual Item * createItem(Item::Value const & value,
                              Item::Probability const & probability) = 0;

    //-----------------------------------------------------------------------------
    //! Method which clears all maps and deletes the correponding environmental
    //! parameters. Further sets the name to an empty string and emits the #edited signal
    //! to inform the EnvironmentDisplayPresenter instance about the changes.
    //-----------------------------------------------------------------------------
    virtual void reset() = 0;
};

#endif // ENVIRONMENTINTERFACE_H
