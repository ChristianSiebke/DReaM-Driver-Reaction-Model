/*********************************************************************
* Copyright (c) 2019, 2020 in-tech
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "gmock/gmock.h"

#include <list>
#include <string>

#include "Interfaces/roadInterface/roadSignalInterface.h"

class FakeRoadSignal: public RoadSignalInterface
{
public:
    MOCK_CONST_METHOD0(GetId, std::string ());
    MOCK_CONST_METHOD0(GetS, double ());
    MOCK_CONST_METHOD0(GetT, double ());
    MOCK_CONST_METHOD1(IsValidForLane, bool (int laneId));
    MOCK_CONST_METHOD0(GetType, std::string ());
    MOCK_CONST_METHOD0(GetSubType, std::string ());
    MOCK_CONST_METHOD0(GetValue, double ());
    MOCK_CONST_METHOD0(GetUnit, RoadSignalUnit ());
    MOCK_CONST_METHOD0(GetText, std::string ());
    MOCK_CONST_METHOD0(GetDependencies, std::list<std::string> ());
    MOCK_CONST_METHOD0(GetIsDynamic, bool());
    MOCK_CONST_METHOD0(GetWidth, double ());
    MOCK_CONST_METHOD0(GetHeight, double ());
    MOCK_CONST_METHOD0(GetZOffset, double ());
    MOCK_CONST_METHOD0(GetOrientation, bool ());
    MOCK_CONST_METHOD0(GetHOffset, double ());
};
