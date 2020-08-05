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
//! @file  SituationCalculation.h
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief calculate relations while assess the situation
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_SituationAssessment
//-----------------------------------------------------------------------------

#pragma once
#include <boost/geometry/geometries/adapted/c_array.hpp>
#include "Common/boostGeometryCommon.h"
#include "osi/osi_sensordata.pb.h"
#include "boundingBoxCalculation.h"

class SituationCalculation
{
public:
    SituationCalculation(int cycleTime, StochasticsInterface *stochastics);

    void Initialize(const VehicleModelParameters *vehicleParameters);

    void UpdateMembers(egoData* Ego,
                       std::list<std::unique_ptr<AgentRepresentation>> *Agents,
                       StaticEnvironmentData *Environment);
    void UpdateMembers(BoundingBox *Ego,
                       std::list<std::unique_ptr<AgentRepresentation>> *Agents,
                       StaticEnvironmentData *Environment);

    std::vector<double> CalculateTTC(std::vector<int> *AgentWithMinTtc);
    std::vector<double> CalculateETTC(std::vector<int> *AgentWithMinEttc);
    std::vector<double> CalculateTHW(std::vector<int> *AgentWithMinThw);

    speedLimit CheckSignsForRelevantSpeedLimit();
    stopSign CheckSignsForRelevantStopSign();
    double CheckForSpeedLimitViolation(double speedLimit);

    int GetEgoId()
    {
        return Ego.GetInitialValues().id;
    }

    //!
    //! \brief GetTimeLimit
    //! Get the current maximum timelimit of the value that we want to approximate (e.g. ttc, thw)
    //! \param valuetoapprox    situation asset
    //! \return                 timelimit
    //!
    double GetTimeLimit(std::string valuetoapprox)
    {
        if (valuetoapprox=="ttc")
            return ttcCritical;
        if (valuetoapprox=="ettc")
            return ettcCritical;
        if (valuetoapprox=="thw")
            return thwCritical;
        return INFINITY;
    }

    //!
    //! \brief GetMinDistance
    //! Get the minimum distance between two vehicles for pre-consideration of their relations (e.g. ttc, thw).
    //! It is calculatet from two circle-radii constructed from the Ref to the front-right of each vehicle.
    //! \param agent    agent to consider
    //! \return         minimum distance in m
    //!
    double GetMinDistance(BoundingBox *agent);

    double ApproxRelations(BoundingBox *agent, std::string valuetoapprox);

    double ApproxTimeToMinDistance_NoAcceleration (BoundingBox *agent);

    double calcDistance(BoundingBox *agent, bool isaccel, double t);

private:

    StochasticsInterface *_stochastics;

    BoundingBox Ego;
    int EgoId;
    std::list<std::unique_ptr<AgentRepresentation>> *agents;
    StaticEnvironmentData *Environment;
    const VehicleModelParameters *VehicleParameters;

    SA_BoundingBoxCalculation boundingBoxCalculation;

    double collisionDetectionLongitudinalBoundary = 0; ///!< Additional length added to the vehicle boundary when checking for collision detection
    double collisionDetectionLateralBoundary = 0; ///!< Additional width added to the vehicle boundary when checking for collision detection
    double ttcCritical = 3;
    double ettcCritical = 3;
    double thwCritical = 2;
    int cycleTime;

    speedLimit SpeedLimit;

    std::list<std::pair<double,double>> lastSpeedLimit;
};
