/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "Interfaces/trafficObjectInterface.h"
#include "WorldObjectAdapter.h"
#include "Localization.h"

class TrafficObjectAdapter : public WorldObjectAdapter, public TrafficObjectInterface
{
private:
    double laneDirection;
    const World::Localization::Localizer& localizer;

    World::Localization::Result locateResult;
    mutable std::vector<GlobalRoadPosition> boundaryPoints;

    OpenDriveId openDriveId;

    void InitLaneDirection(double hdg);

public:
    TrafficObjectAdapter(OWL::Interfaces::WorldData& worldData,
                         const World::Localization::Localizer& localizer,
                         OWL::Primitive::AbsPosition position,
                         OWL::Primitive::Dimension dimension,
                         OWL::Primitive::AbsOrientation orientation,
                         const OpenDriveId odId);

    ObjectTypeOSI GetType() const override;
    double GetDistanceToStartOfRoad() const override;
    double GetDistanceToStartOfRoad(MeasurementPoint mp) const override;
    double GetDistanceToStartOfRoad(MeasurementPoint mp, std::string roadId) const override;
    double GetVelocity(VelocityScope velocityScope = VelocityScope::Absolute) const override;
    double GetLaneDirection() const;
    double GetLaneRemainder(Side side) const override;
    bool Locate() override;
    void Unlocate() override;

    OpenDriveId GetOpenDriveId() const override;

    // object is not inteded to be copied or assigned
    TrafficObjectAdapter(const TrafficObjectAdapter&) = delete;
    TrafficObjectAdapter(TrafficObjectAdapter&&) = delete;
    TrafficObjectAdapter& operator=(const TrafficObjectAdapter&) = delete;
    TrafficObjectAdapter& operator=(TrafficObjectAdapter&&) = delete;
    ~TrafficObjectAdapter() override;
};


