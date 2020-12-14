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

#include <QString>
#include "include/signalInterface.h"
#include "include/fmuHandlerInterface.h"
#include "include/worldInterface.h"
#include "include/agentInterface.h"
#include "include/parameterInterface.h"
#include "osi3/osi_groundtruth.pb.h"
#include "osi3/osi_sensordata.pb.h"
#include "osi3/osi_sensorview.pb.h"

#ifdef USE_EXTENDED_OSI
    #include "osi3/osi_trafficcommand.pb.h"
    #include "osi3/osi_trafficupdate.pb.h"
    #include "osi3/sl45_vehiclecommunicationdata.pb.h"
    #include "osi3/sl45_motioncommand.pb.h"
#endif

#include "common/openScenarioDefinitions.h"

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

    void Init() override;

    void PreStep(int time) override;

    void PostStep(int time) override;

    //! Public for testing
    //! Reads an output value of the FMU
    FmuHandlerInterface::FmuValue& GetValue(fmi2_value_reference_t valueReference, VariableType variableType);

#ifdef USE_EXTENDED_OSI
    //! Adds a trajectory from OpenSCENARIO to a OSI TrafficAction
    static void AddTrafficCommandActionFromOpenScenarioTrajectory(osi3::TrafficAction *trafficAction,
                                                                  const openScenario::Trajectory& trajectory);

    //! Adds a position from OpenSCENARIO to a OSI TrafficAction
    static void AddTrafficCommandActionFromOpenScenarioPosition(osi3::TrafficAction *trafficAction,
                                                                const openScenario::Position &position,
                                                                WorldInterface *worldInterface,
                                                                const std::function<void(const std::string &)> &errorCallback);
#endif

    //! Parameter of the FMU that was defined in the profile
    template <typename T>
    struct FmuParameter
    {
        T value;
        fmi2_value_reference_t valueReference;

        FmuParameter(T value, fmi2_value_reference_t valueReference) :
            value(value),
            valueReference(valueReference)
        {}
    };

    template <typename T>
    using FmuParameters = std::vector<FmuParameter<T>>;

private:

    //! Reads the parameters in the profile that should be forwarded to the FMU
    void ParseFmuParameters(const ParameterInterface* parameters);

    //! Sets the parameters in the FMU
    void SetFmuParameters();

    //! Sets the SensorView as input for the FMU
    void SetSensorViewInput(const osi3::SensorView& data);

    //! Sets the SensorData as input for the FMU
    void SetSensorDataInput(const osi3::SensorData& data);

    //! Reads the SensorData from the FMU
    void GetSensorData();

#ifdef USE_EXTENDED_OSI
    //! Sets the TrafficCommand as input for the FMU
    void SetTrafficCommandInput(const osi3::TrafficCommand& data);

    //! Sets the VehicleCommunicationData as input for the FMU
    void SetVehicleCommunicationDataInput(const setlevel4to5::VehicleCommunicationData& data);

    //! Reads the MotionCommand from the FMU
    void GetMotionCommand();

    //! Reads the TrafficUpdate from the FMU
    void GetTrafficUpdate();
#endif

    //! Writes an OSI message into a JSON file
    void WriteJson(const google::protobuf::Message& message, const QString& fileName);

    osi3::SensorViewConfiguration GenerateDefaultSensorViewConfiguration();

    WorldInterface* world;
    std::map<ValueReferenceAndType, FmuHandlerInterface::FmuValue>* fmuVariableValues = nullptr;
    const Fmu2Variables& fmuVariables;

    FmuParameters<int> fmuIntegerParameters;
    FmuParameters<double> fmuDoubleParameters;
    FmuParameters<bool> fmuBoolParameters;
    FmuParameters<std::string> fmuStringParameters;

    std::string serializedSensorData;
    std::string previousSerializedSensorData;
    std::string serializedSensorView;
    std::string previousSerializedSensorView;
    void* previousSensorData{nullptr};
    osi3::SensorViewConfiguration sensorViewConfig;
    osi3::SensorViewConfiguration sensorViewConfigRequest;
    std::string serializedSensorViewConfig;
    std::string serializedSensorViewConfigRequest;
    osi3::SensorData sensorData;
    std::string serializedGroundtruth;

#ifdef USE_EXTENDED_OSI
    std::string serializedTrafficCommand;
    std::string previousSerializedTrafficCommand;
    std::string serializedVehicleCommunicationData;
    std::string previousSerializedVehicleCommunicationData;
    osi3::TrafficUpdate trafficUpdate;
    void* previousTrafficUpdate{nullptr};

    osi3::TrafficCommand trafficCommand;
    std::map<int, std::unique_ptr<osi3::TrafficCommand>> trafficCommands{};
    setlevel4to5::VehicleCommunicationData vehicleCommunicationData;
    setlevel4to5::MotionCommand motionCommand;
    void* previousMotionCommand{nullptr};
#endif


    std::optional<std::string> sensorViewVariable;
    std::optional<std::string> sensorViewConfigVariable;
    std::optional<std::string> sensorViewConfigRequestVariable;
    std::optional<std::string> sensorDataInVariable;
    std::optional<std::string> sensorDataOutVariable;
    std::optional<std::string> groundtruthVariable;

#ifdef USE_EXTENDED_OSI
    std::optional<std::string> motionCommandVariable;
    std::optional<std::string> trafficCommandVariable;
    std::optional<std::string> trafficUpdateVariable;
    std::optional<std::string> vehicleCommunicationDataVariable;
#endif

    const std::map<std::string, std::map<std::string, std::optional<std::string>&>> variableMapping
    {
        {"Input",
            {
                {"SensorView", sensorViewVariable},
                {"SensorViewConfig", sensorViewConfigVariable},
                {"SensorData", sensorDataInVariable},
#ifdef USE_EXTENDED_OSI
                {"TrafficCommand", trafficCommandVariable},
                {"VehicleCommunicationData", vehicleCommunicationDataVariable}
#endif
            }
        },
        {"Output",
            {
                {"SensorData", sensorDataOutVariable},
                {"SensorViewConfigRequest", sensorViewConfigRequestVariable},
#ifdef USE_EXTENDED_OSI
                {"MotionCommand", motionCommandVariable},
                {"TrafficUpdate", trafficUpdateVariable}
#endif
            }
        },
        {"Init",
            {
                {"Groundtruth", groundtruthVariable}
            }
        }
    };

    bool writeSensorView{false};
    bool writeSensorViewConfig{false};
    bool writeSensorViewConfigRequest{false};
    bool writeSensorData{false};
    bool writeGroundtruth{false};

#ifdef USE_EXTENDED_OSI
    bool writeTrafficCommand{false};
    bool writeTrafficUpdate{false};
    bool writeMotionCommand{false};
    bool writeVehicleCommunicationData{false};
#endif

    //! check for double buffering of OSI messages allocated by FMU
    bool enforceDoubleBuffering{false};

    QString outputDir{};

    Common::Vector2d previousPosition{0.0,0.0};
};
