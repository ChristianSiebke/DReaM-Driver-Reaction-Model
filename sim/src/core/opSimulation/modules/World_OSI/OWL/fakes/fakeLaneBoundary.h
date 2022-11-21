/********************************************************************************
 * Copyright (c) 2018-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include "gmock/gmock.h"
#include "OWL/DataTypes.h"

namespace OWL::Fakes {
class LaneBoundary : public OWL::Interfaces::LaneBoundary
{
public:
    MOCK_CONST_METHOD0(GetId, OWL::Id ());
    MOCK_CONST_METHOD0(GetWidth, double ());
    MOCK_CONST_METHOD0(GetSStart, double ());
    MOCK_CONST_METHOD0(GetSEnd, double ());
    MOCK_CONST_METHOD0(GetType, LaneMarking::Type ());
    MOCK_CONST_METHOD0(GetColor, LaneMarking::Color ());
    MOCK_CONST_METHOD0(GetSide, OWL::LaneMarkingSide ());
    MOCK_METHOD2(AddBoundaryPoint, void (const Common::Vector2d& point, double heading));
    MOCK_CONST_METHOD1(CopyToGroundTruth, void (osi3::GroundTruth& target));
};
}
