/*******************************************************************************
* Copyright (c) 2018, 2019, 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "fmuWrapper.h"

#include <cstdint>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>

#include <QtGlobal>

#include "OsmpFmuHandler.h"
#include "common/commonTools.h"
#include "include/agentInterface.h"
#include "include/fmuHandlerInterface.h"
#include "include/parameterInterface.h"


std::string log_prefix(const std::string &agentIdString, const std::string &componentName)
{
    return "Agent " + agentIdString + ": Component " + componentName + ": ";
}

AlgorithmFmuWrapperImplementation::AlgorithmFmuWrapperImplementation(std::string componentName,
                                                                     bool isInit,
                                                                     int priority,
                                                                     int offsetTime,
                                                                     int responseTime,
                                                                     int cycleTime,
                                                                     WorldInterface *world,
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
    startTime{0},
    callbacks{callbacks},
    componentName{componentName}
{
    LOGDEBUG(log_prefix(std::to_string(GetAgent()->GetId()), componentName) + "constructor started");

    std::stringstream ss;
    ss << std::setw(4) << std::setfill('0');
    ss << GetAgent()->GetId();
    agentIdString = ss.str();

    cdata.stepSize    = cycleTime / 1000.0;
    cdata.stepSizeSetByUser = 1;

    cdata.write_log_files    = 0;
    cdata.write_output_files = 0;
    cdata.log_file_name      = nullptr;
    cdata.output_file_name   = nullptr;

    auto fmuTypeParameter = helper::map::query(parameters->GetParametersString(), "FmuType");
    if (fmuTypeParameter.has_value())
    {
        fmuType = fmuTypeParameter.value();
    }
    else
    {
        LOGWARN(log_prefix(agentIdString, componentName) + "FmuType not set, defaulting to Generic");
        fmuType = "Generic";
    }

    const auto fmuPath = helper::map::query(parameters->GetParametersString(), "FmuPath");
    THROWIFFALSE(fmuPath.has_value(), "Missing parameter \"FmuPath\"");
    FMU_configPath = fmuPath.value();

    SetupFilenames();
    SetOutputPath();

    const auto logging = helper::map::query(parameters->GetParametersBool(), "Logging").value_or(DEFAULT_LOGGING);
    if (logging)
    {
        SetupLog();
    }

    const auto csvOutput = helper::map::query(parameters->GetParametersBool(), "CsvOutput").value_or(DEFAULT_CSV_OUTPUT);
    if (csvOutput)
    {
        SetupOutput();
    }

    auto unzipOncePerInstance = helper::map::query(parameters->GetParametersBool(), "UnzipOncePerInstance").value_or(DEFAULT_UNZIP_ONCE_PER_INSTANCE);

#ifdef unix
    if (!unzipOncePerInstance)
    {
        LOGWARN(log_prefix(agentIdString, componentName) + "UnzipOncePerInstance is forced to 'true' on Linux systems");
        unzipOncePerInstance = true;
    }
#endif

    SetupUnzip(unzipOncePerInstance);

    LOGDEBUG(log_prefix(agentIdString, componentName) + "constructor finished");
}

void AlgorithmFmuWrapperImplementation::SetOutputPath()
{
    std::filesystem::path fmuPath = cdata.FMUPath;
    const std::string& resultsPath = GetParameters()->GetRuntimeInformation().directories.output;
    std::filesystem::path agentOutputPath = std::filesystem::path(resultsPath) / "FmuWrapper" / ("Agent" + agentIdString) / fmuPath.filename().replace_extension().string();
    outputPath = agentOutputPath.string();
}

void AlgorithmFmuWrapperImplementation::SetupFilenames()
{
    std::filesystem::path fmuPath(FMU_configPath);

    if (!fmuPath.is_absolute())
    {
        std::filesystem::path configBasePath(GetParameters()->GetRuntimeInformation().directories.configuration);
        fmuPath = configBasePath / fmuPath;
    }

    THROWIFFALSE(std::filesystem::exists(fmuPath), "FMU file '" + fmuPath.string() + "' doesn't exist");

    FMU_absPath = fmuPath.string();         // keep string for context struct
    cdata.FMUPath =  FMU_absPath.c_str();   // set FMU absolute path in context struct

    const std::string FMU_name = fmuPath.filename().replace_extension().string();

    // log file name: <fmuName>.log
    logFileName.assign(FMU_name);
    logFileName.append(".log");

    // output filename: <fmuName>.csv
    outputFileName.assign(FMU_name);
    outputFileName.append(".csv");
}

void AlgorithmFmuWrapperImplementation::SetupLog()
{
    std::filesystem::path logPath{outputPath};

    MkDirOrThrowError(logPath);

    // construct log file name including full path
    logPath = logPath / logFileName;

    logFileFullName = logPath.string();
    cdata.log_file_name = logFileFullName.c_str();

    cdata.write_log_files = 1;
}

void AlgorithmFmuWrapperImplementation::SetupOutput()
{
    std::filesystem::path csvPath{outputPath};

    MkDirOrThrowError(outputPath);

    // construct output file name including full path
    csvPath = csvPath / outputFileName;

    outputFileFullName = csvPath.string();
    cdata.output_file_name = outputFileFullName.c_str();

    cdata.write_output_files = 1;
}

void AlgorithmFmuWrapperImplementation::SetupUnzip(const bool individualUnzip)
{
    std::filesystem::path unzipRoot = std::filesystem::temp_directory_path() / std::tmpnam(nullptr);

    if (individualUnzip)
    {
        // make unzip folder unique for each agent by adding agentId to the path
        unzipRoot = unzipRoot / agentIdString;
    }

    // make dir if not existing
    MkDirOrThrowError(unzipRoot);

    // set unzipPath to NULL to have the temporary folders removed in fmi1_end_handling
    cdata.unzipPath = nullptr;

    // set unzip temporary path
    tmpPath = unzipRoot.string();
    cdata.tmpPath = const_cast<char*>(tmpPath.c_str());
}

AlgorithmFmuWrapperImplementation::~AlgorithmFmuWrapperImplementation()
{
    LOGDEBUG(log_prefix(agentIdString, componentName) + "destructor started");

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
        LOGERROR(log_prefix(agentIdString, componentName) + "Error in FMU end handling");
    }

    if (fmuHandler)
    {
        delete fmuHandler;
        fmuHandler = nullptr;
    }

    LOGDEBUG(log_prefix(agentIdString, componentName) + "destructor finished");
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
    LOGDEBUG(log_prefix(agentIdString, componentName) + "UpdateOutput started");

    cdata_global_ptr = &cdata;      // reassign global pointer, required for FMI 1.0 logging

    if (!cdata.slave_initialized)
    {
        throw std::logic_error("FMU has to be initialized before calling UpdateOutput");
    }

    fmuHandler->UpdateOutput(localLinkId, data, time);

    LOGDEBUG(log_prefix(agentIdString, componentName) + "UpdateOutput finished");
}

void AlgorithmFmuWrapperImplementation::Trigger(int time)
{
    LOGDEBUG(log_prefix(agentIdString, componentName) + "Start of Trigger (time: " + std::to_string(time) + ")");

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

    LOGDEBUG(log_prefix(agentIdString, componentName) + "End of Trigger");
}

void AlgorithmFmuWrapperImplementation::InitFmu()
{
    LOGDEBUG(log_prefix(agentIdString, componentName) + "FMU init start");
    cdata_global_ptr = nullptr;

    auto fmiStatus = fmuChecker(&cdata); //! unpack FMU
    HandleFmiStatus(fmiStatus, "fmuChecker");

    fmuVariables = GetFmuVariables();

    if (fmuType == "OSMP")
    {
        LOGDEBUG(log_prefix(agentIdString, componentName) + "Instantiating OSMP FMU handler");
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
        LOGERRORANDTHROW(log_prefix(agentIdString, componentName) + "Unsupported FMU type: " + fmuType)
    }

    switch (cdata.version)
    {
        case fmi_version_1_enu:
            HandleFmiStatus(fmi1_cs_prep_init(&cdata), "prep init");
            ReadOutputValues();
            fmuHandler->Init();
            fmiStatus = fmi1_cs_prep_simulate(&cdata);
            break;
        case fmi_version_2_0_enu:
            HandleFmiStatus(fmi2_cs_prep_init(&cdata), "prep init");
            ReadOutputValues();
            fmuHandler->Init();
            fmiStatus = fmi2_cs_prep_simulate(&cdata);
            break;
        default:
            throw std::runtime_error("Invalid FMI version");
    }

    HandleFmiStatus(fmiStatus, "prep simulate");

    isInitialized = true;

    LOGDEBUG(log_prefix(agentIdString, componentName) + "FMU init finished");
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
            fmi1_integer_t value_int_out[1];
            fmi1_real_t value_real_out[1];
            fmi2_string_t value_string_out[1];

            switch(dataType)
            {
            case VariableType::Bool:
                value_bool_out[0] = false;
                fmi1_import_get_boolean(cdata.fmu1, value_ref, 1, value_bool_out);
                fmuVariableValues[valRefAndType].boolValue = value_bool_out[0];
                LOGDEBUG(log_prefix(agentIdString, componentName) + "bool value '" + fmuVarName + "': " + std::to_string(value_bool_out[0]));
                break;
            case VariableType::Int:
                value_int_out[0] = 0;
                fmi1_import_get_integer(cdata.fmu1, value_ref, 1, value_int_out);
                fmuVariableValues[valRefAndType].intValue = value_int_out[0];
                LOGDEBUG(log_prefix(agentIdString, componentName) + "int value '" + fmuVarName + "': " + std::to_string(value_int_out[0]));
                break;
            case VariableType::Double:
                value_real_out[0] = 0.0;
                fmi1_import_get_real(cdata.fmu1, value_ref, 1, value_real_out);
                fmuVariableValues[valRefAndType].realValue = value_real_out[0];
                LOGDEBUG(log_prefix(agentIdString, componentName) + "real value '" + fmuVarName + "': " + std::to_string(value_real_out[0]));
                break;
            case VariableType::Enum:
                value_int_out[0] = 0;
                fmi1_import_get_integer(cdata.fmu1, value_ref, 1, value_int_out);
                fmuVariableValues[valRefAndType].intValue = value_int_out[0];
                LOGDEBUG(log_prefix(agentIdString, componentName) + "int value '" + fmuVarName + "': " + std::to_string(value_int_out[0]));
                break;
            case VariableType::String:
                value_string_out[0] = nullptr;
                fmi1_import_get_string(cdata.fmu1, value_ref, 1, value_string_out);
                fmuVariableValues[valRefAndType].stringValue = value_string_out[0];
                LOGDEBUG(log_prefix(agentIdString, componentName) + "string value '" + fmuVarName + "': " + std::string(value_string_out[0]));
                break;
            default:
                LOGERRORANDTHROW(log_prefix(agentIdString, componentName) + "Invalid FMI datatype during ReadOutputValues()");
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
            fmi2_integer_t value_int_out[1];
            fmi2_real_t value_real_out[1];
            fmi2_string_t value_string_out[1];

            switch(dataType)
            {
            case VariableType::Bool:
                value_bool_out[0] = false;
                fmi2_import_get_boolean(cdata.fmu2, value_ref, 1, value_bool_out);
                fmuVariableValues[valRefAndType].boolValue = value_bool_out[0];
                LOGDEBUG(log_prefix(agentIdString, componentName) + "bool value '" + fmuVarName + "': " + std::to_string(value_bool_out[0]));
                break;
            case VariableType::Int:
                value_int_out[0] = 0;
                fmi2_import_get_integer(cdata.fmu2, value_ref, 1, value_int_out);
                fmuVariableValues[valRefAndType].intValue = value_int_out[0];
                LOGDEBUG(log_prefix(agentIdString, componentName) + "int value '" + fmuVarName + "': " + std::to_string(value_int_out[0]));
                break;
            case VariableType::Double:
                value_real_out[0] = 0.0;
                fmi2_import_get_real(cdata.fmu2, value_ref, 1, value_real_out);
                fmuVariableValues[valRefAndType].realValue = value_real_out[0];
                LOGDEBUG(log_prefix(agentIdString, componentName) + "real value '" + fmuVarName + "': " + std::to_string(value_real_out[0]));
                break;
            case VariableType::Enum:
                value_int_out[0] = 0;
                fmi2_import_get_integer(cdata.fmu2, value_ref, 1, value_int_out);
                fmuVariableValues[valRefAndType].intValue = value_int_out[0];
                LOGDEBUG(log_prefix(agentIdString, componentName) + "int value '" + fmuVarName + "': " + std::to_string(value_int_out[0]));
                break;
            case VariableType::String:
                value_string_out[0] = nullptr;
                fmi2_import_get_string(cdata.fmu2, value_ref, 1, value_string_out);
                fmuVariableValues[valRefAndType].stringValue = value_string_out[0];
                LOGDEBUG(log_prefix(agentIdString, componentName) + "string value '" + fmuVarName + "': " + std::string(value_string_out[0]));
                break;
            default:
                LOGERRORANDTHROW(log_prefix(agentIdString, componentName) + "Invalid FMI datatype during ReadOutputValues()");
            }
        }
    }
}

void AlgorithmFmuWrapperImplementation::HandleFmiStatus(const jm_status_enu_t& fmiStatus, const std::string& logPrefix)
{
    switch(fmiStatus)
    {
    case jm_status_success:
        LOGDEBUG(log_prefix(agentIdString, componentName) + logPrefix + " successful");
        break;

        case jm_status_warning:
            LOGDEBUG(log_prefix(agentIdString, componentName) + logPrefix + " returned with warning");
            break;

        case jm_status_error:
            switch (cdata.version)
            {
                case fmi_version_1_enu:
                    fmi1_end_handling(&cdata);
                    break;
                case fmi_version_2_0_enu:
                    fmi2_end_handling(&cdata);
                    break;
                default:
                    LOGERRORANDTHROW(log_prefix(agentIdString, componentName) + "Invalid FMI version")
                }
                LOGERRORANDTHROW(log_prefix(agentIdString, componentName) + logPrefix + " returned with error")
        }
}

void AlgorithmFmuWrapperImplementation::MkDirOrThrowError(const std::filesystem::path& path)
{
    try
    {
        std::filesystem::create_directories(path);
    }
    catch(std::filesystem::filesystem_error& e)
    {
        LOGERRORANDTHROW(log_prefix(agentIdString, componentName) + "could not create folder " + path.string() + ": " + e.what());
    }
}

const FmuHandlerInterface *AlgorithmFmuWrapperImplementation::GetFmuHandler() const
{
    return fmuHandler;
}

void AlgorithmFmuWrapperImplementation::Init()
{
    if (isInitialized) return;
    InitFmu();
}

[[nodiscard]] const FmuVariables &AlgorithmFmuWrapperImplementation::GetFmuVariables() const
{
    if (!isInitialized) LOGERRORANDTHROW("Access to field in uninitialized context.")
    return fmuVariables;
}

[[nodiscard]] const fmu_check_data_t &AlgorithmFmuWrapperImplementation::GetCData() const
{
    if (!isInitialized) LOGERRORANDTHROW("Access to field in uninitialized context.")
    return cdata;
}

[[nodiscard]] const FmuHandlerInterface::FmuValue& AlgorithmFmuWrapperImplementation::GetValue(fmi2_value_reference_t valueReference, VariableType variableType) const
{
    if (!isInitialized) LOGERRORANDTHROW("Access to field in uninitialized context.")
    ValueReferenceAndType valueReferenceAndType;
    valueReferenceAndType.emplace<FMI2>(valueReference, variableType);
    return fmuVariableValues.at(valueReferenceAndType);
}
