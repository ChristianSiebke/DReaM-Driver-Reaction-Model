/******************************************************************************
* Copyright (c) 2019 in-tech GmbH
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
//! @file  boundingBoxCalculation.h
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief class to predict the bounding box of objects in future timesteps
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_SituationAssessment
//-----------------------------------------------------------------------------

#pragma once
#include <boost/geometry/geometries/adapted/c_array.hpp>
#include "Common/boostGeometryCommon.h"
#include "ContainerStructures.h"
#include "osi/osi_sensordata.pb.h"

class BoundingBox
{   
    struct InitialBoundingBox
    {
        int id;
        point_t initialPosition;
        double initialVelocityX;
        double initialVelocityY;
        double initialVelocity;
        double initialYaw;
        double initialyawRate;
        double initialAccelerationX;
        double initialAccelerationY;
        // Initial bounding box in locale coordinate system
        double frontLength;
        double rearLength;
        double widthHalf;
    };

public:

    void UpdateInitialBoundingBox(const AgentRepresentation *baseMoving)
    {
        // inital object values at current position
        initialBB.initialPosition = {baseMoving->Get_internal_Data().GetState()->pos.xPos, baseMoving->Get_internal_Data().GetState()->pos.yPos};
        initialBB.initialVelocityX = baseMoving->Get_internal_Data().GetState()->velocity_x;
        initialBB.initialVelocityY = baseMoving->Get_internal_Data().GetState()->velocity_y;
        initialBB.initialVelocity = std::hypot(initialBB.initialVelocityX, initialBB.initialVelocityY);
        initialBB.initialYaw = baseMoving->Get_internal_Data().GetState()->pos.yawAngle;
        initialBB.initialyawRate = baseMoving->Get_internal_Data().GetState()->yaw_velocity;

        initialBB.initialAccelerationX = baseMoving->Get_internal_Data().GetState()->acceleration_long * sin(initialBB.initialYaw);
        initialBB.initialAccelerationY = baseMoving->Get_internal_Data().GetState()->acceleration_long * cos(initialBB.initialYaw);

        // Initial bounding box in locale coordinate system
        initialBB.frontLength = baseMoving->Get_internal_Data().GetProperties()->distanceReftoLeadingEdge;
        initialBB.rearLength = baseMoving->Get_internal_Data().GetProperties()->lx - baseMoving->Get_internal_Data().GetProperties()->distanceReftoLeadingEdge;
        initialBB.widthHalf  = baseMoving->Get_internal_Data().GetProperties()->ly / 2.0;
    }

    void UpdateInitialBoundingBox(AgentInterface *agent)
    {
        initialBB.id = agent->GetId();
        // Initial bounding box in locale coordinate system
        initialBB.frontLength = agent->GetDistanceReferencePointToLeadingEdge();
        initialBB.rearLength = agent->GetLength() - agent->GetDistanceReferencePointToLeadingEdge();
        initialBB.widthHalf  = (agent->GetWidth())/2;

        initialBB.initialPosition = {agent->GetPositionX(), agent->GetPositionY()};
        initialBB.initialVelocityX = agent->GetVelocity(VelocityScope::DirectionX);
        initialBB.initialVelocityY = agent->GetVelocity(VelocityScope::DirectionY);
        initialBB.initialVelocity = std::hypot(initialBB.initialVelocityX, initialBB.initialVelocityY);
        initialBB.initialYaw = agent->GetYaw();
        initialBB.initialyawRate = agent->GetYawRate();
        initialBB.initialAccelerationX = agent->GetAcceleration() * cos(initialBB.initialYaw);
        initialBB.initialAccelerationY = agent->GetAcceleration() * sin(initialBB.initialYaw);;
    }

    void UpdateInitialBoundingBox(EgoData *agent, const VehicleModelParameters *VehicleParameters)
    {
        initialBB.id = agent->GetState()->id;

        // Initial bounding box in locale coordinate system
        initialBB.frontLength = VehicleParameters->distanceReferencePointToLeadingEdge;
        initialBB.rearLength = VehicleParameters->length - VehicleParameters->distanceReferencePointToLeadingEdge;
        initialBB.widthHalf  = (VehicleParameters->width) / 2.0;

        initialBB.initialPosition = {agent->GetState()->pos.xPos, agent->GetState()->pos.yPos};
        initialBB.initialVelocityX = agent->GetState()->velocity_x;
        initialBB.initialVelocityY = agent->GetState()->velocity_y;
        initialBB.initialVelocity = std::hypot(initialBB.initialVelocityX, initialBB.initialVelocityY);
        initialBB.initialYaw = agent->GetState()->pos.yawAngle;
        initialBB.initialyawRate = agent->GetState()->yaw_velocity;
        initialBB.initialAccelerationX = agent->GetState_Ego()->acceleration_long * cos(initialBB.initialYaw);
        initialBB.initialAccelerationY = agent->GetState_Ego()->acceleration_long * sin(initialBB.initialYaw);
    }

    InitialBoundingBox GetInitialValues()
    {
        return initialBB;
    }

    double GetPositionXAtTime(double timeStepInSeconds, bool isaccel)
    {
        traveledDistanceX = initialBB.initialVelocityX * timeStepInSeconds + (isaccel ? (0.5 * initialBB.initialAccelerationX * timeStepInSeconds * timeStepInSeconds) : 0);
        return traveledDistanceX;
    }
    double GetPositionYAtTime(double timeStepInSeconds, bool isaccel)
    {
        traveledDistanceY = initialBB.initialVelocityY * timeStepInSeconds + (isaccel ? (0.5 * initialBB.initialAccelerationY * timeStepInSeconds * timeStepInSeconds) : 0);
        return traveledDistanceY;
    }

private:

    InitialBoundingBox initialBB;
    double traveledDistanceX;
    double traveledDistanceY;
};

/** \addtogroup Algorithm_SituationAssessment
 * @{
 * \brief Utility class to predict the bounding box of objects in future timesteps
* @} */
class SA_BoundingBoxCalculation
{
public:
    SA_BoundingBoxCalculation();

    void Initialize(const VehicleModelParameters *vehicleParameters);

    void UpdateInitialBoundingBox(const AgentRepresentation *baseMoving);
    void UpdateInitialOwnBoundingBox(BoundingBox *agent);

    /*!
     * \brief Predicts the bounding box of a moving object in local coordinates
     * \param timeStepInSeconds relative time since current timeStep
     * \param baseMoving        object to predict
     * \return predicted bounding box
     */
    polygon_t CalculateBoundingBox(double timeStepInSeconds, bool accel);

    /*!
     * \brief Predicts the bounding box of the own agent in local coordinates (coordinate system does not move along)
     * \param timeStepInSeconds relative time since current timeStep
     * \return predicted bounding box
     */
    polygon_t CalculateOwnBoundingBox(double timeStepInSeconds, bool accel);

    BoundingBox GetMovingBoundingBox()
    {
        return Moving;
    }

    BoundingBox GetEgoBoundingBox()
    {
        return Ego;
    }

private:

    double collisionDetectionLongitudinalBoundary = 0.3; ///!< Additional length added to the vehicle boundary when checking for collision detection
    double collisionDetectionLateralBoundary = 0.3; ///!< Additional width added to the vehicle boundary when checking for collision detection

    BoundingBox Moving;
    BoundingBox Ego;

};
