/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#include "sensor_driverCalculations.h"

double SensorDriverCalculations::GetLateralDistanceToObject(const std::string& roadId, const AgentInterface *otherObject)
{
    if (otherObject == nullptr)
    {
        return INFINITY;
    }

    double lateralPositionOfOtherObject = otherObject->GetObjectPosition().referencePoint.at(roadId).roadPosition.t;
    if (otherObject->GetObjectPosition().referencePoint.at(roadId).laneId == egoAgent.GetLaneIdFromRelative(0))
    {
        return lateralPositionOfOtherObject - egoAgent.GetPositionLateral();
    }
    if (otherObject->GetObjectPosition().referencePoint.at(roadId).laneId == egoAgent.GetLaneIdFromRelative(1))
    {
        return lateralPositionOfOtherObject - egoAgent.GetPositionLateral() + 0.5 * egoAgent.GetLaneWidth() + 0.5 * egoAgent.GetLaneWidth(1);
    }
    if (otherObject->GetObjectPosition().referencePoint.at(roadId).laneId == egoAgent.GetLaneIdFromRelative(-1))
    {
        return lateralPositionOfOtherObject - egoAgent.GetPositionLateral() - 0.5 * egoAgent.GetLaneWidth() - 0.5 * egoAgent.GetLaneWidth(-1);
    }
    return INFINITY;
}
