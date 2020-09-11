/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#pragma once

#include "gmock/gmock.h"
#include "include/worldObjectInterface.h"

class FakeWorldObject : public virtual WorldObjectInterface
{
public:
    MOCK_CONST_METHOD0(GetType, ObjectTypeOSI());
    MOCK_CONST_METHOD0(GetPositionX, double());
    MOCK_CONST_METHOD0(GetPositionY, double());
    MOCK_CONST_METHOD0(GetObjectPosition, const ObjectPosition& ());
    MOCK_CONST_METHOD0(GetWidth, double());
    MOCK_CONST_METHOD0(GetLength, double());
    MOCK_CONST_METHOD0(GetHeight, double());
    MOCK_CONST_METHOD0(GetYaw, double());
    MOCK_CONST_METHOD0(GetId, int());
    MOCK_CONST_METHOD0(GetBoundingBox2D, const polygon_t& ());
    MOCK_CONST_METHOD2(GetDistanceToStartOfRoad, double(MeasurementPoint, std::string));
    MOCK_CONST_METHOD0(GetDistanceReferencePointToLeadingEdge, double());
    MOCK_CONST_METHOD2(GetLaneRemainder, double(const std::string& roadId, Side));
    MOCK_CONST_METHOD0(GetLaneDirection, double());
    MOCK_CONST_METHOD0(GetVelocity, double());
    MOCK_CONST_METHOD1(GetVelocity, double(VelocityScope));
    MOCK_CONST_METHOD0(GetAcceleration, double());
    MOCK_METHOD0(Unlocate, void());
    MOCK_METHOD0(Locate, bool());
};

