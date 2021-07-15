/*******************************************************************************
* Copyright (c) 2018, 2019, 2020, 2021 in-tech GmbH
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

#include "common/vector3d.h"
#include "include/agentInterface.h"
#include "include/roadInterface/roadInterface.h"
#include "common/openScenarioDefinitions.h"

#include "OWL/DataTypes.h"
#include "OWL/OpenDriveTypeMapper.h"
#include "common/osiUtils.h"

#include "WorldData.h"
#include "WorldDataException.h"
#include "WorldDataQuery.h"

#include "osi3/osi_groundtruth.pb.h"
#include "osi3/osi_sensorviewconfiguration.pb.h"

namespace OWL {

WorldData::WorldData(const CallbackInterface* callbacks) :
    callbacks(callbacks)
{
#ifdef USE_PROTOBUF_ARENA
    osiGroundTruth = google::protobuf::Arena::CreateMessage<osi3::GroundTruth>(&arena);
    if(!osiGroundTruth->GetArena())
    {
        LOGWARN("Protobuf arena allocation was defined when building the simulator but the loaded OSI library does not support arena alloaction.");
    }
#else
    osiGroundTruth = std::make_unique<osi3::GroundTruth>();
#endif

    osi3::utils::SetVersion(*osiGroundTruth);
}

SensorView_ptr WorldData::GetSensorView(osi3::SensorViewConfiguration& conf, int agentId)
{
    const auto host_id = GetOwlId(agentId);
#ifdef USE_PROTOBUF_ARENA
    SensorView_ptr sv = SensorView_ptr(google::protobuf::Arena::CreateMessage<osi3::SensorView>(&arena));
#else
    SensorView_ptr sv = std::make_unique<osi3::SensorView>();
#endif

    osi3::utils::SetVersion(*sv);

    sv->mutable_sensor_id()->CopyFrom(conf.sensor_id());
    sv->mutable_mounting_position()->CopyFrom(conf.mounting_position());
    sv->mutable_mounting_position_rmse()->CopyFrom(conf.mounting_position());

    auto filteredGroundTruth = GetFilteredGroundTruth(conf, GetMovingObject(host_id));
    sv->mutable_global_ground_truth()->CopyFrom(*filteredGroundTruth);
    sv->mutable_global_ground_truth()->mutable_host_vehicle_id()->set_value(host_id);
    sv->mutable_host_vehicle_id()->set_value(host_id);

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
    const auto& movingObject = GetMovingObject(host_id);
    hostData.mutable_location_rmse()->CopyFrom(zeroError);

#ifdef USE_PROTOBUF_ARENA
    GroundTruth_ptr tempGroundTruth = google::protobuf::Arena::CreateMessage<osi3::GroundTruth>(&arena);
#else
    GroundTruth_ptr tempGroundTruth = std::make_unique<osi3::GroundTruth>();
#endif
    movingObject.CopyToGroundTruth(*tempGroundTruth);
    hostData.mutable_location()->CopyFrom(tempGroundTruth->mutable_moving_object(0)->base());
    sv->mutable_host_vehicle_data()->CopyFrom(hostData);

    return sv;
}

const osi3::GroundTruth &WorldData::GetOsiGroundTruth() const
{
    return *osiGroundTruth;
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

void WorldData::SetRoadGraph(const RoadGraph&& roadGraph, const RoadGraphVertexMapping&& vertexMapping)
{
    this->roadGraph = roadGraph;
    this->vertexMapping = vertexMapping;
}

WorldData::GroundTruth_ptr WorldData::GetFilteredGroundTruth(const osi3::SensorViewConfiguration& conf, const OWL::Interfaces::MovingObject& reference)
{
    bool referenceObjectAdded = false;

#ifdef USE_PROTOBUF_ARENA
    GroundTruth_ptr filteredGroundTruth = google::protobuf::Arena::CreateMessage<osi3::GroundTruth>(&arena);
#else
    GroundTruth_ptr filteredGroundTruth = std::make_unique<osi3::GroundTruth>();
#endif

    Primitive::AbsPosition relativeSensorPos
        { conf.mounting_position().position().x(),
          conf.mounting_position().position().y(),
          conf.mounting_position().position().z() };


    const auto& orientation = reference.GetAbsOrientation();
    relativeSensorPos.RotateYaw(orientation.yaw);
    auto absoluteSensorPos = reference.GetReferencePointPosition() + relativeSensorPos;

    const double fieldOfView = conf.field_of_view_horizontal();
    double leftBoundaryAngle;
    double rightBoundaryAngle;
    if (fieldOfView >= 2 * M_PI)
    {
        leftBoundaryAngle = M_PI;
        rightBoundaryAngle = -M_PI;
    }
    else
    {
        leftBoundaryAngle = CommonHelper::SetAngleToValidRange(orientation.yaw + conf.mounting_position().orientation().yaw() + fieldOfView / 2.0);
        rightBoundaryAngle = CommonHelper::SetAngleToValidRange(orientation.yaw + conf.mounting_position().orientation().yaw() - fieldOfView / 2.0);
    }

    const double range = conf.range();

    const auto& filteredMovingObjects = GetMovingObjectsInSector(absoluteSensorPos, range, leftBoundaryAngle, rightBoundaryAngle);
    const auto& filteredStationaryObjects = GetStationaryObjectsInSector(absoluteSensorPos, range, leftBoundaryAngle, rightBoundaryAngle);
    const auto& filteredTrafficSigns = GetTrafficSignsInSector(absoluteSensorPos, range, leftBoundaryAngle, rightBoundaryAngle);
    const auto& filteredRoadMarkings = GetRoadMarkingsInSector(absoluteSensorPos, range, leftBoundaryAngle, rightBoundaryAngle);
    const auto& filteredLanes = GetLanes();
    const auto& filteredLaneBoundaries = GetLaneBoundaries();

    for (const auto& object : filteredMovingObjects)
    {
        object->CopyToGroundTruth(*filteredGroundTruth);

        if (object->GetId() == reference.GetId())
        {
            referenceObjectAdded = true;
        }
    }

    if (!referenceObjectAdded)
    {
        reference.CopyToGroundTruth(*filteredGroundTruth);
    }

    for (const auto& object : filteredStationaryObjects)
    {
        object->CopyToGroundTruth(*filteredGroundTruth);
    }

    for (const auto& trafficSign : filteredTrafficSigns)
    {
        trafficSign->CopyToGroundTruth(*filteredGroundTruth);
    }

    for (const auto& roadMarking : filteredRoadMarkings)
    {
        roadMarking->CopyToGroundTruth(*filteredGroundTruth);
    }

    for (const auto& lane : filteredLanes)
    {
        lane.second->CopyToGroundTruth(*filteredGroundTruth);
    }

    for (const auto& laneboundaries : filteredLaneBoundaries)
    {
        laneboundaries.second->CopyToGroundTruth(*filteredGroundTruth);
    }

    return filteredGroundTruth;
}

std::vector<const Interfaces::StationaryObject*> WorldData::GetStationaryObjectsInSector(const Primitive::AbsPosition& origin,
                                                                                         double radius,
                                                                                         double leftBoundaryAngle,
                                                                                         double rightBoundaryAngle)
{
    std::vector<Interfaces::StationaryObject*> objects;

    for (const auto& mapItem: stationaryObjects)
    {
        objects.push_back(mapItem.second);
    }

    return ApplySectorFilter(objects, origin, radius, leftBoundaryAngle, rightBoundaryAngle);
}

std::vector<const Interfaces::MovingObject*> WorldData::GetMovingObjectsInSector(const Primitive::AbsPosition& origin,
                                                                                 double radius,
                                                                                 double leftBoundaryAngle,
                                                                                 double rightBoundaryAngle)
{
    std::vector<Interfaces::MovingObject*> objects;

    for (const auto& mapItem: movingObjects)
    {
        objects.push_back(mapItem.second);
    }

    return ApplySectorFilter(objects, origin, radius, leftBoundaryAngle, rightBoundaryAngle);
}

std::vector<const Interfaces::TrafficSign*> WorldData::GetTrafficSignsInSector(const Primitive::AbsPosition& origin,
                                                                               double radius,
                                                                               double leftBoundaryAngle,
                                                                               double rightBoundaryAngle)
{
    std::vector<Interfaces::TrafficSign*> objects;

    for (const auto& mapItem : trafficSigns)
    {
        objects.push_back(mapItem.second);
    }

    return ApplySectorFilter(objects, origin, radius, leftBoundaryAngle, rightBoundaryAngle);
}

std::vector<const Interfaces::RoadMarking*> WorldData::GetRoadMarkingsInSector(const Primitive::AbsPosition& origin, double radius, double leftBoundaryAngle, double rightBoundaryAngle)
{
    std::vector<Interfaces::RoadMarking*> objects;

    for (const auto& mapItem : roadMarkings)
    {
        objects.push_back(mapItem.second);
    }

    return ApplySectorFilter(objects, origin, radius, leftBoundaryAngle, rightBoundaryAngle);
}

bool LaneTypeIsDriving(const RoadLaneType& laneType)
{
    switch (laneType)
    {
        case RoadLaneType::Driving:
        case RoadLaneType::Median:
        case RoadLaneType::Exit:
        case RoadLaneType::Entry:
        case RoadLaneType::OnRamp:
        case RoadLaneType::OffRamp:
        case RoadLaneType::ConnectingRamp:
            return true;
        case RoadLaneType::None:
        case RoadLaneType::Shoulder:
        case RoadLaneType::Border:
        case RoadLaneType::Stop:
        case RoadLaneType::Restricted:
        case RoadLaneType::Parking:
        case RoadLaneType::Biking:
        case RoadLaneType::Sidewalk:
        case RoadLaneType::Curb:
            return false;
        default:
            return false;
    }
}

void WorldData::AddLane(const Id id, RoadLaneSectionInterface& odSection, const RoadLaneInterface& odLane, const std::vector<Id> laneBoundaries)
{
    int odLaneId = odLane.GetId();

    Section& section = *(sections.at(&odSection));
    osi3::Lane* osiLane = osiGroundTruth->add_lane();
    Lane& lane = *(new Implementation::Lane(osiLane, &section));
    osiLane->mutable_id()->set_value(id);
    osiLane->mutable_classification()->set_centerline_is_driving_direction(odLaneId < 0);
    const bool isLeft = odLaneId > 0;

    if (isLeft)
    {
        for (const auto& laneBoundary : laneBoundaries)
        {
            osiLane->mutable_classification()->add_left_lane_boundary_id()->set_value(laneBoundary);
        }
    }
    else
    {
        for (const auto& laneBoundary : laneBoundaries)
        {
            osiLane->mutable_classification()->add_right_lane_boundary_id()->set_value(laneBoundary);
        }
    }

    for (const auto& odLaneMapItem : odSection.GetLanes())
    {
        try
        {
            if (odLaneMapItem.first == odLaneId + 1)
            {
                // is left neighbor
                osi3::Lane* leftOsiLane = osiLanes.at(odLaneMapItem.second);
                Lane& leftLane = *(lanes.at(leftOsiLane->id().value()));

                lane.SetLeftLane(leftLane, !isLeft);
                leftLane.SetRightLane(lane, isLeft);
            }
            else if (odLaneMapItem.first == odLaneId - 1)
            {
                // is right neighbor
                osi3::Lane* rightOsiLane = osiLanes.at(odLaneMapItem.second);
                Lane& rightLane = *(lanes.at(rightOsiLane->id().value()));
                lane.SetRightLane(rightLane, isLeft);
                rightLane.SetLeftLane(lane, !isLeft);
            }
            else if (odLaneId == -1 && odLaneMapItem.first == 1)
            {
                // is left neighbour across centerline
                osi3::Lane* leftOsiLane = osiLanes.at(odLaneMapItem.second);
                Lane& leftLane = *(lanes.at(leftOsiLane->id().value()));

                lane.SetLeftLane(leftLane, false);
                leftLane.SetRightLane(lane, false);
            }
            else if (odLaneId == 1 && odLaneMapItem.first == -1)
            {
                // is right neighbour across centerline
                osi3::Lane* rightOsiLane = osiLanes.at(odLaneMapItem.second);
                Lane& rightLane = *(lanes.at(rightOsiLane->id().value()));
                lane.SetRightLane(rightLane, false);
                rightLane.SetLeftLane(lane, false);
            }
        }
        catch (std::out_of_range&)
        {
            // Thats' fine. Adjacent Ids will be set when adding the missing lane.
        }
    }

    if (odLaneId == -1)
    {
        lane.SetLeftLaneBoundaries(section.GetCenterLaneBoundary());
    }

    if (odLaneId == 1)
    {
        lane.SetRightLaneBoundaries(section.GetCenterLaneBoundary());
    }

    lane.SetLaneType(OpenDriveTypeMapper::OdToOwlLaneType(odLane.GetType()));
    if (odSection.GetRoad()->GetJunctionId() != "-1")
    {
        osiLane->mutable_classification()->set_type(osi3::Lane_Classification_Type::Lane_Classification_Type_TYPE_INTERSECTION);
    }
    else if (LaneTypeIsDriving(odLane.GetType()))
    {
        osiLane->mutable_classification()->set_type(osi3::Lane_Classification_Type::Lane_Classification_Type_TYPE_DRIVING);
    }
    else
    {
        osiLane->mutable_classification()->set_type(osi3::Lane_Classification_Type::Lane_Classification_Type_TYPE_NONDRIVING);
    }
    osiLane->mutable_classification()->mutable_road_condition()->set_surface_temperature(293.0);
    osiLane->mutable_classification()->mutable_road_condition()->set_surface_water_film(0.0);
    osiLane->mutable_classification()->mutable_road_condition()->set_surface_ice(0.0);
    osiLane->mutable_classification()->mutable_road_condition()->set_surface_roughness(5.0);
    osiLane->mutable_classification()->mutable_road_condition()->set_surface_texture(0.0);
    osiLanes[&odLane] = osiLane;
    lanes[id] = &lane;
    laneIdMapping[id] = static_cast<OWL::OdId>(odLaneId);

    section.AddLane(lane);
}

Id WorldData::AddLaneBoundary(const Id id, const RoadLaneRoadMark &odLaneRoadMark, double sectionStart, LaneMarkingSide side)
{
    constexpr double standardWidth = 0.15;
    constexpr double boldWidth = 0.3;
    osi3::LaneBoundary* osiLaneBoundary = osiGroundTruth->add_lane_boundary();
    osiLaneBoundary->mutable_id()->set_value(id);
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
    laneBoundaries[id] = laneBoundary;

    return id;
}

void WorldData::SetCenterLaneBoundary(const RoadLaneSectionInterface &odSection, std::vector<Id> laneBoundaryIds)
{
    Section& section = *(sections.at(&odSection));

    section.SetCenterLaneBoundary(laneBoundaryIds);
}

void WorldData::AddSection(const RoadInterface& odRoad, const RoadLaneSectionInterface& odSection)
{
    auto section = new Implementation::Section(odSection.GetStart());

    sections[&odSection] = section;

    auto road = roads.at(&odRoad);
    road->AddSection(*section);
}

void WorldData::AddRoad(const RoadInterface& odRoad)
{
    auto road = new Implementation::Road(odRoad.GetInDirection(), odRoad.GetId());

    roads[&odRoad] = road;
    roadsById[odRoad.GetId()] = road;
}

void WorldData::AddJunction(const JunctionInterface *odJunction)
{
    auto junction = new Implementation::Junction(odJunction->GetId());
    junctions[odJunction] = junction;
    junctionsById[odJunction->GetId()] = junction;
    for (const auto& connection : odJunction->GetConnections())
    {
        const auto connectingRoadId = connection.second->GetConnectingRoadId();
        for (const auto& [odRoad, road] : roads)
        {
            if (odRoad->GetId() == connectingRoadId)
            {
                junction->AddConnectingRoad(road);
                break;
            }
        }
    }
}

void WorldData::AddJunctionConnection(const JunctionInterface *odJunction, const RoadInterface &odRoad)
{
    auto road = roads.at(&odRoad);
    for (const auto& [junction, owlJunction] : junctions)
    {
        if (junction->GetId() == odJunction->GetId())
        {
            owlJunction->AddConnectingRoad(road);
            break;
        }
    }
}

void WorldData::AddJunctionPriority(const JunctionInterface *odJunction, const std::string &high, const std::string &low)
{
    const auto& junction = junctions.at(odJunction);
    junction->AddPriority(high, low);
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

        osi3::Lane* firstOsiLane = osiLanes.at(firstOdLane);
        osi3::Lane* secondOsiLane = osiLanes.at(secondOdLane);

        Q_UNUSED(firstOsiLane);
        Q_UNUSED(secondOsiLane);
    }
}

void WorldData::AddLaneSuccessor(/* const */ RoadLaneInterface& odLane,
        /* const */ RoadLaneInterface& successorOdLane)
{
    osi3::Lane* osiLane = osiLanes.at(&odLane);
    const osi3::Lane* successorOsiLane = osiLanes.at(&successorOdLane);

    Lane* lane = lanes.at(osiLane->id().value());
    Lane* nextLane = lanes.at(successorOsiLane->id().value());
    lane->AddNext(nextLane);
}

void WorldData::AddLanePredecessor(/* const */ RoadLaneInterface& odLane,
        /* const */ RoadLaneInterface& predecessorOdLane)
{
    osi3::Lane* osiLane = osiLanes.at(&odLane);
    const osi3::Lane* predecessorOsiLane = osiLanes.at(&predecessorOdLane);

    Lane* lane = lanes.at(osiLane->id().value());
    Lane* prevLane = lanes.at(predecessorOsiLane->id().value());
    lane->AddPrevious(prevLane);
}

Interfaces::MovingObject& WorldData::AddMovingObject(const Id id, void* linkedObject)
{
    osi3::MovingObject* osiMovingObject = osiGroundTruth->add_moving_object();
    auto movingObject = new MovingObject(osiMovingObject, linkedObject);

    osiMovingObject->mutable_id()->set_value(id);
    movingObjects[id] = movingObject;

    return *movingObject;
}

void WorldData::RemoveMovingObjectById(Id id)
{
    bool found = false;
    auto osiMovingObjects = osiGroundTruth->moving_object();

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

Interfaces::StationaryObject& WorldData::AddStationaryObject(const Id id, void* linkedObject)
{
    osi3::StationaryObject* osiStationaryObject = osiGroundTruth->add_stationary_object();
    auto stationaryObject = new StationaryObject(osiStationaryObject, linkedObject);

    osiStationaryObject->mutable_id()->set_value(id);
    stationaryObjects[id] = stationaryObject;

    return *stationaryObject;
}

Interfaces::TrafficSign& WorldData::AddTrafficSign(const Id id, const std::string odId)
{
    osi3::TrafficSign* osiTrafficSign = osiGroundTruth->add_traffic_sign();
    auto trafficSignal = new TrafficSign(osiTrafficSign);

    trafficSignIdMapping[odId] = id;

    osiTrafficSign->mutable_id()->set_value(id);
    trafficSigns[id] = trafficSignal;

    return *trafficSignal;
}

Interfaces::TrafficLight &WorldData::AddTrafficLight(const Id id, const std::string odId, bool withYellow)
{
    osi3::TrafficLight* osiLightRed = osiGroundTruth->add_traffic_light();
    osiLightRed->mutable_classification()->set_color(osi3::TrafficLight_Classification_Color_COLOR_RED);
    osi3::TrafficLight* osiLightGreen = osiGroundTruth->add_traffic_light();
    osiLightGreen->mutable_classification()->set_color(osi3::TrafficLight_Classification_Color_COLOR_GREEN);
    osi3::TrafficLight* osiLightYellow = nullptr;
    if (withYellow)
    {
    osiLightYellow = osiGroundTruth->add_traffic_light();
    osiLightYellow->mutable_classification()->set_color(osi3::TrafficLight_Classification_Color_COLOR_YELLOW);
    }
    auto trafficLight = new TrafficLight(osiLightRed, osiLightYellow, osiLightGreen);

    trafficSignIdMapping[odId] = id;

    osiLightRed->mutable_id()->set_value(id);
    trafficLights[id] = trafficLight;

    return *trafficLight;
}

Interfaces::RoadMarking& WorldData::AddRoadMarking(const Id id)
{
    osi3::RoadMarking* osiRoadMarking = osiGroundTruth->add_road_marking();
    auto roadMarking = new Implementation::RoadMarking(osiRoadMarking);

    osiRoadMarking->mutable_id()->set_value(id);
    roadMarkings[id] = roadMarking;

    return *roadMarking;
}

void WorldData::AssignTrafficSignToLane(Id laneId, Interfaces::TrafficSign& trafficSign)
{
    lanes.at(laneId)->AddTrafficSign(trafficSign);
    trafficSign.SetValidForLane(laneId);
}

void WorldData::AssignRoadMarkingToLane(Id laneId, Interfaces::RoadMarking& roadMarking)
{
    lanes.at(laneId)->AddRoadMarking(roadMarking);
    roadMarking.SetValidForLane(laneId);
}

void WorldData::AssignTrafficLightToLane(Id laneId, Interfaces::TrafficLight& trafficLight)
{
    lanes.at(laneId)->AddTrafficLight(trafficLight);
    trafficLight.SetValidForLane(laneId);
}

void WorldData::SetSectionSuccessor(const RoadLaneSectionInterface &section, const RoadLaneSectionInterface &successorSection)
{
    Section* currSection = sections.at(&section);
    Section* succSection = sections.at(&successorSection);
    currSection->AddNext(*succSection);
}

void WorldData::SetSectionPredecessor(const RoadLaneSectionInterface &section, const RoadLaneSectionInterface &predecessorSection)
{
    Section* currSection = sections.at(&section);
    Section* prevSection = sections.at(&predecessorSection);
    currSection->AddPrevious(*prevSection);
}

void WorldData::SetRoadPredecessor(const RoadInterface& road, const RoadInterface& predecessorRoad)
{
    roads.at(&road)->SetPredecessor(predecessorRoad.GetId());
}

void WorldData::SetRoadSuccessor(const RoadInterface& road, const RoadInterface& successorRoad)
{
    roads.at(&road)->SetSuccessor(successorRoad.GetId());
}

auto IdMatches(std::string id)
{
  return [id](const auto& junctionMapping){return id == junctionMapping.second;};
}

void WorldData::SetRoadSuccessorJunction(const RoadInterface& road, const JunctionInterface* successorJunction)
{
    OWL::Interfaces::Road* owlRoad = roads.at(&road);
    owlRoad->SetSuccessor(successorJunction->GetId());
}

void WorldData::SetRoadPredecessorJunction(const RoadInterface& road, const JunctionInterface* predecessorJunction)
{
    OWL::Interfaces::Road* owlRoad = roads.at(&road);
    owlRoad->SetPredecessor(predecessorJunction->GetId());
}

void WorldData::AddLaneGeometryPoint(const RoadLaneInterface& odLane,
                                     const Common::Vector2d& pointLeft,
                                     const Common::Vector2d& pointCenter,
                                     const Common::Vector2d& pointRight,
                                     const double sOffset,
                                     const double curvature,
                                     const double heading)
{
    osi3::Lane* osiLane = osiLanes.at(&odLane);

    Lane* lane = lanes.at(osiLane->id().value());

    lane->AddLaneGeometryJoint(pointLeft, pointCenter, pointRight, sOffset, curvature, heading);

    const auto odLaneId = odLane.GetId();
    const bool isLeft = odLaneId > 0;

    for (auto& laneBoundaryIndex : (isLeft ? lane->GetLeftLaneBoundaries() : lane->GetRightLaneBoundaries()))
    {
        auto& laneBoundary = laneBoundaries.at(laneBoundaryIndex);
        if (laneBoundary->GetSStart() <= sOffset && laneBoundary->GetSEnd() >= sOffset)
        {
            laneBoundary->AddBoundaryPoint(isLeft ? pointLeft : pointRight, heading);
        }
    }
}

void WorldData::AddCenterLinePoint(const RoadLaneSectionInterface &odSection, const Common::Vector2d &pointCenter, const double sOffset, double heading)
{
    const auto& section = sections.at(&odSection);
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
    Clear();
}

const std::unordered_map<Id, Lane*>& WorldData::GetLanes() const
{
    return lanes;
}

const std::unordered_map<Id, LaneBoundary *> &WorldData::GetLaneBoundaries() const
{
    return laneBoundaries;
}

const std::unordered_map<std::string, Road*>& WorldData::GetRoads() const
{
    return roadsById;
}

const std::map<std::string, Junction*>& WorldData::GetJunctions() const
{
    return junctionsById;
}

const std::unordered_map<Id, StationaryObject*>& WorldData::GetStationaryObjects() const
{
    return stationaryObjects;
}

const StationaryObject& WorldData::GetStationaryObject(Id id) const
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

const std::unordered_map<Id, Interfaces::RoadMarking*>& WorldData::GetRoadMarkings() const
{
    return roadMarkings;
}

const std::unordered_map<Id, Interfaces::TrafficLight *> &WorldData::GetTrafficLights() const
{
    return trafficLights;
}

void WorldData::SetEnvironment(const openScenario::EnvironmentAction& environment)
{
    if (environment.weather.sun.intensity < THRESHOLD_ILLUMINATION_LEVEL1)
    {
        osiGroundTruth->mutable_environmental_conditions()->set_ambient_illumination(osi3::EnvironmentalConditions_AmbientIllumination_AMBIENT_ILLUMINATION_LEVEL1);
    }
    else if (environment.weather.sun.intensity < THRESHOLD_ILLUMINATION_LEVEL2)
    {
        osiGroundTruth->mutable_environmental_conditions()->set_ambient_illumination(osi3::EnvironmentalConditions_AmbientIllumination_AMBIENT_ILLUMINATION_LEVEL2);
    }
    else if (environment.weather.sun.intensity < THRESHOLD_ILLUMINATION_LEVEL3)
    {
        osiGroundTruth->mutable_environmental_conditions()->set_ambient_illumination(osi3::EnvironmentalConditions_AmbientIllumination_AMBIENT_ILLUMINATION_LEVEL3);
    }
    else if (environment.weather.sun.intensity < THRESHOLD_ILLUMINATION_LEVEL4)
    {
        osiGroundTruth->mutable_environmental_conditions()->set_ambient_illumination(osi3::EnvironmentalConditions_AmbientIllumination_AMBIENT_ILLUMINATION_LEVEL4);
    }
    else if (environment.weather.sun.intensity < THRESHOLD_ILLUMINATION_LEVEL5)
    {
        osiGroundTruth->mutable_environmental_conditions()->set_ambient_illumination(osi3::EnvironmentalConditions_AmbientIllumination_AMBIENT_ILLUMINATION_LEVEL5);
    }
    else if (environment.weather.sun.intensity < THRESHOLD_ILLUMINATION_LEVEL6)
    {
        osiGroundTruth->mutable_environmental_conditions()->set_ambient_illumination(osi3::EnvironmentalConditions_AmbientIllumination_AMBIENT_ILLUMINATION_LEVEL6);
    }
    else if (environment.weather.sun.intensity < THRESHOLD_ILLUMINATION_LEVEL7)
    {
        osiGroundTruth->mutable_environmental_conditions()->set_ambient_illumination(osi3::EnvironmentalConditions_AmbientIllumination_AMBIENT_ILLUMINATION_LEVEL7);
    }
    else if (environment.weather.sun.intensity < THRESHOLD_ILLUMINATION_LEVEL8)
    {
        osiGroundTruth->mutable_environmental_conditions()->set_ambient_illumination(osi3::EnvironmentalConditions_AmbientIllumination_AMBIENT_ILLUMINATION_LEVEL8);
    }
    else
    {
        osiGroundTruth->mutable_environmental_conditions()->set_ambient_illumination(osi3::EnvironmentalConditions_AmbientIllumination_AMBIENT_ILLUMINATION_LEVEL9);
    }
    if (environment.weather.fog.visualRange < THRESHOLD_FOG_DENSE)
    {
        osiGroundTruth->mutable_environmental_conditions()->set_fog(osi3::EnvironmentalConditions_Fog_FOG_DENSE);
    }
    else if (environment.weather.fog.visualRange < THRESHOLD_FOG_THICK)
    {
        osiGroundTruth->mutable_environmental_conditions()->set_fog(osi3::EnvironmentalConditions_Fog_FOG_THICK);
    }
    else if (environment.weather.fog.visualRange < THRESHOLD_FOG_LIGHT)
    {
        osiGroundTruth->mutable_environmental_conditions()->set_fog(osi3::EnvironmentalConditions_Fog_FOG_LIGHT);
    }
    else if (environment.weather.fog.visualRange < THRESHOLD_FOG_MIST)
    {
        osiGroundTruth->mutable_environmental_conditions()->set_fog(osi3::EnvironmentalConditions_Fog_FOG_MIST);
    }
    else if (environment.weather.fog.visualRange < THRESHOLD_FOG_POOR)
    {
        osiGroundTruth->mutable_environmental_conditions()->set_fog(osi3::EnvironmentalConditions_Fog_FOG_POOR_VISIBILITY);
    }
    else if (environment.weather.fog.visualRange < THRESHOLD_FOG_MODERATE)
    {
        osiGroundTruth->mutable_environmental_conditions()->set_fog(osi3::EnvironmentalConditions_Fog_FOG_MODERATE_VISIBILITY);
    }
    else if (environment.weather.fog.visualRange < THRESHOLD_FOG_GOOD)
    {
        osiGroundTruth->mutable_environmental_conditions()->set_fog(osi3::EnvironmentalConditions_Fog_FOG_GOOD_VISIBILITY);
    }
    else
    {
        osiGroundTruth->mutable_environmental_conditions()->set_fog(osi3::EnvironmentalConditions_Fog_FOG_EXCELLENT_VISIBILITY);
    }
}

const MovingObject& WorldData::GetMovingObject(Id id) const
{
    return *(movingObjects.at(id));
}

const RoadGraph& WorldData::GetRoadGraph() const
{
    return roadGraph;
}

const RoadGraphVertexMapping& WorldData::GetRoadGraphVertexMapping() const
{
    return vertexMapping;
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
    for (auto trafficSign : trafficSigns)
    {
        delete trafficSign.second;
    }
    trafficSigns.clear();

    for (auto movingObject : movingObjects)
    {
        delete movingObject.second;
    }
    movingObjects.clear();

    for (auto stationaryObject : stationaryObjects)
    {
        delete stationaryObject.second;
    }
    stationaryObjects.clear();

    for (auto lane : lanes)
    {
        delete lane.second;
    }
    lanes.clear();

    for (auto laneBoundary : laneBoundaries)
    {
        delete laneBoundary.second;
    }
    laneBoundaries.clear();

    for (auto section : sections)
    {
        delete section.second;
    }
    sections.clear();

    for (auto road : roads)
    {
        delete road.second;
    }
    roads.clear();

    for (auto junction : junctions)
    {
        delete junction.second;
    }
    junctions.clear();

    for (auto roadMarking : roadMarkings)
    {
        delete roadMarking.second;
    }
    roadMarkings.clear();

    laneIdMapping.clear();
    osiGroundTruth->Clear();
}

}

