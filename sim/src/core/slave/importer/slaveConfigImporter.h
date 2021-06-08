/*******************************************************************************
* Copyright (c) 2018, 2019, 2020 in-tech GmbH
*               2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  SlaveConfigImporter.h
//! @brief This file contains the importer of the slave configuration.
//-----------------------------------------------------------------------------
#pragma once

#include <QDomDocument>
#include <unordered_map>

#include "configurationFiles.h"
#include "slaveConfig.h"
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
class SlaveConfigImporter
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

    //Importing specific sections of the SlaveConfig
    /*!
    * \brief Imports the ExperimentConfig section of the slave configuration
    *
    * @param[in]     experimentConfigElement    Element containing the information
    * @param[out]    experimentConfig           Struct into which the values get saved
    */
    static void ImportExperiment(QDomElement experimentElement,
                                       ExperimentConfig& experimentConfig);

    /*!
    * \brief Imports the ScenarioConfig section of the slave configuration
    *
    * @param[in]     scenarioConfigElement    Element containing the information
    * @param[out]    scenarioConfig           Struct into which the values get saved
    */
    static void ImportScenario(QDomElement scenarioElement,
                                     const std::string configurationDir,
                                     ScenarioConfig& scenarioConfig);

    /*!
    * \brief Imports the EnvironmentConfig section of the slave configuration
    *
    * @param[in]     environmentConfigElement    Element containing the information
    * @param[out]    environmentConfig           Struct into which the values get saved
    */
    static void ImportEnvironment(QDomElement environmentElement,
                                        EnvironmentConfig& environmentConfig);

    /*!
     * \brief ImportSpawnPointsConfig Imports the SpawnPointConfig section of the slave configuration
     * \param spawnPointsConfigElement Element containing the information
     * \param spawnPointsInfo struct into which the values get saved
     * \return
     */
    static void ImportSpawners(const QDomElement& spawnersElement,
                                        SpawnPointLibraryInfoCollection& spawnPointsInfo);

    static void ImportObservations(const QDomElement& observationsElement,
                                        ObservationInstanceCollection& observations);

    //Overall import function
    /*!
    * \brief Imports the entire slave configuration
    * \details Calls all sections specific methods and saves the result in the CombiantionConfig
    *
    * @param[in]     configurationDir   Path of the configuration files
    * @param[in]     slaveConfigFile    Name of the SlaveConfig file
    * @param[out]    slaveConfig        Class into which the values get saved
    * @return        true, if successful
    */
    static bool Import(const std::string& configurationDir,
                       const std::string& slaveConfigFile,
                       Configuration::SlaveConfig& slaveConfig);

private:
    static std::string GetLibrary(const QDomElement& root, const std::string& key, const std::string& tag);
    static constexpr auto supportedConfigVersion = "0.8.1";

    //! \brief Identifier with correspondiong default values for mandatory libraries
    //! \note: The identifier is identical to the XML tag
    inline static const ExperimentConfig::Libraries defaultLibraryMapping =
    {
        {"DataStoreLibrary", "BasicDataStore"},
        {"EventDetectorLibrary", "EventDetector"},
        {"ManipulatorLibrary", "Manipulator"},
        {"WorldLibrary", "World"},
        {"StochasticsLibrary", "Stochastics"}
    };
};

} //namespace Importer
