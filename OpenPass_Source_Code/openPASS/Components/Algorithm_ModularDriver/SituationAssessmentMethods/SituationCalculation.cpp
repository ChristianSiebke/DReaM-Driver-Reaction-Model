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
//! @file  SituationCalculation.cpp
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief calculate relations while assess the situation
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_SituationAssessment
//-----------------------------------------------------------------------------

#include "SituationCalculation.h"
#include "ContainerStructures.h"

SituationCalculation::SituationCalculation(int cycleTime, StochasticsInterface *stochastics):
    cycleTime(cycleTime),
    _stochastics(stochastics)
{
}

void SituationCalculation::Initialize(const VehicleModelParameters *VehicleParameters)
{
    this->VehicleParameters = VehicleParameters;
}

std::vector<double> SituationCalculation::CalculateETTC(std::vector<int> *AgentWithMinEttc)
{
    std::vector<double> minEttcVector {};
    for (auto&& it = agents->begin() ; it != agents->end(); it++)
    {        
        boundingBoxCalculation.UpdateInitialBoundingBox(it->get());
        BoundingBox Moving = boundingBoxCalculation.GetMovingBoundingBox();
        double ettc = ApproxRelations(&Moving, "ettc");
        while (ettc <= (ettcCritical))
        {
            polygon_t objectBoundingBox = boundingBoxCalculation.CalculateBoundingBox(ettc, true);
            polygon_t ownBoundingBox = boundingBoxCalculation.CalculateOwnBoundingBox(ettc, true);

            if (bg::intersects(ownBoundingBox, objectBoundingBox))
            {
                minEttcVector.push_back(ettc);
                AgentWithMinEttc->push_back(it->get()->Get_internal_Data().GetState()->id);
                break;
            }
            ettc += cycleTime * 0.001;
        }
    }

    return minEttcVector;
}

std::vector<double> SituationCalculation::CalculateTTC(std::vector<int> *AgentWithMinTtc)
{
    std::vector<double> minTtcVector {};
    for (auto&& it = agents->begin() ; it != agents->end(); it++)
    {
        boundingBoxCalculation.UpdateInitialBoundingBox(it->get());
        BoundingBox Moving = boundingBoxCalculation.GetMovingBoundingBox();
        double ttc = ApproxRelations(&Moving, "ttc");

        while (ttc <= (ttcCritical))
        {
            polygon_t objectBoundingBox = boundingBoxCalculation.CalculateBoundingBox(ttc, false);
            polygon_t ownBoundingBox = boundingBoxCalculation.CalculateOwnBoundingBox(ttc, false);

            if (bg::intersects(ownBoundingBox, objectBoundingBox))
            {
                minTtcVector.push_back(ttc);
                AgentWithMinTtc->push_back(it->get()->Get_internal_Data().GetState()->id);
                break;
            }

            ttc += cycleTime * 0.001;
        }
    }

    return minTtcVector;
}

std::vector<double> SituationCalculation::CalculateTHW(std::vector<int> *AgentWithMinThw)
{
    std::vector<double> minThwVector {};
    for (auto&& it = agents->begin() ; it != agents->end(); it++)
    {
        boundingBoxCalculation.UpdateInitialBoundingBox(it->get());
        BoundingBox Moving = boundingBoxCalculation.GetMovingBoundingBox();
        double thw = ApproxRelations(&Moving, "thw");

        if (thw<=thwCritical)
        {
            polygon_t objectBoundingBox = boundingBoxCalculation.CalculateBoundingBox(0, false);
            while (thw <= (thwCritical))
            {
                polygon_t ownBoundingBox = boundingBoxCalculation.CalculateOwnBoundingBox(thw, false);

                if (bg::intersects(ownBoundingBox, objectBoundingBox))
                {
                    minThwVector.push_back(thw);
                    AgentWithMinThw->push_back(it->get()->Get_internal_Data().GetState()->id);
                    break;
                }
                thw += cycleTime * 0.001;
            }
        }
    }

    return minThwVector;
}

SpeedLimit SituationCalculation::CheckSignsForRelevantSpeedLimit()
{
    double relative_distance = {std::numeric_limits<double>::max()};
    std::vector<CommonTrafficSign::Entity> *trafficSigns = &Environment->trafficRuleInformation.laneEgo.trafficSigns;
    for (std::vector<CommonTrafficSign::Entity>::iterator it = trafficSigns->begin() ; it!= trafficSigns->end(); ++it)
    {
        if (abs(it->relativeDistance) < Environment->roadGeometry.visibilityDistance/2 && abs(it->relativeDistance) < relative_distance)
        {
            double sl = speedLimit.TransformSignToSpeed(it.base());
            if(sl != -1)
            {
                relative_distance = it->relativeDistance;
                speedLimit.UpdateSpeedLimitAndDistance(sl, it->relativeDistance);
            }
        }
    }
    return speedLimit;
}

StopSign SituationCalculation::CheckSignsForRelevantStopSign()
{
    double relative_distance = {std::numeric_limits<double>::max()};
    std::vector<CommonTrafficSign::Entity> *trafficSigns = &Environment->trafficRuleInformation.laneEgo.trafficSigns;
    StopSign StopSign;

    for (std::vector<CommonTrafficSign::Entity>::iterator it = trafficSigns->begin() ; it!= trafficSigns->end(); ++it)
    {
        if (abs(it->relativeDistance) < Environment->roadGeometry.visibilityDistance/2
            && abs(it->relativeDistance) < relative_distance
            && it->relativeDistance >= 0
            && it->type == 206)
        {
            StopSign.Sign = it.base();
            StopSign.exists = true;
        }
    }
    return StopSign;
}

double SituationCalculation::CheckForSpeedLimitViolation(double speedLimit)
{
    double Egovelocity = std::hypot(Ego.GetInitialValues().initialVelocityX, Ego.GetInitialValues().initialVelocityY);
    double violationDifference = Egovelocity - speedLimit;
    return violationDifference;
}

void SituationCalculation::UpdateMembers(EgoData *agent,
                                         std::list<std::unique_ptr<AgentRepresentation>> *agents,
                                         StaticEnvironmentData *Environment)
{
    Ego.UpdateInitialBoundingBox(agent, VehicleParameters);
    this->agents = agents;
    this->Environment = Environment;
    boundingBoxCalculation.UpdateInitialOwnBoundingBox(&Ego);
}

void SituationCalculation::UpdateMembers(BoundingBox *agent,
                                         std::list<std::unique_ptr<AgentRepresentation>> *agents,
                                         StaticEnvironmentData *Environment)
{
    Ego = *agent;
    this->agents = agents;
    this->Environment = Environment;
    boundingBoxCalculation.UpdateInitialOwnBoundingBox(agent);
}

double SituationCalculation::ApproxRelations(BoundingBox *agent, std::string valuetoapprox)
{
    double timelimit = GetTimeLimit(valuetoapprox);

    double dmin = GetMinDistance(agent);

    double tmin = 0;
    if (valuetoapprox=="ettc")
    {
        double dslast = std::numeric_limits<double>::max();
        while (tmin <= timelimit)
        {
            double ds = calcDistance(agent, true, tmin);
            if (ds-dslast>0)
            {
                return timelimit;
            }
            if (ds <= dmin)
            {
                return tmin;
            }
            tmin += cycleTime * 0.001;
            dslast = ds;
        }
    }

    tmin = ApproxTimeToMinDistance_NoAcceleration(agent);

    double ds0 = calcDistance(agent, false, 0);
    double ds = calcDistance(agent, false, tmin);

    if (ds0 <= dmin) //if the two vehicles are at the initial position nearer than the minDistance
        return 0;
    if ((tmin < 0) || (ds > dmin) || (tmin > timelimit))
        return timelimit;

    while(ds <= dmin && tmin>=0.0)
    {
        tmin -= cycleTime * 0.001;
        ds = calcDistance(agent, false, tmin);
    }
    return tmin;
}

double SituationCalculation::GetMinDistance(BoundingBox *agent)
{
    double dRefCOGA = agent->GetInitialValues().frontLength;
    double widthA = agent->GetInitialValues().widthHalf;
    double dminAgent = sqrt(dRefCOGA*dRefCOGA + (widthA)*(widthA));

    double dRefCOGE = Ego.GetInitialValues().frontLength;
    double widthE = Ego.GetInitialValues().widthHalf;
    double dminEgo = sqrt(dRefCOGE*dRefCOGE + (widthE)*(widthE));
    return dminAgent+dminEgo;
}

double SituationCalculation::ApproxTimeToMinDistance_NoAcceleration (BoundingBox *agent)
{
    double xAgent = agent->GetInitialValues().initialPosition.x();
    double yAgent = agent->GetInitialValues().initialPosition.y();
    double xEgo = Ego.GetInitialValues().initialPosition.x();
    double yEgo = Ego.GetInitialValues().initialPosition.y();

    double vxAgent = agent->GetInitialValues().initialVelocityX;
    double vyAgent = agent->GetInitialValues().initialVelocityY;

    double vxEgo = Ego.GetInitialValues().initialVelocityX;
    double vyEgo = Ego.GetInitialValues().initialVelocityY;

    double C = vxEgo - vxAgent;
    double D = xEgo - xAgent;
    double E = vyEgo - vyAgent;
    double F = yEgo - yAgent;

    double G = (C*C + E*E);
    double H = 2*(C*D + E*F);
    double tmin = -H / (2*G);

    tmin = round(tmin*10.0)*0.1;

    return tmin;
}

double SituationCalculation::calcDistance(BoundingBox *agent, bool isaccel, double t)
{
    double dx = (Ego.GetPositionXAtTime(t,isaccel)-agent->GetPositionXAtTime(t,isaccel));
    double dy = (Ego.GetPositionYAtTime(t,isaccel)-agent->GetPositionYAtTime(t,isaccel));
    double ds = sqrt(dx*dx+dy*dy);
    return ds;
}
