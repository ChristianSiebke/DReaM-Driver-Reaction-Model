/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  SlaveConfigInterface.h
//! @brief This file contains the interface to retrieve the parameters of the slave configuration.
//-----------------------------------------------------------------------------

#pragma once

#include <list>
#include <unordered_map>

#include "Interfaces/parameterInterface.h"
#include "Common/globalDefinitions.h"
#include "Common/spawnPointLibraryDefinitions.h"

using IntProbabilities = std::unordered_map<int, double>;
using DoubleProbabilities = std::unordered_map<double, double>;
using StringProbabilities = std::unordered_map<std::string, double>;

struct hash_fn
{
    std::size_t operator()(const openpass::parameter::NormalDistribution& normalDistribution) const
    {
        std::size_t mean = std::hash<double>()(normalDistribution.mean);
        std::size_t standardDeviation = std::hash<double>()(normalDistribution.standardDeviation);
        std::size_t min = std::hash<double>()(normalDistribution.min);
        std::size_t max = std::hash<double>()(normalDistribution.max);

        return mean ^ standardDeviation ^ min ^ max;
    }
};

using NormalDistributionProbabilities = std::unordered_map<openpass::parameter::NormalDistribution, double, hash_fn>;

struct ExperimentConfig
{
    using Libraries = std::unordered_map<std::string, std::string>;

    int experimentId;
    int numberOfInvocations;
    bool logCyclicsToCsv;
    std::uint32_t randomSeed;
    std::vector<std::string> loggingGroups;         //!< Holds the names of enabled logging groups
    Libraries libraries;
};

struct ScenarioConfig
{
    std::string scenarioPath {};                    //!< Path of the scenario file
};

struct EnvironmentConfig
{
    StringProbabilities timeOfDays {};
    IntProbabilities visibilityDistances {};
    DoubleProbabilities frictions {};
    StringProbabilities weathers {};
};

//-----------------------------------------------------------------------------
//! Interface provides access to the slave configuration parameters
//-----------------------------------------------------------------------------
class SlaveConfigInterface
{
public:
    SlaveConfigInterface() = default;
    SlaveConfigInterface(const SlaveConfigInterface&) = delete;
    SlaveConfigInterface(SlaveConfigInterface&&) = delete;
    SlaveConfigInterface& operator=(const SlaveConfigInterface&) = delete;
    SlaveConfigInterface& operator=(SlaveConfigInterface&&) = delete;
    virtual ~SlaveConfigInterface() = default;

    /*!
    * \brief Returns a pointer to the experimentConfig
    *
    * @return        experimentConfig
    */
    virtual ExperimentConfig& GetExperimentConfig() = 0;

    /*!
    * \brief Returns a pointer to the scenarioConfig
    *
    * @return        scenarioConfig
    */
    virtual ScenarioConfig& GetScenarioConfig() = 0;

    virtual SpawnPointLibraryInfoCollection& GetSpawnPointsConfig() = 0;

    /*!
    * \brief Returns a pointer to the environmentConfig
    *
    * @return        environmentConfig
    */
    virtual EnvironmentConfig& GetEnvironmentConfig() = 0;

    /*!
     * \brief Returns the name of the profiles catalog
     */
    virtual const std::string& GetProfilesCatalog() const = 0;

    /*!
     * \brief Sets the name of the profiles catalog
     */
    virtual void SetProfilesCatalog(std::string profilesCatalog) = 0;
};
