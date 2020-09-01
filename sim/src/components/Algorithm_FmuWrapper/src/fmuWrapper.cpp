/*******************************************************************************
* Copyright (c) 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include <cstdint>
#include <memory>
#include <string>
#include <sstream>

#include <boost/filesystem.hpp>

#include <QtGlobal>

#include "include/agentInterface.h"
#include "include/fmuHandlerInterface.h"
#include "include/parameterInterface.h"

#include "common/commonTools.h"

#include "fmuWrapper.h"
#include "OsmpFmuHandler.h"

AlgorithmFmuWrapperImplementation::AlgorithmFmuWrapperImplementation(std::string componentName,
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
                                                                     AgentInterface* agent) :
    UnrestrictedModelInterface(componentName,
                               isInit,
                               priority,
                               offsetTime,
                               responseTime,
                               cycleTime,
                               stochastics,
                               world,
                               parameters,
                               publisher,
                               callbacks,
                               agent),
    startTime { 0 },
    callbacks  { callbacks }
{
    LOGDEBUG("constructor started");

    agentIdString   = std::to_string(GetAgent()->GetId());

    cdata.stepSize    = cycleTime / 1000.0;
    cdata.stepSizeSetByUser = 1;

    cdata.write_log_files    = 0;
    cdata.write_output_files = 0;
    cdata.log_file_name      = nullptr;
    cdata.output_file_name   = nullptr;

    auto fmuTypeParameter = parameters->GetParametersString().find("FmuType");
    if (fmuTypeParameter != parameters->GetParametersString().end())
    {
        fmuType = fmuTypeParameter->second;
    }
    else
    {
        LOGWARN("FmuType not set, defaulting to Generic");
        fmuType = "Generic";
    }

    FMU_fullName = parameters->GetParametersString().at("FmuPath");

    SetupFilenames();

    if (parameters->GetParametersBool().at("Logging"))
    {
        SetupLog();
    }

    if (parameters->GetParametersBool().at("CsvOutput"))
    {
        SetupOutput();
    }

    bool unzipOncePerInstance = parameters->GetParametersBool().at("UnzipOncePerInstance");

#ifdef unix
    if (!unzipOncePerInstance)
    {
        LOGWARN("UnzipOncePerInstance is forced to 'true' on Linux systems");
        unzipOncePerInstance = true;
    }
#endif

    SetupUnzip(unzipOncePerInstance);

    LOGDEBUG("constructor finished");
}

void AlgorithmFmuWrapperImplementation::SetupFilenames()
{
    // log file name: <fmuName>/LogFile-<agentId>.txt
    logFileName.assign("LogFile-");
    logFileName.append(agentIdString);
    logFileName.append(".txt");

    // output filename: <fmuName>/Output-<agentId>.txt
    outputFileName.assign("Output-");
    outputFileName.append(agentIdString);
    outputFileName.append(".txt");

    boost::filesystem::path fmuPath(FMU_fullName);

    FMU_path = fmuPath.parent_path().string();
    FMU_name = fmuPath.filename().string();

    cdata.FMUPath = FMU_fullName.c_str();
}

void AlgorithmFmuWrapperImplementation::SetupLog()
{
    boost::filesystem::path logPath = boost::filesystem::path(FMU_path) / "Log" / FMU_name;

    MkDirOrThrowError(logPath);

    // construct log file name including full path
    logPath = logPath / logFileName;

    logFileFullName = logPath.string();
    cdata.log_file_name = logFileFullName.c_str();

    cdata.write_log_files = 1;
}

void AlgorithmFmuWrapperImplementation::SetupOutput()
{
    boost::filesystem::path outputPath = boost::filesystem::path(FMU_path) / "Output" / FMU_name;

    MkDirOrThrowError(outputPath);

    // construct output file name including full path
    outputPath = outputPath / outputFileName;

    outputFileFullName = outputPath.string();
    cdata.output_file_name = outputFileFullName.c_str();

    cdata.write_output_files = 1;
}

void AlgorithmFmuWrapperImplementation::SetupUnzip(const bool individualUnzip)
{
    boost::filesystem::path unzipRoot = boost::filesystem::path(FMU_fullName).parent_path() / "Unzip" / FMU_name;

    if (individualUnzip)
    {
        // make unzip folder unique for each agent by adding agentId to the path
        unzipRoot = unzipRoot / agentIdString;
    }

    folderUnzip = unzipRoot.string();

    // make dir if not existing
    MkDirOrThrowError(unzipRoot);

    // set unzipPath to NULL to have the temporary folders removed in fmi1_end_handling
    cdata.unzipPath = nullptr;

    // store unzip directory in cdata
    // cdata.unzipPath != 0 prevents folder from being removed in end_handling
    cdata.unzipPath = folderUnzip.c_str();

    // set unzip folder name in working variable
    cdata.tmpPath = const_cast<char*>(folderUnzip.c_str());
}

AlgorithmFmuWrapperImplementation::~AlgorithmFmuWrapperImplementation()
{
    LOGDEBUG("destructor started");

    jm_status_enu_t status = jm_status_success;

    if (isInitialized)
    {
        cdata_global_ptr = &cdata;      // reassign global pointer, required for FMI 1.0 logging
        switch (cdata.version)
        {
        case fmi_version_1_enu:
            status = fmi1_end_handling(&cdata);
            break;
        case fmi_version_2_0_enu:
            status = fmi2_end_handling(&cdata);
            break;
        }
        isInitialized = false;
    }

    if (status == jm_status_error)
    {
        LOGERROR("Error in FMU end handling");
    }

    if (fmuHandler)
    {
        delete fmuHandler;
        fmuHandler = nullptr;
    }

    LOGDEBUG("destructor finished");
}

void AlgorithmFmuWrapperImplementation::UpdateInput(int localLinkId,
                                                    const std::shared_ptr<SignalInterface const> &data,
                                                    int time)
{
    if (!isInitialized)
    {
        InitFmu();
        startTime = time;
    }

    fmuHandler->UpdateInput(localLinkId, data, time);
}

void AlgorithmFmuWrapperImplementation::UpdateOutput(int localLinkId,
                                                     std::shared_ptr<SignalInterface const>& data,
                                                     int time)
{
    LOGDEBUG("UpdateOutput started");

    cdata_global_ptr = &cdata;      // reassign global pointer, required for FMI 1.0 logging

    if (!cdata.slave_initialized)
    {
        throw std::logic_error("FMU has to be initialized before calling UpdateOutput");
    }

    fmuHandler->UpdateOutput(localLinkId, data, time);

    LOGDEBUG("UpdateOutput finished");
}

void AlgorithmFmuWrapperImplementation::Trigger(int time)
{
    LOGDEBUG("Start of Trigger (time: " + std::to_string(time) + ")");

    if (!isInitialized)
    {
        InitFmu();
        startTime = time;
    }

    cdata_global_ptr = &cdata;      // reassign global pointer, required for FMI 1.0 logging

    fmuHandler->PreStep(time);

    jm_status_enu_t fmiStatus;

    switch (cdata.version)
    {
        case fmi_version_1_enu:
            fmiStatus = fmi1_cs_simulate_step(&cdata, (time - startTime) / 1000.0);
            break;
        case fmi_version_2_0_enu:
            fmiStatus = fmi2_cs_simulate_step(&cdata, (time - startTime) / 1000.0);
            break;
        default:
            throw std::runtime_error("Invalid FMI version");
    }

    HandleFmiStatus(fmiStatus, "simulation step");

    ReadOutputValues();

    fmuHandler->PostStep(time);

    LOGDEBUG("End of Trigger");
}

void AlgorithmFmuWrapperImplementation::InitFmu()
{
    LOGDEBUG("FMU init start");
    cdata_global_ptr = nullptr;

    auto fmiStatus = fmuChecker(&cdata); //! unpack FMU
    HandleFmiStatus(fmiStatus, "fmuChecker");

    switch (cdata.version)
    {
        case fmi_version_1_enu:
            fmiStatus = fmi1_cs_prep_simulate(&cdata); //! initialize simulation
            break;
        case fmi_version_2_0_enu:
            fmiStatus = fmi2_cs_prep_simulate(&cdata);
            break;
        default:
            throw std::runtime_error("Invalid FMI version");
    }

    HandleFmiStatus(fmiStatus, "prep simulate");

    isInitialized = true;

    fmuVariables = GetFmuVariables();

    if (fmuType == "OSMP")
    {
        LOGDEBUG("Instantiating OSMP FMU handler");
        fmuHandler = new OsmpFmuHandler(&cdata,
                                        GetWorld(),
                                        GetAgent(),
                                        callbacks,
                                        std::get<FMI2>(fmuVariables),
                                        &fmuVariableValues,
                                        GetParameters());
    }
    else
    {
        const std::string msg = "Unsupported FMU type: " + fmuType;
        LOGERROR(msg);
        throw std::runtime_error(msg);
    }

    LOGDEBUG("FMU init finished");
}

FmuVariables AlgorithmFmuWrapperImplementation::GetFmuVariables()
{
    FmuVariables fmuVariables;
    if (cdata.version == fmi_version_enu_t::fmi_version_1_enu)
    {
        fmuVariables.emplace<FMI1>();
        fmi1_import_variable_list_t *fmuVariableList = fmi1_import_get_variable_list(cdata.fmu1);
        size_t fmuVariableCount = fmi1_import_get_variable_list_size(fmuVariableList);

        for (size_t i = 0; i < fmuVariableCount; ++i)
        {
            fmi1_import_variable_t* fmuVar = fmi1_import_get_variable(fmuVariableList, static_cast<unsigned int>(i));
            const std::string fmuVarName(fmi1_import_get_variable_name(fmuVar));
            const fmi1_value_reference_t fmuValueReference = fmi1_import_get_variable_vr(fmuVar);
            const fmi1_base_type_enu_t fmuVarType = fmi1_import_get_variable_base_type(fmuVar);
            const VariableType variableType = FmiTypeToCType(fmuVarType);
            std::get<FMI1>(fmuVariables)[fmuVarName] = std::make_pair(fmuValueReference, variableType);
        }

        fmi1_import_free_variable_list(fmuVariableList);
    }
    else if (cdata.version == fmi_version_enu_t::fmi_version_2_0_enu)
    {
        fmuVariables.emplace<FMI2>();
        fmi2_import_variable_list_t *fmuVariableList = fmi2_import_get_variable_list(cdata.fmu2, 0);
        size_t fmuVariableCount = fmi2_import_get_variable_list_size(fmuVariableList);

        for (size_t i = 0; i < fmuVariableCount; ++i)
        {
            fmi2_import_variable_t* fmuVar = fmi2_import_get_variable(fmuVariableList, static_cast<unsigned int>(i));
            const std::string fmuVarName(fmi2_import_get_variable_name(fmuVar));
            const fmi2_value_reference_t fmuValueReference = fmi2_import_get_variable_vr(fmuVar);
            const fmi2_base_type_enu_t fmuVarType = fmi2_import_get_variable_base_type(fmuVar);
            const VariableType variableType = FmiTypeToCType(fmuVarType);
            std::get<FMI2>(fmuVariables)[fmuVarName] = std::make_pair(fmuValueReference, variableType);
        }

        fmi2_import_free_variable_list(fmuVariableList);
    }
    else
    {
        throw std::runtime_error("Invalid FMI version");
    }
    return fmuVariables;
}

VariableType AlgorithmFmuWrapperImplementation::FmiTypeToCType(const fmi1_base_type_enu_t fmiType)
{
    switch (fmiType)
    {
        case fmi1_base_type_bool:
            return VariableType::Bool;

        case fmi1_base_type_int:
            return VariableType::Int;

        case fmi1_base_type_real:
            return VariableType::Double;

        case fmi1_base_type_str:
            return VariableType::String;

        case fmi1_base_type_enum:
            return VariableType::Enum;

        default:
            throw std::runtime_error("Invalid type is not supported.");
    }
}

VariableType AlgorithmFmuWrapperImplementation::FmiTypeToCType(const fmi2_base_type_enu_t fmiType)
{
    switch (fmiType)
    {
        case fmi2_base_type_bool:
            return VariableType::Bool;

        case fmi2_base_type_int:
            return VariableType::Int;

        case fmi2_base_type_real:
            return VariableType::Double;

        case fmi2_base_type_str:
            return VariableType::String;

        case fmi2_base_type_enum:
            return VariableType::Enum;

        default:
            throw std::runtime_error("Invalid type is not supported.");
    }
}

void AlgorithmFmuWrapperImplementation::ReadOutputValues()
{
    if (cdata.version == fmi_version_enu_t::fmi_version_1_enu)
    {
        for (auto [fmuVarName, valRefAndType1] : std::get<FMI1>(fmuVariables))
        {
            // put value reference in array for get function
            fmi1_value_reference_t value_ref[1];
            VariableType dataType;
            std::tie(value_ref[0], dataType) = valRefAndType1;
            ValueReferenceAndType valRefAndType;
            valRefAndType.emplace<FMI1>(valRefAndType1);

            fmi1_boolean_t value_bool_out[1];
            fmi1_integer_t  value_int_out[1];
            fmi1_real_t  value_real_out[1];
            switch(dataType)
            {
            case VariableType::Bool:
                value_bool_out[0] = false;
                fmi1_import_get_boolean(cdata.fmu1, value_ref, 1, value_bool_out);
                fmuVariableValues[valRefAndType].boolValue = value_bool_out[0];
                LOGDEBUG("bool value '" + fmuVarName + "': " + std::to_string(value_bool_out[0]));
                break;
            case VariableType::Int:
                value_int_out[0] = 0;
                fmi1_import_get_integer(cdata.fmu1, value_ref, 1, value_int_out);
                fmuVariableValues[valRefAndType].intValue = value_int_out[0];
                LOGDEBUG("int value '" + fmuVarName + "': " + std::to_string(value_int_out[0]));
                break;
            case VariableType::Double:
                value_real_out[0] = 0.0;
                fmi1_import_get_real(cdata.fmu1, value_ref, 1, value_real_out);
                fmuVariableValues[valRefAndType].realValue = value_real_out[0];
                LOGDEBUG("real value '" + fmuVarName + "': " + std::to_string(value_real_out[0]));
                break;
            case VariableType::Enum:
                value_int_out[0] = 0;
                fmi1_import_get_integer(cdata.fmu1, value_ref, 1, value_int_out);
                fmuVariableValues[valRefAndType].intValue = value_int_out[0];
                LOGDEBUG("int value '" + fmuVarName + "': " + std::to_string(value_int_out[0]));
                break;
            default:
                throw std::logic_error("Unsupported datatype");
            }
        }
    }
    else if (cdata.version == fmi_version_enu_t::fmi_version_2_0_enu)
    {
        for (auto [fmuVarName, valRefAndType2] : std::get<FMI2>(fmuVariables))
        {
            // put value reference in array for get function
            fmi2_value_reference_t value_ref[1];
            VariableType dataType;
            std::tie(value_ref[0], dataType) = valRefAndType2;
            ValueReferenceAndType valRefAndType;
            valRefAndType.emplace<FMI2>(valRefAndType2);

            fmi2_boolean_t value_bool_out[1];
            fmi2_integer_t  value_int_out[1];
            fmi2_real_t  value_real_out[1];
            switch(dataType)
            {
            case VariableType::Bool:
                value_bool_out[0] = false;
                fmi2_import_get_boolean(cdata.fmu2, value_ref, 1, value_bool_out);
                fmuVariableValues[valRefAndType].boolValue = value_bool_out[0];
                LOGDEBUG("bool value '" + fmuVarName + "': " + std::to_string(value_bool_out[0]));
                break;
            case VariableType::Int:
                value_int_out[0] = 0;
                fmi2_import_get_integer(cdata.fmu2, value_ref, 1, value_int_out);
                fmuVariableValues[valRefAndType].intValue = value_int_out[0];
                LOGDEBUG("int value '" + fmuVarName + "': " + std::to_string(value_int_out[0]));
                break;
            case VariableType::Double:
                value_real_out[0] = 0.0;
                fmi2_import_get_real(cdata.fmu2, value_ref, 1, value_real_out);
                fmuVariableValues[valRefAndType].realValue = value_real_out[0];
                LOGDEBUG("real value '" + fmuVarName + "': " + std::to_string(value_real_out[0]));
                break;
            case VariableType::Enum:
                value_int_out[0] = 0;
                fmi2_import_get_integer(cdata.fmu2, value_ref, 1, value_int_out);
                fmuVariableValues[valRefAndType].intValue = value_int_out[0];
                LOGDEBUG("int value '" + fmuVarName + "': " + std::to_string(value_int_out[0]));
                break;
            default:
                throw std::logic_error("Unsupported datatype");
            }
        }
    }
}

void AlgorithmFmuWrapperImplementation::HandleFmiStatus(const jm_status_enu_t& fmiStatus, const std::string& logPrefix)
{
    switch(fmiStatus)
    {
        case jm_status_success:
            LOGDEBUG(logPrefix + " successfull");
            break;

        case jm_status_warning:
            LOGDEBUG(logPrefix + " returned with warning");
            break;

        case jm_status_error:
            fmi1_end_handling(&cdata);
            const std::string msg = logPrefix + " returned with error";
            LOGERROR(msg);
            throw std::runtime_error(msg);
    }
}

void AlgorithmFmuWrapperImplementation::MkDirOrThrowError(const boost::filesystem::path path)
{
    try
    {
        boost::filesystem::create_directories(path);
    }
    catch(boost::filesystem::filesystem_error& e)
    {
        const std::string msg = "could not create folder " + path.string() + ": " + e.what();
        LOGERROR(msg);
        throw std::runtime_error(msg);
    }
}
