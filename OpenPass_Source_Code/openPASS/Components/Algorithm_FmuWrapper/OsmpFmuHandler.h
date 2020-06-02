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
#include "osi3/osi_groundtruth.pb.h"
#include "osi3/osi_trafficupdate.pb.h"
#include "osi3/osi_trafficcommand.pb.h"
#include "osi3/osi_sensorview.pb.h"
#include "Common/openScenarioDefinitions.h"

class CallbackInterface;

class OsmpFmuHandler : public FmuHandlerInterface
{
public:
    OsmpFmuHandler(fmu_check_data_t* cdata,
                   WorldInterface* world,
                   AgentInterface* agent,
                   const CallbackInterface* callbacks,
                   const Fmu2Variables& fmuVariables,
                   std::map<ValueReferenceAndType, FmuValue>* fmuVariableValues) :
        FmuHandlerInterface(cdata, agent, callbacks),
        world(world),
        fmuVariableValues(fmuVariableValues),
        fmuVariables(fmuVariables),
        previousPosition(agent->GetPositionX(), agent->GetPositionY())
    {
    }

    void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const>& data, int time) override;

    void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const>& data, int time) override;

    void PreStep() override;

    void PostStep() override;

    void SetSensorViewInput(const osi3::SensorView& data);

    void SetTrafficCommandInput(const osi3::TrafficCommand& data);

    void GetTrafficUpdate();

    static osi3::TrafficCommand GetTrafficCommandFromOpenScenarioTrajectory(openScenario::Trajectory trajectory);

private:
    FmuHandlerInterface::FmuValue& GetValue(fmi2_value_reference_t valueReference, VariableType variableType);

    osi3::SensorViewConfiguration GenerateSensorViewConfiguration();

    WorldInterface* world;
    std::map<ValueReferenceAndType, FmuHandlerInterface::FmuValue>* fmuVariableValues = nullptr;
    const Fmu2Variables& fmuVariables;

    std::string serializedSensorView;
    std::string serializedTrafficCommand;
    osi3::TrafficUpdate trafficUpdate;
    osi3::TrafficCommand trafficCommand;

    Common::Vector2d previousPosition{0.0,0.0};
};
