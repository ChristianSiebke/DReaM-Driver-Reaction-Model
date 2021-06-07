/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020, 2021 in-tech GmbH
*               2020 HLRS, University of Stuttgart.
*               2020 BMW AG
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "SceneryConverter.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <variant>

#include <QFile>

#include "EntityRepository.h"
#include "GeometryConverter.h"
#include "SceneryEntities.h"
#include "TrafficObjectAdapter.h"
#include "WorldData.h"
#include "WorldDataQuery.h"
#include "common/opMath.h"

namespace Internal {

ConversionStatus ConnectJunction(const SceneryInterface *scenery, const JunctionInterface *junction,
                                 PathInJunctionConnector connectPathInJunction)
{
    for (auto &entry : junction->GetConnections())
    {
        ConnectionInterface *connection = entry.second;

        std::string incomingRoadId = connection->GetIncommingRoadId();
        auto *incomingRoad = scenery->GetRoad(incomingRoadId);

        std::string connectingId = connection->GetConnectingRoadId();
        auto *connectingRoad = scenery->GetRoad(connectingId);

        std::string outgoingRoadId;

        ContactPointType connectingContactPoint = connection->GetContactPoint();
        ContactPointType incomingContactPoint;
        ContactPointType outgoingContactPoint;

        const auto incomingRoadLinkType = (connectingContactPoint == ContactPointType::Start ? RoadLinkType::Predecessor : RoadLinkType::Successor);
        const auto outgoingRoadLinkType = (connectingContactPoint == ContactPointType::Start ? RoadLinkType::Successor : RoadLinkType::Predecessor);
        for (auto roadLink : connectingRoad->GetRoadLinks())
        {
            if (roadLink->GetType() == incomingRoadLinkType)
            {
                incomingContactPoint = roadLink->GetContactPoint();
            }
            if (roadLink->GetType() == outgoingRoadLinkType)
            {
                outgoingRoadId = roadLink->GetElementId();
                outgoingContactPoint = roadLink->GetContactPoint();
            }
        }

        auto *outgoingRoad = scenery->GetRoad(outgoingRoadId);

        if (!incomingRoad || !connectingRoad || !outgoingRoad)
        {
            return {false, "Road linkage of junction " + junction->GetId() +
                               ": Potentially wrong ID of incoming, connecting, or outgoing road."};
        }

        if (incomingRoad->GetLaneSections().empty())
        {
            return {false, "Road linkage of junction " + junction->GetId() + ": Incoming road (" +
                               incomingRoad->GetId() + ") has no sections"};
        }

        if (connectingRoad->GetLaneSections().empty())
        {
            return {false, "Road linkage of junction " + junction->GetId() + ": Connecting road (" +
                               incomingRoad->GetId() + ") has no sections"};
        }

        auto laneIdMapping = connection->GetLinks();
        connectPathInJunction(junction, incomingRoad, connectingRoad, outgoingRoad, incomingContactPoint, connectingContactPoint, outgoingContactPoint,
                              laneIdMapping);
    }

    return {true};
}
} // namespace Internal

Position SceneryConverter::RoadCoord2WorldCoord(const RoadInterface *road, double s, double t, double yaw)
{
    auto &geometries = road->GetGeometries();
    auto geometry = std::find_if(geometries.cbegin(), geometries.cend(),
                                 [&](const RoadGeometryInterface *geometry) { return (geometry->GetS() <= s) && (geometry->GetS() + geometry->GetLength() >= s); });
    if (geometry == geometries.end())
    {
        throw std::runtime_error("GetPositionForRoadCoordinates: No matching geometry within given s-coordinate");
    }
    auto coordinates = (*geometry)->GetCoord(s - (*geometry)->GetS(), t);
    auto absoluteYaw = CommonHelper::SetAngleToValidRange((*geometry)->GetDir(s - (*geometry)->GetS()) + yaw);
    return {coordinates.x, coordinates.y, absoluteYaw, 0};
}

SceneryConverter::SceneryConverter(SceneryInterface *scenery,
                                   openpass::entity::RepositoryInterface &repository,
                                   OWL::Interfaces::WorldData &worldData,
                                   const World::Localization::Localizer &localizer,
                                   const CallbackInterface *callbacks) :
    scenery(scenery),
    repository(repository),
    worldData(worldData),
    localizer(localizer),
    callbacks(callbacks)
{
}

RoadLaneInterface *SceneryConverter::GetOtherLane(RoadLaneSectionInterface *otherSection,
                                                  int otherId)
{
    // search for connected lane in OpenDrive data structures since LaneId of
    // LINEAR_LANE Objects might be reordered according to direction definition
    RoadLaneInterface *connectedRoadLane;

    for (auto findIt = otherSection->GetLanes().begin();
         otherSection->GetLanes().end() != findIt;
         ++findIt)
    {
        connectedRoadLane = findIt->second;
        if (connectedRoadLane->GetId() == otherId)
        {
            return connectedRoadLane;
        }
    }

    return nullptr;
}

RoadInterface *SceneryConverter::GetConnectedRoad(RoadLinkInterface *roadLink)
{
    if (ContactPointType::Start != roadLink->GetContactPoint() &&
        ContactPointType::End != roadLink->GetContactPoint())
    {
        LOG(CbkLogLevel::Error, "no contact point defined for road link");
        return nullptr;
    }

    RoadInterface *connectedRoad = nullptr;
    for (auto &item : scenery->GetRoads())
    {
        RoadInterface *itemRoad = item.second;
        if (roadLink->GetElementId() == itemRoad->GetId())
        {
            connectedRoad = itemRoad;
            break;
        }
    }

    if (!connectedRoad)
    {
        LOG(CbkLogLevel::Error, "missing road for ElementId " + roadLink->GetElementId());
        return nullptr;
    }

    return connectedRoad;
}

void SceneryConverter::MarkDirectionRoad(RoadInterface *road,
                                         bool inDirection)
{
    LOG(CbkLogLevel::Debug, "direction of road " + road->GetId() + ": " + (inDirection ? "true" : "false"));

    road->SetInDirection(inDirection);

    for (RoadLaneSectionInterface *roadLaneSection : road->GetLaneSections())
    {
        roadLaneSection->SetInDirection(inDirection);

        for (auto &item : roadLaneSection->GetLanes())
        {
            RoadLaneInterface *roadLane = item.second;
            roadLane->SetInDirection(inDirection);
        }
    }
}

bool SceneryConverter::MarkDirections()
{
    std::list<RoadInterface *> pendingRoads;
    std::transform(scenery->GetRoads().begin(),
                   scenery->GetRoads().end(),
                   std::back_inserter(pendingRoads),
                   [](const std::pair<std::string, RoadInterface *> &item) { return item.second; });

    while (!pendingRoads.empty()) // proceed until all roads have been marked
    {
        RoadInterface *road = pendingRoads.front();

        MarkDirectionRoad(road, true); // first road in cluster defines direction within cluster
        if (road->GetJunctionId() != "-1")
        {
            pendingRoads.pop_front();
            continue;
        }

        std::list<RoadInterface *> tmpCluster; // contains currently processed cluster
        tmpCluster.splice(tmpCluster.end(), pendingRoads, pendingRoads.begin());

        LOG(CbkLogLevel::Debug, "process road cluster");

        while (!tmpCluster.empty()) // process current cluster
        {
            road = tmpCluster.front();
            tmpCluster.pop_front();

            if (road->GetLaneSections().empty())
            {
                LOG(CbkLogLevel::Error, "no sections given for road " + road->GetId());
                return false;
            }

            // collect all road links for this road and save them in this mapping
            std::map<std::tuple<RoadInterface *, RoadInterface *>, std::tuple<RoadLinkType, ContactPointType>> collectedRoadLinks;
            for (RoadLinkInterface *roadLink : road->GetRoadLinks())
            {
                if (RoadLinkElementType::Road == roadLink->GetElementType())
                {
                    RoadInterface *connectedRoad = GetConnectedRoad(roadLink);
                    if (!connectedRoad)
                    {
                        return false;
                    }

                    if (road == connectedRoad)
                    {
                        LOG(CbkLogLevel::Error, "self references are not supported, road " + road->GetId());
                        return false;
                    }

                    // if the connection between these two roads is found for the first time,
                    // add it to the mapping
                    if (0 == collectedRoadLinks.count(std::make_tuple(road, connectedRoad)) &&
                        0 == collectedRoadLinks.count(std::make_tuple(connectedRoad, road)))
                    {
                        collectedRoadLinks.insert({std::make_tuple(road,
                                                                   connectedRoad),
                                                   std::make_tuple(roadLink->GetType(),
                                                                   roadLink->GetContactPoint())}); // connectedRoad is not part of a junction
                    }
                }
                else if (RoadLinkElementType::Junction == roadLink->GetElementType())
                {
                    //handle junctions
                }
                else
                {
                    assert(0); // catched by parser
                    return false;
                }
            }

            // process collected road links
            for (auto &item : collectedRoadLinks)
            {
                RoadInterface *connectedRoad = std::get<1>(item.first);
                RoadLinkType connectedRoadLinkType = std::get<0>(item.second);
                ContactPointType connectedContactPointType = std::get<1>(item.second);

                auto findIt = std::find_if(pendingRoads.begin(),
                                           pendingRoads.end(),
                                           [&connectedRoad](RoadInterface *road) { return road == connectedRoad; });
                if (pendingRoads.end() == findIt)
                {
                    continue; // road already processed (no not overwrite content)
                }

                bool connectedRoadInDirection = false;

                if (RoadLinkType::Predecessor != connectedRoadLinkType &&
                    RoadLinkType::Successor != connectedRoadLinkType) // catch neighbor road link type
                {
                    LOG(CbkLogLevel::Error, "only predecessor and successor road links are supported");
                    return false;
                }

                assert(ContactPointType::Start == connectedContactPointType ||
                       ContactPointType::End == connectedContactPointType); // catched by importer

                // if the connected road is in direction, depends whether the
                // current road is in direction and how they are connected
                bool roadInDirection = road->GetInDirection();
                if (RoadLinkType::Predecessor == connectedRoadLinkType)
                {
                    if (ContactPointType::Start == connectedContactPointType)
                    {
                        connectedRoadInDirection = !roadInDirection;
                    }
                    else if (ContactPointType::End == connectedContactPointType)
                    {
                        connectedRoadInDirection = roadInDirection;
                    }
                }
                else if (RoadLinkType::Successor == connectedRoadLinkType)
                {
                    if (ContactPointType::Start == connectedContactPointType)
                    {
                        connectedRoadInDirection = roadInDirection;
                    }
                    else if (ContactPointType::End == connectedContactPointType)
                    {
                        connectedRoadInDirection = !roadInDirection;
                    }
                }

                MarkDirectionRoad(connectedRoad,
                                  connectedRoadInDirection);

                // update cluster
                auto findIter = std::find(pendingRoads.begin(), pendingRoads.end(), connectedRoad);
                if (pendingRoads.end() != findIter)
                {
                    pendingRoads.remove(connectedRoad);
                    tmpCluster.push_back(connectedRoad);
                }
            } // for each road link
        }     // process next road within current cluster
    }         // for each cluster

    return true;
}

bool SceneryConverter::IndexElements()
{
    int linearSectionId = 0;
    for (auto &item : scenery->GetRoads())
    {
        RoadInterface *road = item.second;

        for (RoadLaneSectionInterface *roadLaneSection : road->GetLaneSections())
        {
            roadLaneSection->SetId(linearSectionId);
            ++linearSectionId;
        }
    }

    return true;
}

bool SceneryConverter::ConnectLaneToLane(RoadLaneInterface *currentLane,
                                         ContactPointType currentContactPoint,
                                         RoadLaneInterface *otherLane)
{
    // calculate direction parameters
    bool currentDestBegin = ContactPointType::Start == currentContactPoint;

    if (!currentDestBegin)
    {
        worldData.AddLaneSuccessor(*currentLane, *otherLane);
    }
    else
    {
        worldData.AddLanePredecessor(*currentLane, *otherLane);
    }

    return true;
}

bool SceneryConverter::ConnectLaneToSection(RoadLaneInterface *currentLane,
                                            ContactPointType currentContactPoint,
                                            RoadLaneSectionInterface *otherLaneSection)
{
    if (currentLane->GetId() == 0)
    {
        return true;
    } // no center lanes

    if (currentContactPoint == ContactPointType::Start)
    {
        // process predecessor
        if (currentLane->GetPredecessor().empty())
        {
            return true;
        }
        RoadLaneInterface *otherLane = GetOtherLane(otherLaneSection, currentLane->GetPredecessor().front());
        if (otherLane && !ConnectLaneToLane(currentLane,
                                            currentContactPoint,
                                            otherLane))
        {
            LOG(CbkLogLevel::Error, "could not connect lanes");
            return false;
        }
    }
    else
    {
        // process successor
        if (currentLane->GetSuccessor().empty())
        {
            return true;
        }
        RoadLaneInterface *otherLane = GetOtherLane(otherLaneSection, currentLane->GetSuccessor().front());
        if (otherLane && !ConnectLaneToLane(currentLane,
                                            currentContactPoint,
                                            otherLane))
        {
            LOG(CbkLogLevel::Error, "could not connect lanes");
            return false;
        }
    }

    return true;
}

bool SceneryConverter::ConnectLanes(RoadLaneSectionInterface *firstLaneSection,
                                    ContactPointType firstContactPoint,
                                    RoadLaneSectionInterface *secondLaneSection,
                                    ContactPointType secondContactPoint)
{
    for (auto &item : firstLaneSection->GetLanes())
    {
        if (!ConnectLaneToSection(item.second,
                                  firstContactPoint,
                                  secondLaneSection))
        {
            return false;
        }
    }

    for (auto &item : secondLaneSection->GetLanes())
    {
        if (!ConnectLaneToSection(item.second,
                                  secondContactPoint,
                                  firstLaneSection))
        {
            return false;
        }
    }

    return true;
}

bool SceneryConverter::ConnectRoadExternalWithElementTypeRoad(RoadInterface *road)
{
    for (RoadLinkInterface *roadLink : road->GetRoadLinks())
    {
        if (roadLink->GetElementType() != RoadLinkElementType::Road)
        {
            continue;
        }

        auto otherRoad = scenery->GetRoad(roadLink->GetElementId());
        RoadLaneSectionInterface *otherSection;

        if (roadLink->GetContactPoint() == ContactPointType::Start)
        {
            otherSection = otherRoad->GetLaneSections().front();
        }
        else
        {
            otherSection = otherRoad->GetLaneSections().back();
        }

        if (roadLink->GetType() == RoadLinkType::Successor)
        {
            if (!ConnectExternalRoadSuccessor(road, otherRoad, otherSection))
            {
                return false;
            }
        }
        else
        {
            if (!ConnectExternalRoadPredecessor(road, otherRoad, otherSection))
            {
                return false;
            }
        }
    }

    return true;
}

bool SceneryConverter::ConnectExternalRoadSuccessor(const RoadInterface *currentRoad, const RoadInterface *otherRoad,
                                                    RoadLaneSectionInterface *otherSection)
{
    worldData.SetRoadSuccessor(*currentRoad, *otherRoad);
    RoadLaneSectionInterface *currentSection = currentRoad->GetLaneSections().back();
    worldData.SetSectionSuccessor(*currentSection, *otherSection);
    for (auto &laneEntry : currentSection->GetLanes())
    {
        RoadLaneInterface *lane = laneEntry.second;
        std::list<int> successorLaneId = lane->GetSuccessor();
        if (successorLaneId.size() == 1)
        {
            try
            {
                RoadLaneInterface *successorLane = otherSection->GetLanes().at(successorLaneId.front());
                worldData.AddLaneSuccessor(*lane, *successorLane);
            }
            catch (const std::out_of_range &)
            {
                LOG(CbkLogLevel::Error, "LaneSuccessorId not found");
                return false;
            }
        }
    }
    return true;
}

bool SceneryConverter::ConnectExternalRoadPredecessor(const RoadInterface *currentRoad, const RoadInterface *otherRoad,
                                                      RoadLaneSectionInterface *otherSection)
{
    worldData.SetRoadPredecessor(*currentRoad, *otherRoad);
    RoadLaneSectionInterface *currentSection = currentRoad->GetLaneSections().front();
    worldData.SetSectionPredecessor(*currentSection, *otherSection);
    for (auto &laneEntry : currentSection->GetLanes())
    {
        RoadLaneInterface *lane = laneEntry.second;
        std::list<int> predecessorLaneId = lane->GetPredecessor();
        if (predecessorLaneId.size() == 1)
        {
            try
            {
                RoadLaneInterface *predecessorLane = otherSection->GetLanes().at(predecessorLaneId.front());
                worldData.AddLanePredecessor(*lane, *predecessorLane);
            }
            catch (const std::out_of_range &)
            {
                LOG(CbkLogLevel::Error, "LanePredecessorId not found");
                return false;
            }
        }
    }
    return true;
}

bool SceneryConverter::ConnectRoadInternal(RoadInterface *road)
{
    // connect sections within a road
    auto it = road->GetLaneSections().begin();
    RoadLaneSectionInterface *previousLaneSection = *it;
    ++it;
    while (it !=
           road->GetLaneSections().end()) // skipped for junctions since openDrive connecting roads contain only one lane section
    {
        RoadLaneSectionInterface *laneSection = *it;

        worldData.SetSectionSuccessor(*previousLaneSection, *laneSection);
        worldData.SetSectionPredecessor(*laneSection, *previousLaneSection);

        if (!ConnectLanes(previousLaneSection,
                          ContactPointType::End,
                          laneSection,
                          ContactPointType::Start))
        {
            LOG(CbkLogLevel::Error, "could not connect sections");
            return false;
        }

        previousLaneSection = laneSection;
        ++it;
    }

    return true;
}

bool SceneryConverter::ConnectJunction(const JunctionInterface *junction)
{
    worldData.AddJunction(junction);
    // this indirection to an internal function is a first step towards better testability. please do not remove.
    if (auto [status, error_message] = Internal::ConnectJunction(scenery, junction,
                                                                 [&](const JunctionInterface *junction, const RoadInterface *incomingRoad, const RoadInterface *connectingRoad, const RoadInterface *outgoingRoad,
                                                                     ContactPointType incomingContactPoint, ContactPointType connectingContactPoint, ContactPointType outgoingContactPoint,
                                                                     std::map<int, int> laneIdMapping) {
                                                                     ConnectPathInJunction(junction, incomingRoad, connectingRoad, outgoingRoad, incomingContactPoint,
                                                                                           connectingContactPoint, outgoingContactPoint, laneIdMapping);
                                                                 });
        status)
    {
        for (const auto &priority : junction->GetPriorities())
        {
            worldData.AddJunctionPriority(junction, priority.high, priority.low);
        }
        return true;
    }
    else
    {
        LOG(CbkLogLevel::Error, error_message);
        return false;
    }
}

std::optional<int> GetOutgoingLaneId (const RoadLaneInterface& connectingLane, bool connectAtStart)
{
    if (connectAtStart ? connectingLane.GetSuccessor().empty() : connectingLane.GetPredecessor().empty())
    {
        return std::nullopt;
    }
    else
    {
        return connectAtStart ? connectingLane.GetSuccessor().front() : connectingLane.GetPredecessor().front();
    }
}

void SceneryConverter::ConnectPathInJunction(const JunctionInterface *junction, const RoadInterface *incomingRoad, const RoadInterface *connectingRoad,
                                             const RoadInterface *outgoingRoad, ContactPointType incomingContactPoint, ContactPointType connectingContactPoint, ContactPointType outgoingContactPoint,
                                             std::map<int, int> laneIdMapping)
{
    bool connectAtStart = (connectingContactPoint == ContactPointType::Start);
    if (incomingContactPoint == ContactPointType::Start)
    {
        worldData.SetRoadPredecessorJunction(*incomingRoad, junction);
    }
    else
    {
        worldData.SetRoadSuccessorJunction(*incomingRoad, junction);
    }

    if (outgoingContactPoint == ContactPointType::Start)
    {
        worldData.SetRoadPredecessorJunction(*outgoingRoad, junction);
    }
    else
    {
        worldData.SetRoadSuccessorJunction(*outgoingRoad, junction);
    }

    RoadLaneSectionInterface *incomingRoadSection;
    if (incomingContactPoint == ContactPointType::Start)
    {
        incomingRoadSection = incomingRoad->GetLaneSections().front();
    }
    else
    {
        incomingRoadSection = incomingRoad->GetLaneSections().back();
    }

    RoadLaneSectionInterface *connectingRoadFirstSection = connectAtStart ? connectingRoad->GetLaneSections().front() : connectingRoad->GetLaneSections().back();
    RoadLaneSectionInterface *connectingRoadLastSection = connectAtStart ? connectingRoad->GetLaneSections().back() : connectingRoad->GetLaneSections().front();
    RoadLaneSectionInterface *outgoingRoadSection;
    if (outgoingContactPoint == ContactPointType::Start)
    {
        outgoingRoadSection = outgoingRoad->GetLaneSections().front();
    }
    else
    {
        outgoingRoadSection = outgoingRoad->GetLaneSections().back();
    }

    for (auto lanePair : laneIdMapping)
    {
        RoadLaneInterface *incomingLane = incomingRoadSection->GetLanes().at(lanePair.first);
        RoadLaneInterface *connectingLane = connectingRoadFirstSection->GetLanes().at(lanePair.second);
        ConnectLaneToLane(incomingLane, incomingContactPoint, connectingLane);
    }

    for (auto [connectingLaneId, connectingLane] : connectingRoadLastSection->GetLanes())
    {
        auto outgoingLaneId = GetOutgoingLaneId(*connectingLane, connectAtStart);
        if (!outgoingLaneId.has_value() || connectingLaneId == 0)
        {
            continue;
        }
        RoadLaneInterface *outgoingLane = outgoingRoadSection->GetLanes().at(outgoingLaneId.value());
        ConnectLaneToLane(outgoingLane, outgoingContactPoint, connectingLane);
    }
}

bool SceneryConverter::ConnectRoads()
{
    for (auto &item : scenery->GetRoads())
    {
        RoadInterface *road = item.second;

        if (!ConnectRoadExternalWithElementTypeRoad(road))
        {
            LOG(CbkLogLevel::Error, "could not connect external road " + item.second->GetId());
            return false;
        }

        if (!ConnectRoadInternal(road))
        {
            LOG(CbkLogLevel::Error, "could not connect internal road " + item.second->GetId());
            return false;
        }
    }

    for (auto &item : scenery->GetJunctions())
    {
        if (!ConnectJunction(item.second))
        {
            return false;
        }
    }
    return true;
}

bool SceneryConverter::ConvertRoads()
{
    // define a unique directions of roads/lanes within each road cluster
    if (!MarkDirections())
    {
        return false;
    }

    CreateRoads();

    // connect roads in data layer
    if (!ConnectRoads())
    {
        return false;
    }

    GeometryConverter::Convert(*scenery, worldData);
    return true;
}

void SceneryConverter::ConvertObjects()
{
    CreateObjects();
    CreateRoadSignals();
}

void SceneryConverter::CreateObjects()
{
    for (auto &item : scenery->GetRoads())
    {
        RoadInterface *road = item.second;

        std::vector<RoadLaneSectionInterface *> roadLaneSections = road->GetLaneSections();

        for (RoadObjectInterface *object : road->GetRoadObjects())
        {
            auto section = worldDataQuery.GetSectionByDistance(road->GetId(), object->GetS());

            if (section == nullptr) // potential error in OpenDRIVE file
            {
                LOG(CbkLogLevel::Warning, "Object ignored: s-coordinate not within road");
                continue;
            }

            const auto position = RoadCoord2WorldCoord(road, object->GetS(), object->GetT(), object->GetHdg());

            if (object->GetType() == RoadObjectType::crosswalk)
            {
                CreateRoadMarking(object, position, section->GetLanes());
            }
            else if(object->IsContinuous())
            {
                CreateContinuousObject(object, road);
            }
            else
            {
                CreateObject(object, position);
            }
        }
    }
}

void SceneryConverter::CreateObject(const RoadObjectInterface *object, const Position& position)
{
    OWL::Primitive::AbsPosition absPos{position.xPos, position.yPos, 0};
    OWL::Primitive::Dimension dim{object->GetLength(), object->GetWidth(), object->GetHeight()};
    OWL::Primitive::AbsOrientation orientation{position.yawAngle, object->GetPitch(), object->GetRoll()};
    const auto id = repository.Register(openpass::entity::EntityType::StationaryObject, openpass::utils::GetEntityInfo(*object));

    trafficObjects.emplace_back(std::make_unique<TrafficObjectAdapter>(
                                    id,
                                    worldData,
                                    localizer,
                                    absPos,
                                    dim,
                                    orientation,
                                    object->GetId()));
}

void SceneryConverter::CreateContinuousObject(const RoadObjectInterface *object, const RoadInterface *road)
{
    auto section = worldDataQuery.GetSectionByDistance(road->GetId(), object->GetS());
    const double sStart = object->GetS();
    const double sEnd = sStart + object->GetLength();
    const auto laneElements = section->GetLanes().front()->GetLaneGeometryElements();
    for (const auto& laneElement : laneElements)
    {
        const double elementStart = laneElement->joints.current.sOffset;
        const double elementEnd = laneElement->joints.next.sOffset;
        const double objectStart = std::max(sStart, elementStart);
        const double objectEnd = std::min(sEnd, elementEnd);
        if (objectStart < objectEnd)
        {
            const auto startPosition = RoadCoord2WorldCoord(road, objectStart, object->GetT(), 0);
            const auto endPosition = RoadCoord2WorldCoord(road, objectEnd, object->GetT(), 0);
            const double deltaX = endPosition.xPos - startPosition.xPos;
            const double deltaY = endPosition.yPos - startPosition.yPos;
            const double length = std::hypot(deltaX, deltaY);
            double yaw = std::atan2(deltaY, deltaX);

            OWL::Primitive::AbsPosition pos{(startPosition.xPos + endPosition.xPos) / 2.0, (startPosition.yPos + endPosition.yPos) / 2.0, 0};
            OWL::Primitive::Dimension dim{length, object->GetWidth(), object->GetHeight()};
            OWL::Primitive::AbsOrientation orientation{yaw, object->GetPitch(), object->GetRoll()};
            const auto id = repository.Register(openpass::entity::EntityType::StationaryObject, openpass::utils::GetEntityInfo(*object));

            trafficObjects.emplace_back(std::make_unique<TrafficObjectAdapter>(
                                            id,
                                            worldData,
                                            localizer,
                                            pos,
                                            dim,
                                            orientation,
                                            object->GetId()));
        }
    }
}

std::vector<OWL::Id> SceneryConverter::CreateLaneBoundaries(RoadLaneInterface &odLane, RoadLaneSectionInterface &odSection)
{
    std::vector<OWL::Id> laneBoundaries;
    for (const auto &roadMarkEntry : odLane.GetRoadMarks())
    {
        const auto roadMarkType = roadMarkEntry->GetType();

        if (roadMarkType == RoadLaneRoadMarkType::Solid_Solid ||
            roadMarkType == RoadLaneRoadMarkType::Solid_Broken ||
            roadMarkType == RoadLaneRoadMarkType::Broken_Solid ||
            roadMarkType == RoadLaneRoadMarkType::Broken_Broken)
        {
            const auto id_left = repository.Register(openpass::utils::GetEntityInfo(*roadMarkEntry));
            const auto id_right = repository.Register(openpass::utils::GetEntityInfo(*roadMarkEntry));

            worldData.AddLaneBoundary(id_left, *roadMarkEntry, odSection.GetStart(), OWL::LaneMarkingSide::Left);
            worldData.AddLaneBoundary(id_right, *roadMarkEntry, odSection.GetStart(), OWL::LaneMarkingSide::Right);

            laneBoundaries.push_back(id_left.value);
            laneBoundaries.push_back(id_right.value);
        }
        else
        {
            const auto id_single = repository.Register(openpass::utils::GetEntityInfo(*roadMarkEntry));
            worldData.AddLaneBoundary(id_single, *roadMarkEntry, odSection.GetStart(), OWL::LaneMarkingSide::Single);
            laneBoundaries.push_back(id_single.value);
        }
    }

    if (odLane.GetRoadMarks().empty())
    {
        //!If there are no RoadMarks defined in OpenDRIVE, there should still be a LaneBoundary, so we treat it as if there was a RoadMark of type none
        const RoadLaneRoadMark defaultRoadMark(0,
                                               RoadLaneRoadDescriptionType::Center,
                                               RoadLaneRoadMarkType::None,
                                               RoadLaneRoadMarkColor::Undefined,
                                               RoadLaneRoadMarkLaneChange::Both,
                                               RoadLaneRoadMarkWeight::Undefined);
        const auto id = repository.Register(openpass::utils::GetEntityInfo(defaultRoadMark));
        worldData.AddLaneBoundary(id, defaultRoadMark, odSection.GetStart(), OWL::LaneMarkingSide::Single);
        laneBoundaries.push_back(id.value);
    }

    return laneBoundaries;
}

void SceneryConverter::CreateRoadSignals()
{
    for (auto &item : scenery->GetRoads())
    {
        RoadInterface *road = item.second;

        std::vector<std::pair<RoadSignalInterface *, std::list<std::string>>> dependentSignals;

        for (RoadSignalInterface *signal : road->GetRoadSignals())
        {
            if (signal->GetIsDynamic())
            {
                LOG(CbkLogLevel::Warning, std::string("Ignoring dynamic signal: ") + signal->GetId());
                continue;
            }

            // Gather all dependent signals
            if (!signal->GetDependencies().empty())
            {
                dependentSignals.emplace_back(signal, signal->GetDependencies());
                continue;
            }

            auto section = worldDataQuery.GetSectionByDistance(road->GetId(), signal->GetS());

            if (section == nullptr) // potential error in OpenDRIVE file
            {
                LOG(CbkLogLevel::Warning, "Object ignored: s-coordinate not within road");
                continue;
            }

            double yaw = signal->GetHOffset() + (signal->GetOrientation() ? 0 : M_PI);
            auto position = RoadCoord2WorldCoord(road, signal->GetS(), signal->GetT(), yaw);
            if (OpenDriveTypeMapper::roadMarkings.find(signal->GetType()) != OpenDriveTypeMapper::roadMarkings.end())
            {
                CreateRoadMarking(signal, position, section->GetLanes());
            }

            else
            {
                CreateTrafficSign(signal, position, section->GetLanes());
            }
        }

        // First instantiate all signals and then add dependencies accordingly afterwards
        for (const auto &[supplementarySign, parentIds] : dependentSignals)
        {
            for (const auto &parentId : parentIds)
            {
                auto mapEntry = worldData.GetTrafficSignIdMapping().find(parentId);
                if (mapEntry == worldData.GetTrafficSignIdMapping().end())
                {
                    LOGWARN("Parent id '" + parentId + "' not found in world data's traffic sign id mapping for supplementary sign's id '" + supplementarySign->GetId() + "'");
                    continue;
                }
                auto parentSign = worldData.GetTrafficSigns().find(mapEntry->second);

                if (parentSign == worldData.GetTrafficSigns().cend())
                {
                    LOGWARN("Could not add supplementary sign \"" + supplementarySign->GetId() + "\" to sign \"" + parentId + "\"");
                    continue;
                }

                double yaw = supplementarySign->GetHOffset() + (supplementarySign->GetOrientation() ? 0 : M_PI);
                auto position = RoadCoord2WorldCoord(road, supplementarySign->GetS(), supplementarySign->GetT(), yaw);
                if (!parentSign->second->AddSupplementarySign(supplementarySign, position))
                {
                    LOGWARN("Unsupported supplementary sign type " + supplementarySign->GetType() + " (id: " + supplementarySign->GetId() + ")");
                }
            }
        }
    }
}

void SceneryConverter::CreateTrafficSign(RoadSignalInterface *signal, Position position, const OWL::Interfaces::Lanes &lanes)
{
    const auto id = repository.Register(openpass::utils::GetEntityInfo(*signal));
    OWL::Interfaces::TrafficSign &trafficSign = worldData.AddTrafficSign(id, signal->GetId());

    trafficSign.SetS(signal->GetS());

    if (!trafficSign.SetSpecification(signal, position))
    {
        const std::string message = "Unsupported traffic sign type: " + signal->GetType() + "-" + signal->GetSubType() + " (id: " + signal->GetId() + ")";
        LOG(CbkLogLevel::Warning, message);
    }

    for (auto lane : lanes)
    {
        OWL::OdId odId = worldData.GetLaneIdMapping().at(lane->GetId());
        if (signal->IsValidForLane(odId))
        {
            worldData.AssignTrafficSignToLane(lane->GetId(), trafficSign);
        }
    }
}

void SceneryConverter::CreateRoadMarking(RoadSignalInterface *signal, Position position, const OWL::Interfaces::Lanes &lanes)
{
    const auto id = repository.Register(openpass::utils::GetEntityInfo(*signal));
    OWL::Interfaces::RoadMarking &roadMarking = worldData.AddRoadMarking(id);

    roadMarking.SetS(signal->GetS());

    if (!roadMarking.SetSpecification(signal, position))
    {
        const std::string message = "Unsupported traffic sign type: " + signal->GetType() + (" (id: " + signal->GetId() + ")");
        LOG(CbkLogLevel::Warning, message);
        return;
    }

    for (auto lane : lanes)
    {
        OWL::OdId odId = worldData.GetLaneIdMapping().at(lane->GetId());
        if (signal->IsValidForLane(odId))
        {
            worldData.AssignRoadMarkingToLane(lane->GetId(), roadMarking);
        }
    }
}

void SceneryConverter::CreateRoadMarking(RoadObjectInterface* object, Position position, const OWL::Interfaces::Lanes& lanes)
{
    const auto id = repository.Register(openpass::utils::GetEntityInfo(*object));
    OWL::Interfaces::RoadMarking& roadMarking = worldData.AddRoadMarking(id);

    roadMarking.SetS(object->GetS());

    if (!roadMarking.SetSpecification(object, position))
    {
        const std::string message = "Unsupported traffic sign type: (id: " + object->GetId() + ")";
        LOG(CbkLogLevel::Warning, message);
        return;
    }

    for (auto lane : lanes)
    {
        OWL::OdId odId = worldData.GetLaneIdMapping().at(lane->GetId());
        if (object->IsValidForLane(odId))
        {
            worldData.AssignRoadMarkingToLane(lane->GetId(), roadMarking);
        }
    }
}

void SceneryConverter::CreateRoads()
{
    for (auto roadEntry : scenery->GetRoads())
    {
        RoadInterface &odRoad = *(roadEntry.second);

        worldData.AddRoad(odRoad);

        for (auto section : odRoad.GetLaneSections())
        {
            RoadLaneSectionInterface &odSection = *section;

            worldData.AddSection(odRoad, odSection);

            for (auto laneEntry : odSection.GetLanes())
            {
                RoadLaneInterface &odLane = *(laneEntry.second);
                if (odLane.GetId() == 0)
                {
                    auto laneBoundaries = CreateLaneBoundaries(odLane, odSection);
                    worldData.SetCenterLaneBoundary(odSection, laneBoundaries);
                }
            }
            for (auto laneEntry : odSection.GetLanes())
            {
                RoadLaneInterface &odLane = *(laneEntry.second);
                if (odLane.GetId() != 0)
                {
                    auto laneBoundaries = CreateLaneBoundaries(odLane, odSection);
                    const auto id = repository.Register(openpass::utils::GetEntityInfo(odLane));
                    worldData.AddLane(id, odSection, odLane, laneBoundaries);
                }
            }
        }
    }
}

std::pair<RoadGraph, RoadGraphVertexMapping> RoadNetworkBuilder::Build()
{
    RoadGraph roadGraph;
    RoadGraphVertexMapping vertices;

    for (auto &[roadId, road] : scenery.GetRoads())
    {
        bool hasLeftLanes = road->GetLaneSections().front()->GetLanes().crbegin()->first > 0;
        bool hasRightLanes = road->GetLaneSections().front()->GetLanes().cbegin()->first < 0;
        if (hasLeftLanes)
        {
            RouteElement routeElement{roadId, false};
            vertices[routeElement] = add_vertex(routeElement, roadGraph);
        }
        if (hasRightLanes)
        {
            RouteElement routeElement{roadId, true};
            vertices[routeElement] = add_vertex(routeElement, roadGraph);
        }
    }

    for (auto &[roadId, road] : scenery.GetRoads())
    {
        bool hasLeftLanes = road->GetLaneSections().front()->GetLanes().crbegin()->first > 0;
        bool hasRightLanes = road->GetLaneSections().front()->GetLanes().cbegin()->first < 0;
        for (auto &link : road->GetRoadLinks())
        {
            if (link->GetElementType() == RoadLinkElementType::Road)
            {
                if (link->GetType() == RoadLinkType::Successor)
                {
                    auto &successorId = link->GetElementId();
                    if (link->GetContactPoint() == ContactPointType::Start)
                    {
                        if (hasRightLanes)
                        {
                            add_edge(vertices[{roadId, true}], vertices[{successorId, true}], roadGraph);
                        }
                        if (hasLeftLanes)
                        {
                            add_edge(vertices[{successorId, false}], vertices[{roadId, false}], roadGraph);
                        }
                    }
                    else
                    {
                        if (hasRightLanes)
                        {
                            add_edge(vertices[{roadId, true}], vertices[{successorId, false}], roadGraph);
                        }
                        if (hasLeftLanes)
                        {
                            add_edge(vertices[{successorId, false}], vertices[{roadId, true}], roadGraph);
                        }
                    }
                }
                if (link->GetType() == RoadLinkType::Predecessor)
                {
                    auto &predecessorId = link->GetElementId();
                    if (link->GetContactPoint() == ContactPointType::Start)
                    {
                        if (hasRightLanes)
                        {
                            add_edge(vertices[{predecessorId, true}], vertices[{roadId, false}], roadGraph);
                        }
                        if (hasLeftLanes)
                        {
                            add_edge(vertices[{roadId, false}], vertices[{predecessorId, true}], roadGraph);
                        }
                    }
                    else
                    {
                        if (hasRightLanes)
                        {
                            add_edge(vertices[{predecessorId, true}], vertices[{roadId, true}], roadGraph);
                        }
                        if (hasLeftLanes)
                        {
                            add_edge(vertices[{roadId, false}], vertices[{predecessorId, false}], roadGraph);
                        }
                    }
                }
            }
        }
    }

    return {roadGraph, vertices};
}
