/********************************************************************************
 * Copyright (c) 2017-2018 ITK Engineering GmbH
 *               2018-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  SimulationConfigImporter.h
//! @brief This file contains the importer of the simulation configuration.
//-----------------------------------------------------------------------------
#pragma once

#include <QDomDocument>
#include <unordered_map>

#include "configurationFiles.h"
#include "simulationConfig.h"
#include "modelElements/parameters.h"
#include "parameterImporter.h"

namespace Importer {

//-----------------------------------------------------------------------------
/** \brief This class povides the import functionality
*   \details All methods in this class are static
*
*   \ingroup Importers
*/
//-----------------------------------------------------------------------------
class SimulationConfigImporter
{
public:
    /*!
    * \brief Imports the libraries used by the simulator
    *
    * @param[in]     librariesElement   Element containing the libraries information
    * @return        Libraries (defaulted if element or single elements are missing)
    * @see           defaultLibraryMapping
    */
    static ExperimentConfig::Libraries ImportLibraries(QDomElement rootElement);

    //Importing specific sections of the SimulationConfig
    /*!
    * \brief Imports the ExperimentConfig section of the simulation configuration
    *
    * @param[in]     experimentConfigElement    Element containing the information
    * @param[out]    experimentConfig           Struct into which the values get saved
    */
    static void ImportExperiment(QDomElement experimentElement,
                                       ExperimentConfig& experimentConfig);

    /*!
    * \brief Imports the ScenarioConfig section of the simulation configuration
    *
    * @param[in]     scenarioConfigElement    Element containing the information
    * @param[out]    scenarioConfig           Struct into which the values get saved
    */
    static void ImportScenario(QDomElement scenarioElement,
                                     const std::string configurationDir,
                                     ScenarioConfig& scenarioConfig);

    /*!
    * \brief Imports the EnvironmentConfig section of the simulation configuration
    *
    * @param[in]     environmentConfigElement    Element containing the information
    * @param[out]    environmentConfig           Struct into which the values get saved
    */
    static void ImportEnvironment(QDomElement environmentElement,
                                        EnvironmentConfig& environmentConfig);

    /*!
     * \brief ImportSpawnPointsConfig Imports the SpawnPointsConfig section of the simulation configuration
     * \param spawnersElement Element containing the information
     * \param spawnPointsInfo struct into which the values get saved
     * \return
     */
    static void ImportSpawners(const QDomElement& spawnersElement,
                                        SpawnPointLibraryInfoCollection &spawnPointsInfo);

    static void ImportObservations(const QDomElement& observationsElement,
                                        ObservationInstanceCollection& observations);

    //Overall import function
    /*!
    * \brief Imports the entire simulation configuration
    * \details Calls all sections specific methods and saves the result in the CombinationConfig
    *
    * @param[in]     configurationDir   Path of the configuration files
    * @param[in]     simulationConfigFile    Name of the SimulationConfig file
    * @param[out]    simulationConfig        Class into which the values get saved
    * @return        true, if successful
    */
    static bool Import(const std::string& configurationDir,
                       const std::string& simulationConfigFile,
                       Configuration::SimulationConfig& simulationConfig);

private:
    static std::string GetLibrary(const QDomElement& root, const std::string& key, const std::string& tag);
    static constexpr auto supportedConfigVersion = "0.8.2";

    //! \brief Identifier with correspondiong default values for mandatory libraries
    //! \note: The identifier is identical to the XML tag
    inline static const ExperimentConfig::Libraries defaultLibraryMapping =
    {
        {"DataBufferLibrary", "BasicDataBuffer"},
        {"EventDetectorLibrary", "EventDetector"},
        {"ManipulatorLibrary", "Manipulator"},
        {"WorldLibrary", "World"},
        {"StochasticsLibrary", "Stochastics"}
    };
};

} //namespace Importer
