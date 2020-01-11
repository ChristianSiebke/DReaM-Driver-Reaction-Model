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
//! @file  EnvironmentModel.h
//! @ingroup trafficSimulationPlugin
//! @brief The Environment Model implements the functionality of the
//!        EnvironmentInterface class.
//-----------------------------------------------------------------------------

#ifndef ENVIRONMENTMODEL_H
#define ENVIRONMENTMODEL_H

#include "openPASS-TrafficSimulation/EnvironmentInterface.h"

#include "Models/EnvironmentItemModel.h"

//-----------------------------------------------------------------------------
//! @brief The Environment Model implements the functionality of the
//!        EnvironmentInterface class.
//-----------------------------------------------------------------------------
class EnvironmentModel : public EnvironmentInterface
{

    Q_OBJECT

public:
    EnvironmentModel(QObject *parent = nullptr);
    virtual ~EnvironmentModel() override;

public:
    virtual void setName(Name const & _name) override;
    virtual Name getName() const override;

public:
    virtual TimeOfDayMap * getTimeOfDayMap() const override;
    virtual VisibilityDistanceMap * getVisibilityDistanceMap() const override;
    virtual FrictionMap * getFrictionMap() const override;
    virtual WeatherMap * getWeatherMap() const override;

public:
    virtual void setStatus(Status const &_status) override;
    virtual Status getStatus() const override;

public:
    virtual Item * createItem(Item::Value const & value, Item::Probability const & probability) override;
    virtual void reset() override;

private:
    Name name;                                                  //!< the name of the environment configuration
    TimeOfDayMap * const timeOfDayMap;                          //!< the pointer to the map of time of day parameters
    VisibilityDistanceMap * const visibilityDistanceMap;        //!< the pointer to the visibility distance map
    FrictionMap * const frictionMap;                            //!< the pointer to the map of friction coefficients
    WeatherMap * const weatherMap;                              //!< the pointer to the map of wheather parameters
    Status status;                                              //!< the current status of the settings. "true" means that the configuration is principally ready for simulation

};

#endif // ENVIRONMENTMODEL_H
