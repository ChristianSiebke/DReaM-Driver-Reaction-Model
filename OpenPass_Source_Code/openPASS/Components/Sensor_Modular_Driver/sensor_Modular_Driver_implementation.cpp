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
//! @file  sensor_Modular_Driver_implementation.cpp
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief provide all for the driver relevant information
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_ModularDriver
//-----------------------------------------------------------------------------

#include <memory>
#include <qglobal.h>
#include "sensor_Modular_Driver_implementation.h"
#include "MentalModelLane.h"
// #include "complexsignals.cpp"

Sensor_Modular_Driver_Implementation::Sensor_Modular_Driver_Implementation(std::string componentName,
                                                               bool isInit,
                                                               int priority,
                                                               int offsetTime,
                                                               int responseTime,
                                                               int cycleTime,
                                                               StochasticsInterface *stochastics,
                                                               WorldInterface *world,
                                                               const ParameterInterface *parameters,
                                                               const std::map<int, ObservationInterface *> *observations,
                                                               const CallbackInterface *callbacks,
                                                               AgentInterface *agent) :
    SensorInterface(componentName,
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
                    agent)
{
}

void Sensor_Modular_Driver_Implementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(localLinkId);
    Q_UNUSED(data);
    Q_UNUSED(time);
}

void Sensor_Modular_Driver_Implementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    try
    {
        switch (localLinkId)
        {
        case 0:
            data = std::make_shared<structSignal<StaticEnvironmentData> const>(StaticEnvironment);
            break;
        case 1:
            data = std::make_shared<structSignal<EgoData> const>(Ego);
            break;
        case 2:
            data = std::make_shared<structSignal<std::list<SurroundingMovingObjectsData>> const>(SurroundingMovingObjects);
            break;
        default:
            const std::string msg = COMPONENTNAME + " invalid link";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    catch(const std::bad_alloc&)
    {
        const std::string msgDefault = COMPONENTNAME + " could not instantiate signal";
        LOG(CbkLogLevel::Debug, msgDefault);
        throw std::runtime_error(msgDefault);
    }

}

void Sensor_Modular_Driver_Implementation::Trigger(int time)
{
    Q_UNUSED(time);

    AgentInterface *agent = GetAgent();
    SetEgoData(agent);
    std::map<int, AgentInterface*> Agents = GetWorld()->GetAgents();
    SetMovingObjects(agent, &Agents);

    StaticEnvironment = {};
    SetMentalModelGeometryLaneInformation();
    GetTrafficRuleInformation();
    GetRoadGeometry();
}

void Sensor_Modular_Driver_Implementation::SetMentalModelGeometryLaneInformation()
{
    std::unordered_map<uint64_t, OWL::Interfaces::Road*> currentRoads = static_cast<OWL::Interfaces::WorldData*>(world->GetWorldData())->GetRoads();//->GetRoadById(GetCurrentOsiLaneId("1"));

    OWL::Interfaces::Road  *currentRoad = currentRoads.at(GetCurrentOsiLaneId("1"));

    if (currentRoad != _currentRoad)
    {
        _currentRoad = const_cast<OWL::Road*>(currentRoad);
        lanes.clear();
        for(auto section : currentRoad->GetSections())
        {
            for (auto lane : section->GetLanes())
            {
                ConvertRoadToMMLane(const_cast<OWL::Lane*>(lane), static_cast<OWL::WorldData*>(GetWorld()->GetWorldData())->GetLaneIdMapping().at(lane->GetId()));
            }
        }
    }
}

void Sensor_Modular_Driver_Implementation::ConvertRoadToMMLane(OWL::Lane* lane, int64_t odId)
{
    MentalModelLane mmLane(lane->GetId(), odId, lane->GetLength(), lane->GetWidth(0));

    //TODO Lanes posses only one succsessor!
    // change lane assignment
    //------------------------------------------------------
    OWL::CRoad* currentRoad = &lane->GetRoad();

    //TODO implement invalide lane (catch case: no further road exist)
    auto nextlane = lane->GetNext();
    if (!(nextlane.empty())) {
        mmLane.AddSuccessor(nextlane.front());
    } else {
        mmLane.AddSuccessor(OWL::InvalidId);
    }

    auto previouslane = lane->GetPrevious();
    if (!(previouslane.empty())) {
        mmLane.AddPredecessor(previouslane.front());
    } else {
        mmLane.AddPredecessor(OWL::InvalidId);
    }

    for(auto geometry : lane->GetLaneGeometryElements()) {
        double x = geometry->joints.current.points.reference.x;
        double y = geometry->joints.current.points.reference.y;
        double hdg = geometry->joints.current.sHdg;
        double s = geometry->joints.current.sOffset;

        mmLane.AddPoint(x, y, hdg, s);
    }

    if (lanes.find(odId)==lanes.end())
    {
        lanes.emplace(std::make_pair(odId, std::vector{mmLane}));
    }
    else
    {
        lanes[odId].push_back(mmLane);
    }
}

void Sensor_Modular_Driver_Implementation::SetEgoData(AgentInterface *agent)
{
    AgentVehicleType VehicleType = agent->GetVehicleType();

    State State;
    State.id                          = agent->GetId();
    State.pos.xPos                    = agent->GetPositionX();
    State.pos.yPos                    = agent->GetPositionY();
    State.roadid                      = 0;
    State.drivingdirection            = 1;
    State.laneid                      = agent->GetMainLaneId();
    State.roadPos                     = agent->GetRoadPosition();
    State.velocity_x                  = agent->GetVelocity(VelocityScope::DirectionX);
    State.velocity_y                  = agent->GetVelocity(VelocityScope::DirectionY);
    State.acceleration_abs_inertial   = agent->GetAcceleration();
    State.hornSwitch                  = agent->GetHorn();
    State.headLightSwitch             = agent->GetHeadLight();
    State.highBeamLightSwitch         = agent->GetHighBeamLight();
    State.flasherSwitch               = agent->GetFlasher();
    State.indicatorstate              = agent->GetIndicatorState();
    State.brakeLight                  = agent->GetBrakeLight();
    State.spawntime                   = agent->GetSpawnTime();
    State.pos.yawAngle                = agent->GetYaw();
    State.yaw_velocity                = agent->GetYawRate();
    State.yaw_acceleration            = 0; //not implemented: agent->GetYawAcceleration();

    State_Ego State_Ego;
    State_Ego.velocity_long           = agent->GetVelocity(VelocityScope::Longitudinal);
    State_Ego.velocity_lat            = agent->GetVelocity(VelocityScope::Lateral);
    State_Ego.acceleration_long       = agent->GetAcceleration(); //not implemented: agent->GetAccelerationX();
    State_Ego.acceleration_lat        = 0; //not implemented: agent->GetAccelerationY();
    State_Ego.currentGear             = agent->GetGear();
    State_Ego.accelPedal              = agent->GetEffAccelPedal();
    State_Ego.brakePedal              = agent->GetEffBrakePedal();
    State_Ego.engineSpeed             = agent->GetEngineSpeed();
    State_Ego.steeringWheelAngle      = agent->GetSteeringWheelAngle();
    State_Ego.distance_to_end_of_lane = agent->GetDistanceToEndOfLane(GetWorld()->GetVisibilityDistance(), 0);

    Ego.SetState(State);
    Ego.SetState_Ego(State_Ego);
    Ego.SetVehicleType(VehicleType);
}

void Sensor_Modular_Driver_Implementation::SetMovingObjects(AgentInterface *agent, const std::map<int, AgentInterface*> *Agents)
{
    SurroundingMovingObjects.clear();

    double sightdistance = GetWorld()->GetVisibilityDistance();
    std::list<std::pair<int,int>> AgentAndDist;
    double roadPos_s = agent->GetRoadPosition().s;
    for (std::map<int, AgentInterface*>::const_iterator it=Agents->begin(); it!=Agents->end(); it++)
    {
        double absdist2ego = abs(roadPos_s-it->second->GetRoadPosition().s);
        AgentAndDist.push_back(std::make_pair(absdist2ego, it->first));
    }
    AgentAndDist.sort();
    std::list<std::pair<int,int>>::iterator it1, it2;
    it1 = AgentAndDist.begin();
    it2 = AgentAndDist.end();
    advance(it1, 10);
    AgentAndDist.erase(it1,it2);

    for (std::list<std::pair<int,int>>::const_iterator i=AgentAndDist.begin(); i!=AgentAndDist.end(); i++)
    {
        AgentInterface* it = Agents->at(i->second);
        double absdist2ego = abs(agent->GetRoadPosition().s-it->GetRoadPosition().s);
        if (it!=agent &&
           (absdist2ego<sightdistance))
        {
            AgentVehicleType VehicleType                      = it->GetVehicleType();

            State_Moving State;
            State.id                      = it->GetId();
            State.velocity_x              = it->GetVelocity(VelocityScope::DirectionX);
            State.velocity_y              = it->GetVelocity(VelocityScope::DirectionY);
            State.velocity_long           = it->GetVelocity(VelocityScope::Longitudinal);
            State.velocity_lat            = it->GetVelocity(VelocityScope::Lateral);
            State.pos.xPos                = it->GetPositionX();
            State.pos.yPos                = it->GetPositionY();
            State.pos.yawAngle            = it->GetYaw();
            State.yaw_velocity            = it->GetYawRate();
            State.roadPos                 = it->GetRoadPosition();
            State.acceleration_long       = it->GetAcceleration();
            State.acceleration_lat        = 0;
            State.brakeLight              = it->GetBrakeLight();
            State.hornSwitch              = it->GetHorn();
            State.flasherSwitch           = it->GetFlasher();
            State.headLightSwitch         = it->GetHeadLight();
            State.highBeamLightSwitch     = it->GetHighBeamLight();
            State.indicatorstate          = it->GetIndicatorState();
            State.drivingdirection        = 1;
            State.laneid                  = it->GetMainLaneId();
            State.secondarycoveredlanes   = it->GetSecondaryCoveredLanes();

            Properties Properties;
            Properties.lx                       = it->GetLength();
            Properties.ly                       = it->GetWidth();
            Properties.lz                       = it->GetHeight();
            Properties.distanceReftoLeadingEdge = it->GetDistanceReferencePointToLeadingEdge();
            Properties.trackWidth               = it->GetTrackWidth();
            Properties.heightCOG                = it->GetHeightCOG();
            Properties.wheelbase                = it->GetWheelbase();

            SurroundingMovingObjectsData CurrentMovingObject(VehicleType, State, Properties);  
            CurrentMovingObject.SetDistanceToEgo(absdist2ego);

            SurroundingMovingObjects.push_back(CurrentMovingObject);
            //SurroundingMovingObjects.sort(DistanceComparator());
        }
    }    

    //if (SurroundingMovingObjects.size()>20)
    //{
    //    int difference = SurroundingMovingObjects.size() - 20;
    //    std::list<SurroundingMovingObjectsData>::iterator it1, it2;
    //    it1 = it2 = SurroundingMovingObjects.end();
    //    advance(it1, -difference);
    //    SurroundingMovingObjects.erase(it1, it2);
    //}
}

void Sensor_Modular_Driver_Implementation::GetTrafficRuleInformation()
{
    StaticEnvironment.trafficRuleInformation.laneEgo    = GetTrafficRuleLaneInformationEgo();
    StaticEnvironment.trafficRuleInformation.laneLeft   = GetTrafficRuleLaneInformationLeft();
    StaticEnvironment.trafficRuleInformation.laneRight  = GetTrafficRuleLaneInformationRight();
}

LaneInformationTrafficRules Sensor_Modular_Driver_Implementation::GetTrafficRuleLaneInformationEgo()
{
    LaneInformationTrafficRules laneInformation;
    const double visibilityDistance = GetWorld()->GetVisibilityDistance();

    laneInformation.trafficSigns    = GetAgent()->GetTrafficSignsInRange(visibilityDistance);

    return laneInformation;
}

LaneInformationTrafficRules Sensor_Modular_Driver_Implementation::GetTrafficRuleLaneInformationLeft()
{
    LaneInformationTrafficRules laneInformation;
    const int relativeLaneId = 1;
    const double visibilityDistance = GetWorld()->GetVisibilityDistance();

    laneInformation.trafficSigns    = GetAgent()->GetTrafficSignsInRange(visibilityDistance, relativeLaneId);

    return laneInformation;
}

LaneInformationTrafficRules Sensor_Modular_Driver_Implementation::GetTrafficRuleLaneInformationRight()
{
    LaneInformationTrafficRules laneInformation;
    const int relativeLaneId = -1;
    const double visibilityDistance = GetWorld()->GetVisibilityDistance();

    laneInformation.trafficSigns    = GetAgent()->GetTrafficSignsInRange(visibilityDistance, relativeLaneId);

    return laneInformation;
}

void Sensor_Modular_Driver_Implementation::GetRoadGeometry()
{
    StaticEnvironment.roadGeometry.visibilityDistance            = GetWorld()->GetVisibilityDistance();
    GetGeometryLaneInformationEgo(StaticEnvironment.roadGeometry.laneEgo);
    GetGeometryLaneInformationLeft(StaticEnvironment.roadGeometry.laneLeft);
    GetGeometryLaneInformationRight(StaticEnvironment.roadGeometry.laneRight);
    StaticEnvironment.numberoflanes = GetAgent()->GetNumberOfLanes();
}

void Sensor_Modular_Driver_Implementation::GetGeometryLaneInformationEgo(LaneInformation &laneInformation)
{
    const double visibilityDistance = GetWorld()->GetVisibilityDistance();
    laneInformation.exists                  = true;     // Ego lane must exist by definition, or else vehicle would have despawned by now. Information not necessary atm!
    if (laneInformation.exists)
    {
        if (laneInformation.laneid != GetAgent()->GetMainLaneId())
        {
            laneInformation.laneid          = GetAgent()->GetMainLaneId();
            laneInformation.mentalModelLanes= lanes[(laneInformation.laneid)];
        }
        laneInformation.curvature           = GetAgent()->GetLaneCurvature();
        laneInformation.width               = GetAgent()->GetLaneWidth();
        laneInformation.distanceToEndOfLane = GetAgent()->GetDistanceToEndOfLane(visibilityDistance);
        laneInformation.laneid              = GetAgent()->GetMainLaneId();
    }
    else
    {
        laneInformation = {};
    }
}
void Sensor_Modular_Driver_Implementation::GetGeometryLaneInformationLeft(LaneInformation &laneInformation)
{
    const int relativeLaneId = 1;
    const double visibilityDistance = GetWorld()->GetVisibilityDistance();

    laneInformation.exists                  = GetAgent()->ExistsLaneLeft();
    if (laneInformation.exists)
    {
        if (laneInformation.laneid != GetAgent()->GetLaneIdLeft())
        {
            laneInformation.laneid          = GetAgent()->GetLaneIdLeft();
            laneInformation.mentalModelLanes= lanes[(laneInformation.laneid)];
        }
        laneInformation.curvature           = GetAgent()->GetLaneCurvature(relativeLaneId);
        laneInformation.width               = GetAgent()->GetLaneWidth(relativeLaneId);
        laneInformation.distanceToEndOfLane = GetAgent()->GetDistanceToEndOfLane(visibilityDistance, relativeLaneId);        
    }
    else
    {
        laneInformation = {};
    }
}

void Sensor_Modular_Driver_Implementation::GetGeometryLaneInformationRight(LaneInformation &laneInformation)
{
    const int relativeLaneId = -1;
    const double visibilityDistance = GetWorld()->GetVisibilityDistance();

    laneInformation.exists                  = GetAgent()->ExistsLaneRight();
    if (laneInformation.exists)
    {
        if (laneInformation.laneid != GetAgent()->GetLaneIdRight())
        {
            laneInformation.laneid          = GetAgent()->GetLaneIdRight();
            laneInformation.mentalModelLanes= lanes[(laneInformation.laneid)];
        }
        laneInformation.curvature           = GetAgent()->GetLaneCurvature(relativeLaneId);
        laneInformation.width               = GetAgent()->GetLaneWidth(relativeLaneId);
        laneInformation.distanceToEndOfLane = GetAgent()->GetDistanceToEndOfLane(visibilityDistance, relativeLaneId);       
    }
    else
    {
        laneInformation = {};
    }
}
