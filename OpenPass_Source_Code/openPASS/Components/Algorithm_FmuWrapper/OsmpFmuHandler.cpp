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
#include "Common/dynamicsSignal.h"
#include "Common/trajectorySignal.h"
#include "CoreModules/World_OSI/WorldData.h"

extern "C" {
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




void OsmpFmuHandler::UpdateInput(int localLinkId, const std::shared_ptr<const SignalInterface>& data, [[maybe_unused]] int time)
{
    if (localLinkId == 10)
    {
        const std::shared_ptr<TrajectorySignal const> signal = std::dynamic_pointer_cast<TrajectorySignal const>(data);
        if (signal && signal->componentState == ComponentState::Acting)
        {
            trafficCommand = GetTrafficCommandFromOpenScenarioTrajectory(signal->trajectory);
        }
    }
}

constexpr double EPSILON = 0.001;

void OsmpFmuHandler::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const>& data, int time)
{
    if (localLinkId == 0)
    {
        const auto& baseMoving = trafficUpdate.mutable_update()->mutable_base();
        double velocity = std::sqrt(baseMoving->velocity().x() * baseMoving->velocity().x() + baseMoving->velocity().y() * baseMoving->velocity().y());
        double yaw = baseMoving->orientation().yaw();
        double acceleration = baseMoving->acceleration().x() * std::cos(yaw) + baseMoving->acceleration().y() * std::sin(yaw);
        double centripetalAcceleration = -baseMoving->acceleration().x() * std::sin(yaw) + baseMoving->acceleration().y() * std::cos(yaw);
        double positionX = baseMoving->position().x();
        double positionY = baseMoving->position().y();
        if (std::abs(positionX) < EPSILON && std::abs(positionY) < EPSILON)
        {
            positionX = previousPosition.x;
            positionY = previousPosition.y;
        }
        double yawRate = baseMoving->orientation_rate().yaw();
        double steeringWheelAngle = 0.0; //TODO
        double deltaX = positionX - previousPosition.x;
        double deltaY = positionY - previousPosition.y;
        double travelDistance = std::sqrt(deltaX * deltaX + deltaY * deltaY);
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
}

void OsmpFmuHandler::PreStep()
{
    osi3::SensorViewConfiguration sensorViewConfig = GenerateSensorViewConfiguration();
    auto* worldData = static_cast<OWL::Interfaces::WorldData*>(world->GetWorldData());
    osi3::SensorView sensorView = worldData->GetSensorView(sensorViewConfig, agent->GetId());

    SetSensorViewInput(sensorView);
    SetTrafficCommandInput(trafficCommand);
}

void OsmpFmuHandler::PostStep()
{
    GetTrafficUpdate();
}

void OsmpFmuHandler::SetSensorViewInput(const osi3::SensorView& data)
{
    fmi2_integer_t fmuInputValues[3];
    fmi2_value_reference_t valueReferences[3] = {fmuVariables.at("OSMPSensorViewIn.base.lo").first,
                                                 fmuVariables.at("OSMPSensorViewIn.base.hi").first,
                                                 fmuVariables.at("OSMPSensorViewIn.size").first};

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

void OsmpFmuHandler::SetTrafficCommandInput(const osi3::TrafficCommand& data)
{
    fmi2_integer_t fmuInputValues[3];
    fmi2_value_reference_t valueReferences[3] = {fmuVariables.at("OSMPTrafficCommandIn.base.lo").first,
                                                 fmuVariables.at("OSMPTrafficCommandIn.base.hi").first,
                                                 fmuVariables.at("OSMPTrafficCommandIn.size").first};

    data.SerializeToString(&serializedTrafficCommand);
    encode_pointer_to_integer(serializedTrafficCommand.data(),
                              fmuInputValues[1],
                              fmuInputValues[0]);

    // TODO rb: this new check needs approval
    auto length = serializedTrafficCommand.length();

    if (length > std::numeric_limits<fmi2_integer_t>::max())
    {
        throw std::runtime_error("Serialized buffer length of osi::TrafficCommand exceeds fmi2 integer size");
    }

    fmuInputValues[2] = static_cast<fmi2_integer_t>(length);

    // TODO rb: handle status?
    auto fmiStatus = fmi2_import_set_integer(cdata->fmu2,
                                             valueReferences,       // array of value reference
                                             3,             // number of elements
                                             fmuInputValues);     // array of values
}

FmuHandlerInterface::FmuValue& OsmpFmuHandler::GetValue(fmi2_value_reference_t valueReference, VariableType variableType)
{
    ValueReferenceAndType valueReferenceAndType;
    valueReferenceAndType.emplace<FMI2>(valueReference, variableType);
    return fmuVariableValues->at(valueReferenceAndType);
}

osi3::TrafficCommand OsmpFmuHandler::GetTrafficCommandFromOpenScenarioTrajectory(openScenario::Trajectory trajectory)
{
    osi3::TrafficCommand trafficCommand;
    auto trafficAction = trafficCommand.add_action();
    auto trajectoryAction = trafficAction->mutable_trajectory_action();
    for (const auto& trajectoryPoint : trajectory.points)
    {
        auto statePoint = trajectoryAction->add_trajectory_point();
        statePoint->mutable_time_stamp()->set_seconds(static_cast<google::protobuf::int64>(trajectoryPoint.time));
        statePoint->mutable_time_stamp()->set_nanos(static_cast<google::protobuf::uint32>(std::fmod(trajectoryPoint.time * 1e9, 1e9)));
        statePoint->mutable_state()->mutable_position()->set_x(trajectoryPoint.x);
        statePoint->mutable_state()->mutable_position()->set_y(trajectoryPoint.y);
        statePoint->mutable_state()->mutable_orientation()->set_yaw(trajectoryPoint.yaw);
    }
    return trafficCommand;
}

void OsmpFmuHandler::GetTrafficUpdate()
{
    void* buffer = decode_integer_to_pointer(GetValue(fmuVariables.at("OSMPTrafficUpdateOut.base.hi").first, VariableType::Int).intValue,
                                             GetValue(fmuVariables.at("OSMPTrafficUpdateOut.base.lo").first, VariableType::Int).intValue);
    trafficUpdate.ParseFromArray(buffer, GetValue(fmuVariables.at("OSMPTrafficUpdateOut.size").first, VariableType::Int).intValue);
}

osi3::SensorViewConfiguration OsmpFmuHandler::GenerateSensorViewConfiguration()
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
