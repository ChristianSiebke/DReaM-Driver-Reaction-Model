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

#include <string>
#include "roadElementTypes.h"
//#include "RoadElementInterface.h"

class RoadSignalInterface// : public RoadElementInterface
{
public:
    RoadSignalInterface() = default;
    RoadSignalInterface(const RoadSignalInterface&) = delete;
    RoadSignalInterface(RoadSignalInterface&&) = delete;
    RoadSignalInterface& operator=(const RoadSignalInterface&) = delete;
    RoadSignalInterface& operator=(RoadSignalInterface&&) = delete;
    virtual ~RoadSignalInterface() = default;

    virtual std::string GetId() const = 0;
    virtual double GetS() const = 0;
    virtual bool IsValidForLane(int laneId) const = 0;
    virtual std::string GetType() const = 0;
    virtual std::string GetSubType() const = 0;
    virtual double GetValue() const = 0;
    virtual RoadSignalUnit GetUnit() const = 0;
    virtual std::string GetText() const = 0;
    virtual std::list<std::string> GetDependencies() const = 0;
};

