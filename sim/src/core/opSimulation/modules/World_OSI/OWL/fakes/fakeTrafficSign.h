/********************************************************************************
 * Copyright (c) 2018-2020 in-tech GmbH
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

#include "common/globalDefinitions.h"

class RoadSignalInterface;

namespace OWL::Fakes {
class TrafficSign : public OWL::Interfaces::TrafficSign
{
public:
    MOCK_CONST_METHOD0(GetId,
                       std::string());
    MOCK_CONST_METHOD0(GetS,
                       double());
    MOCK_CONST_METHOD2(GetValueAndUnitInSI,
                       std::pair<double, CommonTrafficSign::Unit> (const double osiValue, const osi3::TrafficSignValue_Unit osiUnit));
    MOCK_CONST_METHOD1(GetSpecification,
                       CommonTrafficSign::Entity(double));
    MOCK_CONST_METHOD0(GetReferencePointPosition,
                       Primitive::AbsPosition ());
    MOCK_CONST_METHOD0(GetDimension,
                       Primitive::Dimension ());
    MOCK_CONST_METHOD1(IsValidForLane,
                       bool(OWL::Id laneId));
    MOCK_METHOD1(SetS,
                 void(double));
    MOCK_METHOD1(SetValidForLane,
                 void(OWL::Id));
    MOCK_METHOD2(SetSpecification,
                 bool(RoadSignalInterface*, Position position));
    MOCK_METHOD2(AddSupplementarySign,
                 bool (RoadSignalInterface* odSignal, Position position));
    MOCK_CONST_METHOD1(CopyToGroundTruth,
                       void(osi3::GroundTruth&));
};
}
