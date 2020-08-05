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
//! @file  TrafficSimulationInterface.h
//! @ingroup trafficSimulationPlugin
//! @brief The TrafficSimulationInterface class forms the body of a traffic simulation
//!        configuration.
//!
//!        It references the experiment, scenario, traffic and environment configuration
//!        classes and, hence, corresponds to the root element of the combination XML file.
//-----------------------------------------------------------------------------
#ifndef TRAFFICSIMULATIONINTERFACE_H
#define TRAFFICSIMULATIONINTERFACE_H

#include "ExperimentInterface.h"
#include "EnvironmentInterface.h"
#include "ScenarioInterface.h"
#include "TrafficInterface.h"

#include <QObject>

//-----------------------------------------------------------------------------
//! @brief The TrafficSimulationInterface class forms the body of a traffic simulation
//!        configuration.
//!
//!        It references the experiment, scenario, traffic and environment configuration
//!        classes and, hence, corresponds to the root element of the combination XML file.
//-----------------------------------------------------------------------------
class TrafficSimulationInterface : public QObject
{
    Q_OBJECT

public:
    using ProfilesCatalogue = QString;


public:
    explicit TrafficSimulationInterface(QObject * const parent = nullptr)
        : QObject(parent){}

    virtual ~TrafficSimulationInterface() = default;

public:
    //-----------------------------------------------------------------------------
    //! Returns the pointer to the experiment configuration object, i.e. an instance
    //! of the ExperimentInterface class.
    //!
    //! @return             the pointer to the experiment configuration object
    //-----------------------------------------------------------------------------
    virtual ExperimentInterface * getExperiment() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the pointer to the environment configuration object, i.e. an instance
    //! of the EnvironmentInterface class.
    //!
    //! @return             the pointer to the environment configuration object
    //-----------------------------------------------------------------------------
    virtual EnvironmentInterface *getEnvironment() const =0;

    //-----------------------------------------------------------------------------
    //! Returns the pointer to the scenario configuration object, i.e. an instance
    //! of the ScenarioInterface class.
    //!
    //! @return             the pointer to the scenario configuration object
    //-----------------------------------------------------------------------------
    virtual ScenarioInterface *getScenario() const =0;

    //-----------------------------------------------------------------------------
    //! Returns the pointer to the traffic configuration object, i.e. an instance
    //! of the TrafficInterface class.
    //!
    //! @return             the pointer to the traffic configuration object
    //-----------------------------------------------------------------------------
    virtual TrafficInterface * getTraffic() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the current profiles catalogue to the one passed as an argument.
    //!
    //! @param[in]      _profilesCatalogue      the profiles catalogue to be adopted
    //-----------------------------------------------------------------------------
    virtual void setProfilesCatalogue(ProfilesCatalogue const & _profilesCatalogue) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the current profiles catalogue.
    //!
    //! @return             the current profiles catalogue
    //-----------------------------------------------------------------------------
    virtual ProfilesCatalogue getProfilesCatalogue() const = 0;

};


#endif // TRAFFICSIMULATIONINTERFACE
