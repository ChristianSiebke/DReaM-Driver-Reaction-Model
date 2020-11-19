/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
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
#include <QFile>

#include "common/dynamicsSignal.h"
#include "common/sensorDataSignal.h"
#include "common/trajectorySignal.h"
#include "core/slave/modules/World_OSI/WorldData.h"
#include "google/protobuf/util/json_util.h"
#include "variant_visitor.h"

extern "C"
{
#include "fmilib.h"
#include "fmuChecker.h"
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
    previousPosition(agent->GetPositionX(), agent->GetPositionY())
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
                LOGERRORANDTHROW("Unkown FMU \""+type+"\" variable \""+value+"\"")
            }
        }
    }

    auto writeSensorDataFlag = parameters->GetParametersBool().find("WriteSensorDataOutput");
    if (writeSensorDataFlag != parameters->GetParametersBool().end())
    {
        writeSensorData = writeSensorDataFlag->second;
    }
    auto writeSensorViewFlag = parameters->GetParametersBool().find("WriteSensorViewOutput");
    if (writeSensorViewFlag != parameters->GetParametersBool().end())
    {
        writeSensorView = writeSensorViewFlag->second;
    }
    auto writeSensorViewConfigFlag = parameters->GetParametersBool().find("WriteSensorViewConfigOutput");
    if (writeSensorViewConfigFlag != parameters->GetParametersBool().end())
    {
        writeSensorViewConfig = writeSensorViewConfigFlag->second;
    }
    auto writeGroundtruthFlag = parameters->GetParametersBool().find("WriteGroundtruthOutput");
    if (writeGroundtruthFlag != parameters->GetParametersBool().end())
    {
        writeGroundtruth = writeGroundtruthFlag->second;
    }
#ifdef USE_EXTENDED_OSI
    auto writeTrafficCommandFlag = parameters->GetParametersBool().find("WriteTrafficCommandOutput");
    if (writeTrafficCommandFlag != parameters->GetParametersBool().end())
    {
        writeTrafficCommand = writeTrafficCommandFlag->second;
    }
    auto writeTrafficUpdateFlag = parameters->GetParametersBool().find("WriteTrafficUpdateOutput");
    if (writeTrafficUpdateFlag != parameters->GetParametersBool().end())
    {
        writeTrafficUpdate = writeTrafficUpdateFlag->second;
    }
    auto writeMotionCommandFlag = parameters->GetParametersBool().find("WriteMotionCommandOutput");
    if (writeMotionCommandFlag != parameters->GetParametersBool().end())
    {
        writeMotionCommand = writeMotionCommandFlag->second;
    }
    auto writeVehicleCommunicationDataFlag = parameters->GetParametersBool().find("WriteVehicleCommunicationDataOutput");
    if (writeVehicleCommunicationDataFlag != parameters->GetParametersBool().end())
    {
        writeVehicleCommunicationData = writeVehicleCommunicationDataFlag->second;
    }

    bool writeJsonOutput = writeSensorData || writeSensorView || writeTrafficCommand || writeTrafficUpdate || writeMotionCommand || writeVehicleCommunicationData || writeGroundtruth;
#else
    bool writeJsonOutput = writeSensorData || writeSensorView || writeGroundtruth;
#endif

    if (writeJsonOutput)
    {
        outputDir = QString::fromStdString(parameters->GetRuntimeInformation().directories.output) +
                    QDir::separator() + cdata->modelName + QDir::separator() +
                    "Agent" + QString::number(agent->GetId());
        QDir directory{outputDir};
        if (!directory.exists())
        {
            directory.mkpath(outputDir);
        }
    }

    auto enforceDoubleBufferingFlag = parameters->GetParametersBool().find("EnforceDoubleBuffering");
    if (enforceDoubleBufferingFlag != parameters->GetParametersBool().end())
    {
        enforceDoubleBuffering = enforceDoubleBufferingFlag->second;
    }

    ParseFmuParameters(parameters);

}


void OsmpFmuHandler::UpdateInput(int localLinkId, const std::shared_ptr<const SignalInterface>& data, [[maybe_unused]] int time)
{
    if (localLinkId == 2)
    {
        auto signal = std::dynamic_pointer_cast<SensorDataSignal const>(data);

        if (!signal)
        {
            const std::string msg = "AlgorithmFmuHandler invalid singnaltype";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

        sensorData = signal->sensorData;
    }
#ifdef USE_EXTENDED_OSI
    trafficCommands.try_emplace(time, std::make_unique<osi3::TrafficCommand>());

    if (localLinkId == 10)
    {
        auto signal = std::dynamic_pointer_cast<TrajectorySignal const>(data);

        if (signal && signal->componentState == ComponentState::Acting)
        {
            AddTrafficCommandActionFromOpenScenarioTrajectory(trafficCommands[time]->add_action(), signal->trajectory);
        }
    }
    else if (localLinkId == 11)
    {
        auto signal = std::dynamic_pointer_cast<AcquirePositionSignal const>(data);

        if (signal && signal->componentState == ComponentState::Acting)
        {
            AddTrafficCommandActionFromOpenScenarioPosition(trafficCommands[time]->add_action(),
                                                            signal->position,
                                                            this->world,
                                                            [this](const std::string &message) { LOGERROR(message); });
        }
    }

    trafficCommand = *trafficCommands[time];
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
            acceleration = baseMoving->acceleration().x() * std::cos(yaw) + baseMoving->acceleration().y() * std::sin(yaw);
            centripetalAcceleration = -baseMoving->acceleration().x() * std::sin(yaw) + baseMoving->acceleration().y() * std::cos(yaw);
            positionX = baseMoving->position().x();
            positionY = baseMoving->position().y();
            yawRate = baseMoving->orientation_rate().yaw();

        }
        else
        {
            LOGERRORANDTHROW("Cannot provide DynamicsSignal, as neither TrafficUpdate nor MotionCommand are connected");
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
                                                      steeringWheelAngle,
                                                      centripetalAcceleration,
                                                      travelDistance);
    }
#endif
}

void OsmpFmuHandler::Init()
{
    SetFmuParameters();
    if (groundtruthVariable.has_value())
    {
        auto* worldData = static_cast<OWL::Interfaces::WorldData*>(world->GetWorldData());
        auto& groundtruth = worldData->GetOsiGroundTruth();

        fmi2_integer_t fmuInputValues[3];
        fmi2_value_reference_t valueReferences[3] = {fmuVariables.at(groundtruthVariable.value()+".base.lo").first,
                                                     fmuVariables.at(groundtruthVariable.value()+".base.hi").first,
                                                     fmuVariables.at(groundtruthVariable.value()+".size").first};

        groundtruth.SerializeToString(&serializedGroundtruth);
        encode_pointer_to_integer(serializedGroundtruth.data(),
                                  fmuInputValues[1],
                                  fmuInputValues[0]);
        fmuInputValues[2] = serializedGroundtruth.length();

        fmi2_import_set_integer(cdata->fmu2,
                                valueReferences,     // array of value reference
                                3,                   // number of elements
                                fmuInputValues);     // array of values
        if (writeGroundtruth)
        {
            WriteJson(groundtruth, "Groundtruth.json");
        }
    }
    if (sensorViewConfigRequestVariable.has_value())
    {
        void* buffer = decode_integer_to_pointer(GetValue(fmuVariables.at(sensorViewConfigRequestVariable.value()+".base.hi").first, VariableType::Int).intValue,
                                                 GetValue(fmuVariables.at(sensorViewConfigRequestVariable.value()+".base.lo").first, VariableType::Int).intValue);
        const auto size = static_cast<std::string::size_type>(GetValue(fmuVariables.at(sensorViewConfigRequestVariable.value()+".size").first, VariableType::Int).intValue);
        serializedSensorViewConfig = {static_cast<char *>(buffer), size};
        sensorViewConfig.ParseFromString(serializedSensorViewConfig);

        fmi2_integer_t fmuInputValues[3];
        fmi2_value_reference_t valueReferences[3] = {fmuVariables.at(sensorViewVariable.value()+".base.lo").first,
                                                     fmuVariables.at(sensorViewVariable.value()+".base.hi").first,
                                                     fmuVariables.at(sensorViewVariable.value()+".size").first};

        encode_pointer_to_integer(serializedSensorViewConfig.data(),
                                  fmuInputValues[1],
                                  fmuInputValues[0]);
        fmuInputValues[2] = serializedSensorView.length();

        fmi2_import_set_integer(cdata->fmu2,
                                valueReferences,     // array of value reference
                                3,                   // number of elements
                                fmuInputValues);     // array of values
        if (writeSensorViewConfig)
        {
            WriteJson(sensorViewConfig, "SensorViewConfig.json");
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
    }
    if (sensorDataInVariable)
    {
        SetSensorDataInput(sensorData);
        if (writeSensorData)
        {
            WriteJson(sensorData, "SensorData-" + QString::number(time) + ".json");
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

FmuHandlerInterface::FmuValue& OsmpFmuHandler::GetValue(fmi2_value_reference_t valueReference, VariableType variableType)
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
        LOGERROR(msg);
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
        LOGERRORANDTHROW("Serialized buffer length of osi::TrafficCommand exceeds fmi2 integer size");
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

void logAndThrow(const std::string& message, const std::function<void(const std::string&)> &errorCallback) noexcept(false) {
    if (errorCallback) errorCallback(message);
    throw std::runtime_error(message);
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
                       if (!referencedAgentInterface) logAndThrow("Reference to agent '" + entityRef + "' could not be resolved", errorCallback);

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
                       logAndThrow("Position variant not supported for 'openScenario::AcquirePositionAction'", errorCallback);
                   }},
               position);
}
#endif

void OsmpFmuHandler::ParseFmuParameters(const ParameterInterface* parameters)
{
    for(const auto& [key, value] : parameters->GetParametersInt())
    {
        if (key.substr(0, 10) == "Parameter_")
        {
            const auto variableName = key.substr(10);
            const auto foundVariable = fmuVariables.find(variableName);
            if (foundVariable == fmuVariables.cend())
            {
                LOGERRORANDTHROW("No variable with name \"" + variableName + "\" found in the FMU");
            }
            else if (foundVariable->second.second != VariableType::Int)
            {
                LOGERRORANDTHROW("Variable \"" + variableName + "\" has different type in FMU");
            }
            fmuIntegerParameters.emplace_back(value, foundVariable->second.first);
        }
    }
    for(const auto& [key, value] : parameters->GetParametersBool())
    {
        if (key.substr(0, 10) == "Parameter_")
        {
            const auto variableName = key.substr(10);
            const auto foundVariable = fmuVariables.find(variableName);
            if (foundVariable == fmuVariables.cend())
            {
                LOGERRORANDTHROW("No variable with name \"" + variableName + "\" found in the FMU");
            }
            else if (foundVariable->second.second != VariableType::Bool)
            {
                LOGERRORANDTHROW("Variable \"" + variableName + "\" has different type in FMU");
            }
            fmuBoolParameters.emplace_back(value, foundVariable->second.first);
        }
    }
    for(const auto& [key, value] : parameters->GetParametersDouble())
    {
        if (key.substr(0, 10) == "Parameter_")
        {
            const auto variableName = key.substr(10);
            const auto foundVariable = fmuVariables.find(variableName);
            if (foundVariable == fmuVariables.cend())
            {
                LOGERRORANDTHROW("No variable with name \"" + variableName + "\" found in the FMU");
            }
            else if (foundVariable->second.second != VariableType::Double)
            {
                LOGERRORANDTHROW("Variable \"" + variableName + "\" has different type in FMU");
            }
            fmuDoubleParameters.emplace_back(value, foundVariable->second.first);
        }
    }
    for(const auto& [key, value] : parameters->GetParametersString())
    {
        if (key.substr(0, 10) == "Parameter_")
        {
            const auto variableName = key.substr(10);
            const auto foundVariable = fmuVariables.find(variableName);
            if (foundVariable == fmuVariables.cend())
            {
                LOGERRORANDTHROW("No variable with name \"" + variableName + "\" found in the FMU");
            }
            else if (foundVariable->second.second != VariableType::String)
            {
                LOGERRORANDTHROW("Variable \"" + variableName + "\" has different type in FMU");
            }
            fmuStringParameters.emplace_back(value, foundVariable->second.first);
        }
    }
}

void OsmpFmuHandler::SetFmuParameters()
{
    fmi2_status_t fmiStatus = fmi2_status_ok;

    fmi2_value_reference_t realvrs[1];
    fmi2_real_t realData[1];
    for (const auto& fmuParameter : fmuDoubleParameters)
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
        LOG(CbkLogLevel::Warning, "setting FMI variables returned warning");
    }
    else if (fmiStatus == fmi2_status_error)
    {
        LOG(CbkLogLevel::Error, "setting FMI variables returned error");
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
        LOG(CbkLogLevel::Warning, "setting FMI variables returned warning");
    }
    else if (fmiStatus == fmi2_status_error)
    {
        LOG(CbkLogLevel::Error, "setting FMI variables returned error");
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
        LOG(CbkLogLevel::Warning, "setting FMI variables returned warning");
    }
    else if (fmiStatus == fmi2_status_error)
    {
        LOG(CbkLogLevel::Error, "setting FMI variables returned error");
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
        LOG(CbkLogLevel::Warning, "setting FMI variables returned warning");
    }
    else if (fmiStatus == fmi2_status_error)
    {
        LOG(CbkLogLevel::Error, "setting FMI variables returned error");
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
        LOGERRORANDTHROW("FMU has no double buffering");
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
        LOGERRORANDTHROW("FMU has no double buffering");
    }

    previousTrafficUpdate = buffer;
    trafficUpdate.ParseFromArray(buffer, GetValue(fmuVariables.at(trafficUpdateVariable.value() + ".size").first, VariableType::Int).intValue);
}
#endif

void OsmpFmuHandler::WriteJson(const google::protobuf::Message& message, const QString& fileName)
{
    QFile file{outputDir + "/" + fileName};
    file.open(QIODevice::WriteOnly);
    std::string outputString;
    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    google::protobuf::util::MessageToJsonString(message, &outputString, options);
    file.write(outputString.data());
    file.close();
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
