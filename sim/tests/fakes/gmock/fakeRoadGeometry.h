/********************************************************************************
 * Copyright (c) 2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "include/roadInterface/roadGeometryInterface.h"
#include "gmock/gmock.h"

class FakeRoadGeometry : public RoadGeometryInterface
{
  public:
    MOCK_CONST_METHOD2(GetCoord, Common::Vector2d (double geometryOffset, double laneOffset));
    MOCK_CONST_METHOD1(GetDir, double (double geometryOffset));
    MOCK_CONST_METHOD0(GetS, double ());
    MOCK_CONST_METHOD0(GetHdg, double ());
    MOCK_CONST_METHOD0(GetLength, double ());
};
