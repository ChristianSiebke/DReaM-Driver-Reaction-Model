/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "Interfaces/signalInterface.h"
#include "Interfaces/fmuHandlerInterface.h"
#include "Interfaces/worldInterface.h"
#include "Interfaces/agentInterface.h"
#include "Interfaces/parameterInterface.h"
#include "osi3/osi_groundtruth.pb.h"
#include "osi3/osi_sensordata.pb.h"
#include "osi3/osi_sensorview.pb.h"
#include "osi3/osi_trafficcommand.pb.h"
#include "osi3/osi_trafficupdate.pb.h"
#include "Common/openScenarioDefinitions.h"
#include <QString>

class CallbackInterface;

class OsmpFmuHandler : public FmuHandlerInterface
{
public:
    OsmpFmuHandler(fmu_check_data_t* cdata,
                   WorldInterface* world,
                   AgentInterface* agent,
                   const CallbackInterface* callbacks,
                   const Fmu2Variables& fmuVariables,
                   std::map<ValueReferenceAndType, FmuValue>* fmuVariableValues,
                   const ParameterInterface *parameters);

    void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const>& data, int time) override;

    void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const>& data, int time) override;

    void PreStep(int time) override;

    void PostStep(int time) override;

    //! Converts a trajectory from OpenSCENARIO to a OSI TrafficCommand
    static osi3::TrafficCommand GetTrafficCommandFromOpenScenarioTrajectory(openScenario::Trajectory trajectory);

private:
    //! Sets the SensorView as input for the FMU
    void SetSensorViewInput(const osi3::SensorView& data);

    //! Sets the TrafficCommand as input for the FMU
    void SetTrafficCommandInput(const osi3::TrafficCommand& data);

    //! Reads the TrafficUpdate from the FMU
    void GetTrafficUpdate();

    //! Reads the SensorData from the FMU
    void GetSensorData();

    //! Writes an OSI message into a JSON file
    void WriteJson(const google::protobuf::Message& message, const QString& fileName);

    FmuHandlerInterface::FmuValue& GetValue(fmi2_value_reference_t valueReference, VariableType variableType);

    osi3::SensorViewConfiguration GenerateSensorViewConfiguration();

    WorldInterface* world;
    std::map<ValueReferenceAndType, FmuHandlerInterface::FmuValue>* fmuVariableValues = nullptr;
    const Fmu2Variables& fmuVariables;

    std::string serializedSensorView;
    std::string previousSerializedSensorView;
    std::string serializedTrafficCommand;
    std::string previousSerializedTrafficCommand;
    osi3::TrafficUpdate trafficUpdate;
    void* previousTrafficUpdate{nullptr};
    osi3::TrafficCommand trafficCommand;
    void* previousSensorData{nullptr};
    osi3::SensorData sensorData;

    std::optional<std::string> sensorViewVariable;
    std::optional<std::string> sensorDataVariable;
    std::optional<std::string> trafficCommandVariable;
    std::optional<std::string> trafficUpdateVariable;

    bool writeSensorView{false};
    bool writeSensorData{false};
    bool writeTrafficCommand{false};
    bool writeTrafficUpdate{false};

    //! check for double buffering of OSI messages allocated by FMU
    bool enforceDoubleBuffering{false};

    QString outputDir{};

    Common::Vector2d previousPosition{0.0,0.0};
};
