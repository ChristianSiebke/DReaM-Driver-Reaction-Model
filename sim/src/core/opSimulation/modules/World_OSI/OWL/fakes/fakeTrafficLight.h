/********************************************************************************
 * Copyright (c) 2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include <utility>

#include "gmock/gmock.h"
#include "OWL/DataTypes.h"

namespace OWL::Fakes {
class TrafficLight : public OWL::Interfaces::TrafficLight
{
public:
    MOCK_CONST_METHOD0(GetId, std::string ());
    MOCK_CONST_METHOD0(GetS, double ());
    MOCK_CONST_METHOD1(IsValidForLane, bool (OWL::Id laneId));
    MOCK_CONST_METHOD0(GetReferencePointPosition, Primitive::AbsPosition ());
    MOCK_METHOD1(SetS, void (double sPos));
    MOCK_METHOD2(SetSpecification, bool (RoadSignalInterface* signal, const Position& position));
    MOCK_METHOD1(SetValidForLane, void (OWL::Id laneId));
    MOCK_CONST_METHOD1(CopyToGroundTruth, void (osi3::GroundTruth& target));
    MOCK_METHOD1(SetState, void (CommonTrafficLight::State newState));
    MOCK_CONST_METHOD1(GetSpecification, CommonTrafficLight::Entity (const double relativeDistance));
    MOCK_CONST_METHOD0(GetState, CommonTrafficLight::State ());
};
}
