/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*               2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  Config.h
//! @brief This file contains the representation of opSimulationManager
//!        configuration.
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <optional>
#include <vector>

namespace SimulationManager {
namespace Configuration {

class SimulationConfig;
using SimulationConfigs = std::vector<SimulationConfig>;

///
/// \brief Used as value storage of parsed configuration parameters
///
struct Config
{
public:
    Config(
        std::optional<int> logLevel,
        std::optional<std::string> logFileOpSimulationManager,
        std::optional<std::string> simulation,
        std::optional<std::string> libraries,
        SimulationConfigs simulationConfigs) :
        logLevel{CheckOrDefault(logLevel.value_or(defaultLogLevel))},
        logFileOpSimulationManager{logFileOpSimulationManager.value_or(defaultLogFileOpSimulationManager)},
        simulation{simulation.value_or(defaultSimulation)},
        libraries{libraries.value_or(defaultLibraries)},
        simulationConfigs{simulationConfigs}
    {}

    Config() :
        logLevel{defaultLogLevel},
        logFileOpSimulationManager{defaultLogFileOpSimulationManager},
        simulation{defaultSimulation},
        libraries{defaultLibraries},
        simulationConfigs{}
    {}

    const int logLevel;
    const std::string logFileOpSimulationManager;
    const std::string simulation;
    const std::string libraries;
    const SimulationConfigs simulationConfigs;

private:
    static constexpr int defaultLogLevel = 0;
    static constexpr char defaultLogFileOpSimulationManager[] = "opSimulationManager.log";
    static constexpr char defaultSimulation[] = "opSimulation";
    static constexpr char defaultLibraries[] = "lib";

    //-------------------------------------------------------------------------
    //! \brief Checks if the passed value is in between the minimum and maximum
    //!  	   log level boundaries.
    //! \param[in] logLevel Value to be checked.
    //! \returns logLevel if passed, defaultLogLevel if not
    //-------------------------------------------------------------------------
    static int CheckOrDefault(int logLevel)
    {
        return (logLevel >= 0 && logLevel <= 5) ? logLevel : defaultLogLevel;
    }
};

} // namespace Configuration
} // namespace SimulationManager
