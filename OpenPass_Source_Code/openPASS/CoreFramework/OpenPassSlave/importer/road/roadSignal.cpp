/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#include "roadSignal.h"
#include <exception>

std::string RoadSignal::GetType() const
{
    return signal.type;
}

std::string RoadSignal::GetSubType() const
{
    return signal.subtype;
}

std::string RoadSignal::GetId() const
{
    return signal.id;
}

double RoadSignal::GetValue() const
{
    return signal.value;
}

RoadSignalUnit RoadSignal::GetUnit() const
{
    return signal.unit;
}

std::string RoadSignal::GetText() const
{
    return signal.text;
}

double RoadSignal::GetS() const
{
    return signal.s;
}

double RoadSignal::GetT() const
{
    return signal.t;
}

bool RoadSignal::IsValidForLane(int laneId) const
{
    return signal.validity.all ||
      ( std::find( signal.validity.lanes.begin(), signal.validity.lanes.end(), laneId) != signal.validity.lanes.end() );
}

double RoadSignal::GetHeight() const
{
    return signal.height;
}

double RoadSignal::GetWidth() const
{
    return signal.width;
}

double RoadSignal::GetLength() const
{
    return signal.length;
}

double RoadSignal::GetPitch() const
{
   return signal.pitch;
}

double RoadSignal::GetRoll() const
{
    return signal.roll;
}

double RoadSignal::GetYaw() const
{
    return signal.yaw;
}

bool RoadSignal::GetIsDynamic() const
{
    return signal.dynamic == "yes";
}

std::list<std::string> RoadSignal::GetDependencies() const
{
    return signal.dependencyIds;
}

