/********************************************************************************
 * Copyright (c) 2019 in-tech GmbH
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

namespace OWL::Fakes{
class Road: public OWL::Interfaces::Road
{
public:
    MOCK_CONST_METHOD0(GetId, const std::string&());
    MOCK_CONST_METHOD0(GetSections, std::list<const OWL::Interfaces::Section*>&());
    MOCK_METHOD1(AddSection, void(OWL::Interfaces::Section& ));
    MOCK_CONST_METHOD0(GetLength, double());
    MOCK_CONST_METHOD0(GetSuccessor, const std::string& ());
    MOCK_CONST_METHOD0(GetPredecessor, const std::string& ());
    MOCK_METHOD1(SetSuccessor, void (const std::string&));
    MOCK_METHOD1(SetPredecessor, void (const std::string&));
    MOCK_CONST_METHOD0(IsInStreamDirection, bool ());
    MOCK_CONST_METHOD1(GetDistance, double (OWL::MeasurementPoint));
};
}
