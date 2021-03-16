/*******************************************************************************
* Copyright (c) 2017, 2019, 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "include/modelInterface.h"
#include "common/primitiveSignals.h"

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <tuple>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <map>
#include <boost/filesystem.hpp>

extern "C" {
#include "fmilib.h"
#include "fmuChecker.h"
}

#include "include/fmuHandlerInterface.h"

class AlgorithmFmuWrapperImplementation : public UnrestrictedModelInterface
{
public:
    const std::string COMPONENTNAME = "AlgorithmFmuWrapper";

    AlgorithmFmuWrapperImplementation(std::string componentName,
                                        bool isInit,
                                        int priority,
                                        int offsetTime,
                                        int responseTime,
                                        int cycleTime,
                                        WorldInterface* world,
                                        StochasticsInterface *stochastics,
                                        const ParameterInterface *parameters,
                                        PublisherInterface * const publisher,
                                        const CallbackInterface *callbacks,
                                        AgentInterface* agent);

    AlgorithmFmuWrapperImplementation(const AlgorithmFmuWrapperImplementation&) = delete;
    AlgorithmFmuWrapperImplementation(AlgorithmFmuWrapperImplementation&&) = delete;
    AlgorithmFmuWrapperImplementation& operator=(const AlgorithmFmuWrapperImplementation&) = delete;
    AlgorithmFmuWrapperImplementation& operator=(AlgorithmFmuWrapperImplementation&&) = delete;

    virtual ~AlgorithmFmuWrapperImplementation();

    virtual void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time);
    virtual void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time);
    virtual void Trigger(int time);

private:
    void ReadOutputValues();

    /*!
     * \brief Constructs an output base path from the core results directory and agent id
     */
    void SetOutputPath();

    /*!
     * \brief Sets up filenames for FMU data output and logging.
     */
    void SetupFilenames();

    /*!
     * \brief Sets up FMU checker log output.
     *
     * Directory "Log" will be created beneath FMU path.
     */
    void SetupLog();

    /*!
     * \brief Sets up FMU data log output.
     *
     * Directory "Output" will be created beneath FMU path.
     */
    void SetupOutput();

    /*!
     * \brief Sets up FMU unzipping paths and settings.
     *
     * Default is to have the fmu unzipped only once, but if the capability
     * \c canBeInstantiatedOnlyOncePerProcess is set to \c true, then every agent needs an
     * unzip folder on its own.
     *
     * \param[in]   individualUnzip     If false, unzip folder of existing FMU will be reused.
     */
    void SetupUnzip(const bool individualUnzip);

    /*!
     * \brief Initializes the FMU
     *
     * Calls the FMU checker which unzips and validates the FMU and parses the configuration
     * of the FMU. The type of the FMU is detectd and stored for later use. After that an
     * initializing simulation step is performed on the FMU.
     *
     * Supported types of FMU are ACC and SWW
     */
    void InitFmu();

    /*!
     * \brief Calls a FMIL function and handles the return status.
     *
     * \param[in]   func        Function to call with \c cdata as argument
     * \param[in]   cdata       The FMIL cdata structure
     * \param[in]   logPrefix   Prefix to prepend to log messages
     *
     * \note Calls \c fmi1_end_handling with \c cdata as argument on FMI error.
     */
    void HandleFmiStatus(const jm_status_enu_t& fmiStatus, const std::string& logPrefix);

    /*!
     * \brief Retrieve available variables from FMU
     */
    FmuVariables GetFmuVariables();

    /*!
     * \brief Converts the FMU variable type to a unique C++ type id.
     *
     * \param[in]   fmiType     FMILibrary specific type specifier
     *
     * \return  Hash code of the associated C++ type id
     */
    VariableType FmiTypeToCType(const fmi1_base_type_enu_t fmiType);

    /*!
     * \brief Converts the FMU variable type to a unique C++ type id.
     *
     * \param[in]   fmiType     FMILibrary specific type specifier
     *
     * \return  Hash code of the associated C++ type id
     */
    VariableType FmiTypeToCType(const fmi2_base_type_enu_t fmiType);

    /*!
     * \brief Safely cretes a directory.
     *
     * Create a directory with all missing directory along its path. Throws an error
     * if the directory cannot be created for any reason.
     *
     * \param[in]   path        Path of directory to be created (relative or absolute)
     *
     * \throws      std::runtime_error
     */
    void MkDirOrThrowError(const boost::filesystem::path& path);

    struct fmu_check_data_t cdata;   //!< check data to be passed around between the FMIL functions

    int startTime;               //!< Time of FMU start (simulation timebase)

    const CallbackInterface*  callbacks;     //!< callback interface

    std::string FMU_name;           //!< Name of FMU file without extension
    std::string FMU_absPath;        //!< Absolute path to the FMU file including
    std::string FMU_configPath;     //!< Relative path to the FMU file (originating in core config directory)
    std::string tmpPath;            //!< Temporary path used for unzipping the FMU archive
    std::string outputPath;         //!< Output base directory (inside core results directory)
    std::string logFileFullName;    //!< Absolute path to the log file
    std::string logFileName;        //!< Name of the log file
    std::string outputFileFullName; //!< Absolute path to the CSV output file
    std::string outputFileName;     //!< Name of the CSV output file

    std::string agentIdString;    //!< agent identifier as string

    //! Mapping from FMU variable name to it's FMI value reference and C++ type id. Provided to type-specific wrapper implementation on construction.
    FmuVariables fmuVariables;

    //! Mapping from FMI value reference and C++ type id to FmuWrapper value (union). Provided to type-specific wrapper implementation on construction.
    std::map<ValueReferenceAndType, FmuHandlerInterface::FmuValue> fmuVariableValues;

    bool isInitialized{false};                                                              //!< Specifies, if the FMU has already be initialized
    FmuHandlerInterface* fmuHandler = nullptr;                                              //!< Points to the instance of the FMU type-specific implementation
    std::string fmuType;        //!< Type of the FMU
};
