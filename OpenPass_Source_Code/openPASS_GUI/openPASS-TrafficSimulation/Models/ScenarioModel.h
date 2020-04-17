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
//! @file  ScenarioModel.h
//! @ingroup trafficSimulationPlugin
//! @brief The Scenario Model class implements the functionality of the
//!        ScenarioItemInterface class.
//-----------------------------------------------------------------------------

#ifndef SCENARIOMODEL_H
#define SCENARIOMODEL_H

#include "interfaces/openPASS-TrafficSimulation/ScenarioInterface.h"

//-----------------------------------------------------------------------------
//! @brief The Scenario Model class implements the functionality of the
//!        ScenarioItemInterface class.
//-----------------------------------------------------------------------------
class ScenarioModel : public ScenarioInterface
{
public:
    explicit ScenarioModel(QObject *parent = nullptr);

    virtual ~ScenarioModel() override;

public:
    virtual void setName(Name const & newName) override;
    virtual Name getName() const override;

public:
    virtual ScenarioFile getScenarioFile() const override;
    virtual void setScenarioFile(ScenarioFile const & filename) override;

public:
    virtual Entities * getEntities() const override;

public:
    virtual void setStatus(Status const &_status) override;
    virtual Status getStatus() const override;


private:
    Name name;                  //!< the name of the scenario configuration
    ScenarioFile scenarioFile;  //!< the path to the OpenScenario file
    Entities * const entities;  //!< the map of scenario agents (referred to as 'entities' in the XOSC file standard).
    Status status;              //!< the status of the scenario configuration

};

#endif // SCENARIOMODEL_H
