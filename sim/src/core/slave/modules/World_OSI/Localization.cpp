/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#include <exception>
#include <numeric>

#include "Localization.h"
#include "PointQuery.h"

#include <boost/function_output_iterator.hpp>

namespace World {
namespace Localization {

std::vector<Common::Vector2d> CalculateEdges(const std::vector<Common::Vector2d>& corners)
{
    std::vector<Common::Vector2d> edges;
    for (size_t i = 0; i < corners.size(); i++)
    {
        edges.push_back(corners[(i + 1) % corners.size()] - corners[i]);
    }
    return edges;
}

//! \brief Calculates the intersection polygon of a lane element with an object bounding box
//!
//! This method calculates all points of the intersection polygon of a lane element with the bounding box of an object.
//! It is assumed, that the object bounding box is rectangular, the element polygon is convex and that the points are given in clockwise order.
//! Solve the linear equation "boundary point + lambda * boundary edge = element point + kappa * element edge" for each pair of edges to get the intersection of the edges.
//! If both lamda and kappa are between 0 and 1, then the intersection lies on both edges. If the determinat is 0, then the two egdes are parallel.
//!
//! \param elementPoints    corner points of the lane element in clockwise order
//! \param boundaryPoints   corner points of the object bounding box in clockwise order
//! \return points of the intersection polygon
std::vector<Common::Vector2d> GetIntersectionPoints (const std::vector<Common::Vector2d>& elementPoints, const std::vector<Common::Vector2d>& boundaryPoints)
{
    std::vector<Common::Vector2d> intersectionPoints;
    std::vector<Common::Vector2d> elementEdges = CalculateEdges(elementPoints);
    std::vector<Common::Vector2d> boundaryEdges = CalculateEdges(boundaryPoints);

    double lambda[4][4];
    double kappa[4][4];
    bool parallel[4][4];
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t k = 0; k < 4; k++)
        {
            double determinant = boundaryEdges[i].x * elementEdges[k].y - boundaryEdges[i].y * elementEdges[k].x;
            parallel[i][k] = (std::abs(determinant) < CommonHelper::EPSILON);
            lambda[i][k] = ( - boundaryPoints[i].x * elementEdges[k].y + boundaryPoints[i].y * elementEdges[k].x
                             + elementPoints[k].x * elementEdges[k].y - elementPoints[k].y * elementEdges[k].x ) / determinant;
            kappa[i][k] = ( - boundaryPoints[i].x * boundaryEdges[i].y + boundaryPoints[i].y * boundaryEdges[i].x
                            + elementPoints[k].x * boundaryEdges[i].y - elementPoints[k].y * boundaryEdges[i].x) / determinant;
            if (lambda[i][k] > 0 && lambda[i][k] < 1 && kappa[i][k] > 0 && kappa[i][k] < 1)
            {
                //0 < lambda < 1 and 0 < kappa < 1 => edges intersect. Intersection point is left hand side (and rhs) of the above equation
                double intersectionPointX = boundaryPoints[i].x + lambda[i][k] * boundaryEdges[i].x;
                double intersectionPointY = boundaryPoints[i].y + lambda[i][k] * boundaryEdges[i].y;
                intersectionPoints.emplace_back(intersectionPointX, intersectionPointY);
            }
        }
    }
    // For each element corner check, if it is inside the object bounding box. If true, it is also part of the intersection polygon.
    for (size_t k = 0; k < 4; k++)
    {
        if ((!parallel[0][k] ? kappa[0][k] * kappa[2][k] < 0 : (1 - kappa[0][(k -1) % 4]) * (1 - kappa[2][(k -1) % 4]) < 0) &&
            (!parallel[1][k] ? kappa[1][k] * kappa[3][k] < 0 : (1 - kappa[1][(k -1) % 4]) * (1 - kappa[3][(k -1) % 4]) < 0))
        {
            intersectionPoints.emplace_back(elementPoints[k]);
        }
    }
    // For each object corner check, if it is inside the element polygon. If true, it is also part of the intersection polygon.
    for (size_t i = 0; i < 4; i++)
    {
        if (World::Localization::PointQuery::IsWithin(elementPoints[1], elementPoints[2], elementPoints[0], elementPoints[3], boundaryPoints[i]))
        {
            intersectionPoints.emplace_back(boundaryPoints[i]);
        }
    }
    return intersectionPoints;
}

std::function<void (const RTreeElement&)>  LocateOnGeometryElement(const OWL::Interfaces::WorldData& worldData,
                                                              const std::vector<Common::Vector2d>& objectBoundary,
                                                              const Common::Vector2d& referencePoint,
                                                              const Common::Vector2d& mainLaneLocator,
                                                              const double& hdg,
                                                              LocatedObject& locatedObject)
{
    return  [&](auto const& value)
    {
        const LocalizationElement& localizationElement = *value.second;

        auto intersection = GetIntersectionPoints(localizationElement.polygon, objectBoundary);

        if(intersection.size() < 3)
        {
            //Actual polygons do not intersect -> Skip this GeometryElement
            return;
        }

        WorldToRoadCoordinateConverter converter(localizationElement);

        const OWL::Interfaces::Lane* lane = localizationElement.lane;
        const auto laneOsiId = lane->GetId();
        const auto laneOdId = worldData.GetLaneIdMapping().at(laneOsiId);
        const auto roadId = lane->GetRoad().GetId();

        if (converter.IsConvertible(referencePoint))
        {
            auto [s, t, yaw] = converter.GetRoadCoordinate(referencePoint, hdg);
            locatedObject.referencePoint[roadId] = GlobalRoadPosition(roadId, laneOdId, s, t, yaw);
        }

        if (converter.IsConvertible(mainLaneLocator))
        {
            auto [s, t, yaw] = converter.GetRoadCoordinate(mainLaneLocator, hdg);
            locatedObject.mainLaneLocator[roadId] = GlobalRoadPosition(roadId, laneOdId, s, t, yaw);
        }

        for (const auto& point : intersection)
        {
             auto [s, t, yaw] = converter.GetRoadCoordinate(point, hdg);
             assert(s >= 0.0);

             auto& intersection = locatedObject.laneOverlaps[lane];
             intersection.s_min = std::min(intersection.s_min, s);
             intersection.s_max = std::max(intersection.s_max, s);

             auto width = lane->GetWidth(s);
             intersection.min_delta_left = std::clamp(width / 2.0 - t, 0.0, intersection.min_delta_left);
             intersection.min_delta_right = std::clamp(width / 2.0 + t, 0.0, intersection.min_delta_right);

        }
    };
}

LocatedObject LocateOnGeometryElements(const bg_rTree& rTree, const OWL::Interfaces::WorldData& worldData, const std::vector<Common::Vector2d>& agentBoundary, CoarseBoundingBox theAgent,
                        const Common::Vector2d referencePoint, const Common::Vector2d mainLaneLocator, double hdg)
{
 LocatedObject locatedObject;
 rTree.query(bgi::intersects(theAgent),
                         boost::make_function_output_iterator(LocateOnGeometryElement(worldData, agentBoundary, referencePoint, mainLaneLocator, hdg, locatedObject)));

 return locatedObject;
}

std::function<void (const RTreeElement&)> LocateOnGeometryElement(const OWL::Interfaces::WorldData& worldData, const Common::Vector2d& point,
                                                                  const double& hdg, std::map<const std::string, GlobalRoadPosition>& result)
{
    return  [&](auto const& value)
    {
        const LocalizationElement& localizationElement = *value.second;

        boost::geometry::de9im::mask mask("**F******"); // within
        if (!boost::geometry::relate(point_t{point.x, point.y}, localizationElement.boost_polygon, mask)) //Check wether point is inside polygon
        {
            return;
        }

        WorldToRoadCoordinateConverter converter(localizationElement);

        const OWL::Interfaces::Lane* lane = localizationElement.lane;
        const auto laneOsiId = lane->GetId();
        const auto laneOdId = worldData.GetLaneIdMapping().at(laneOsiId);
        const auto roadId = lane->GetRoad().GetId();

        if (converter.IsConvertible(point))
        {
            auto [s, t, yaw] = converter.GetRoadCoordinate(point, hdg);
            result[roadId] = GlobalRoadPosition(roadId, laneOdId, s, t, yaw);
        }
    };
}

std::map<const std::string, GlobalRoadPosition> LocateOnGeometryElements(const bg_rTree& rTree, const OWL::Interfaces::WorldData& worldData,
                           const Common::Vector2d point, double hdg)
{
    std::map<const std::string, GlobalRoadPosition> result;
    CoarseBoundingBox box = GetSearchBox({point});
    rTree.query(bgi::intersects(box),
                            boost::make_function_output_iterator(LocateOnGeometryElement(worldData, point, hdg, result)));

    return result;
}

polygon_t GetBoundingBox(double x, double y, double length, double width, double rotation, double center)
{
    double halfWidth = width / 2;

    point_t boxPoints[]
    {
        point_t{center - length, -halfWidth},
        point_t{center - length,  halfWidth},
        point_t{center,           halfWidth},
        point_t{center,          -halfWidth},
        point_t{center - length, -halfWidth}
    };

    polygon_t box;
    polygon_t boxTemp;
    bg::append(box, boxPoints);

    bt::translate_transformer<double, 2, 2> translate(x, y);

    // rotation in mathematical negativ order (boost) -> invert to match
    bt::rotate_transformer<bg::radian, double, 2, 2> rotate(-rotation);

    bg::transform(box, boxTemp, rotate);
    bg::transform(boxTemp, box, translate);

    return box;
}

void CreateLaneAssignments(OWL::Interfaces::WorldObject& object, const std::map<const OWL::Interfaces::Lane*, OWL::LaneOverlap >& laneOverlaps)
{
    for (auto [lane, laneOverlap] : laneOverlaps)
    {
        auto movingObject = dynamic_cast<OWL::MovingObject*>(&object);
        if (movingObject)
        {
            const_cast<OWL::Interfaces::Lane*>(lane)->AddMovingObject(*movingObject, laneOverlap);
            object.AddLaneAssignment(*lane);
            continue;
        }

        auto stationaryObject = dynamic_cast<OWL::StationaryObject*>(&object);
        if (stationaryObject)
        {
            const_cast<OWL::Interfaces::Lane*>(lane)->AddStationaryObject(*stationaryObject, laneOverlap);
            object.AddLaneAssignment(*lane);
            continue;
        }
    }
}

Result Localizer::BuildResult(const LocatedObject& locatedObject) const
{
    std::set<int> touchedLaneIds;
    std::map<std::string, RoadInterval> touchedRoads;

    bool isOnRoute = !locatedObject.referencePoint.empty() && !locatedObject.mainLaneLocator.empty();

    for (const auto& [lane, laneOverlap] : locatedObject.laneOverlaps)
    {
        std::string roadId = lane->GetRoad().GetId();
        touchedRoads[roadId].lanes.push_back(worldData.GetLaneIdMapping().at(lane->GetId()));
        touchedRoads[roadId].sStart = std::min(touchedRoads[roadId].sStart, laneOverlap.s_min);
        touchedRoads[roadId].sEnd = std::max(touchedRoads[roadId].sEnd, laneOverlap.s_max);
        double remainder_left = std::max(touchedRoads[roadId].remainder.left, laneOverlap.min_delta_left);
        double remainder_right = std::max(touchedRoads[roadId].remainder.right, laneOverlap.min_delta_right);
        touchedRoads[roadId].remainder = {remainder_left, remainder_right};
    }

    ObjectPosition position{locatedObject.referencePoint, locatedObject.mainLaneLocator, touchedRoads};

    Result result(position,
                  isOnRoute);

    return result;
}

Localizer::Localizer(const OWL::Interfaces::WorldData& worldData) :
    worldData(worldData)
{
}

void Localizer::Init()
{
    for (const auto& [laneId, lane] : worldData.GetLanes())
    {
        for (const auto& laneGeometryElement : lane->GetLaneGeometryElements())
        {
            elements.emplace_back(*laneGeometryElement);
        }
    }
    for (const auto& element : elements)
    {
        rTree.insert(std::make_pair(element.search_box, &element));
    }
}

Result Localizer::Locate(const polygon_t& boundingBox, OWL::Interfaces::WorldObject& object) const
{
    const auto& referencePointPosition = object.GetReferencePointPosition();
    const auto& orientation = object.GetAbsOrientation();

    std::vector<Common::Vector2d> agentBoundary;
    for (point_t point : boundingBox.outer())
    {
        agentBoundary.emplace_back(bg::get<0>(point), bg::get<1>(point));
    }
    agentBoundary.pop_back();

    CoarseBoundingBox searchBox = GetSearchBox(agentBoundary);

    Common::Vector2d referencePoint{referencePointPosition.x, referencePointPosition.y};
    Common::Vector2d mainLaneLocator{referencePoint};
    if (object.Is<OWL::Interfaces::MovingObject>())
    {
        const auto& movingObject = object.As<OWL::Interfaces::MovingObject>();
        double distanceRefToFront = movingObject->GetDistanceReferencePointToLeadingEdge();
        mainLaneLocator = {referencePoint.x + distanceRefToFront * cos(orientation.yaw), referencePoint.y + distanceRefToFront * sin(orientation.yaw)};
    }

    const auto& locatedObject = LocateOnGeometryElements(rTree,
                                                         worldData,
                                                         agentBoundary,
                                                         searchBox,
                                                         referencePoint,
                                                         mainLaneLocator,
                                                         orientation.yaw);

    auto result = BuildResult(locatedObject);
    if (result.isOnRoute)
    {
        CreateLaneAssignments(object, locatedObject.laneOverlaps);
    }

    return result;
}

std::map<const std::string, GlobalRoadPosition> Localizer::Locate(const Common::Vector2d& point, const double& hdg) const
{
    return LocateOnGeometryElements(rTree,
                                    worldData,
                                    point,
                                    hdg);
}

void Localizer::Unlocate(OWL::Interfaces::WorldObject& object) const
{
    object.ClearLaneAssignments();
}

}
}
