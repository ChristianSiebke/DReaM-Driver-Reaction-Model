/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2016, 2017 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include <algorithm>
#include <utility>
#include <limits>
#include <cassert>
#include <iostream>
#include <string>
#include <memory>
#include <cmath>
#include <QFile>

#include "GeometryConverter.h"
#include "WorldToRoadCoordinateConverter.h"
#include "Common/vector2d.h"
#include "WorldData.h"

GeometryConverter::GeometryConverter(SceneryInterface *scenery,
                                     OWL::Interfaces::WorldData& worldData,
                                     const CallbackInterface *callbacks) :
    scenery(scenery),
    worldData(worldData),
    callbacks(callbacks)
{}

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

const RoadLaneWidth* GeometryConverter::GetRelevantRoadLaneWidth(double sectionOffset, const RoadLaneInterface *roadLane)
{
    auto roadLaneIt = roadLane->GetWidths().begin();
    while(roadLane->GetWidths().end() != roadLaneIt)
    {
        if((*roadLaneIt)->GetSOffset() <= sectionOffset)
        {
            auto roadLaneNextIt = std::next(roadLaneIt);
            if(roadLane->GetWidths().end() == roadLaneNextIt ||
                    (*roadLaneNextIt)->GetSOffset() > sectionOffset)
            {
                break;
            }
        }

        ++roadLaneIt;
    }

    if(roadLaneIt == roadLane->GetWidths().end())
    {
        return nullptr;
    }
    else
    {
      return *roadLaneIt;
    }
}

const RoadLaneOffset* GeometryConverter::GetRelevantRoadLaneOffset(double roadOffset, RoadInterface* road)
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
    auto roadMarkIt = roadLane->getRoadMarks().begin();

    while(roadMarkIt != roadLane->getRoadMarks().end())
    {
        if((*roadMarkIt)->GetSOffset() <= sectionOffset)
        {
            auto roadMarkNextIt = std::next(roadMarkIt);
            if(roadMarkNextIt == roadLane->getRoadMarks().end() ||
               (*roadMarkNextIt)->GetSOffset() > sectionOffset)
            {
                break;
            }
        }

        ++roadMarkIt;
    }

    if(roadMarkIt == roadLane->getRoadMarks().end())
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
    const RoadLaneWidth* roadLaneWidth = GetRelevantRoadLaneWidth(sectionOffset, roadLane);

    if(!roadLaneWidth)
    {
        LOG(CbkLogLevel::Warning, "no lane width given");
        return 0.0;
    }

    return CalculateWidthAtSectionPosition(roadLaneWidth, sectionOffset);
}

double GeometryConverter::CalculateLaneOffset(RoadInterface* road, double roadPosition)
{
    const RoadLaneOffset* roadLaneOffset = GetRelevantRoadLaneOffset(roadPosition, road);

    if(!roadLaneOffset)
    {
        LOG(CbkLogLevel::Warning, "no lane offset given");
        return 0.0;
    }

    return CalculateOffsetAtRoadPosition(roadLaneOffset, roadPosition);
}

bool GeometryConverter::CalculateLanes(int side,
                                       RoadLaneSectionInterface* roadSection,
                                       RoadInterface* road,
                                       RoadGeometryInterface* roadGeometry,
                                       double geometryOffset,
                                       double sectionOffsetStart,
                                       double sectionOffsetEnd,
                                       double roadOffset,
                                       double roadSectionStart,
                                       int index)
{
    Q_UNUSED(sectionOffsetStart);
    Q_UNUSED(sectionOffsetEnd);
    Q_UNUSED(index);
    auto& roadLanes = roadSection->GetLanes();
    double previousWidth = 0.0;
    std::list<RoadLaneInterface*> orderedRoadLanes; // ordered from inner lane to outer lane (only one side of road)

    // (OpenDrive) ids of road lanes given in ascending order
    if(0 < side)
    {
        std::map<int, RoadLaneInterface*>::iterator roadLaneIt;
        for(roadLaneIt = roadLanes.begin();
            roadLanes.end() != roadLaneIt;
            ++roadLaneIt)
        {
            if(0 < roadLaneIt->first)
            {
                orderedRoadLanes.push_back(roadLaneIt->second);
            }
        }
    }
    else if (0 > side)
    {
        std::map<int, RoadLaneInterface*>::reverse_iterator roadLaneIt;
        for(roadLaneIt = roadLanes.rbegin();
            roadLanes.rend() != roadLaneIt;
            ++roadLaneIt)
        {
            if(0 > roadLaneIt->first)
            {
                orderedRoadLanes.push_back(roadLaneIt->second);
            }
        }
    }
    else
    {
        std::map<int, RoadLaneInterface*>::iterator roadLaneIt;
        for(roadLaneIt = roadLanes.begin();
            roadLanes.end() != roadLaneIt;
            ++roadLaneIt)
        {
            if(0 == roadLaneIt->first)
            {
                orderedRoadLanes.push_back(roadLaneIt->second);
            }
        }
    }

    for(RoadLaneInterface* roadLane : orderedRoadLanes)
    {
        double sectionOffset = roadOffset - roadSectionStart;
        double laneOffset = CalculateLaneOffset(road, roadOffset);
        double laneWidth = CalculateLaneWidth(roadLane, sectionOffset);

        Common::Vector2d pointLeft, pointCenter, pointRight;

        pointLeft = roadGeometry->GetCoord(side,
                                           geometryOffset,
                                           previousWidth,
                                           laneOffset,
                                           laneWidth,
                                           1);

        pointCenter = roadGeometry->GetCoord(side,
                                             geometryOffset,
                                             previousWidth,
                                             laneOffset,
                                             laneWidth,
                                             0);

        pointRight = roadGeometry->GetCoord(side,
                                            geometryOffset,
                                            previousWidth,
                                            laneOffset,
                                            laneWidth,
                                            -1);

        double curvature = roadGeometry->GetCurvature(side,
                                                      geometryOffset,
                                                      previousWidth,
                                                      laneOffset,
                                                      laneWidth);
        double heading = roadGeometry->GetDir(side,
                                              geometryOffset,
                                              previousWidth,
                                              laneOffset,
                                              laneWidth);
        if(roadLane->GetId() == 0)
        {
            worldData.AddCenterLinePoint(*roadSection, pointCenter, roadOffset, heading);
        }
        else
        {
            worldData.AddLaneGeometryPoint(*roadLane,
                                           pointLeft, pointCenter, pointRight,
                                           roadOffset, curvature, heading);
        }

        previousWidth += laneWidth;
    }

    return true;
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

bool GeometryConverter::Convert()
{
    return CalculateRoads() && CalculateIntersections();
}

bool GeometryConverter::CalculatePoints(double geometryOffsetStart,
                                        double geometryOffsetEnd,
                                        int numberLaneGeomPoints,
                                        RoadLaneSectionInterface *roadSection,
                                        RoadInterface *road,
                                        RoadGeometryInterface *roadGeometry,
                                        double sectionOffsetStart,
                                        double sectionOffsetEnd,
                                        double roadGeometryStart,
                                        double roadSectionStart,
                                        bool includeStartPoint)
{
    // calculate points
    double geometryOffset = geometryOffsetStart + (includeStartPoint ? 0 : SAMPLING_RATE);
    for(int index = 0; index < numberLaneGeomPoints; ++index)
    {
        // account for last sample
        if(geometryOffset > geometryOffsetEnd)
        {
            geometryOffset = geometryOffsetEnd;
        }

        if(!CalculateLanes(0, // center lane
                           roadSection,
                           road,
                           roadGeometry,
                           geometryOffset,
                           sectionOffsetStart,
                           sectionOffsetEnd,
                           roadGeometryStart + geometryOffset,
                           roadSectionStart,
                           index))
        {
            return false;
        }

        if(!CalculateLanes(1, // left lanes
                           roadSection,
                           road,
                           roadGeometry,
                           geometryOffset,
                           sectionOffsetStart,
                           sectionOffsetEnd,
                           roadGeometryStart + geometryOffset,
                           roadSectionStart,
                           index))
        {
            return false;
        }

        if(!CalculateLanes(-1, // right lanes
                           roadSection,
                           road,
                           roadGeometry,
                           geometryOffset,
                           sectionOffsetStart,
                           sectionOffsetEnd,
                           roadGeometryStart + geometryOffset,
                           roadSectionStart,
                           index))
        {
            return false;
        }

        geometryOffset += SAMPLING_RATE;
    }

    return true;
}

void GeometryConverter::CalculateGeometryOffsetStart(double roadSectionStart,
                                                     double roadGeometryStart,
                                                     double* geometryOffsetStart,
                                                     double* sectionOffsetStart)
{
    // geometry begins within section
    if(roadSectionStart <= roadGeometryStart)
    {
        *geometryOffsetStart = 0.0;
        *sectionOffsetStart = roadGeometryStart - roadSectionStart;
    }
    // geometry begins before section
    else
    {
        *geometryOffsetStart = roadSectionStart - roadGeometryStart;
        *sectionOffsetStart = 0.0;
    }
}

void GeometryConverter::CalculateGeometryOffsetEnd(double roadSectionStart,
                                                   double roadSectionNextStart,
                                                   double roadGeometryStart,
                                                   double roadGeometryEnd,
                                                   double roadGeometryLength,
                                                   double* geometryOffsetEnd,
                                                   double* sectionOffsetEnd)
{
    // geometry ends within section
    if(roadSectionNextStart >= roadGeometryEnd)
    {
        *geometryOffsetEnd = roadGeometryLength;
        *sectionOffsetEnd = roadGeometryEnd - roadSectionStart;
    }
    // geometry ends after section
    else
    {
        *geometryOffsetEnd = roadSectionNextStart - roadGeometryStart;
        *sectionOffsetEnd = roadSectionNextStart;
    }
}

bool GeometryConverter::CalculatePointsOfAffectedGeometry(double roadSectionStart,
                                                          double roadGeometryStart,
                                                          double roadSectionNextStart,
                                                          double roadGeometryEnd,
                                                          double roadGeometryLength,
                                                          RoadLaneSectionInterface* roadSection,
                                                          RoadInterface* road,
                                                          RoadGeometryInterface* roadGeometry,
                                                          bool includeStartPoint)
{
    double geometryOffsetStart;
    double sectionOffsetStart;
    CalculateGeometryOffsetStart(roadSectionStart,
                                 roadGeometryStart,
                                 &geometryOffsetStart,
                                 &sectionOffsetStart);

    double geometryOffsetEnd;
    double sectionOffsetEnd;
    CalculateGeometryOffsetEnd(roadSectionStart,
                               roadSectionNextStart,
                               roadGeometryStart,
                               roadGeometryEnd,
                               roadGeometryLength,
                               &geometryOffsetEnd,
                               &sectionOffsetEnd);

    // allocate memory (account for last sample in even/odd cases)
    int numberLaneGeomPoints = static_cast<int>(std::ceil((geometryOffsetEnd - geometryOffsetStart) / SAMPLING_RATE)) +
            (includeStartPoint ? 1 : 0);

/*
    for(auto &roadLaneItem : roadLanes)
    {
        AllocateLaneGeometry(roadLaneItem.second,
                             numberLaneGeomPoints,
                             sectionOffsetStart,
                             sectionOffsetEnd);
    }
*/

    bool status = CalculatePoints(geometryOffsetStart,
                                  geometryOffsetEnd,
                                  numberLaneGeomPoints,
                                  roadSection,
                                  road,
                                  roadGeometry,
                                  sectionOffsetStart,
                                  sectionOffsetEnd,
                                  roadGeometryStart,
                                  roadSectionStart,
                                  includeStartPoint);

    return status;
}

bool GeometryConverter::CalculateGeometries(double roadSectionStart,
                                            double roadSectionNextStart,
                                            RoadInterface* road,
                                            RoadLaneSectionInterface *roadSection)
{
    double laneBoundaryOffsetStart = 0;
    bool includeStartPoint = true;

    while (laneBoundaryOffsetStart + roadSectionStart < roadSectionNextStart)
    {
        double laneBoundaryOffsetEnd = roadSectionNextStart - roadSectionStart;
        for (const auto& roadLane : roadSection->GetLanes())
        {
            for (const auto& roadMark : roadLane.second->getRoadMarks())
            {
                double sOffset = roadMark->GetSOffset();
                if (sOffset > laneBoundaryOffsetStart && sOffset < laneBoundaryOffsetEnd)
                {
                    laneBoundaryOffsetEnd = sOffset;
                }
            }
        }

        if(!CalculateGeometriesBetweenRoadMarks(roadSectionStart + laneBoundaryOffsetStart, roadSectionStart + laneBoundaryOffsetEnd, road, roadSection, includeStartPoint))
        {
            return false;
        }
        laneBoundaryOffsetStart = laneBoundaryOffsetEnd;
        includeStartPoint = false;
    }

    return true;
}

bool GeometryConverter::CalculateGeometriesBetweenRoadMarks(double roadSectionStart,
                                                            double roadSectionNextStart,
                                                            RoadInterface *road,
                                                            RoadLaneSectionInterface *roadSection,
                                                            bool includeStartPoint)
{
    bool status;
    std::list<RoadGeometryInterface*> roadGeometries = road->GetGeometries();

    double roadGeometryStart;
    double roadGeometryEnd;

    for(RoadGeometryInterface* roadGeometry: roadGeometries)
    {
        status = CalculateGeometry(roadSectionStart,
                                   roadSectionNextStart,
                                   road,
                                   roadSection,
                                   roadGeometry,
                                   roadGeometryStart,
                                   roadGeometryEnd,
                                   includeStartPoint);

        if(!status) return status;
    }

    status = true;
    return status;
}

bool GeometryConverter::CalculateGeometry(double roadSectionStart,
                                          double roadSectionNextStart,
                                          RoadInterface* road,
                                          RoadLaneSectionInterface *roadSection,
                                          RoadGeometryInterface* roadGeometry,
                                          double& roadGeometryStart,
                                          double& roadGeometryEnd,
                                          bool includeStartPoint)
{
    double roadGeometryLength = roadGeometry->GetLength();

    roadGeometryStart = roadGeometry->GetS();
    roadGeometryEnd = roadGeometryStart + roadGeometryLength;

    // if section is affected by geometry
    if(roadSectionStart < roadGeometryEnd &&
            roadSectionNextStart > roadGeometryStart)
    {
        bool status = CalculatePointsOfAffectedGeometry(roadSectionStart,
                                                        roadGeometryStart,
                                                        roadSectionNextStart,
                                                        roadGeometryEnd,
                                                        roadGeometryLength,
                                                        roadSection,
                                                        road,
                                                        roadGeometry,
                                                        includeStartPoint);
        if(status == false) return status;
    }

    return true;
}

bool GeometryConverter::CalculateRoads()
{
    for(auto &roadItem : scenery->GetRoads())
    {
        RoadInterface *road = roadItem.second;

        std::vector<RoadLaneSectionInterface*> roadLaneSections = road->GetLaneSections();

        //for(size_t i = 0; i < numberOfRoadLaneSections; i++)
        //for (const auto roadSection : roadLaneSections)
        for (auto roadLaneSectionIt = roadLaneSections.begin();
             roadLaneSectionIt != roadLaneSections.end();
             roadLaneSectionIt++)
        {
            RoadLaneSectionInterface* roadSection = *roadLaneSectionIt;
            std::map<int, RoadLaneInterface*>& roadLanes = roadSection->GetLanes();

            if(!roadLanes.empty())
            {
                double roadSectionStart = roadSection->GetStart();
                double roadSectionNextStart = std::numeric_limits<double>::max();

                if (std::next(roadLaneSectionIt) != roadLaneSections.end()) // if not a single element in the list
                {
                    roadSectionNextStart = (*std::next(roadLaneSectionIt))->GetStart();
                }

                // collect geometry sections
                bool status;
                status = CalculateGeometries(roadSectionStart,
                                             roadSectionNextStart,
                                             road,
                                             roadSection);

                if(status == false) return status;
            } // if lanes are not empty
        }
    }

    return true;
}

bool GeometryConverter::IsEqual(const double valueA, const double valueB)
{
    return std::abs(valueA - valueB) < EPS;
}

bool GeometryConverter::CalculateIntersections()
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

    return true;
}

std::pair<OWL::Id, std::vector<LaneGeometryPolygon>> GeometryConverter::BuildRoadPolygons(const OWL::Road* const road)
{
    std::vector<LaneGeometryPolygon> polygons;
    OWL::Id roadId = road->GetId();

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

std::function<LaneGeometryPolygon (const OWL::Primitive::LaneGeometryElement* const)> GeometryConverter::CreateBuildPolygonFromLaneGeometryFunction(const OWL::Id roadId,
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

                junction->AddIntersectionInfo(worldData.GetRoadIdMapping().at(roadPolygonsIter->first), intersectionInfo.value());
                junction->AddIntersectionInfo(worldData.GetRoadIdMapping().at(roadPolygonsToCompareIter->first), crossIntersectionInfo.value());
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

IntersectingConnectionRank GeometryConverter::GetRelativeRank(const OWL::Id roadId, const OWL::Id intersectingRoadId, const OWL::Junction * const junction)
{
    if (std::find(junction->GetPriorities().begin(),
                  junction->GetPriorities().end(),
                  std::make_pair(roadId, intersectingRoadId)) != junction->GetPriorities().end())
    {
        return IntersectingConnectionRank::Lower;
    }
    else if (std::find(junction->GetPriorities().begin(),
             junction->GetPriorities().end(),
             std::make_pair(intersectingRoadId, roadId)) != junction->GetPriorities().end())
    {
        return IntersectingConnectionRank::Higher;
    }

    return IntersectingConnectionRank::Undefined;
}
