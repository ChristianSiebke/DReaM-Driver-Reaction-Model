/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2016, 2017 ITK Engineering GmbH
*               2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "common/opMath.h"
#include <algorithm>
#include <utility>
#include <limits>
#include <cassert>
#include <iostream>
#include <string>
#include <memory>
#include <QFile>
#include <deque>

#include "GeometryConverter.h"
#include "RamerDouglasPeucker.h"
#include "WorldToRoadCoordinateConverter.h"
#include "common/vector2d.h"
#include "WorldData.h"

void GeometryConverter::CalculateRoads(const SceneryInterface& scenery,
                                       OWL::Interfaces::WorldData& worldData)
{
    for(auto [roadId, road] : scenery.GetRoads())
    {
        std::vector<RoadLaneSectionInterface*> roadLaneSections = road->GetLaneSections();

        for (auto roadLaneSectionIt = roadLaneSections.begin();
             roadLaneSectionIt != roadLaneSections.end();
             roadLaneSectionIt++)
        {
            RoadLaneSectionInterface* roadSection = *roadLaneSectionIt;
            auto& roadLanes = roadSection->GetLanes();

            if(!roadLanes.empty())
            {
                double roadSectionStart = roadSection->GetStart();
                double roadSectionEnd = std::numeric_limits<double>::max();

                if (std::next(roadLaneSectionIt) != roadLaneSections.end()) // if not a single element in the list
                {
                    roadSectionEnd = (*std::next(roadLaneSectionIt))->GetStart();
                }

                // collect geometry sections
                CalculateSection(worldData,
                                 roadSectionStart,
                                 roadSectionEnd,
                                 road,
                                 roadSection);
            } // if lanes are not empty
        }
    }
}

void GeometryConverter::CalculateSection(OWL::Interfaces::WorldData& worldData,
                                         double roadSectionStart,
                                         double roadSectionEnd,
                                         const RoadInterface* road,
                                         const RoadLaneSectionInterface *roadSection)
 {
     double roadMarkStart = 0;
     SampledGeometry sampledGeometries;
     bool firstRoadMark{true};

     while (roadMarkStart + roadSectionStart < roadSectionEnd)
     {
         double roadMarkEnd = std::numeric_limits<double>::max();
         for (const auto& roadLane : roadSection->GetLanes())
         {
             for (const auto& roadMark : roadLane.second->GetRoadMarks())
             {
                 double sOffset = roadMark->GetSOffset();
                 if (sOffset > roadMarkStart && sOffset < roadMarkEnd)
                 {
                     roadMarkEnd = sOffset;
                 }
             }
         }

         auto sampledGeometry = CalculateSectionBetweenRoadMarkChanges(roadSectionStart + roadMarkStart,
                                                                       std::min(roadSectionStart + roadMarkEnd, roadSectionEnd),
                                                                       road,
                                                                       roadSection);
         if (firstRoadMark)
         {
             sampledGeometries = sampledGeometry;
             firstRoadMark = false;
         }
         else
         {
             sampledGeometries.Combine(sampledGeometry);
         }

         roadMarkStart = roadMarkEnd;
     }

     JointsBuilder jointsBuilder{sampledGeometries};

     jointsBuilder.CalculatePoints()
                  .CalculateHeadings()
                  .CalculateCurvatures();

     AddPointsToWorld(worldData, jointsBuilder.GetJoints());
 }

SampledGeometry GeometryConverter::CalculateSectionBetweenRoadMarkChanges(double roadSectionStart,
                                                                                    double roadSectionEnd,
                                                                                    const RoadInterface* road,
                                                                                    const RoadLaneSectionInterface* roadSection)
{
    SampledGeometry sampledGeometries;
    std::list<RoadGeometryInterface*> roadGeometries = road->GetGeometries();
    bool firstGeometry{true};

    for(auto roadGeometry = roadGeometries.cbegin(); roadGeometry != roadGeometries.cend(); ++roadGeometry)
    {
        auto next = std::next(roadGeometry);
        //To prevent rounding issues, the length is calculated as difference between the start s of the geometries
        double roadGeometryLength = (next == roadGeometries.end()) ? (*roadGeometry)->GetLength()
                                                                : (*next)->GetS() - (*roadGeometry)->GetS();
        auto sampledGeometry = CalculateGeometry(roadSectionStart,
                                                 roadSectionEnd,
                                                 road,
                                                 roadSection,
                                                 *roadGeometry,
                                                 roadGeometryLength);
        if (sampledGeometry.borderPoints.empty())
        {
            continue;
        }
        if (firstGeometry)
        {
            sampledGeometries = sampledGeometry;
            firstGeometry = false;
        }
        else
        {
            sampledGeometries.Combine(sampledGeometry);
        }
    }
    return sampledGeometries;
}

SampledGeometry GeometryConverter::CalculateGeometry(double roadSectionStart,
                                                     double roadSectionEnd,
                                                     const RoadInterface* road,
                                                     const RoadLaneSectionInterface *roadSection,
                                                     const RoadGeometryInterface* roadGeometry,
                                                     double roadGeometryLength)
{
    auto roadGeometryStart = roadGeometry->GetS();
    auto roadGeometryEnd = roadGeometryStart + roadGeometryLength;

    // if section is not affected by geometry
    if(roadSectionStart > roadGeometryEnd ||
            roadSectionEnd < roadGeometryStart)
    {
        return {};
    }

    double geometryOffsetStart = CalculateGeometryOffsetStart(roadSectionStart,
                                                              roadGeometryStart);

    double geometryOffsetEnd = CalculateGeometryOffsetEnd(roadSectionEnd,
                                                          roadGeometryStart,
                                                          roadGeometryEnd,
                                                          roadGeometryLength);
    auto borderPoints = CalculateJoints(geometryOffsetStart,
                                        geometryOffsetEnd,
                                        roadSection,
                                        road,
                                        roadGeometry,
                                        roadGeometryStart,
                                        roadSectionStart);

    borderPoints = RamerDouglasPeucker::Simplify<BorderPoints>(borderPoints);

    return {borderPoints, roadGeometry->GetDir(geometryOffsetStart), roadGeometry->GetDir(geometryOffsetEnd)};
}

double GeometryConverter::CalculateGeometryOffsetStart(double roadSectionStart,
                                                     double roadGeometryStart)
{
    // geometry begins within section
    if(roadSectionStart <= roadGeometryStart)
    {
        return 0.0;
    }
    // geometry begins before section
    else
    {
        return roadSectionStart - roadGeometryStart;
    }
}

double GeometryConverter::CalculateGeometryOffsetEnd(double roadSectionEnd,
                                                   double roadGeometryStart,
                                                   double roadGeometryEnd,
                                                   double roadGeometryLength)
{
    // geometry ends within section
    if(roadSectionEnd >= roadGeometryEnd)
    {
        return roadGeometryLength;
    }
    // geometry ends after section
    else
    {
        return roadSectionEnd - roadGeometryStart;
    }
}

std::vector<BorderPoints> GeometryConverter::CalculateJoints(double geometryOffsetStart,
                                                             double geometryOffsetEnd,
                                                             const RoadLaneSectionInterface *roadSection,
                                                             const RoadInterface *road,
                                                             const RoadGeometryInterface *roadGeometry,
                                                             double roadGeometryStart,
                                                             double roadSectionStart)
{
    std::vector<BorderPoints> borderPoints;
    double geometryOffset = geometryOffsetStart;
    while (geometryOffset < geometryOffsetEnd + SAMPLING_RATE)
    {
        // account for last sample
        if(geometryOffset > geometryOffsetEnd)
        {
            geometryOffset = geometryOffsetEnd;
        }

        borderPoints.push_back(CalculateBorderPoints(roadSection,
                                                  road,
                                                  roadGeometry,
                                                  geometryOffset,
                                                  roadGeometryStart + geometryOffset,
                                                  roadSectionStart));

        geometryOffset += SAMPLING_RATE;
    }
    return borderPoints;
}

BorderPoints GeometryConverter::CalculateBorderPoints(const RoadLaneSectionInterface* roadSection,
                                                      const RoadInterface* road,
                                                      const RoadGeometryInterface* roadGeometry,
                                                      double geometryOffset,
                                                      double sCoordinate,
                                                      double roadSectionStart)
{

    const auto& roadLanes = roadSection->GetLanes();
    const auto maxLaneId = roadLanes.crbegin()->first;
    const auto minLaneId = roadLanes.cbegin()->first;

    const double sectionOffset = std::max(sCoordinate - roadSectionStart, 0.0); //Take only positive value to prevent rounding imprecision
    const double laneOffset = CalculateLaneOffset(road, sCoordinate);
    const auto centerPoint = roadGeometry->GetCoord(geometryOffset, laneOffset);
    const double heading = roadGeometry->GetDir(geometryOffset);
    const double sin_hdg = std::sin(heading);
    const double cos_hdg = std::cos(heading);

    std::deque<BorderPoint> points;

    // start with positive lanes
    // 1, 2, 3... -> order necessary to accumulate right starting from center
    double borderOffset = 0.0;
    for (int laneId = 1; laneId <= maxLaneId; laneId++)
    {
        const auto laneIter = roadLanes.find(laneId);
        if (laneIter == roadLanes.end())
        {
            throw std::runtime_error("Missing lane with id "+ std::to_string(laneId));
        }

        auto lane = laneIter->second;
        if (!lane->GetWidths().empty())
        {
            borderOffset += CalculateLaneWidth(lane, sectionOffset);
        }
        else if (!lane->GetBorders().empty())
        {
            borderOffset = CalculateLaneBorder(lane, sectionOffset);
        }
        else
        {
            throw std::runtime_error("RoadLane requires either Width or Border definition.");
        }

        const double x = centerPoint.x - borderOffset * sin_hdg;
        const double y = centerPoint.y + borderOffset * cos_hdg;
        points.emplace_front(Common::Vector2d{x,y}, roadLanes.at(laneId));
    }

    points.emplace_back(centerPoint, roadLanes.at(0));

    borderOffset = 0.0;
    for (int laneId = -1; laneId >= minLaneId; laneId--)
    {
        const auto laneIter = roadLanes.find(laneId);
        if (laneIter == roadLanes.end())
        {
            throw std::runtime_error("Missing lane with id "+ std::to_string(laneId));
        }

        auto lane = laneIter->second;
        if (!lane->GetWidths().empty())
        {
            borderOffset += CalculateLaneWidth(lane, sectionOffset);
        }
        else if (!lane->GetBorders().empty())
        {
            borderOffset = CalculateLaneBorder(lane, sectionOffset);
        }
        else
        {
            throw std::runtime_error("RoadLane requires either Width or Border definition.");
        }

        const double x = centerPoint.x + borderOffset * sin_hdg;
        const double y = centerPoint.y - borderOffset * cos_hdg;
        points.emplace_back(Common::Vector2d{x,y}, roadLanes.at(laneId));
    }

    return BorderPoints{sCoordinate, {points.begin(), points.end()}};
}

void GeometryConverter::AddPointsToWorld(OWL::Interfaces::WorldData& worldData, const Joints& joints)
{
    for (const auto& joint : joints)
    {
        for (const auto& [laneId, laneJoint] : joint.laneJoints)
        {
            if (laneId == 0)
            {
                worldData.AddCenterLinePoint(*laneJoint.lane->GetLaneSection(), laneJoint.center, joint.s, laneJoint.heading);
            }
            else
            {
                worldData.AddLaneGeometryPoint(*laneJoint.lane, laneJoint.left, laneJoint.center, laneJoint.right, joint.s, laneJoint.curvature, laneJoint.heading);
            }
        }
    }
}

double GeometryConverter::CalculateCoordZ(RoadInterface *road, double offset)
{
    double coordZ = 0.0;
    const RoadElevation* roadElevation = GetRelevantRoadElevation(offset, road);

    if(roadElevation)
    {
        double ds = offset - roadElevation->GetS();

        coordZ = roadElevation->GetA() +
                 roadElevation->GetB() * ds +
                 roadElevation->GetC() * ds * ds +
                 roadElevation->GetD() * ds * ds * ds;
    }

    return coordZ;
}

double GeometryConverter::CalculateSlope(RoadInterface *road, double offset)
{
    double slope = 0.0;
    const RoadElevation* roadElevation = GetRelevantRoadElevation(offset, road);

    if(roadElevation)
    {
        slope = CalculateSlopeAtRoadPosition(roadElevation, offset);
    }

    return slope;
}

const RoadElevation* GeometryConverter::GetRelevantRoadElevation(double roadOffset, RoadInterface* road)
{
    auto roadLaneIt = road->GetElevations().begin();
    while(road->GetElevations().end() != roadLaneIt)
    {
        if((*roadLaneIt)->GetS() <= roadOffset)
        {
            auto roadLaneNextIt = std::next(roadLaneIt);
            if(road->GetElevations().end() == roadLaneNextIt ||
                    (*roadLaneNextIt)->GetS() > roadOffset)
            {
                break;
            }
        }

        ++roadLaneIt;
    }

    if(roadLaneIt == road->GetElevations().end())
    {
        return nullptr;
    }
    else
    {
      return *roadLaneIt;
    }
}

const RoadLaneWidth* GeometryConverter::GetRelevantRoadLaneWidth(double sectionOffset, const std::list<RoadLaneWidth*> widthsOrBorders)
{
    auto roadLaneIt = widthsOrBorders.begin();
    while(widthsOrBorders.end() != roadLaneIt)
    {
        if((*roadLaneIt)->GetSOffset() <= sectionOffset)
        {
            auto roadLaneNextIt = std::next(roadLaneIt);
            if(widthsOrBorders.end() == roadLaneNextIt ||
                    (*roadLaneNextIt)->GetSOffset() > sectionOffset)
            {
                break;
            }
        }

        ++roadLaneIt;
    }

    if(roadLaneIt == widthsOrBorders.end())
    {
        return nullptr;
    }
    else
    {
      return *roadLaneIt;
    }
}

const RoadLaneOffset* GeometryConverter::GetRelevantRoadLaneOffset(double roadOffset, const RoadInterface* road)
{
    auto laneOffsetIt = road->GetLaneOffsets().begin();
    while(laneOffsetIt != road->GetLaneOffsets().end())
    {
        auto laneOffsetNextIt = std::next(laneOffsetIt);

        if(road->GetLaneOffsets().end() == laneOffsetNextIt ||
                (*laneOffsetNextIt)->GetS() > roadOffset)
        {
        }

        ++laneOffsetIt;
    }

    auto roadLaneIt = road->GetLaneOffsets().begin();
    while(road->GetLaneOffsets().end() != roadLaneIt)
    {
        if((*roadLaneIt)->GetS() <= roadOffset)
        {
            auto roadLaneNextIt = std::next(roadLaneIt);
            if(road->GetLaneOffsets().end() == roadLaneNextIt ||
                    (*roadLaneNextIt)->GetS() > roadOffset)
            {
                break;
            }
        }

        ++roadLaneIt;
    }

    if(roadLaneIt == road->GetLaneOffsets().end())
    {
        return nullptr;
    }
    else
    {
      return *roadLaneIt;
    }
}

const RoadLaneRoadMark* GeometryConverter::GetRelevantRoadLaneRoadMark(double sectionOffset, const RoadLaneInterface* roadLane)
{
    auto roadMarkIt = roadLane->GetRoadMarks().begin();

    while(roadMarkIt != roadLane->GetRoadMarks().end())
    {
        if((*roadMarkIt)->GetSOffset() <= sectionOffset)
        {
            auto roadMarkNextIt = std::next(roadMarkIt);
            if(roadMarkNextIt == roadLane->GetRoadMarks().end() ||
               (*roadMarkNextIt)->GetSOffset() > sectionOffset)
            {
                break;
            }
        }

        ++roadMarkIt;
    }

    if(roadMarkIt == roadLane->GetRoadMarks().end())
    {
        return nullptr;
    }
    else
    {
      return *roadMarkIt;
    }
}

double GeometryConverter::CalculateLaneWidth(const RoadLaneInterface *roadLane, double sectionOffset)
{
    const RoadLaneWidth* roadLaneWidth = GetRelevantRoadLaneWidth(sectionOffset, roadLane->GetWidths());

    if(!roadLaneWidth)
    {
        throw std::runtime_error("No lane width given");
    }

    return CalculateWidthAtSectionPosition(roadLaneWidth, sectionOffset);
}

double GeometryConverter::CalculateLaneBorder(const RoadLaneInterface *roadLane, double sectionOffset)
{
    const RoadLaneWidth* roadLaneBorder = GetRelevantRoadLaneWidth(sectionOffset, roadLane->GetBorders());

    if(!roadLaneBorder)
    {
        throw std::runtime_error("No lane border given");
    }

    return CalculateWidthAtSectionPosition(roadLaneBorder, sectionOffset);
}

double GeometryConverter::CalculateLaneOffset(const RoadInterface* road, double roadPosition)
{
    const RoadLaneOffset* roadLaneOffset = GetRelevantRoadLaneOffset(roadPosition, road);

    if (!roadLaneOffset)
    {
        return 0.0;
    }

    return CalculateOffsetAtRoadPosition(roadLaneOffset, roadPosition);
}

double GeometryConverter::CalculateWidthAtSectionPosition(const RoadLaneWidth* width, double position)
{
    double ds = position - width->GetSOffset();

    return width->GetA() +
           width->GetB() * ds +
           width->GetC() * ds * ds +
           width->GetD() * ds * ds * ds;
}

double GeometryConverter::CalculateSlopeAtRoadPosition(const RoadElevation* roadElevation, double position)
{
    double ds = position - roadElevation->GetS();

    double deltaZ =     roadElevation->GetB() +
                    2 * roadElevation->GetC() * ds +
                    3 * roadElevation->GetD() * ds * ds;

    return std::atan(deltaZ);
}

double GeometryConverter::CalculateOffsetAtRoadPosition(const RoadLaneOffset* roadOffset, double position)
{
    double ds = position - roadOffset->GetS();

    double deltaT = roadOffset->GetA() +
                    roadOffset->GetB() * ds +
                    roadOffset->GetC() * ds * ds +
                    roadOffset->GetD() * ds * ds *ds;

    return deltaT;
}

void GeometryConverter::Convert(const SceneryInterface& scenery, OWL::Interfaces::WorldData& worldData)
{
    CalculateRoads(scenery, worldData);
    CalculateIntersections(worldData);
}

bool GeometryConverter::IsEqual(const double valueA, const double valueB)
{
    return std::abs(valueA - valueB) < EPS;
}

void GeometryConverter::CalculateIntersections(OWL::Interfaces::WorldData& worldData)
{
    for (const auto& [id, junction]: worldData.GetJunctions())
    {
        JunctionPolygons junctionPolygons;
        std::transform(junction->GetConnectingRoads().begin(),
                       junction->GetConnectingRoads().end(),
                       std::inserter(junctionPolygons, junctionPolygons.begin()),
                       BuildRoadPolygons);

        CalculateJunctionIntersectionsFromRoadPolygons(junctionPolygons, junction);
    }
}

RoadPolygons GeometryConverter::BuildRoadPolygons(const OWL::Road* const road)
{
    std::vector<LaneGeometryPolygon> polygons;
    auto& roadId = road->GetId();

    for (const auto section : road->GetSections())
    {
        for (const auto lane : section->GetLanes())
        {
            OWL::Id laneId = lane->GetId();

            const auto buildPolygonFromLaneGeometryElement = CreateBuildPolygonFromLaneGeometryFunction(roadId, laneId);
            std::transform(lane->GetLaneGeometryElements().begin(),
                           lane->GetLaneGeometryElements().end(),
                           std::back_inserter(polygons),
                           buildPolygonFromLaneGeometryElement);
        }
    }

    return std::make_pair(roadId, polygons);
}

std::function<LaneGeometryPolygon (const OWL::Primitive::LaneGeometryElement* const)> GeometryConverter::CreateBuildPolygonFromLaneGeometryFunction(const std::string& roadId,
                                                                                                                                                    const OWL::Id laneId)
{
    return [roadId, laneId](const auto elem) -> LaneGeometryPolygon
    {
        point_t currentLeftPoint{elem->joints.current.points.left.x, elem->joints.current.points.left.y};
        point_t currentRightPoint{elem->joints.current.points.right.x, elem->joints.current.points.right.y};
        point_t nextRightPoint{elem->joints.next.points.right.x, elem->joints.next.points.right.y};
        point_t nextLeftPoint{elem->joints.next.points.left.x, elem->joints.next.points.left.y};
        polygon_t polygon;

        bg::append(polygon, currentLeftPoint);
        bg::append(polygon, currentRightPoint);
        bg::append(polygon, nextRightPoint);
        bg::append(polygon, nextLeftPoint);
        bg::append(polygon, currentLeftPoint);
        bg::correct(polygon);

        return LaneGeometryPolygon{roadId,
                                   laneId,
                                   elem,
                                   polygon};
    };
}

void GeometryConverter::CalculateJunctionIntersectionsFromRoadPolygons(const JunctionPolygons& junctionPolygons,
                                                                       OWL::Junction* const junction)
{
    auto roadPolygonsIter = junctionPolygons.begin();
    while (roadPolygonsIter != junctionPolygons.end())
    {
        auto roadPolygonsToCompareIter = roadPolygonsIter;
        roadPolygonsToCompareIter++;

        while (roadPolygonsToCompareIter != junctionPolygons.end())
        {
            const auto intersectionInfo = CalculateIntersectionInfoForRoadPolygons(*roadPolygonsIter, *roadPolygonsToCompareIter, junction);
            if (intersectionInfo)
            {
                const auto crossIntersectionInfo = CalculateIntersectionInfoForRoadPolygons(*roadPolygonsToCompareIter, *roadPolygonsIter, junction);

                junction->AddIntersectionInfo(roadPolygonsIter->first, intersectionInfo.value());
                junction->AddIntersectionInfo(roadPolygonsToCompareIter->first, crossIntersectionInfo.value());
            }

            roadPolygonsToCompareIter++;
        }

        roadPolygonsIter++;
    }
}

std::optional<OWL::IntersectionInfo> GeometryConverter::CalculateIntersectionInfoForRoadPolygons(const RoadPolygons& roadPolygons,
                                                                                                 const RoadPolygons& roadPolygonsToCompare,
                                                                                                 const OWL::Junction * const junction)
{
    OWL::IntersectionInfo info;
    info.intersectingRoad = roadPolygonsToCompare.first;
    info.relativeRank = GetRelativeRank(roadPolygons.first, roadPolygonsToCompare.first, junction);

    for (const auto& laneGeometryPolygon : roadPolygons.second)
    {
        for (const auto& polygonToCompare : roadPolygonsToCompare.second)
        {
            std::vector<polygon_t> intersectionPolygons;
            bg::intersection(laneGeometryPolygon.polygon, polygonToCompare.polygon, intersectionPolygons);

            if (!intersectionPolygons.empty() && !(bg::area(intersectionPolygons.front()) < EPS))
            {
                World::Localization::LocalizationElement localizationElement{*laneGeometryPolygon.laneGeometryElement};
                World::Localization::WorldToRoadCoordinateConverter processor{localizationElement};

                double minS = std::numeric_limits<double>::max();
                double maxS = 0;

                for (const auto& point : intersectionPolygons.front().outer())
                {
                    double s = processor.GetS(Common::Vector2d(bg::get<0>(point), bg::get<1>(point)));
                    minS = std::min(minS, s);
                    maxS = std::max(maxS, s);
                }

                std::pair<OWL::Id, OWL::Id> intersectingLanesPair{laneGeometryPolygon.laneId, polygonToCompare.laneId};
                const auto intersectingLanesPairIter = info.sOffsets.find(intersectingLanesPair);

                // if these lane ids are already marked as intersecting, update the startSOffset and endSOffset to reflect new intersection information
                if (intersectingLanesPairIter != info.sOffsets.end())
                {
                    double recordedStartS = (*intersectingLanesPairIter).second.first;
                    double recordedEndS = (*intersectingLanesPairIter).second.second;
                    (*intersectingLanesPairIter).second.first = std::min(recordedStartS, minS);
                    (*intersectingLanesPairIter).second.second = std::max(recordedEndS, maxS);
                }
                else
                {
                    info.sOffsets.emplace(intersectingLanesPair, std::make_pair(minS, maxS));
                }

            }
        }
    }

    return info.sOffsets.size() > 0
            ? std::make_optional(info)
            : std::nullopt;
}

IntersectingConnectionRank GeometryConverter::GetRelativeRank(const std::string& roadId, const std::string& intersectingRoadId, const OWL::Junction * const junction)
{
    if (std::find_if(junction->GetPriorities().begin(),
                     junction->GetPriorities().end(),
                     [&](const auto& priority){return priority.first == roadId && priority.second == intersectingRoadId;}) != junction->GetPriorities().end())
    {
        return IntersectingConnectionRank::Lower;
    }
    else if (std::find_if(junction->GetPriorities().begin(),
                          junction->GetPriorities().end(),
                          [&](const auto& priority){return priority.first == intersectingRoadId && priority.second == roadId;}) != junction->GetPriorities().end())
    {
        return IntersectingConnectionRank::Higher;
    }

    return IntersectingConnectionRank::Undefined;
}
