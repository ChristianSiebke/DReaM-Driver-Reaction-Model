/******************************************************************************
* Copyright (c) 2019 TU Dresden
* Copyright (c) 2019 AMFD GmbH
*
* This program and the accompanying materials are made available under the
* terms of the Eclipse Public License 2.0 which is available at
* https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*****************************************************************************/

//-----------------------------------------------------------------------------
//! @file  sensor_criticalityImplementation.cpp
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief provide all for the bird-view criticality calculation needed information
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_ModularDriver
//-----------------------------------------------------------------------------

#include <memory>
#include <list>
#include <qglobal.h>
#include "Interfaces/worldInterface.h"
#include "sensor_criticalityImplementation.h"

SensorCriticalityImplementation::SensorCriticalityImplementation(
        std::string componentName,
        bool isInit,
        int priority,
        int offsetTime,
        int responseTime,
        int cycleTime,
        StochasticsInterface *stochastics,
        WorldInterface *world,
        const ParameterInterface *parameters,
        const std::map<int, ObservationInterface*> *observations,
        const CallbackInterface *callbacks,
        AgentInterface *agent) :
        SensorInterface(
        componentName,
        isInit,
        priority,
        offsetTime,
        responseTime,
        cycleTime,
        stochastics,
        world,
        parameters,
        observations,
        callbacks,
        agent),
        situationCalculation(cycleTime, stochastics),
        situationlogging(cycleTime, &situationCalculation)
{
    agentId = GetAgent()->GetId();
    std::map<std::string, const std::string> map = GetParameters()->GetParametersString();
    for (std::map<std::string, const std::string>::iterator it = map.begin(); it!=map.end(); it++)
    {
        loggingGroups.push_back(it->second);
    }

    try
    {
        observerInstance = GetObservations()->at(0);
        if(observerInstance == nullptr)
        {
            throw std::runtime_error("");
        }
    }
    catch(...)
    {
        const std::string msg = COMPONENTNAME + " invalid observation module setup";
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }

    situationlogging.Initialize(loggingGroups, observerInstance);
    LoggingActivated = GetParameters()->GetParametersBool().at("0");
}

void SensorCriticalityImplementation::UpdateInput(int, const std::shared_ptr<SignalInterface const> &, int)
{
}

void SensorCriticalityImplementation::UpdateOutput(int ,std::shared_ptr<SignalInterface const> &, int)
{
}

void SensorCriticalityImplementation::Trigger(int time)
{
    if (LoggingActivated)
    {
        timeMSec = time;
        AgentInterface *agent = GetAgent();
        auto collisionPartners = GetAgent()->GetCollisionPartners();
        indexLaneEgo = GetAgent()->GetMainLaneId();
        std::map<int, AgentInterface*> Agents = world->GetAgents();

        UpdateEgoData(agent);
        SetMovingObjects(agent, &Agents, time);

        Environment.roadGeometry.visibilityDistance = GetWorld()->GetVisibilityDistance();
        Environment.trafficRuleInformation.laneEgo = GetTrafficRuleLaneInformationEgo();

        situationCalculation.UpdateMembers(&Ego, &SurroundingMovingObjects, &Environment);
        situationlogging.CheckForLoggingStatesAndLog(time, "Bird:");
    }
}

void SensorCriticalityImplementation::UpdateEgoData(AgentInterface *agent)
{
    Ego.UpdateInitialBoundingBox(agent);
}

void SensorCriticalityImplementation::SetMovingObjects(AgentInterface *agent, const std::map<int, AgentInterface*> *Agents, int time)
{
    SurroundingMovingObjects.clear();

    double sightdistance = GetWorld()->GetVisibilityDistance();
    for (std::map<int, AgentInterface*>::const_iterator it=Agents->begin(); it!=Agents->end(); ++it)
    {
        if (it->second!=agent && (abs(agent->GetPositionX()-it->second->GetPositionX())<sightdistance))
        {
            AgentVehicleType VehicleType                      = it->second->GetVehicleType();

            State_Moving State;
            State.id                      = it->second->GetId();
            State.velocity_x              = it->second->GetVelocity(VelocityScope::DirectionX);
            State.velocity_y              = it->second->GetVelocity(VelocityScope::DirectionY);
            State.velocity_long           = it->second->GetVelocity(VelocityScope::Longitudinal);
            State.velocity_lat            = it->second->GetVelocity(VelocityScope::Lateral);
            State.pos.xPos                = it->second->GetPositionX();
            State.pos.yPos                = it->second->GetPositionY();
            State.pos.yawAngle            = it->second->GetYaw();
            State.yaw_velocity            = it->second->GetYawRate();
            State.roadPos                 = it->second->GetRoadPosition();
            State.acceleration_long       = it->second->GetAcceleration();
            State.acceleration_lat        = 0;
            State.brakeLight              = it->second->GetBrakeLight();
            State.hornSwitch              = it->second->GetHorn();
            State.flasherSwitch           = it->second->GetFlasher();
            State.headLightSwitch         = it->second->GetHeadLight();
            State.highBeamLightSwitch     = it->second->GetHighBeamLight();
            State.indicatorstate          = it->second->GetIndicatorState();
            State.drivingdirection        = 1;
            State.laneid                  = it->second->GetMainLaneId();
            State.secondarycoveredlanes   = it->second->GetSecondaryCoveredLanes();

            Properties Properties;
            Properties.lx                       = it->second->GetLength();
            Properties.ly                       = it->second->GetWidth();
            Properties.lz                       = it->second->GetHeight();
            Properties.distanceReftoLeadingEdge = it->second->GetDistanceReferencePointToLeadingEdge();
            Properties.trackWidth               = it->second->GetTrackWidth();
            Properties.heightCOG                = it->second->GetHeightCOG();
            Properties.wheelbase                = it->second->GetWheelbase();

            SurroundingMovingObjectsData CurrentMovingObject(VehicleType, State, Properties);
            std::unique_ptr<AgentRepresentation> ptr_Agent_Representation = std::make_unique<AgentRepresentation>(CurrentMovingObject, time);
            SurroundingMovingObjects.push_back(std::move(ptr_Agent_Representation));
        }
    }
}

LaneInformationTrafficRules SensorCriticalityImplementation::GetTrafficRuleLaneInformationEgo()
{
    LaneInformationTrafficRules laneInformation;
    const double visibilityDistance = GetWorld()->GetVisibilityDistance();

    laneInformation.trafficSigns = GetAgent()->GetTrafficSignsInRange(visibilityDistance);

    return laneInformation;
}

