/********************************************************************************
 * Copyright (c) 2016-2017 ITK Engineering GmbH
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  GeometryConverter.h
//! @brief This file contains the converter of the scenery geometry.
//-----------------------------------------------------------------------------

#pragma once

#include <map>
#include <list>
#include "include/callbackInterface.h"
#include "include/sceneryInterface.h"
#include "include/worldInterface.h"
#include "WorldData.h"
#include "JointsBuilder.h"

struct LaneGeometryPolygon
{
    std::string roadId;
    OWL::Id laneId;
    const OWL::Primitive::LaneGeometryElement * const laneGeometryElement;
    polygon_t polygon;
};
using RoadPolygons = std::pair<std::string, std::vector<LaneGeometryPolygon>>;
using JunctionPolygons = std::map<std::string, std::vector<LaneGeometryPolygon>>;

//-----------------------------------------------------------------------------
//! Functions for the convertion of the road geometries in a section. First, the roads,
//! lane sections and lanes have to be converted using SceneryConverter, which then
//! provides the mappings from OpenDrive to OSI of those elements.
//-----------------------------------------------------------------------------
namespace GeometryConverter
{
//-----------------------------------------------------------------------------
//! \brief Converts OpenDrive road network to OSI objects
//!
//! This function reads from the imported OpenDrive scenery and adds all roads,
//! sections, lanes and junctions to the WorldData by calling the respective
//! functions of the WorldData.
//!
//! \param  scenery     Scenery with the OpenDrive roads
//! \param  worldData   worldData that is built by this function
//-----------------------------------------------------------------------------
void Convert(const SceneryInterface& scenery, OWL::Interfaces::WorldData& worldData);

//! Converts the Roads section in OpenDrive to OSI
//!
//! \param  scenery     Scenery with the OpenDrive roads
//! \param  worldData   worldData that is built by this function
void CalculateRoads(const SceneryInterface& scenery, OWL::Interfaces::WorldData& worldData);

//! Converts a single section of an OpenDrive road to OSI
//!
//! \param  worldData       worldData that is built by this function
//! \param roadSectionStart start s coordinate of the section
//! \param roadSectionEnd   end s coordinate of the section
//! \param road             road the section is part of
//! \param roadSection      section to convert
void CalculateSection(OWL::Interfaces::WorldData& worldData,
                      double roadSectionStart,
                      double roadSectionEnd,
                      const RoadInterface* road,
                      const RoadLaneSectionInterface* roadSection);

//! Samples the lane boundary points of the lanes of one section between two consecutive
//! changes in the RoadMarks of the lanes with a defined maximum lateral error
//!
//! \param roadSectionStart start s coordinate of the section / first RoadMark change
//! \param roadSectionEnd   end s coordinate of the section / second RoadMark change
//! \param road             road the section is part of
//! \param roadSection      section to convert
//! \return                 sampled boundary points
SampledGeometry CalculateSectionBetweenRoadMarkChanges(double roadSectionStart,
                                                       double roadSectionEnd,
                                                       const RoadInterface *road,
                                                       const RoadLaneSectionInterface *roadSection);

//! Samples the lane boundary points of one geometry for one section with a defined maximum lateral error
//!
//! \param roadSectionStart start s coordinate of the section / first RoadMark change
//! \param roadSectionEnd       end s coordinate of the section / second RoadMark change
//! \param road                 road the section is part of
//! \param roadSection          section to convert
//! \param roadGeometry         geometry to sample
//! \param roadGeometryLength   length of the geometry
//! \return                 sampled boundary points
SampledGeometry CalculateGeometry(double roadSectionStart,
                                  double roadSectionEnd,
                                  const RoadInterface *road,
                                  const RoadLaneSectionInterface *roadSection,
                                  const RoadGeometryInterface *roadGeometry,
                                  double roadGeometryLength);

//! Calculates the start s offset of a geometry inside a section
//!
//! \param roadSectionStart     start s coordinate of the section
//! \param roadGeometryStart    start s coordinate of the geometry
//! \return                     start s offset of the geometry in the section
double CalculateGeometryOffsetStart(double roadSectionStart,
                                    double roadGeometryStart);

//! Calculates the end s offset of a geometry inside a section
//!
//! \param roadSectionEnd       end s coordinate of the section
//! \param roadGeometryStart    start s coordinate of the geometry
//! \param roadGeometryEnd      end s coordinate of the section
//! \param roadGeometryLength   length of the geometry
//! \return                     end s offset of the geometry in the section
double CalculateGeometryOffsetEnd(double roadSectionEnd,
                                  double roadGeometryStart,
                                  double roadGeometryEnd,
                                  double roadGeometryLength);

//! Calculates the lane boundary points with a high sampling rate
//!
//! \param geometryOffsetStart  start s offset of the geometry in the section
//! \param geometryOffsetEnd    end s offset of the geometry in the section
//! \param roadSection          section to convert
//! \param road                 road the section is part of
//! \param roadGeometry         geometry to sample
//! \param roadGeometryStart    start s coordinate of the geometry
//! \param roadSectionStart     start s coordinate of the section
//! \return                     sampled lane boundary points
std::vector<BorderPoints> CalculateJoints(double geometryOffsetStart,
                                          double geometryOffsetEnd,
                                          const RoadLaneSectionInterface *roadSection,
                                          const RoadInterface *road,
                                          const RoadGeometryInterface *roadGeometry,
                                          double roadGeometryStart,
                                          double roadSectionStart);

//! Calculates the lane boundary points of a single joint (i.e. at one s coordinate)
//!
//! \param roadSection          section to convert
//! \param road                 road the section is part of
//! \param roadGeometry         geometry to sample
//! \param geometryOffset       s offset on the geometry
//! \param roadOffset           s coordinate on the road
//! \param roadSectionStart     start s coordinate of the section
//! \return                     sampled lane boundary points
BorderPoints CalculateBorderPoints(const RoadLaneSectionInterface* roadSection,
                                   const RoadInterface *road,
                                   const RoadGeometryInterface *roadGeometry,
                                   double geometryOffset,
                                   double roadOffset,
                                   double roadSectionStart);

//! Adds the sampled joints to the world
//!
//! \param  worldData   worldData that is built by this function
//! \param joints       sampled joints
void AddPointsToWorld(OWL::Interfaces::WorldData& worldData, const Joints& joints);

//-----------------------------------------------------------------------------
//! Calculates height coordinate according to OpenDrive elevation profiles.
//!
//! @param[in]  road           OpenDrive road data structure
//! @param[in]  offset         Absolute offset on reference line within road
//! @return                    height coordinate
//-----------------------------------------------------------------------------
double CalculateCoordZ(RoadInterface *road, double offset);

//-----------------------------------------------------------------------------
//! Calculates slope according to OpenDrive elevation profiles.
//!
//! @param[in]  road           OpenDrive road data structure
//! @param[in]  offset         Absolute offset on reference line within road
//! @return                    slope
//-----------------------------------------------------------------------------
double CalculateSlope(RoadInterface *road, double offset);

//-----------------------------------------------------------------------------
//! Calculates the width of the provided lane.
//!
//! @param[in]  roadLane            OpenDrive road lane (for width information)
//! @param[in]  sectionOffset       Offset within the OpenDrive section
//! @return                         Lane width, 0.0 if no width was specified
//-----------------------------------------------------------------------------
double CalculateLaneWidth(const RoadLaneInterface* roadLane, double sectionOffset);

//-----------------------------------------------------------------------------
//! Calculates the border of the provided lane.
//!
//! @param[in]  roadLane            OpenDrive road lane (for width information)
//! @param[in]  sectionOffset       Offset within the OpenDrive section
//! @return                         Lane border, 0.0 if no width was specified
//-----------------------------------------------------------------------------
double CalculateLaneBorder(const RoadLaneInterface* roadLane, double sectionOffset);

double CalculateLaneOffset(const RoadInterface* road, double roadPosition);

//-----------------------------------------------------------------------------
//! Calculates the slope of a lane at a given position
//!
//! @param[in] elevation        elevation input data (has to match given position)
//! @param[in] position         position w.r.t. start of section
//! @return                     width at given position
//-----------------------------------------------------------------------------
double CalculateSlopeAtRoadPosition(const RoadElevation* elevation, double position);

double CalculateOffsetAtRoadPosition(const RoadLaneOffset* roadOffset, double position);

//-----------------------------------------------------------------------------
//! Calculates the width of a lane at a given position
//!
//! @param[in] width            width input data (has to match given position)
//! @param[in] position         position w.r.t. start of section
//! @return                     width at given position
//-----------------------------------------------------------------------------
double CalculateWidthAtSectionPosition(const RoadLaneWidth* width, double position);

//-----------------------------------------------------------------------------
//! Get the RoadElevation which is relevant for the given position
//!
//! @param[in] sectionOffset    position w.r.t. start of section
//! @param[in] roadLane         the RoadLaneInterface input data
//! @return                     relevant RoadElevation
//-----------------------------------------------------------------------------
const RoadElevation* GetRelevantRoadElevation(double sectionOffset, RoadInterface *road);

//-----------------------------------------------------------------------------
//! Get the RoadLaneWidth which is relevant for the given position
//!
//! @param[in] sectionOffset    position w.r.t. start of section
//! @param[in] widthsOrBorders        container with all potential RoadLaneWidth pointers
//! @return                     relevant RoadLaneWidth
//-----------------------------------------------------------------------------
const RoadLaneWidth* GetRelevantRoadLaneWidth(double sectionOffset, const std::vector<RoadLaneWidth *> widthsOrBorders);

const RoadLaneOffset* GetRelevantRoadLaneOffset(double roadOffset, const RoadInterface* road);

const RoadLaneRoadMark* GetRelevantRoadLaneRoadMark(double sectionOffset, const RoadLaneInterface* roadLane);

//-----------------------------------------------------------------------------
//! Tests if the provided values' difference is smaller than EPS
//!
//! @param[in] valueA           input value A
//! @param[in] valueB           input value B
//! @return                     true, if valueA and valueB are (approximately) the same, false otherwise
//-----------------------------------------------------------------------------
bool IsEqual(const double valueA, const double valueB);

//-----------------------------------------------------------------------------
//! \brief CalculateIntersections calculates any intersections on the
//!        scenery's junctions
//-----------------------------------------------------------------------------
void CalculateIntersections(OWL::Interfaces::WorldData& worldData);

//-----------------------------------------------------------------------------
//! \brief BuildRoadPolygons builds all polygons for the road and pairs them
//!        with the road's id
//!
//! \param[in] road the road for which polygons will be built
//! \return the pair of the road id to the collection of polygons for the road
//-----------------------------------------------------------------------------
RoadPolygons BuildRoadPolygons(const OWL::Road* const road);

//-----------------------------------------------------------------------------
//! \brief CreateBuildPolygonFromLaneGeometryFunction creates a function that
//!        builds LaneGeometryPolygons from a LaneGeometryElement for a
//!        specific roadId and laneId
//!
//! \param[in] roadId the roadId for which the created function will build
//!            LaneGeometryPolygons
//! \param[in] laneId the laneId for which the created function will build
//!            LaneGeometryPolygons
//! \return a function that creates LaneGeometryPolygons from
//!         LaneGeometryElements for the lane at laneId on the road at roadId
//-----------------------------------------------------------------------------
std::function<LaneGeometryPolygon (const OWL::Primitive::LaneGeometryElement* const)> CreateBuildPolygonFromLaneGeometryFunction(const std::string& roadId,
                                                                                                                                 const OWL::Id laneId);

//-----------------------------------------------------------------------------
//! \brief CalculateJunctionIntersectionsFromRoadPolygons calculates
//!        intersections for a junction from polygons created from that
//!        junction's roads
//!
//! \param[in] junctionPolygons the map of road ids to LaneGeometryPolygons for
//!            the junction
//! \param[out] junction the junction to which intersection information should
//!             be added, given that any interesections are found
//-----------------------------------------------------------------------------
void CalculateJunctionIntersectionsFromRoadPolygons(const JunctionPolygons& junctionPolygons,
                                                    OWL::Junction* const junction);

//-----------------------------------------------------------------------------
//! \brief CalculateIntersectionInfoForRoadPolygons calculates intersection
//!        information between two sets of RoadPolygons
//!
//! \param[in] polygons the base road's set of polygons
//! \param[in] polygonsToCompare the potentially-intersecting road's set of
//!            polygons
//! \param[in] junction the junction for which the intersection info is
//!            calculated
//! \return the IntersectionInfo between the two sets of RoadPolygons, if an
//!         intersection exists between them
//-----------------------------------------------------------------------------
std::optional<OWL::IntersectionInfo> CalculateIntersectionInfoForRoadPolygons(const RoadPolygons& polygons,
                                                                              const RoadPolygons& polygonsToCompare,
                                                                              const OWL::Junction * const junction);

//-----------------------------------------------------------------------------
//! \brief GetRelativeRank Gets the relative rank of an intersecting road with
//!        respect to a road
//!
//! \param[in] roadId the roadId of the road whose relative rank to get
//! \param[in] intersectingRoadId the roadId of the intersecting road
//! \param[in] junction the junction in which the roads intersect
//! \return the relative rank of the intersecting road
//-----------------------------------------------------------------------------
IntersectingConnectionRank GetRelativeRank(const std::string& roadId, const std::string& intersectingRoadId, const OWL::Junction * const junction);

constexpr static const double SAMPLING_RATE = 0.1; // 1m sampling rate of reference line
constexpr static const double EPS = 1e-3;   // epsilon value for geometric comparisons
};

