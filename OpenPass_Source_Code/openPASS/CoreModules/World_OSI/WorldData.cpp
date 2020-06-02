/*******************************************************************************
* Copyright (c) 2018, 2019 in-tech GmbH
*               2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  WorldData.cpp
//! @brief This file provides access to underlying data structures for
//!        scenery and dynamic objects
//-----------------------------------------------------------------------------

#include <exception>
#include <string>
#include <qglobal.h>

#include "Common/vector3d.h"
#include "Interfaces/agentInterface.h"
#include "Interfaces/roadInterface/roadInterface.h"

#include "OWL/DataTypes.h"
#include "OWL/OpenDriveTypeMapper.h"

#include "WorldData.h"
#include "WorldDataException.h"
#include "WorldDataQuery.h"

#include "osi3/osi_groundtruth.pb.h"
#include "osi3/osi_sensorviewconfiguration.pb.h"
#include "osi3/osi_worldinterface.pb.h"
#include "osi3/osi_road.pb.h"

namespace OWL {

osi3::SensorView WorldData::GetSensorView(osi3::SensorViewConfiguration& conf, int agentId)
{
    const auto host_id = GetOwlId(agentId);
    osi3::SensorView sv;

    auto currentInterfaceVersion = osi3::InterfaceVersion::descriptor()->file()->options().GetExtension(osi3::current_interface_version);
    sv.mutable_version()->CopyFrom(currentInterfaceVersion);

    sv.mutable_sensor_id()->CopyFrom(conf.sensor_id());
    sv.mutable_mounting_position()->CopyFrom(conf.mounting_position());
    sv.mutable_mounting_position_rmse()->CopyFrom(conf.mounting_position());

    auto filteredGroundTruth = GetFilteredGroundTruth(conf, GetMovingObjectById(host_id));
    sv.mutable_global_ground_truth()->CopyFrom(filteredGroundTruth);
    sv.mutable_host_vehicle_id()->set_value(host_id);

    auto zeroVector3d = osi3::Vector3d();
    zeroVector3d.set_x(0.0);
    zeroVector3d.set_y(0.0);
    zeroVector3d.set_z(0.0);

    auto zeroOrientation3d = osi3::Orientation3d();
    zeroOrientation3d.set_yaw(0.0);
    zeroOrientation3d.set_pitch(0.0);
    zeroOrientation3d.set_roll(0.0);

    auto zeroError = osi3::BaseMoving();
    zeroError.mutable_position()->CopyFrom(zeroVector3d);
    zeroError.mutable_velocity()->CopyFrom(zeroVector3d);
    zeroError.mutable_acceleration()->CopyFrom(zeroVector3d);
    zeroError.mutable_orientation()->CopyFrom(zeroOrientation3d);
    zeroError.mutable_orientation_rate()->CopyFrom(zeroOrientation3d);
    zeroError.mutable_orientation_acceleration()->CopyFrom(zeroOrientation3d);

    auto hostData = osi3::HostVehicleData();
    auto& movingObject = GetMovingObjectById(host_id);
    hostData.mutable_location_rmse()->CopyFrom(zeroError);

    osi3::GroundTruth tempGroundTruth;
    movingObject.CopyToGroundTruth(tempGroundTruth);
    hostData.mutable_location()->CopyFrom(tempGroundTruth.mutable_moving_object(0)->base());
    sv.mutable_host_vehicle_data()->CopyFrom(hostData);

    return sv;
}

const osi3::GroundTruth &WorldData::GetOsiGroundTruth() const
{
    return osiGroundTruth.groundtruth();
}

OWL::Id WorldData::GetOwlId(int agentId)
{
    const auto& movingObject = std::find_if(movingObjects.cbegin(),
                                            movingObjects.cend(),
                                            [agentId](const std::pair<Id, MovingObject*>& mapItem)
                                            {
                                                return mapItem.second->GetLink<AgentInterface>()->GetId() == agentId;
                                            });

    if (movingObject != movingObjects.cend())
    {
        return movingObject->second->GetId();
    }
    else
    {
        return InvalidId;
    }
}

int WorldData::GetAgentId(const OWL::Id owlId) const
{
    try
    {
        return movingObjects.at(owlId)->GetLink<AgentInterface>()->GetId();
    }
    catch (const std::out_of_range&)
    {
        throw(std::out_of_range("GetAgentId called using OWL Id " + std::to_string(owlId) + " found no related agent"));
    }
}

osi3::GroundTruth WorldData::GetFilteredGroundTruth(const osi3::SensorViewConfiguration& conf, const OWL::Interfaces::MovingObject& reference)
{
    bool referenceObjectAdded = false;
    osi3::GroundTruth filteredGroundTruth;

    Primitive::AbsPosition relativeSensorPos
        { conf.mounting_position().position().x(),
          conf.mounting_position().position().y(),
          conf.mounting_position().position().z() };


    const auto& orientation = reference.GetAbsOrientation();
    relativeSensorPos.RotateYaw(orientation.yaw);
    auto absoluteSensorPos = reference.GetReferencePointPosition() + relativeSensorPos;

    const double yawMax = orientation.yaw + conf.mounting_position().orientation().yaw() + conf.field_of_view_horizontal() / 2.0;
    const double yawMin = orientation.yaw + conf.mounting_position().orientation().yaw() - conf.field_of_view_horizontal() / 2.0;

    const double range = conf.range();

    const auto& filteredMovingObjects = GetMovingObjectsInSector(absoluteSensorPos, range, yawMin, yawMax);
    const auto& filteredStationaryObjects = GetStationaryObjectsInSector(absoluteSensorPos, range, yawMin, yawMax);
    const auto& filteredTrafficSigns = GetTrafficSigns();
    const auto& filteredLanes = GetLanes();

    for (const auto& object : filteredMovingObjects)
    {
        object->CopyToGroundTruth(filteredGroundTruth);

        if (object->GetId() == reference.GetId())
        {
            referenceObjectAdded = true;
        }
    }

    if (!referenceObjectAdded)
    {
        reference.CopyToGroundTruth(filteredGroundTruth);
    }

    for (const auto& object : filteredStationaryObjects)
    {
        object->CopyToGroundTruth(filteredGroundTruth);
    }

    for (const auto& object : filteredTrafficSigns)
    {
        object.second->CopyToGroundTruth(filteredGroundTruth);
    }

    for (const auto& lane : filteredLanes)
    {
        lane.second->CopyToGroundTruth(filteredGroundTruth);
    }

    return filteredGroundTruth;
}

std::vector<const Interfaces::StationaryObject*> WorldData::GetStationaryObjectsInSector(const Primitive::AbsPosition& origin,
                                                                                         double radius,
                                                                                         double absYawMin,
                                                                                         double absYawMax)
{
    std::vector<Interfaces::StationaryObject*> objects;

    for (const auto& mapItem: stationaryObjects)
    {
        objects.push_back(mapItem.second);
    }

    return ApplySectorFilter(objects, origin, radius, absYawMax, absYawMin);
}

std::vector<const Interfaces::MovingObject*> WorldData::GetMovingObjectsInSector(const Primitive::AbsPosition& origin,
                                                                                 double radius,
                                                                                 double absYawMin,
                                                                                 double absYawMax)
{
    std::vector<Interfaces::MovingObject*> objects;

    for (const auto& mapItem: movingObjects)
    {
        objects.push_back(mapItem.second);
    }

    return ApplySectorFilter(objects, origin, radius, absYawMax, absYawMin);
}

/*
TrafficSigns haven't assigned an absiolute position at the moment
std::vector<const Interfaces::TrafficSign*> WorldData::GetTrafficSignsInSector(const Primitive::AbsPosition& origin,
                                                                               double radius,
                                                                               double absYawMin,
                                                                               double absYawMax)
{
    std::vector<Interfaces::TrafficSign*> objects;

    for (const auto& mapItem : trafficSigns)
    {
        objects.push_back(mapItem.second);
    }

    return ApplySectorFilter(objects, origin, radius, absYawMax, absYawMin);
}
*/

double WorldData::NormalizeAngle(double angle)
{
    angle = std::fmod(angle, 2.0 * M_PI);

    if (angle > M_PI)
    {
        angle -= 2.0 * M_PI;
    }
    else if (angle < - M_PI)
    {
        angle += 2.0 * M_PI;
    }

    return angle;
}

void WorldData::AddLane(RoadLaneSectionInterface& odSection, const RoadLaneInterface& odLane, const std::vector<Id> laneBoundaries)
{
    osi3::world::RoadSection* osiSection;

    try
    {
        osiSection = osiSections.at(&odSection);
    }
    catch (std::out_of_range&)
    {
        throw new OWL::NonExistentOsiRef("Section", std::to_string((odSection.GetId())));
    }

    int odLaneId = odLane.GetId();
    Id osiLaneId = CreateUid();

    Section& section = *(sections.at(osiSection->id().value()));
    osi3::world::RoadLane* osiLane = osiSection->add_lane();
    Lane& lane = *(new Implementation::Lane(osiLane, &section));
    osiLane->mutable_id()->set_value(osiLaneId);
    osiLane->mutable_base_lane()->mutable_id()->set_value(osiLaneId);
    osiLane->set_reverse_direction(!odLane.GetInDirection());

    for (const auto& laneBoundary : laneBoundaries)
    {
        osiLane->mutable_base_lane()->mutable_classification()->add_right_lane_boundary_id()->set_value(laneBoundary);
    }

    for (const auto& odLaneMapItem : odSection.GetLanes())
    {
        try
        {
            if ((odLaneId < 0 && odLaneMapItem.first == odLaneId + 1)
                    || (odLaneId > 0 && odLaneMapItem.first == odLaneId - 1))
            {
                // find a left neighbor
                osi3::world::RoadLane* leftOsiLane = osiLanes.at(odLaneMapItem.second);
                Lane& leftLane = *(lanes.at(leftOsiLane->id().value()));
                lane.SetLeftLane(leftLane);
                leftLane.SetRightLane(lane);
            }
            else if ((odLaneId < 0 && odLaneMapItem.first == odLaneId - 1)
                     || (odLaneId > 0 && odLaneMapItem.first == odLaneId + 1))
            {
                // is right neighbor
                osi3::world::RoadLane* rightOsiLane = osiLanes.at(odLaneMapItem.second);
                Lane& rightLane = *(lanes.at(rightOsiLane->id().value()));
                lane.SetRightLane(rightLane);
                rightLane.SetLeftLane(lane);
            }
        }
        catch (std::out_of_range&)
        {
            // Thats' fine. Adjacent Ids will be set when adding the missing lane.
        }
    }

    if (odLaneId == 1 || odLaneId == -1)
    {
        lane.SetLeftLaneBoundaries(section.GetCenterLaneBoundary());
    }

    lane.SetLaneType(OpenDriveTypeMapper::OdToOwlLaneType(odLane.GetType()));
    osiLanes[&odLane] = osiLane;
    lanes[osiLaneId] = &lane;
    laneIdMapping[osiLaneId] = static_cast<OWL::OdId>(odLaneId);

    section.AddLane(lane);
}

Id WorldData::AddLaneBoundary(const RoadLaneRoadMark &odLaneRoadMark, double sectionStart, LaneMarkingSide side)
{
    constexpr double standardWidth = 0.15;
    constexpr double boldWidth = 0.3;
    Id osiLaneBoundaryId = CreateUid();
    osi3::LaneBoundary* osiLaneBoundary = osiGroundTruth.mutable_groundtruth()->add_lane_boundary();
    osiLaneBoundary->mutable_id()->set_value(osiLaneBoundaryId);
    osiLaneBoundary->mutable_classification()->set_color(OpenDriveTypeMapper::OdToOsiLaneMarkingColor(odLaneRoadMark.GetColor()));
    osiLaneBoundary->mutable_classification()->set_type(OpenDriveTypeMapper::OdToOsiLaneMarkingType(odLaneRoadMark.GetType(), side));

    double width = 0.0;
    if (odLaneRoadMark.GetWeight() == RoadLaneRoadMarkWeight::Standard)
    {
        width = standardWidth;
    }
    else if (odLaneRoadMark.GetWeight() == RoadLaneRoadMarkWeight::Bold)
    {
        width = boldWidth;
    }
    LaneBoundary* laneBoundary = new OWL::Implementation::LaneBoundary(osiLaneBoundary, width, sectionStart + odLaneRoadMark.GetSOffset(), sectionStart + odLaneRoadMark.GetSEnd(), side);
    laneBoundaries[osiLaneBoundaryId] = laneBoundary;

    return osiLaneBoundaryId;
}

void WorldData::SetCenterLaneBoundary(const RoadLaneSectionInterface &odSection, std::vector<Id> laneBoundaryIds)
{
    osi3::world::RoadSection* osiSection;

    try
    {
        osiSection = osiSections.at(&odSection);
    }
    catch (std::out_of_range&)
    {
        throw new OWL::NonExistentOsiRef("Section", std::to_string((odSection.GetId())));
    }

    Section& section = *(sections.at(osiSection->id().value()));

    section.SetCenterLaneBoundary(laneBoundaryIds);
}

void WorldData::AddSection(const RoadInterface& odRoad, const RoadLaneSectionInterface& odSection)
{
    osi3::world::Road* osiRoad;

    try
    {
        osiRoad = osiRoads.at(&odRoad);
    }
    catch (std::out_of_range&)
    {
        throw new OWL::NonExistentOsiRef("Road", odRoad.GetId());
    }

    auto osiSection = osiRoad->add_section();
    auto secId = CreateUid();
    osiSection->mutable_id()->set_value(secId);
    osiSection->set_s_offset(odSection.GetStart());

    auto section = new Implementation::Section(osiSection);

    osiSections[&odSection] = osiSection;
    sections[secId] = section;

    auto road = roads.at(osiRoad->id().value());
    road->AddSection(*section);
}

void WorldData::AddRoad(const RoadInterface& odRoad)
{
    osi3::world::Road* osiRoad = osiGroundTruth.add_road();
    auto road = new Implementation::Road(osiRoad, odRoad.GetInDirection());

    Id roadId = CreateUid();
    osiRoad->mutable_id()->set_value(roadId);

    // Note: OpenDrive supports multiple road types across the length of one road but OSI doesn't
    const RoadTypeInformation odRoadType = odRoad.GetRoadType(0.0);
    osiRoad->set_type(OpenDriveTypeMapper::OdToOsiRoadType(odRoadType));

    osiRoads[&odRoad] = osiRoad;
    roads[roadId] = road;
    roadIdMapping[roadId] = odRoad.GetId();
}

void WorldData::AddJunction(const JunctionInterface *odJunction)
{
    Id junctionId = CreateUid();
    auto junction = new Implementation::Junction(junctionId);
    junctions[junctionId] = junction;
    junctionIdMapping[junctionId] = odJunction->GetId();
    for (const auto& connection : odJunction->GetConnections())
    {
        const auto connectingRoadId = connection.second->GetConnectingRoadId();
        for (const auto& [osiId, odId] : roadIdMapping)
        {
            if (odId == connectingRoadId)
            {
                junction->AddConnectingRoad(roads.at(osiId));
                break;
            }
        }
    }
}

void WorldData::AddJunctionConnection(const JunctionInterface *odJunction, const RoadInterface &odRoad)
{
    osi3::world::Road* osiRoad;
    try
    {
        osiRoad = osiRoads.at(&odRoad);
    }
    catch (std::out_of_range&)
    {
        throw new OWL::NonExistentOsiRef("Road", odRoad.GetId());
    }
    auto road = roads.at(osiRoad->id().value());
    for (const auto& [osiId, odId] : junctionIdMapping)
    {
        if (odId == odJunction->GetId())
        {
            junctions.at(osiId)->AddConnectingRoad(road);
            break;
        }
    }
}

void WorldData::AddJunctionPriority(const JunctionInterface *odJunction, const std::string &high, const std::string &low)
{
    const auto& junctionId = std::find_if(junctionIdMapping.cbegin(), junctionIdMapping.cend(),
                                       [odJunction](const auto& junction){return junction.second == odJunction->GetId();});
    auto& junction = junctions.at(junctionId->first);
    const auto& highRoadId = std::find_if(roadIdMapping.cbegin(), roadIdMapping.cend(),
                                       [high](const auto& road){return road.second == high;});
    const auto& lowRoadId = std::find_if(roadIdMapping.cbegin(), roadIdMapping.cend(),
                                       [low](const auto& road){return road.second == low;});
    junction->AddPriority(highRoadId->first, lowRoadId->first);
}

void WorldData::ConnectLanes(/* const */ RoadLaneSectionInterface& firstOdSection,
        /* const */ RoadLaneSectionInterface& secondOdSection,
        const std::map<int, int>& lanePairs,
        bool isPrev)
{
    Q_UNUSED(isPrev)
    for (auto pair : lanePairs)
    {
        /* const */ RoadLaneInterface* firstOdLane = firstOdSection.GetLanes().at(pair.first);
        /* const */ RoadLaneInterface* secondOdLane = secondOdSection.GetLanes().at(pair.second);

        osi3::world::RoadLane* firstOsiLane = osiLanes.at(firstOdLane);
        osi3::world::RoadLane* secondOsiLane = osiLanes.at(secondOdLane);

        Q_UNUSED(firstOsiLane);
        Q_UNUSED(secondOsiLane);
    }
}

void WorldData::AddLanePairing(/* const */ RoadLaneInterface& odLane,
        /* const */ RoadLaneInterface& predecessorOdLane,
        /* const */ RoadLaneInterface& successorOdLane)
{
    osi3::world::RoadLane* osiLane = osiLanes.at(&odLane);
    const osi3::world::RoadLane* predecessorOsiLane = osiLanes.at(&predecessorOdLane);
    const osi3::world::RoadLane* successorOsiLane = osiLanes.at(&successorOdLane);

    osi3::world::RoadLanePairing* pairing = osiLane->add_lane_pairing();

    pairing->mutable_antecessor_lane_id()->set_value(predecessorOsiLane->id().value());
    pairing->mutable_successor_lane_id()->set_value(successorOsiLane->id().value());
}

void WorldData::AddLaneSuccessor(/* const */ RoadLaneInterface& odLane,
        /* const */ RoadLaneInterface& successorOdLane)
{
    osi3::world::RoadLane* osiLane = osiLanes.at(&odLane);
    const osi3::world::RoadLane* successorOsiLane = osiLanes.at(&successorOdLane);

    Lane* lane = lanes.at(osiLane->id().value());
    Lane* nextLane = lanes.at(successorOsiLane->id().value());
    lane->AddNext(nextLane);
}

void WorldData::AddLanePredecessor(/* const */ RoadLaneInterface& odLane,
        /* const */ RoadLaneInterface& predecessorOdLane)
{
    osi3::world::RoadLane* osiLane = osiLanes.at(&odLane);
    const osi3::world::RoadLane* predecessorOsiLane = osiLanes.at(&predecessorOdLane);

    Lane* lane = lanes.at(osiLane->id().value());
    Lane* prevLane = lanes.at(predecessorOsiLane->id().value());
    lane->AddPrevious(prevLane);
}

Interfaces::MovingObject& WorldData::AddMovingObject(void* linkedObject)
{
    Id id = CreateUid();
    osi3::MovingObject* osiMovingObject = osiGroundTruth.mutable_groundtruth()->add_moving_object();
    auto movingObject = new MovingObject(osiMovingObject, linkedObject);

    osiMovingObject->mutable_id()->set_value(id);
    movingObjects[id] = movingObject;

    return *movingObject;
}

void WorldData::RemoveMovingObjectById(Id id)
{
    bool found = false;
    auto osiMovingObjects = osiGroundTruth.groundtruth().moving_object();

    for (int i = 0; i < osiMovingObjects.size(); ++i)
    {
        if (osiMovingObjects.Get(i).id().value() == id)
        {
            osiMovingObjects.SwapElements(i, osiMovingObjects.size() - 1);
            found = true;
        }
    }

    if (found)
    {
        osiMovingObjects.RemoveLast();
        delete movingObjects.at(id);
        movingObjects.erase(id);
    }
}

Interfaces::StationaryObject& WorldData::AddStationaryObject(void* linkedObject)
{
    osi3::StationaryObject* osiStationaryObject = osiGroundTruth.mutable_groundtruth()->add_stationary_object();
    auto stationaryObject = new StationaryObject(osiStationaryObject, linkedObject);
    Id id = CreateUid();

    osiStationaryObject->mutable_id()->set_value(id);
    stationaryObjects[id] = stationaryObject;

    return *stationaryObject;
}

Interfaces::TrafficSign& WorldData::AddTrafficSign(const std::string odId)
{
    osi3::TrafficSign* osiTrafficSign = osiGroundTruth.mutable_groundtruth()->add_traffic_sign();
    auto trafficSignal = new TrafficSign(osiTrafficSign);
    Id id = CreateUid();

    trafficSignIdMapping[odId] = id;

    osiTrafficSign->mutable_id()->set_value(id);
    trafficSigns[id] = trafficSignal;

    return *trafficSignal;
}

void WorldData::AssignTrafficSignToLane(Id laneId, Interfaces::TrafficSign& trafficSign)
{
    lanes.at(laneId)->AddTrafficSign(trafficSign);
    trafficSign.SetValidForLane(laneId);
}

void WorldData::SetSectionSuccessor(const RoadLaneSectionInterface &section, const RoadLaneSectionInterface &successorSection)
{
    osi3::world::RoadSection* firstOsiSection = osiSections.at(&section);
    const osi3::world::RoadSection* secondOsiSection = osiSections.at(&successorSection);

    firstOsiSection->mutable_successor_section()->set_value(secondOsiSection->id().value());

    Section* currSection = sections.at(firstOsiSection->id().value());
    Section* succSection = sections.at(secondOsiSection->id().value());
    currSection->AddNext(*succSection);
}

void WorldData::SetSectionPredecessor(const RoadLaneSectionInterface &section, const RoadLaneSectionInterface &predecessorSection)
{
    osi3::world::RoadSection* firstOsiSection = osiSections.at(&section);
    osi3::world::RoadSection* secondOsiSection = osiSections.at(&predecessorSection);

    firstOsiSection->mutable_antecessor_section()->set_value(secondOsiSection->id().value());
    Section* currSection = sections.at(firstOsiSection->id().value());
    Section* prevSection = sections.at(secondOsiSection->id().value());
    currSection->AddPrevious(*prevSection);
}

void WorldData::SetRoadPredecessor(const RoadInterface& road, const RoadInterface& predecessorRoad)
{
    osi3::world::Road* osiRoad = osiRoads.at(&road);
    osi3::world::Road* predecessorOsiRoad = osiRoads.at(&predecessorRoad);

    osiRoad->mutable_antecessor_road()->mutable_road_id()->set_value(predecessorOsiRoad->id().value());
    roads.at(osiRoad->id().value())->SetPredecessor(predecessorOsiRoad->id().value());
}

void WorldData::SetRoadSuccessor(const RoadInterface& road, const RoadInterface& successorRoad)
{
    osi3::world::Road* osiRoad = osiRoads.at(&road);
    osi3::world::Road* successorOsiRoad = osiRoads.at(&successorRoad);

    osiRoad->mutable_successor_road()->mutable_road_id()->set_value(successorOsiRoad->id().value());
    roads.at(osiRoad->id().value())->SetSuccessor(successorOsiRoad->id().value());
}

auto IdMatches(std::string id)
{
  return [id](const auto& junctionMapping){return id == junctionMapping.second;};
}

void WorldData::SetRoadSuccessorJunction(const RoadInterface& road, const JunctionInterface* successorJunction)
{
    osi3::world::Road* osiRoad = osiRoads.at(&road);
    OWL::Interfaces::Road* owlRoad = roads.at(osiRoad->id().value());
    auto iter = std::find_if(junctionIdMapping.begin(), junctionIdMapping.end(), IdMatches(successorJunction->GetId()));
    if (iter != junctionIdMapping.end())
    {
        owlRoad->SetSuccessor(iter->first);
    }
}

void WorldData::SetRoadPredecessorJunction(const RoadInterface& road, const JunctionInterface* predecessorJunction)
{
    osi3::world::Road* osiRoad = osiRoads.at(&road);
    OWL::Interfaces::Road* owlRoad = roads.at(osiRoad->id().value());
    auto iter = std::find_if(junctionIdMapping.begin(), junctionIdMapping.end(), IdMatches(predecessorJunction->GetId()));
    if (iter != junctionIdMapping.end())
    {
        owlRoad->SetPredecessor(iter->first);
    }
}

void WorldData::AddLaneGeometryPoint(const RoadLaneInterface& odLane,
                                     const Common::Vector2d& pointLeft,
                                     const Common::Vector2d& pointCenter,
                                     const Common::Vector2d& pointRight,
                                     const double sOffset,
                                     const double curvature,
                                     const double heading)
{
    osi3::world::RoadLane* osiLane = osiLanes.at(&odLane);

    osi3::world::LaneGeometryJoint* osiGeometry = osiLane->add_geometry();
    osi3::Vector2d* osiVector;

    Lane* lane = lanes.at(osiLane->id().value());

    lane->AddLaneGeometryJoint(pointLeft, pointCenter, pointRight, sOffset, curvature, heading);

    for (auto& laneBoundaryIndex : lane->GetRightLaneBoundaries())
    {
        auto& laneBoundary = laneBoundaries.at(laneBoundaryIndex);
        if (laneBoundary->GetSStart() <= sOffset && laneBoundary->GetSEnd() >= sOffset)
        {
            laneBoundary->AddBoundaryPoint(pointRight, heading);
        }
    }

    osiVector = osiGeometry->mutable_left();
    osiVector->set_x(pointLeft.x);
    osiVector->set_y(pointLeft.y);

    osiVector = osiGeometry->mutable_reference();
    osiVector->set_x(pointCenter.x);
    osiVector->set_y(pointCenter.y);

    osiVector = osiGeometry->mutable_right();
    osiVector->set_x(pointRight.x);
    osiVector->set_y(pointRight.y);

    osiGeometry->set_curvature(curvature);
    osiGeometry->set_heading(heading);

    int pointCount = osiLane->geometry_size();

    if (pointCount == 1)
    {
        osiLane->set_length(0.0);
    }
    else
    {
        double sStart = lane->GetLaneGeometryElements().front()->joints.current.sOffset;
        osiLane->set_length(sOffset - sStart);
    }
}

void WorldData::AddCenterLinePoint(const RoadLaneSectionInterface &odSection, const Common::Vector2d &pointCenter, const double sOffset, double heading)
{
    const auto& osiSection = osiSections.at(&odSection);
    const auto& section = sections.at(osiSection->id().value());
    for (auto& laneBoundaryIndex : section->GetCenterLaneBoundary())
    {
        auto& laneBoundary = laneBoundaries.at(laneBoundaryIndex);
        if (laneBoundary->GetSStart() <= sOffset && laneBoundary->GetSEnd() >= sOffset)
        {
            laneBoundary->AddBoundaryPoint(pointCenter, heading);
        }
    }
}

WorldData::~WorldData()
{
    for (auto road : roads)
    {
        delete road.second;
    }

    roads.clear();

    for (auto section : sections)
    {
        delete section.second;
    }

    sections.clear();

    for (auto lane : lanes)
    {
        delete lane.second;
    }

    lanes.clear();
}

const std::unordered_map<Id, Lane*>& WorldData::GetLanes() const
{
    return lanes;
}

const std::unordered_map<Id, LaneBoundary *> &WorldData::GetLaneBoundaries() const
{
    return laneBoundaries;
}

const std::map<Id, Section*>& WorldData::GetSections() const
{
    return sections;
}

CSection& WorldData::GetSectionById(Id id) const
{
    return *(sections.at(id));
}


const std::unordered_map<Id, Road*>& WorldData::GetRoads() const
{
    return roads;
}

const std::unordered_map<Id, Junction *>& WorldData::GetJunctions() const
{
    return junctions;
}

CRoad& WorldData::GetRoadById(Id id) const
{
    return *(roads.at(id));
}


const std::unordered_map<Id, StationaryObject*>& WorldData::GetStationaryObjects() const
{
    return stationaryObjects;
}

CStationaryObject& WorldData::GetStationaryObjectById(Id id) const
{
    return *(stationaryObjects.at(id));
}

const std::unordered_map<Id, MovingObject*>& WorldData::GetMovingObjects() const
{
    return movingObjects;
}

const std::unordered_map<Id, Interfaces::TrafficSign *> &WorldData::GetTrafficSigns() const
{
    return trafficSigns;
}

CMovingObject& WorldData::GetMovingObjectById(Id id) const
{
    return *(movingObjects.at(id));
}

void WorldData::Reset()
{
    for (auto movingObject : movingObjects)
    {
        delete movingObject.second;
    }
    movingObjects.clear();
}

void WorldData::Clear()
{
    for (auto sign : trafficSigns)
    {
        delete sign.second;
    }

    for (auto movingObject : movingObjects)
    {
        delete movingObject.second;
    }

    for (auto stationaryObject : stationaryObjects)
    {
        delete stationaryObject.second;
    }

    for (auto lane : lanes)
    {
        delete lane.second;
    }

    for (auto laneBoundary : laneBoundaries)
    {
        delete laneBoundary.second;
    }

    for (auto section : sections)
    {
        delete section.second;
    }

    for (auto road : roads)
    {
        delete road.second;
    }

    for (auto junction : junctions)
    {
        delete junction.second;
    }

    trafficSigns.clear();
    movingObjects.clear();
    stationaryObjects.clear();

    lanes.clear();
    laneBoundaries.clear();
    sections.clear();
    roads.clear();
    junctions.clear();


    laneIdMapping.clear();
    roadIdMapping.clear();

    osiGroundTruth.Clear();
}

}
