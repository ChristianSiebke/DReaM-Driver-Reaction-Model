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
//! @file    agent_representation.h
//! @author  Christian Siebke
//! @author  Konstantin Blenz
//! @author  Christian Gärber
//! @author  Vincent   Adam
//! @date    Tue, 03.12.2019
//! @brief provide container structure of agents seen by the driver
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_ModularDriver
//-----------------------------------------------------------------------------

#include <algorithm>
#include <tuple>
#include "agent_representation.h"
#include "MentalModelLane.h"

void AgentRepresentation::Extrapolate (const StaticEnvironmentData &_roadPerceptionData)
{
    int laneid = internal_Data.GetState()->laneid;

    std::map<int, const std::vector<MentalModelLane>*> Lanes = {};
    PositionAgentOnLanes(laneid, &Lanes, _roadPerceptionData);
    if((Lanes.find(0)!=Lanes.end()) && (internal_Data.GetState()->indicatorstate == IndicatorState::IndicatorState_Off))
    {
        ExtrapolateAlongLane(Lanes.at(0));
    }
    else
    {
        ExtrapolateKinematic();
    }
}

void AgentRepresentation::PositionAgentOnLanes(int laneid, std::map<int, const std::vector<MentalModelLane>*>*Lanes, const StaticEnvironmentData &_roadPerceptionData)
{
    const LaneInformation *Ego = &_roadPerceptionData.roadGeometry.laneEgo;
    const LaneInformation *Left = &_roadPerceptionData.roadGeometry.laneLeft;
    const LaneInformation *Right = &_roadPerceptionData.roadGeometry.laneRight;;

    if (laneid == Ego->laneid)
    {
        Lanes->emplace(std::make_pair(0,&Ego->mentalModelLanes));
        if (Left->exists)
        Lanes->emplace(std::make_pair(1,&Left->mentalModelLanes));
        if (Right->exists)
        Lanes->emplace(std::make_pair(-1,&Right->mentalModelLanes));
    }
    if (laneid == _roadPerceptionData.roadGeometry.laneLeft.laneid)
    {
        if (Left->exists)
        Lanes->emplace(std::make_pair(0,&Left->mentalModelLanes));
        Lanes->emplace(std::make_pair(-1,&Ego->mentalModelLanes));
    }
    if (laneid == _roadPerceptionData.roadGeometry.laneRight.laneid)
    {
        if (Right->exists)
        Lanes->emplace(std::make_pair(0,&Right->mentalModelLanes));
        Lanes->emplace(std::make_pair(1,&Ego->mentalModelLanes));
    }
}

void AgentRepresentation::ExtrapolateKinematic()
{
    double ds = (internal_Data.GetState()->acceleration_long / 2) * std::pow((cycletime / 1000),2) + internal_Data.GetState()->velocity_long * cycletime / 1000;
    internal_Data.GetState()->pos.xPos += (internal_Data.GetState()->acceleration_long * std::cos(CommonHelper::ConvertDegreeToRadian(internal_Data.GetState()->pos.yawAngle)))/2 * std::pow((cycletime / 1000),2)+ internal_Data.GetState()->velocity_x * cycletime / 1000;
    internal_Data.GetState()->pos.yPos += (internal_Data.GetState()->acceleration_long * std::sin(CommonHelper::ConvertDegreeToRadian(internal_Data.GetState()->pos.yawAngle)))/2 * std::pow((cycletime / 1000),2)+ internal_Data.GetState()->velocity_y * cycletime / 1000;

    internal_Data.GetState()->velocity_long += internal_Data.GetState()->acceleration_long * cycletime / 1000 ;
    internal_Data.GetState()->velocity_x += internal_Data.GetState()->acceleration_long * std::cos(CommonHelper::ConvertDegreeToRadian(internal_Data.GetState()->pos.yawAngle)) * cycletime / 1000 ;
    internal_Data.GetState()->velocity_y += internal_Data.GetState()->acceleration_long * std::sin(CommonHelper::ConvertDegreeToRadian(internal_Data.GetState()->pos.yawAngle)) * cycletime / 1000 ;
    internal_Data.GetState()->roadPos.s += ds;

    internal_Data.GetState()->secondarycoveredlanes.clear();

    //CHECK
    //if (internal_Data.GetState()->id==12)
    //{
    //    std::cout << "MM_kin: s=" << internal_Data.GetState()->roadPos.s << ", vlong=" << internal_Data.GetState()->velocity_long << ", along=" << internal_Data.GetState()->acceleration_long << ", x=" << internal_Data.GetState()->pos.xPos << ", y=" << internal_Data.GetState()->pos.yPos << std::endl;
    //}
}

void AgentRepresentation::ExtrapolateAlongLane(const std::vector<MentalModelLane> *lane)
{
    double ds = (internal_Data.GetState()->acceleration_long / 2) * std::pow((cycletime / 1000),2) + internal_Data.GetState()->velocity_long * cycletime / 1000;
    double s = internal_Data.GetState()->roadPos.s + ds;
    double s_near = internal_Data.GetState()->roadPos.s + ds;
    double dsmin = {std::numeric_limits<double>::max()};
    double dslast = {std::numeric_limits<double>::max()};
    double x_near = internal_Data.GetState()->pos.xPos;
    double y_near = internal_Data.GetState()->pos.yPos;
    double yaw_near = internal_Data.GetState()->pos.yawAngle;
    std::tuple<double,double,double,double> previoustuple;
    std::tuple<double,double,double,double> nexttuple;
    std::tuple<double,double,double,double> neighbortuple;

    for (std::vector<MentalModelLane>::const_iterator lanesection = lane->begin(); lanesection != lane->end(); lanesection++)
    {
        const std::vector<std::tuple<double,double,double,double>> Points = lanesection->GetPoints();
        for(std::vector<std::tuple<double,double,double,double>>::const_iterator tuple = Points.begin(); tuple != Points.end(); tuple++)
        {
            double deltas = std::abs(s-std::get<3>(*tuple));
            if (dslast < deltas)
                break;
            if (deltas<dsmin)
            {
                dsmin = deltas;
                s_near = std::get<3>(*tuple);
                x_near = std::get<0>(*tuple);
                y_near = std::get<1>(*tuple);
                yaw_near = std::get<2>(*tuple);
                nexttuple = *(tuple++);
                tuple--;
                previoustuple = *(tuple--);
                tuple++;
            }
            dslast = deltas;
        }
    }

    neighbortuple = s_near < s ? nexttuple : previoustuple;

    internal_Data.GetState()->pos.yawAngle = LinearInterpolation(yaw_near, s_near, std::get<2>(neighbortuple), std::get<3>(neighbortuple), s);
    double x = std::get<0>(neighbortuple);
    double y = std::get<1>(neighbortuple);

    internal_Data.GetState()->pos.xPos = LinearInterpolation(x_near, s_near, std::get<0>(neighbortuple), std::get<3>(neighbortuple), s);
    internal_Data.GetState()->pos.yPos = LinearInterpolation(y_near, s_near, std::get<1>(neighbortuple), std::get<3>(neighbortuple), s);

    internal_Data.GetState()->velocity_long += internal_Data.GetState()->acceleration_long * cycletime / 1000 ;
    internal_Data.GetState()->velocity_x = internal_Data.GetState()->velocity_long * std::cos(CommonHelper::ConvertDegreeToRadian(internal_Data.GetState()->pos.yawAngle));
    internal_Data.GetState()->velocity_y = internal_Data.GetState()->velocity_long * std::sin(CommonHelper::ConvertDegreeToRadian(internal_Data.GetState()->pos.yawAngle));

    internal_Data.GetState()->roadPos.s = s;

    internal_Data.GetState()->secondarycoveredlanes.clear();

    //CHECK
    //if (internal_Data.GetState()->id==12)
    //{
    //    std::cout << "MM: s=" << s << ", vlong=" << internal_Data.GetState()->velocity_long << ", along=" << internal_Data.GetState()->acceleration_long << ", x=" << internal_Data.GetState()->pos.xPos << ", y=" << internal_Data.GetState()->pos.yPos << std::endl;
    //}
}

void AgentRepresentation::LinkPosToRoadPos(int laneid, std::map<int, const std::vector<MentalModelLane*>> lanes)
{
    double dmin = {std::numeric_limits<double>::max()};
    double x = internal_Data.GetState()->pos.xPos;
    double y = internal_Data.GetState()->pos.yPos;
    double xnew = x;
    double ynew = y;
    double yaw = internal_Data.GetState()->pos.yawAngle;
    double s = internal_Data.GetState()->roadPos.s;
    int lane = laneid;
    for (auto it = lanes.begin(); it!=lanes.end(); it++)
    {
        for (auto lit : it->second)
        {
            for(auto& pit : lit->GetPoints())
            {
                double dcurr = sqrt(pow(x-std::get<0>(pit),2)+pow(y-std::get<1>(pit),2));
                if (dcurr<dmin)
                {
                    dmin = dcurr;
                    s = std::get<3>(pit);
                    xnew = std::get<0>(pit);
                    ynew = std::get<1>(pit);
                    yaw = std::get<2>(pit);
                    lane = laneid + it->first;
                }
            }
        }
    }
    internal_Data.GetState()->laneid = lane;
    internal_Data.GetState()->velocity_long = (s-internal_Data.GetState()->roadPos.s)/cycletime + internal_Data.GetState()->acceleration_long * cycletime / 1000 ;
    internal_Data.GetState()->pos.yawAngle = yaw;
    internal_Data.GetState()->velocity_x = (xnew-x)/cycletime + internal_Data.GetState()->acceleration_long * std::cos(internal_Data.GetState()->pos.yawAngle) * cycletime / 1000 ;
    internal_Data.GetState()->velocity_y = (ynew-y)/cycletime + internal_Data.GetState()->acceleration_long * std::sin(internal_Data.GetState()->pos.yawAngle) * cycletime / 1000 ;
    internal_Data.GetState()->roadPos.s = s;
}
