/*******************************************************************************
* Copyright (c) 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  slaveConfig.h
*	\brief This file stores all information of the slave configuration.
*/
//-----------------------------------------------------------------------------

#pragma once

#include "Interfaces/slaveConfigInterface.h"
#include "profiles.h"
#include "Common/opExport.h"
#include "Common/spawnPointLibraryDefinitions.h"

namespace Configuration {

//-----------------------------------------------------------------------------
/** \brief Container class for the slave configuration
* 	\details This class stores all information provided by the slaveConfig.xml
*            and provides access to this information
*/
//-----------------------------------------------------------------------------
class CORESLAVEEXPORT SlaveConfig : public SlaveConfigInterface
{
public:
    virtual ExperimentConfig &GetExperimentConfig() override;

    virtual ScenarioConfig &GetScenarioConfig() override;

    virtual SpawnPointLibraryInfoCollection& GetSpawnPointsConfig() override;

    virtual ObservationInstanceCollection& GetObservationConfig() override;

    virtual EnvironmentConfig &GetEnvironmentConfig() override;

    virtual const std::string& GetProfilesCatalog() const override;

    virtual void SetProfilesCatalog(std::string profilesCatalog) override;

private:
    ExperimentConfig experimentConfig {};
    ScenarioConfig scenarioConfig {};
    SpawnPointLibraryInfoCollection spawnPointsConfig {};
    ObservationInstanceCollection observationConfig {};
    EnvironmentConfig environmentConfig {};
    std::string profilesCatalog {};
};

} //namespace Configuration
