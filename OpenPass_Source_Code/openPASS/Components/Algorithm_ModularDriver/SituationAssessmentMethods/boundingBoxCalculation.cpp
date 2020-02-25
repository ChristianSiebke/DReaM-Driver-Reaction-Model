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
//! @file  boundingBoxCalculation.cpp
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief class to predict the bounding box of objects in future timesteps
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_SituationAssessment
//-----------------------------------------------------------------------------


#include "boundingBoxCalculation.h"
#include "ContainerStructures.h"

SA_BoundingBoxCalculation::SA_BoundingBoxCalculation()
{
}

void SA_BoundingBoxCalculation::UpdateInitialBoundingBox(const AgentRepresentation *baseMoving)
{
    // inital object values at current position
    Moving.UpdateInitialBoundingBox(baseMoving);
}

void SA_BoundingBoxCalculation::UpdateInitialOwnBoundingBox(BoundingBox *agent)
{
    // Initial bounding box in locale coordinate system
    Ego = *agent;
}

polygon_t SA_BoundingBoxCalculation::CalculateBoundingBox(double timeStepInSeconds, bool accel)
{

    double boxPoints[][2]
    {
        { -Moving.GetInitialValues().rearLength, -Moving.GetInitialValues().widthHalf },
        { Moving.GetInitialValues().frontLength, -Moving.GetInitialValues().widthHalf },
        { Moving.GetInitialValues().frontLength, Moving.GetInitialValues().widthHalf },
        { -Moving.GetInitialValues().rearLength, Moving.GetInitialValues().widthHalf },
        { -Moving.GetInitialValues().rearLength, -Moving.GetInitialValues().widthHalf }
    };

    polygon_t box;
    bg::append(box, boxPoints);

    double traveledDistanceX = Moving.GetInitialValues().initialVelocityX * timeStepInSeconds + (accel ? (0.5 * Moving.GetInitialValues().initialAccelerationX * timeStepInSeconds * timeStepInSeconds) : 0);
    double traveledDistanceY = Moving.GetInitialValues().initialVelocityY * timeStepInSeconds + (accel ? (0.5 * Moving.GetInitialValues().initialAccelerationY * timeStepInSeconds * timeStepInSeconds) : 0);
    double traveledDistance = std::hypot(traveledDistanceX, traveledDistanceY);

    // delta from initial position (object's perspective)
    point_t deltaPosition = { 0.0, 0.0 };
    point_t rotatedDeltaPosition;
    double deltaYaw = 0.0;

    if (Moving.GetInitialValues().initialyawRate != 0.0)
    {
        // Radius of the circle the agent is moving on radius = arc length / angle (or their deriviates)
        double radius = Moving.GetInitialValues().initialVelocity / Moving.GetInitialValues().initialyawRate;
        deltaYaw = traveledDistance / radius;
        deltaPosition.x(radius * std::sin(deltaYaw));
        deltaPosition.y(radius * (1 - std::cos(deltaYaw)));
    }
    else
    {
        // Just driving in a straight line
        deltaPosition.x(traveledDistance);
    }

    // rotation in mathematical negative orientation (boost) -> invert to match
    bt::rotate_transformer<bg::radian, double, 2, 2> totalRotation(-Moving.GetInitialValues().initialYaw - deltaYaw);
    bt::rotate_transformer<bg::radian, double, 2, 2> initialRotation(-Moving.GetInitialValues().initialYaw);
    bg::transform(deltaPosition, rotatedDeltaPosition, initialRotation);

    bt::translate_transformer<double, 2, 2> totalTranslation(Moving.GetInitialValues().initialPosition.x() + rotatedDeltaPosition.x(),
                                                             Moving.GetInitialValues().initialPosition.y() + rotatedDeltaPosition.y());

    // rotate, then translate
    polygon_t boxTemp;
    bg::transform(box, boxTemp, totalRotation);
    bg::transform(boxTemp, box, totalTranslation);
    return box;
}

polygon_t SA_BoundingBoxCalculation::CalculateOwnBoundingBox(double timeStepInSeconds, bool isaccel)
{
    double boxPoints[][2]
    {
        { -Ego.GetInitialValues().rearLength, -Ego.GetInitialValues().widthHalf },
        { Ego.GetInitialValues().frontLength, -Ego.GetInitialValues().widthHalf },
        { Ego.GetInitialValues().frontLength, Ego.GetInitialValues().widthHalf },
        { -Ego.GetInitialValues().rearLength, Ego.GetInitialValues().widthHalf },
        { -Ego.GetInitialValues().rearLength, -Ego.GetInitialValues().widthHalf }
    };

    polygon_t box;
    bg::append(box, boxPoints);

    double traveledDistanceX = Ego.GetPositionXAtTime(timeStepInSeconds, isaccel);
    double traveledDistanceY = Ego.GetPositionYAtTime(timeStepInSeconds, isaccel);
    double traveledDistance = std::hypot(traveledDistanceX, traveledDistanceY);

    point_t deltaPosition = { 0.0, 0.0 };
    point_t rotatedDeltaPosition;
    double deltaYaw = 0.0;

    if (Ego.GetInitialValues().initialyawRate != 0.0)
    {
        // Radius of the circle the agent is moving on radius = arc length / angle (or their deriviates)
        double radius = Ego.GetInitialValues().initialVelocity / Ego.GetInitialValues().initialyawRate;
        deltaYaw = traveledDistance / radius;
        deltaPosition.x(radius * std::sin(deltaYaw));
        deltaPosition.y(radius * (1 - std::cos(deltaYaw)));
    }
    else
    {
        // Just driving in a straight line
        deltaPosition.x(traveledDistance);
    }

    // rotation in mathematical negative orientation (boost) -> invert to match
    bt::rotate_transformer<bg::radian, double, 2, 2> totalRotation(-Ego.GetInitialValues().initialYaw - deltaYaw);
    bt::rotate_transformer<bg::radian, double, 2, 2> initialRotation(-Ego.GetInitialValues().initialYaw);
    bg::transform(deltaPosition, rotatedDeltaPosition, initialRotation);

    bt::translate_transformer<double, 2, 2> totalTranslation(Ego.GetInitialValues().initialPosition.x() + rotatedDeltaPosition.x(),
                                                             Ego.GetInitialValues().initialPosition.y() + rotatedDeltaPosition.y());

    // rotate, then translate
    polygon_t boxTemp;
    bg::transform(box, boxTemp, totalRotation);
    bg::transform(boxTemp, box, totalTranslation);
    return box;
}
