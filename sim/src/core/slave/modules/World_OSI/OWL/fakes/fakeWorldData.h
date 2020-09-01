/*******************************************************************************
* Copyright (c) 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#pragma once

#include "gmock/gmock.h"
#include "WorldData.h"

namespace osi3 {
class SensorView;
class SensorViewConfiguration;
}

namespace OWL::Fakes {
class WorldData : public OWL::Interfaces::WorldData
{
public:
    MOCK_METHOD0(Clear, void());
    MOCK_METHOD0(Reset, void());
    MOCK_CONST_METHOD0(GetOsiGroundTruth, const osi3::GroundTruth& ());
    MOCK_CONST_METHOD0(GetRoads,
                       const std::unordered_map<std::string, OWL::Interfaces::Road*>& ());
    MOCK_METHOD1(AddMovingObject,
                 OWL::Interfaces::MovingObject & (void* linkedObject));
    MOCK_METHOD1(AddStationaryObject,
                 OWL::Interfaces::StationaryObject & (void* linkedObject));
    MOCK_METHOD1(RemoveMovingObjectById,
                 void(OWL::Id id));
    MOCK_CONST_METHOD0(GetLaneIdMapping,
                       const std::unordered_map<OWL::Id, OWL::OdId>& ());
    MOCK_CONST_METHOD0(GetLanes,
                       const std::unordered_map<OWL::Id, OWL::Interfaces::Lane*>& ());
    MOCK_METHOD3(AddLane, void(RoadLaneSectionInterface& odSection, const RoadLaneInterface& odLane, const std::vector<OWL::Id> laneBoundaries));
    MOCK_METHOD2(AddSection, void(const RoadInterface&, const RoadLaneSectionInterface&));
    MOCK_METHOD1(AddRoad, void(const RoadInterface&));
    MOCK_METHOD1(AddJunction, void (const JunctionInterface* odJunction));
    MOCK_METHOD2(AddJunctionConnection, void (const JunctionInterface* odJunction, const RoadInterface& odRoad));

    MOCK_METHOD7(AddLaneGeometryPoint, void(const RoadLaneInterface&,
                                            const Common::Vector2d&,
                                            const Common::Vector2d&,
                                            const Common::Vector2d&,
                                            const double,
                                            const double,
                                            const double));

    MOCK_METHOD4(AddCenterLinePoint, void (const RoadLaneSectionInterface& odSection,
                                           const Common::Vector2d& pointCenter,
                                           const double sOffset,
                                           double heading));

    MOCK_METHOD2(AddLaneSuccessor, void(/* const */ RoadLaneInterface&,
                 /* const */ RoadLaneInterface&));

    MOCK_METHOD2(AddLanePredecessor, void(/* const */ RoadLaneInterface&,
                 /* const */ RoadLaneInterface&));

    MOCK_METHOD2(SetRoadSuccessor, void(const RoadInterface&, const RoadInterface&));

    MOCK_METHOD2(SetRoadPredecessor, void(const RoadInterface&, const RoadInterface&));

    MOCK_METHOD2(SetRoadSuccessorJunction, void (const RoadInterface&,  const JunctionInterface*));

    MOCK_METHOD2(SetRoadPredecessorJunction, void (const RoadInterface&,  const JunctionInterface*));

    MOCK_METHOD2(SetSectionSuccessor, void(const RoadLaneSectionInterface&, const RoadLaneSectionInterface&));

    MOCK_METHOD2(SetSectionPredecessor, void(const RoadLaneSectionInterface&, const RoadLaneSectionInterface&));

    MOCK_METHOD4(ConnectLanes, void(/* const */ RoadLaneSectionInterface&,
                 /* const */ RoadLaneSectionInterface&,
                 const std::map<int, int>&,
                 bool));

    MOCK_METHOD1(AddTrafficSign, OWL::Interfaces::TrafficSign&(const std::string odId));

    MOCK_CONST_METHOD0(GetTrafficSigns, const std::unordered_map<OWL::Id, OWL::Interfaces::TrafficSign*>& ());

    MOCK_CONST_METHOD0(GetTrafficSignIdMapping, const std::unordered_map<std::string, OWL::Id>& ());

    MOCK_METHOD0(AddRoadMarking, OWL::Interfaces::RoadMarking&());
    
    MOCK_CONST_METHOD0(GetRoadMarkings, const std::unordered_map<OWL::Id, OWL::Interfaces::RoadMarking*>& ());

    MOCK_CONST_METHOD0(GetRoadGraph, const RoadGraph& ());
    
    MOCK_METHOD2(SetRoadGraph, void  (const RoadGraph&& roadGraph, const RoadGraphVertexMapping&& vertexMapping));

    MOCK_CONST_METHOD0(GetRoadGraphVertexMapping, const RoadGraphVertexMapping& ());

    MOCK_METHOD2(GetSensorView, osi3::SensorView(osi3::SensorViewConfiguration&, int));

    MOCK_CONST_METHOD0(GetLaneBoundaries, const std::unordered_map<OWL::Id, OWL::Interfaces::LaneBoundary*>& ());

    MOCK_METHOD3(AddLaneBoundary, OWL::Id (const RoadLaneRoadMark &odLaneRoadMark, double sectionStart, OWL::LaneMarkingSide side));

    MOCK_METHOD2(SetCenterLaneBoundary, void (const RoadLaneSectionInterface& odSection, std::vector<OWL::Id> laneBoundaryIds));

    MOCK_METHOD3(AddCenterLinePoint, void (const RoadLaneSectionInterface& odSection,
                                           const Common::Vector2d& pointCenter,
                                           const double sOffset));

    MOCK_CONST_METHOD0(GetJunctions, const std::map<std::string, OWL::Junction*>& ());

    MOCK_METHOD2(AssignTrafficSignToLane, void (OWL::Id laneId, OWL::Interfaces::TrafficSign& trafficSign));

    MOCK_METHOD2(AssignRoadMarkingToLane, void (OWL::Id laneId, OWL::Interfaces::RoadMarking& roadMarking));

    MOCK_METHOD3(AddJunctionPriority, void (const JunctionInterface* odJunction, const std::string& high, const std::string& low));

    const OWL::Implementation::InvalidLane& GetInvalidLane() const override
    {
        return invalidLane;
    }

    MOCK_METHOD1(GetOwlId, OWL::Id(int));

    MOCK_CONST_METHOD1(GetAgentId, int(const OWL::Id));

    const OWL::Implementation::InvalidLane invalidLane;
};
}
