/*******************************************************************************
* Copyright (c) 2019, 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "OsmpFmuHandler.h"

#include <cmath>
#include <src/common/acquirePositionSignal.h>

#include <QDir>

#include "common/dynamicsSignal.h"
#include "common/osiUtils.h"
#include "common/sensorDataSignal.h"
#include "common/speedActionSignal.h"
#include "common/stringSignal.h"
#include "common/trajectorySignal.h"
#include "core/slave/modules/World_OSI/WorldData.h"
#include "google/protobuf/util/json_util.h"
#include "variant_visitor.h"

extern "C"
{
#include "fmilib.h"
#include "fmuChecker.h"
}

std::string log_prefix(const std::string &agentIdString)
{
    return "Agent " + agentIdString + ": ";
}

void* decode_integer_to_pointer(fmi2_integer_t hi, fmi2_integer_t lo)
{
#if PTRDIFF_MAX == INT64_MAX
    union addrconv {
        struct {
            int lo;
            int hi;
        } base;
        unsigned long long address;
    } myaddr;
    myaddr.base.lo=lo;
    myaddr.base.hi=hi;
    return reinterpret_cast<void*>(myaddr.address);
#elif PTRDIFF_MAX == INT32_MAX
    return reinterpret_cast<void*>(lo);
#else
#error "Cannot determine 32bit or 64bit environment!"
#endif
}

void encode_pointer_to_integer(const void* ptr,fmi2_integer_t& hi,fmi2_integer_t& lo)
{
#if PTRDIFF_MAX == INT64_MAX
    union addrconv {
        struct {
            int lo;
            int hi;
        } base;
        unsigned long long address;
    } myaddr;
    myaddr.address=reinterpret_cast<unsigned long long>(ptr);
    hi=myaddr.base.hi;
    lo=myaddr.base.lo;
#elif PTRDIFF_MAX == INT32_MAX
    hi=0;
    lo=reinterpret_cast<int>(ptr);
#else
#error "Cannot determine 32bit or 64bit environment!"
#endif
}

OsmpFmuHandler::OsmpFmuHandler(fmu_check_data_t *cdata, WorldInterface *world, AgentInterface *agent, const CallbackInterface *callbacks, const Fmu2Variables &fmuVariables, std::map<ValueReferenceAndType, FmuHandlerInterface::FmuValue> *fmuVariableValues, const ParameterInterface *parameters) :
    FmuHandlerInterface(cdata, agent, callbacks),
    world(world),
    fmuVariableValues(fmuVariableValues),
    fmuVariables(fmuVariables),
    previousPosition(agent->GetPositionX(), agent->GetPositionY()),
    agentIdString(std::to_string(agent->GetId()))
{
    for (const auto& [key, value] : parameters->GetParametersString())
    {
        const auto pos = key.find('_');
        const auto type = key.substr(0, pos);
        const auto variableName = key.substr(pos+1);
        if (type == "Input" || type == "Output" || type == "Init")
        {
            const auto findResult = variableMapping.at(type).find(value);
            if (findResult != variableMapping.at(type).cend())
            {
                findResult->second = variableName;
            }
            else
            {
                LOGERRORANDTHROW(log_prefix(agentIdString) + "Unkown FMU \""+type+"\" variable \""+value+"\"")
            }
        }
    }

    auto writeSensorDataFlag = parameters->GetParametersBool().find("WriteJson_SensorData");
    if (writeSensorDataFlag != parameters->GetParametersBool().end())
    {
        writeSensorData = writeSensorDataFlag->second;
    }
    auto writeTraceSensorDataFlag = parameters->GetParametersBool().find("WriteTrace_SensorData");
    if (writeTraceSensorDataFlag != parameters->GetParametersBool().end())
    {
        writeTraceSensorData = writeTraceSensorDataFlag->second;
    }
    auto writeSensorViewFlag = parameters->GetParametersBool().find("WriteJson_SensorView");
    if (writeSensorViewFlag != parameters->GetParametersBool().end())
    {
        writeSensorView = writeSensorViewFlag->second;
    }
    auto writeTraceSensorViewFlag = parameters->GetParametersBool().find("WriteTrace_SensorView");
    if (writeTraceSensorViewFlag != parameters->GetParametersBool().end())
    {
        writeTraceSensorView = writeTraceSensorViewFlag->second;
    }
    auto writeSensorViewConfigFlag = parameters->GetParametersBool().find("WriteJson_SensorViewConfig");
    if (writeSensorViewConfigFlag != parameters->GetParametersBool().end())
    {
        writeSensorViewConfig = writeSensorViewConfigFlag->second;
    }
    auto writeTraceSensorViewConfigFlag = parameters->GetParametersBool().find("WriteTrace_SensorViewConfig");
    if (writeTraceSensorViewConfigFlag != parameters->GetParametersBool().end())
    {
        writeTraceSensorViewConfig = writeTraceSensorViewConfigFlag->second;
    }
    auto writeSensorViewConfigRequestFlag = parameters->GetParametersBool().find("WriteJson_SensorViewConfigRequest");
    if (writeSensorViewConfigRequestFlag != parameters->GetParametersBool().end())
    {
        writeSensorViewConfigRequest = writeSensorViewConfigRequestFlag->second;
    }
    auto writeTraceSensorViewConfigRequestFlag = parameters->GetParametersBool().find("WriteTrace_SensorViewConfigRequest");
    if (writeTraceSensorViewConfigRequestFlag != parameters->GetParametersBool().end())
    {
        writeTraceSensorViewConfigRequest = writeTraceSensorViewConfigRequestFlag->second;
    }
    auto writeGroundTruthFlag = parameters->GetParametersBool().find("WriteJson_GroundTruth");
    if (writeGroundTruthFlag != parameters->GetParametersBool().end())
    {
        writeGroundTruth = writeGroundTruthFlag->second;
    }
    auto writeTraceGroundTruthFlag = parameters->GetParametersBool().find("WriteTrace_GroundTruth");
    if (writeTraceGroundTruthFlag != parameters->GetParametersBool().end())
    {
        writeTraceGroundTruth = writeTraceGroundTruthFlag->second;
    }
#ifdef USE_EXTENDED_OSI
    auto writeTrafficCommandFlag = parameters->GetParametersBool().find("WriteJson_TrafficCommand");
    if (writeTrafficCommandFlag != parameters->GetParametersBool().end())
    {
        writeTrafficCommand = writeTrafficCommandFlag->second;
    }
    auto writeTraceTrafficCommandFlag = parameters->GetParametersBool().find("WriteTrace_TrafficCommand");
    if (writeTraceTrafficCommandFlag != parameters->GetParametersBool().end())
    {
        writeTraceTrafficCommand = writeTraceTrafficCommandFlag->second;
    }
    auto writeTrafficUpdateFlag = parameters->GetParametersBool().find("WriteJson_TrafficUpdate");
    if (writeTrafficUpdateFlag != parameters->GetParametersBool().end())
    {
        writeTrafficUpdate = writeTrafficUpdateFlag->second;
    }
    auto writeTraceTrafficUpdateFlag = parameters->GetParametersBool().find("WriteTrace_TrafficUpdate");
    if (writeTraceTrafficUpdateFlag != parameters->GetParametersBool().end())
    {
        writeTraceTrafficUpdate = writeTraceTrafficUpdateFlag->second;
    }
    auto writeMotionCommandFlag = parameters->GetParametersBool().find("WriteJson_MotionCommand");
    if (writeMotionCommandFlag != parameters->GetParametersBool().end())
    {
        writeMotionCommand = writeMotionCommandFlag->second;
    }
    auto writeTraceMotionCommandFlag = parameters->GetParametersBool().find("WriteTrace_MotionCommand");
    if (writeTraceMotionCommandFlag != parameters->GetParametersBool().end())
    {
        writeTraceMotionCommand = writeTraceMotionCommandFlag->second;
    }
    auto writeVehicleCommunicationDataFlag = parameters->GetParametersBool().find("WriteJson_VehicleCommunicationData");
    if (writeVehicleCommunicationDataFlag != parameters->GetParametersBool().end())
    {
        writeVehicleCommunicationData = writeVehicleCommunicationDataFlag->second;
    }
    auto writeTraceVehicleCommunicationDataFlag = parameters->GetParametersBool().find("WriteTrace_VehicleCommunicationData");
    if (writeTraceVehicleCommunicationDataFlag != parameters->GetParametersBool().end())
    {
        writeTraceVehicleCommunicationData = writeTraceVehicleCommunicationDataFlag->second;
    }

    bool writeJsonOutput = writeSensorData || writeSensorView || writeSensorViewConfig || writeSensorViewConfigRequest || writeTrafficCommand || writeTrafficUpdate || writeMotionCommand || writeVehicleCommunicationData || writeGroundTruth;
    bool writeTraceOutput = writeTraceSensorData || writeTraceSensorView || writeTraceSensorViewConfig || writeTraceSensorViewConfigRequest || writeTraceTrafficCommand || writeTraceTrafficUpdate || writeTraceMotionCommand || writeTraceVehicleCommunicationData || writeTraceGroundTruth;
#else
    bool writeJsonOutput = writeSensorData || writeSensorView || writeGroundTruth;
     bool writeTraceOutput = writeTraceSensorData || writeTraceSensorView || writeTraceGroundTruth;
#endif

    if (writeJsonOutput)
    {
        std::stringstream ss;
        ss << std::setw(4) << std::setfill('0');
        ss << agent->GetId();

        std::filesystem::path fmuPath = cdata->FMUPath;

        outputDir =
            QString::fromStdString(parameters->GetRuntimeInformation().directories.output) + QDir::separator() +
            "FmuWrapper" + QDir::separator() +
            "Agent" +  QString::fromStdString(ss.str()) + QDir::separator() +
            QString::fromStdString(fmuPath.filename().replace_extension().string()) + QDir::separator() + "JsonFiles";

        QDir directory{outputDir};
        if (!directory.exists())
        {
            directory.mkpath(outputDir);
        }
    }

    if (writeTraceOutput)
    {
        std::stringstream ss;
        ss << std::setw(4) << std::setfill('0');
        ss << agent->GetId();

        std::filesystem::path fmuPath = cdata->FMUPath;

        traceOutputDir = QString::fromStdString(parameters->GetRuntimeInformation().directories.output) + QDir::separator() +
                         "FmuWrapper" + QDir::separator() +
                         "Agent" +  QString::fromStdString(ss.str()) + QDir::separator() +
                         QString::fromStdString(fmuPath.filename().replace_extension().string()) + QDir::separator() + "BinaryTraceFiles";
        QDir directory{traceOutputDir};
        if (!directory.exists())
        {
            directory.mkpath(traceOutputDir);
        }
    }

    auto enforceDoubleBufferingFlag = parameters->GetParametersBool().find("EnforceDoubleBuffering");
    if (enforceDoubleBufferingFlag != parameters->GetParametersBool().end())
    {
        enforceDoubleBuffering = enforceDoubleBufferingFlag->second;
    }

    ParseFmuParameters(parameters);
}

void OsmpFmuHandler::SetSensorViewConfigRequest() {

    void *buffer = decode_integer_to_pointer(GetValue(fmuVariables.at(sensorViewConfigRequestVariable.value() + ".base.hi").first, VariableType::Int).intValue,
                                             GetValue(fmuVariables.at(sensorViewConfigRequestVariable.value() + ".base.lo").first, VariableType::Int).intValue);
    const auto size = static_cast<std::string::size_type>(GetValue(fmuVariables.at(sensorViewConfigRequestVariable.value() + ".size").first, VariableType::Int).intValue);

    previousSerializedSensorViewConfigRequest = serializedSensorViewConfigRequest;

    serializedSensorViewConfigRequest = {static_cast<char *>(buffer), size};
    sensorViewConfigRequest.ParseFromString(serializedSensorViewConfigRequest);
}

void OsmpFmuHandler::SetSensorViewConfig()
{
    // Apply requested config structure from FMU to sensorViewConfig in OpenPASS, which will be sent back to FMU
    serializedSensorViewConfig = serializedSensorViewConfigRequest;
    sensorViewConfig.ParseFromString(serializedSensorViewConfig);

    fmi2_integer_t fmuInputValues[3];
    fmi2_value_reference_t valueReferences[3] = {fmuVariables.at(sensorViewConfigVariable.value() + ".base.lo").first,
                                                 fmuVariables.at(sensorViewConfigVariable.value() + ".base.hi").first,
                                                 fmuVariables.at(sensorViewConfigVariable.value() + ".size").first};

    encode_pointer_to_integer(serializedSensorViewConfig.data(),
                              fmuInputValues[1],
                              fmuInputValues[0]);
    fmuInputValues[2] = serializedSensorView.length();
    fmi2_import_set_integer(cdata->fmu2,
                            valueReferences, // array of value reference
                            3,               // number of elements
                            fmuInputValues); // array of values
}

void OsmpFmuHandler::UpdateInput(int localLinkId, const std::shared_ptr<const SignalInterface> &data, [[maybe_unused]] int time)
{


    if (localLinkId == 2)
    {
        auto signal = std::dynamic_pointer_cast<SensorDataSignal const>(data);

        if (!signal)
        {
            LOGERRORANDTHROW(log_prefix(agentIdString) + "AlgorithmFmuHandler invalid signaltype")
        }

        sensorData = signal->sensorData;
    }
#ifdef USE_EXTENDED_OSI
    trafficCommands.try_emplace(time, std::make_unique<osi3::TrafficCommand>());
    osi3::utils::SetTimestamp(*trafficCommands[time], time);
    osi3::utils::SetVersion(*trafficCommands[time]);

    if (localLinkId == 10)
    {
        const auto signal = std::dynamic_pointer_cast<TrajectorySignal const>(data);
        if (signal && signal->componentState == ComponentState::Acting)
        {
            AddTrafficCommandActionFromOpenScenarioTrajectory(trafficCommands[time]->add_action(), signal->trajectory);
        }
    }
    else if (localLinkId == 11)
    {
        const auto signal = std::dynamic_pointer_cast<AcquirePositionSignal const>(data);
        if (signal && signal->componentState == ComponentState::Acting)
        {
            AddTrafficCommandActionFromOpenScenarioPosition(trafficCommands[time]->add_action(),
                                                            signal->position,
                                                            this->world,
                                                            [this](const std::string &message) { LOGERRORANDTHROW(log_prefix(agentIdString) + message); });
        }
    }
    else if (localLinkId == 12)
    {
        const auto signal = std::dynamic_pointer_cast<StringSignal const>(data);
        if (signal && signal->componentState == ComponentState::Acting)
        {
            trafficCommands[time]->add_action()->mutable_custom_action()->set_command(signal->payload);
        }
    }
    else if (localLinkId == 13)
    {
        const auto signal = std::dynamic_pointer_cast<SpeedActionSignal const>(data);
        if (signal && signal->componentState == ComponentState::Acting)
        {
            trafficCommands[time]->add_action()->mutable_speed_action()->set_absolute_target_speed(signal->targetSpeed);
        }
    }

    trafficCommand = *trafficCommands[time];

    if (sensorViewConfigRequestVariable.has_value())
    {
        SetSensorViewConfigRequest();

        if (previousSerializedSensorViewConfigRequest != serializedSensorViewConfigRequest)
        {
            SetSensorViewConfig();

            if (writeSensorViewConfig)
            {
                WriteJson(sensorViewConfig, "SensorViewConfig-" + QString::number(time) + ".json");
            }
            if (writeSensorViewConfigRequest)
            {
                WriteJson(sensorViewConfigRequest, "SensorViewConfigRequest-" + QString::number(time) + ".json");
            }
        }
    }
#endif
}

constexpr double EPSILON = 0.001;

void OsmpFmuHandler::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const>& data, int time)
{

    if (localLinkId == 6)
    {
        data = std::make_shared<SensorDataSignal const>(sensorData);
    }
#ifdef USE_EXTENDED_OSI
    else if (localLinkId == 0)
    {
        double acceleration{0.0};
        double velocity{0.0};
        double positionX{0.0};
        double positionY{0.0};
        double yaw{0.0};
        double yawRate{0.0};
        double roll{0.0};
        double steeringWheelAngle{0.0};
        double centripetalAcceleration{0.0};
        double travelDistance{0.0};

        if (motionCommandVariable.has_value())
        {
            setlevel4to5::DynamicState dynamicState;
            if(motionCommand.trajectory().trajectory_point_size() > 0)
            {
                dynamicState = motionCommand.trajectory().trajectory_point(0);
            }
            else
            {
                dynamicState = motionCommand.current_state();
            }

            acceleration = dynamicState.acceleration();
            velocity = dynamicState.velocity();
            positionX = dynamicState.position_x();
            positionY = dynamicState.position_y();
            yaw = dynamicState.heading_angle();
        }
        else if (trafficUpdateVariable.has_value())
        {
            const auto& baseMoving = trafficUpdate.mutable_update()->mutable_base();
            velocity = std::sqrt(baseMoving->velocity().x() * baseMoving->velocity().x() + baseMoving->velocity().y() * baseMoving->velocity().y());
            yaw = baseMoving->orientation().yaw();
            roll = baseMoving->orientation().roll();
            acceleration = baseMoving->acceleration().x() * std::cos(yaw) + baseMoving->acceleration().y() * std::sin(yaw);
            centripetalAcceleration = -baseMoving->acceleration().x() * std::sin(yaw) + baseMoving->acceleration().y() * std::cos(yaw);
            positionX = baseMoving->position().x();
            positionY = baseMoving->position().y();
            yawRate = baseMoving->orientation_rate().yaw();

        }
        else
        {
            LOGERRORANDTHROW(log_prefix(agentIdString) + "Cannot provide DynamicsSignal, as neither TrafficUpdate nor MotionCommand are connected");
        }

        if (std::abs(positionX) < EPSILON && std::abs(positionY) < EPSILON)
        {
            positionX = previousPosition.x;
            positionY = previousPosition.y;
        }

        double deltaX = positionX - previousPosition.x;
        double deltaY = positionY - previousPosition.y;
        travelDistance = std::sqrt(deltaX * deltaX + deltaY * deltaY);
        previousPosition = {positionX, positionY};

        data = std::make_shared<DynamicsSignal const>(ComponentState::Acting,
                                                      acceleration,
                                                      velocity,
                                                      positionX,
                                                      positionY,
                                                      yaw,
                                                      yawRate,
                                                      roll,
                                                      steeringWheelAngle,
                                                      centripetalAcceleration,
                                                      travelDistance);
    }
#endif
}

void OsmpFmuHandler::Init()
{
    SetFmuParameters();
    if (groundTruthVariable.has_value())
    {
        auto* worldData = static_cast<OWL::Interfaces::WorldData*>(world->GetWorldData());
        auto& groundTruth = worldData->GetOsiGroundTruth();

        fmi2_integer_t fmuInputValues[3];
        fmi2_value_reference_t valueReferences[3] = {fmuVariables.at(groundTruthVariable.value()+".base.lo").first,
                                                     fmuVariables.at(groundTruthVariable.value()+".base.hi").first,
                                                     fmuVariables.at(groundTruthVariable.value()+".size").first};

        groundTruth.SerializeToString(&serializedGroundTruth);
        encode_pointer_to_integer(serializedGroundTruth.data(),
                                  fmuInputValues[1],
                                  fmuInputValues[0]);
        fmuInputValues[2] = serializedGroundTruth.length();

        fmi2_import_set_integer(cdata->fmu2,
                                valueReferences,     // array of value reference
                                3,                   // number of elements
                                fmuInputValues);     // array of values
        if (writeGroundTruth)
        {
            WriteJson(groundTruth, "GroundTruth.json");
        }
        if (writeTraceGroundTruth)
        {
            AppendMessages(appendedSerializedGroundTruth, serializedGroundTruth);
            WriteBinaryTrace(appendedSerializedGroundTruth, "GroundTruth", 1000/cycleTime);
        }
    }
    if (sensorViewConfigRequestVariable.has_value())
    {
        SetSensorViewConfigRequest();
        SetSensorViewConfig();

        if (writeSensorViewConfig)
        {
            WriteJson(sensorViewConfig, "SensorViewConfig.json");
        }
        if (writeTraceSensorViewConfig)
        {
            AppendMessages(appendedSerializedSensorViewConfig, serializedSensorViewConfig);
            WriteBinaryTrace(appendedSerializedSensorViewConfig, "SensorViewConfig", 1000/cycleTime);
        }
        if (writeSensorViewConfigRequest)
        {
            WriteJson(sensorViewConfigRequest, "SensorViewConfigRequest.json");
        }
        if (writeTraceSensorViewConfigRequest)
        {
            AppendMessages(appendedSerializedSensorViewConfigRequest, serializedSensorViewConfigRequest);
            WriteBinaryTrace(appendedSerializedSensorViewConfigRequest, "SensorViewConfigRequest", 1000/cycleTime);
        }
    }
    else
    {
        sensorViewConfig = GenerateDefaultSensorViewConfiguration();
    }
}

void OsmpFmuHandler::PreStep(int time)
{
    if (sensorViewVariable)
    {
        auto* worldData = static_cast<OWL::Interfaces::WorldData*>(world->GetWorldData());
        auto sensorView = worldData->GetSensorView(sensorViewConfig, agent->GetId());

        SetSensorViewInput(*sensorView);
        if (writeSensorView)
        {
            WriteJson(*sensorView, "SensorView-" + QString::number(time) + ".json");
        }
        if (writeTraceSensorView)
        {
            AppendMessages(appendedSerializedSensorView, serializedSensorView);
            WriteBinaryTrace(appendedSerializedSensorView, "SensorView", time);
        }
    }
    if (sensorDataInVariable)
    {
        SetSensorDataInput(sensorData);
        if (writeSensorData)
        {
            WriteJson(sensorData, "SensorData-" + QString::number(time) + ".json");
        }
        if (writeTraceSensorData)
        {
            AppendMessages(appendedSerializedSensorData, serializedSensorData);
            WriteBinaryTrace(appendedSerializedSensorData, "SensorData", time);
        }
    }
#ifdef USE_EXTENDED_OSI
    if (trafficCommandVariable)
    {
        SetTrafficCommandInput(trafficCommand);
        if (writeTrafficCommand)
        {
            WriteJson(trafficCommand, "TrafficCommand-" + QString::number(time) + ".json");
        }
        if (writeTraceTrafficCommand)
        {
            AppendMessages(appendedSerializedTrafficCommand, serializedTrafficCommand);
            WriteBinaryTrace(appendedSerializedTrafficCommand, "TrafficCommand", time);
        }
    }
    if (vehicleCommunicationDataVariable)
    {
        auto hostVehicleData = vehicleCommunicationData.mutable_host_vehicle_data();
        hostVehicleData->mutable_location()->CopyFrom(sensorData.host_vehicle_location());
        hostVehicleData->mutable_location_rmse()->CopyFrom(sensorData.host_vehicle_location_rmse());

        SetVehicleCommunicationDataInput(vehicleCommunicationData);
        if (writeVehicleCommunicationData)
        {
            WriteJson(vehicleCommunicationData, "VehicleCommunicationData-" + QString::number(time) + ".json");
        }
        if (writeTraceVehicleCommunicationData)
        {
            AppendMessages(appendedSerializedVehicleCommunicationData, serializedVehicleCommunicationData);
            WriteBinaryTrace(appendedSerializedVehicleCommunicationData, "VehicleCommunicationData", time);
        }
    }
#endif
}

void OsmpFmuHandler::PostStep(int time)
{
    if (sensorDataOutVariable)
    {
        GetSensorData();
        if (writeSensorData)
        {
            WriteJson(sensorData, "SensorData-" + QString::number(time) + ".json");
        }
        if (writeTraceSensorData)
        {
            AppendMessages(appendedSerializedSensorData, serializedSensorData);
            WriteBinaryTrace(appendedSerializedSensorData, "SensorData", time);
        }
    }
#ifdef USE_EXTENDED_OSI
    if (motionCommandVariable)
    {
        GetMotionCommand();
        if (writeMotionCommand)
        {
            WriteJson(motionCommand, "MotionCommand-" + QString::number(time) + ".json");
        }
    }

    if (trafficUpdateVariable)
    {
        GetTrafficUpdate();
        if (writeTrafficUpdate)
        {
            WriteJson(trafficUpdate, "TrafficUpdate-" + QString::number(time) + ".json");
        }
    }
#endif
}

FmuHandlerInterface::FmuValue& OsmpFmuHandler::GetValue(fmi2_value_reference_t valueReference, VariableType variableType) const
{
    ValueReferenceAndType valueReferenceAndType;
    valueReferenceAndType.emplace<FMI2>(valueReference, variableType);
    return fmuVariableValues->at(valueReferenceAndType);
}

void OsmpFmuHandler::SetSensorViewInput(const osi3::SensorView& data)
{
    std::swap(serializedSensorView, previousSerializedSensorView);
    fmi2_integer_t fmuInputValues[3];
    fmi2_value_reference_t valueReferences[3] = {fmuVariables.at(sensorViewVariable.value()+".base.lo").first,
                                                 fmuVariables.at(sensorViewVariable.value()+".base.hi").first,
                                                 fmuVariables.at(sensorViewVariable.value()+".size").first};

    data.SerializeToString(&serializedSensorView);
    encode_pointer_to_integer(serializedSensorView.data(),
                              fmuInputValues[1],
                              fmuInputValues[0]);
    fmuInputValues[2] = serializedSensorView.length();

    fmi2_import_set_integer(cdata->fmu2,
                            valueReferences,     // array of value reference
                            3,                   // number of elements
                            fmuInputValues);     // array of values
}

void OsmpFmuHandler::SetSensorDataInput(const osi3::SensorData& data)
{
    std::swap(serializedSensorData, previousSerializedSensorData);
    fmi2_integer_t fmuInputValues[3];
    fmi2_value_reference_t valueReferences[3] = {fmuVariables.at(sensorDataInVariable.value()+".base.lo").first,
                                                 fmuVariables.at(sensorDataInVariable.value()+".base.hi").first,
                                                 fmuVariables.at(sensorDataInVariable.value()+".size").first};

    data.SerializeToString(&serializedSensorData);
    encode_pointer_to_integer(serializedSensorData.data(),
                              fmuInputValues[1],
                              fmuInputValues[0]);
    fmuInputValues[2] = serializedSensorData.length();

    fmi2_import_set_integer(cdata->fmu2,
                            valueReferences,     // array of value reference
                            3,                   // number of elements
                            fmuInputValues);     // array of values
}

void OsmpFmuHandler::GetSensorData()
{
    void* buffer = decode_integer_to_pointer(GetValue(fmuVariables.at(sensorDataOutVariable.value()+".base.hi").first, VariableType::Int).intValue,
                                             GetValue(fmuVariables.at(sensorDataOutVariable.value()+".base.lo").first, VariableType::Int).intValue);

    if (enforceDoubleBuffering && buffer != nullptr && buffer == previousSensorData)
    {
        const std::string msg = "FMU has no double buffering";
        LOGERROR(log_prefix(agentIdString) + msg);
        throw std::runtime_error(msg);
    }

    previousSensorData = buffer;
    sensorData.ParseFromArray(buffer, GetValue(fmuVariables.at(sensorDataOutVariable.value()+".size").first, VariableType::Int).intValue);
}

#ifdef USE_EXTENDED_OSI
void OsmpFmuHandler::SetTrafficCommandInput(const osi3::TrafficCommand& data)
{
    std::swap(serializedTrafficCommand, previousSerializedTrafficCommand);
    fmi2_integer_t fmuInputValues[3];
    fmi2_value_reference_t valueReferences[3] = {fmuVariables.at(trafficCommandVariable.value()+".base.lo").first,
                                                 fmuVariables.at(trafficCommandVariable.value()+".base.hi").first,
                                                 fmuVariables.at(trafficCommandVariable.value()+".size").first};

    data.SerializeToString(&serializedTrafficCommand);
    encode_pointer_to_integer(serializedTrafficCommand.data(),
                              fmuInputValues[1],
                              fmuInputValues[0]);

    auto length = serializedTrafficCommand.length();

    if (length > std::numeric_limits<fmi2_integer_t>::max())
    {
        LOGERRORANDTHROW(log_prefix(agentIdString) + "Serialized buffer length of osi::TrafficCommand exceeds fmi2 integer size");
    }

    fmuInputValues[2] = static_cast<fmi2_integer_t>(length);

    fmi2_import_set_integer(cdata->fmu2,
                            valueReferences,     // array of value reference
                            3,                   // number of elements
                            fmuInputValues);     // array of values
}

void OsmpFmuHandler::SetVehicleCommunicationDataInput(const setlevel4to5::VehicleCommunicationData& data)
{
    std::swap(serializedVehicleCommunicationData, previousSerializedVehicleCommunicationData);
    fmi2_integer_t fmuInputValues[3];
    fmi2_value_reference_t valueReferences[3] = {fmuVariables.at(vehicleCommunicationDataVariable.value()+".base.lo").first,
                                                 fmuVariables.at(vehicleCommunicationDataVariable.value()+".base.hi").first,
                                                 fmuVariables.at(vehicleCommunicationDataVariable.value()+".size").first};

    data.SerializeToString(&serializedVehicleCommunicationData);
    encode_pointer_to_integer(serializedVehicleCommunicationData.data(),
                              fmuInputValues[1],
                              fmuInputValues[0]);
    fmuInputValues[2] = serializedVehicleCommunicationData.length();

    fmi2_import_set_integer(cdata->fmu2,
                            valueReferences,     // array of value reference
                            3,                   // number of elements
                            fmuInputValues);     // array of values
}

void logAndThrow(const std::string& message, const std::function<void(const std::string&)> &errorCallback) noexcept(false) {
    if (errorCallback) errorCallback(message);
    throw std::runtime_error(message);
}

void OsmpFmuHandler::AddTrafficCommandActionFromOpenScenarioTrajectory(osi3::TrafficAction *trafficAction, const openScenario::Trajectory& trajectory)
{
    if (trajectory.timeReference.has_value()) {
        auto trajectoryAction = trafficAction->mutable_follow_trajectory_action();
        for (const auto& trajectoryPoint : trajectory.points)
        {
            auto statePoint = trajectoryAction->add_trajectory_point();
            statePoint->mutable_timestamp()->set_seconds(static_cast<google::protobuf::int64>(trajectoryPoint.time));
            statePoint->mutable_timestamp()->set_nanos(static_cast<google::protobuf::uint32>(std::fmod(trajectoryPoint.time * 1e9, 1e9)));
            statePoint->mutable_position()->set_x(trajectoryPoint.x);
            statePoint->mutable_position()->set_y(trajectoryPoint.y);
            statePoint->mutable_orientation()->set_yaw(trajectoryPoint.yaw);
        }
    } else {
        auto followPathAction = trafficAction->mutable_follow_path_action();
        for (const auto& trajectoryPoint : trajectory.points)
        {
            auto statePoint = followPathAction->add_path_point();
            statePoint->mutable_position()->set_x(trajectoryPoint.x);
            statePoint->mutable_position()->set_y(trajectoryPoint.y);
            statePoint->mutable_orientation()->set_yaw(trajectoryPoint.yaw);
        }
    }
}

void OsmpFmuHandler::AddTrafficCommandActionFromOpenScenarioPosition(osi3::TrafficAction *trafficAction,
                                                                     const openScenario::Position &position,
                                                                     WorldInterface *const worldInterface,
                                                                     const std::function<void(const std::string &)> &errorCallback)
{
    auto acquireGlobalPositionAction = trafficAction->mutable_acquire_global_position_action();

    std::visit(variant_visitor{
                   [&acquireGlobalPositionAction](const openScenario::WorldPosition &worldPosition) {
                       acquireGlobalPositionAction->mutable_position()->set_x(worldPosition.x);
                       acquireGlobalPositionAction->mutable_position()->set_y(worldPosition.y);
                       if (worldPosition.z.has_value())
                           acquireGlobalPositionAction->mutable_position()->set_z(worldPosition.z.value());
                       if (worldPosition.r.has_value())
                           acquireGlobalPositionAction->mutable_orientation()->set_roll(worldPosition.r.value());
                       if (worldPosition.p.has_value())
                           acquireGlobalPositionAction->mutable_orientation()->set_pitch(worldPosition.p.value());
                       if (worldPosition.h.has_value())
                           acquireGlobalPositionAction->mutable_orientation()->set_yaw(worldPosition.h.value());
                   },
                   [&worldInterface, &errorCallback, &acquireGlobalPositionAction](const openScenario::RelativeObjectPosition &relativeObjectPosition) {
                       const auto entityRef = relativeObjectPosition.entityRef;
                       const auto referencedAgentInterface = worldInterface->GetAgentByName(entityRef);
                       if (!referencedAgentInterface) errorCallback("Reference to agent '" + entityRef + "' could not be resolved");

                       acquireGlobalPositionAction->mutable_position()->set_x(referencedAgentInterface->GetPositionX() + relativeObjectPosition.dx);
                       acquireGlobalPositionAction->mutable_position()->set_y(referencedAgentInterface->GetPositionY() + relativeObjectPosition.dy);
                       if (relativeObjectPosition.orientation.has_value())
                       {
                           const auto orientation = relativeObjectPosition.orientation.value();
                           if (orientation.r.has_value())
                               acquireGlobalPositionAction->mutable_orientation()->set_roll(orientation.r.value());
                           if (orientation.p.has_value())
                               acquireGlobalPositionAction->mutable_orientation()->set_pitch(orientation.p.value());
                           if (orientation.h.has_value())
                               acquireGlobalPositionAction->mutable_orientation()->set_yaw(orientation.h.value());
                       }
                   },
                   [&errorCallback](auto &&other) {
                       errorCallback("Position variant not supported for 'openScenario::AcquirePositionAction'");
                   }},
               position);
}
#endif

template <typename GetParametersType, typename FmuTypeParameters, typename UnderlyingType>
void OsmpFmuHandler::ParseFmuParametersByType(const GetParametersType getParametersType,
                                              const ParameterInterface *parameterInterface,
                                              const FmuTypeParameters fmuTypeParameters,
                                              const TypeContainer<UnderlyingType> typeContainer)
{
    for (const auto &[key, value] : std::invoke(getParametersType, parameterInterface))
    {
        if (key.substr(0, 10) != "Parameter_")
            continue;
        auto variableName = key.substr(10);
        UnderlyingType val = value;
        if (std::smatch transformationMatch; std::regex_search(variableName, transformationMatch, transformationRegex))
        {
            const std::string transformationType = transformationMatch[1];
            if (parameterTransformations.find(transformationType) == end(parameterTransformations))
                LOGERRORANDTHROW(log_prefix(agentIdString) + "Unknown transformation type: " + transformationType);
            if (transformationType == "TransformList" && typeContainer.variableType != VariableType::String)
                LOGERRORANDTHROW(log_prefix(agentIdString) + "'TransformList' is only applicable to strings");
            const std::string transformationRule = transformationMatch[2];
            auto mappingEntry = parameterTransformationMappings.find(transformationRule);
            if (mappingEntry == end(parameterTransformationMappings))
                LOGERRORANDTHROW(log_prefix(agentIdString) + "Unknown mapping rule: " + transformationRule);
            variableName = transformationMatch[3];

            //TODO: support mapping to different types
            auto mapping = std::any_cast<std::function<UnderlyingType(const UnderlyingType &)>>(mappingEntry->second);
            if (transformationType == "Transform")
            {
                val = mapping(value);
            }
            if constexpr (std::is_same_v<UnderlyingType, std::string>)
            {
                if (transformationType == "TransformList")
                {
                    std::string s = std::string(value);
                    size_t pos = 0;
                    std::string token;
                    std::vector<UnderlyingType> transformedList;
                    while ((pos = s.find(',')) != std::string::npos)
                    {
                        auto mappedValue = mapping(s.substr(0, pos));
                        transformedList.emplace_back(mappedValue);
                        s.erase(0, pos + 1);
                    }
                    transformedList.emplace_back(mapping(s));
                    val = std::accumulate(++cbegin(transformedList), cend(transformedList), *cbegin(transformedList), [](const std::string &left, const std::string &right) { return left + "," + right; });
                }
            }
        }
        const auto foundVariable = fmuVariables.find(variableName);
        if (foundVariable == fmuVariables.cend())
            LOGERRORANDTHROW(log_prefix(agentIdString) + "No variable with name \"" + variableName + "\" found in the FMU");
        if (foundVariable->second.second != typeContainer.variableType)
            LOGERRORANDTHROW(log_prefix(agentIdString) + "Variable \"" + variableName + "\" has different type in FMU");
        std::invoke(fmuTypeParameters, this).emplace_back(val, foundVariable->second.first);
    }
}

void OsmpFmuHandler::ParseFmuParameters(const ParameterInterface *parameters)
{
    ParseFmuParametersByType(&ParameterInterface::GetParametersInt, parameters, &OsmpFmuHandler::fmuIntegerParameters, TypeContainer<int>{VariableType::Int});
    ParseFmuParametersByType(&ParameterInterface::GetParametersBool, parameters, &OsmpFmuHandler::fmuBoolParameters, TypeContainer<bool>{VariableType::Bool});
    ParseFmuParametersByType(&ParameterInterface::GetParametersDouble, parameters, &OsmpFmuHandler::fmuDoubleParameters, TypeContainer<double>{VariableType::Double});
    ParseFmuParametersByType(&ParameterInterface::GetParametersString, parameters, &OsmpFmuHandler::fmuStringParameters, TypeContainer<std::string>{VariableType::String});
}

void OsmpFmuHandler::SetFmuParameters()
{
    fmi2_status_t fmiStatus = fmi2_status_ok;

    fmi2_value_reference_t realvrs[1];
    fmi2_real_t realData[1];
    for (const auto &fmuParameter : fmuDoubleParameters)
    {
        realData[0] = fmuParameter.value;
        realvrs[0] = fmuParameter.valueReference;
        fmiStatus = fmi2_import_set_real(cdata->fmu2,
                                         realvrs,
                                         1,
                                         realData);
    }

    if (fmiStatus == fmi2_status_warning)
    {
        LOGWARN(log_prefix(agentIdString) + "setting FMI variables returned warning");
    }
    else if (fmiStatus == fmi2_status_error)
    {
        LOGERROR(log_prefix(agentIdString) + "setting FMI variables returned error");
        throw std::logic_error("Error setting FMI variables");
    }

    fmi2_value_reference_t intvrs[1];
    fmi2_integer_t intData[1];
    for (const auto& fmuParameter : fmuIntegerParameters)
    {
        intData[0] = fmuParameter.value;
        intvrs[0] = fmuParameter.valueReference;
        fmiStatus = fmi2_import_set_integer(cdata->fmu2,
                                            intvrs,
                                            1,
                                            intData);
    }

    if (fmiStatus == fmi2_status_warning)
    {
        LOGWARN(log_prefix(agentIdString) + "setting FMI variables returned warning");
    }
    else if (fmiStatus == fmi2_status_error)
    {
        LOGERROR(log_prefix(agentIdString) + "setting FMI variables returned error");
        throw std::logic_error("Error setting FMI variables");
    }

    fmi2_value_reference_t boolvrs[1];
    fmi2_boolean_t boolData[1];
    for (const auto& fmuParameter : fmuBoolParameters)
    {
        boolData[0] = fmuParameter.value;
        boolvrs[0] = fmuParameter.valueReference;
        fmiStatus = fmi2_import_set_boolean(cdata->fmu2,
                                            boolvrs,
                                            1,
                                            boolData);
    }

    if (fmiStatus == fmi2_status_warning)
    {
        LOGWARN(log_prefix(agentIdString) + "setting FMI variables returned warning");
    }
    else if (fmiStatus == fmi2_status_error)
    {
        LOGERROR(log_prefix(agentIdString) + "setting FMI variables returned error");
        throw std::logic_error("Error setting FMI variables");
    }

    fmi2_value_reference_t stringvrs[1];
    fmi2_string_t stringData[1];
    for (const auto& fmuParameter : fmuStringParameters)
    {
        stringData[0] = fmuParameter.value.data();
        stringvrs[0] = fmuParameter.valueReference;
        fmiStatus = fmi2_import_set_string(cdata->fmu2,
                                           stringvrs,
                                           1,
                                           stringData);
    }

    if (fmiStatus == fmi2_status_warning)
    {
        LOGWARN(log_prefix(agentIdString) + "setting FMI variables returned warning");
    }
    else if (fmiStatus == fmi2_status_error)
    {
        LOGERROR(log_prefix(agentIdString) + "setting FMI variables returned error");
        throw std::logic_error("Error setting FMI variables");
    }
}

#ifdef USE_EXTENDED_OSI
void OsmpFmuHandler::GetMotionCommand()
{
    void* buffer = decode_integer_to_pointer(GetValue(fmuVariables.at(motionCommandVariable.value()+".base.hi").first, VariableType::Int).intValue,
                                             GetValue(fmuVariables.at(motionCommandVariable.value()+".base.lo").first, VariableType::Int).intValue);

    if (enforceDoubleBuffering && buffer != nullptr && buffer == previousMotionCommand)
    {
        LOGERRORANDTHROW(log_prefix(agentIdString) + "FMU has no double buffering");
    }

    previousMotionCommand = buffer;
    motionCommand.ParseFromArray(buffer, GetValue(fmuVariables.at(motionCommandVariable.value()+".size").first, VariableType::Int).intValue);
}

void OsmpFmuHandler::GetTrafficUpdate()
{
    void *buffer = decode_integer_to_pointer(GetValue(fmuVariables.at(trafficUpdateVariable.value() + ".base.hi").first, VariableType::Int).intValue,
                                             GetValue(fmuVariables.at(trafficUpdateVariable.value() + ".base.lo").first, VariableType::Int).intValue);

    if (enforceDoubleBuffering && buffer != nullptr && buffer == previousTrafficUpdate)
    {
        LOGERRORANDTHROW(log_prefix(agentIdString) + "FMU has no double buffering");
    }

    previousTrafficUpdate = buffer;
    trafficUpdate.ParseFromArray(buffer, GetValue(fmuVariables.at(trafficUpdateVariable.value() + ".size").first, VariableType::Int).intValue);
}

#endif

void OsmpFmuHandler::WriteJson(const google::protobuf::Message& message, const QString& fileName)
{
    QFile file{outputDir + QDir::separator() + fileName};
    file.open(QIODevice::WriteOnly);
    std::string outputString;
    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    google::protobuf::util::MessageToJsonString(message, &outputString, options);
    file.write(outputString.data());
    file.close();
}

void OsmpFmuHandler::WriteBinaryTrace(std::string& message, const QString& fileName, int time)
{
    const auto currentInterfaceVersion = osi3::InterfaceVersion::descriptor()->file()->options().GetExtension(osi3::current_interface_version);
    auto path = (traceOutputDir + QDir::separator() + fileName + "_" + QString::number(GOOGLE_PROTOBUF_VERSION) + "_"
            + QString::number(currentInterfaceVersion.version_major()) + QString::number(currentInterfaceVersion.version_minor())
            + QString::number(currentInterfaceVersion.version_patch()) + "_" + QString::number(time * cycleTime / 1000) + ".osi").toStdString();
    rename(oldFileName.c_str(), path.c_str());
    if (!traceOutputFile.is_open())
        traceOutputFile.open(path, std::ios::out | std::ios::trunc);
    traceOutputFile  << message;
    traceOutputFile.flush();
    traceOutputFile.close();

    oldFileName = path;
}

osi3::SensorViewConfiguration OsmpFmuHandler::GenerateDefaultSensorViewConfiguration()
{
    osi3::SensorViewConfiguration viewConfiguration;

    viewConfiguration.mutable_sensor_id()->set_value(0);

    viewConfiguration.mutable_mounting_position()->mutable_orientation()->set_pitch(0.0);
    viewConfiguration.mutable_mounting_position()->mutable_orientation()->set_roll(0.0);
    viewConfiguration.mutable_mounting_position()->mutable_orientation()->set_yaw(0.0);

    viewConfiguration.mutable_mounting_position()->mutable_position()->set_x(0.0);
    viewConfiguration.mutable_mounting_position()->mutable_position()->set_y(0.0);
    viewConfiguration.mutable_mounting_position()->mutable_position()->set_z(0.0);

    viewConfiguration.set_field_of_view_horizontal(M_2_PI);
    viewConfiguration.set_range(std::numeric_limits<double>::max());

    return viewConfiguration;
}

std::vector<unsigned char> intToBytes(int paramInt)
{
    std::vector<unsigned char> arrayOfByte(4);
    for (int i = 0; i < 4; i++)
        arrayOfByte[3 - i] = (paramInt >> (i * 8));
    return arrayOfByte;
}

void OsmpFmuHandler::AppendMessages(std::string& appendedMessage, std::string& message) {
    auto length = intToBytes(message.length());
    std::string messageLength{length.begin(), length.end()};
    appendedMessage = appendedMessage + messageLength + message;
}
