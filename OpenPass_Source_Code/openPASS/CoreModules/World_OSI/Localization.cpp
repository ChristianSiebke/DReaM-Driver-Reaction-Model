/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
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
        const auto roadOsiId = lane->GetRoad().GetId();
        const auto roadOdId = worldData.GetRoadIdMapping().at(roadOsiId);

        if (converter.IsConvertible(referencePoint))
        {
            auto [s, t, yaw] = converter.GetRoadCoordinate(referencePoint, hdg);
            locatedObject.referencePoint[roadOdId] = GlobalRoadPosition(roadOdId, laneOdId, s, t, yaw);
        }

        if (converter.IsConvertible(mainLaneLocator))
        {
            auto [s, t, yaw] = converter.GetRoadCoordinate(mainLaneLocator, hdg);
            locatedObject.mainLaneLocator[roadOdId] = GlobalRoadPosition(roadOdId, laneOdId, s, t, yaw);
        }

        for (const auto& point : intersection)
        {
             auto [s, t, yaw] = converter.GetRoadCoordinate(point, hdg);
             assert(s >= 0.0);

             auto& intersection = locatedObject.laneIntersections[lane];
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

void CreateLaneAssignments(OWL::Interfaces::WorldObject& object, const std::map<const OWL::Interfaces::Lane*, LaneIntersection<double> >& laneIntersections)
{
    for (auto [lane, laneIntersection] : laneIntersections)
    {
        auto movingObject = dynamic_cast<OWL::MovingObject*>(&object);
        if (movingObject)
        {
            const_cast<OWL::Interfaces::Lane*>(lane)->AddMovingObject(*movingObject);
            object.AddLaneAssignment(*lane);
            continue;
        }

        auto stationaryObject = dynamic_cast<OWL::StationaryObject*>(&object);
        if (stationaryObject)
        {
            const_cast<OWL::Interfaces::Lane*>(lane)->AddStationaryObject(*stationaryObject);
            object.AddLaneAssignment(*lane);
            continue;
        }
    }
}

void IncorporateRoadDirection (const Route& route, GlobalRoadPosition& point)
{
    const auto& routeElement = std::find_if(route.roads.cbegin(), route.roads.cend(),
                                        [&](const auto& routeElement){return routeElement.roadId == point.roadId;});
    if (routeElement->inRoadDirection)
    {
        return;
    }
    point.roadPosition.t = -point.roadPosition.t;
    point.roadPosition.hdg = std::fmod(point.roadPosition.hdg + 2 * M_PI, 2 * M_PI) - M_PI;
}

Result Localizer::BuildResult(const LocatedObject& locatedObject, const Route& route) const
{
    std::set<int> touchedLaneIds;
    std::map<std::string, RoadInterval> touchedRoads;

    double remainder_left {0.0};
    double remainder_right {0.0};
    bool isOnRoute{true};

    for (const auto& [lane, laneIntersection] : locatedObject.laneIntersections)
    {
        std::string roadId = worldData.GetRoadIdMapping().at(lane->GetRoad().GetId());
        touchedRoads[roadId].lanes.push_back(worldData.GetLaneIdMapping().at(lane->GetId()));
        touchedRoads[roadId].sStart = std::min(touchedRoads[roadId].sStart, laneIntersection.s_min);
        touchedRoads[roadId].sEnd = std::max(touchedRoads[roadId].sEnd, laneIntersection.s_max);
        if (std::find_if(route.roads.cbegin(), route.roads.cend(),
                         [&](const auto& routeElement){return routeElement.roadId == roadId;}) != route.roads.cend())
        {
            touchedLaneIds.insert(worldData.GetLaneIdMapping().at(lane->GetId()));
            remainder_left = std::max(remainder_left, laneIntersection.min_delta_left);
            remainder_right = std::max(remainder_right, laneIntersection.min_delta_right);
        }
    }

    GlobalRoadPosition mainLaneLocator;
    const auto& mainRoad = std::find_if(route.roads.cbegin(), route.roads.cend(),
                                        [&](const auto& routeElement){return locatedObject.mainLaneLocator.count(routeElement.roadId) > 0;});
    if (mainRoad == route.roads.cend())
    {
        isOnRoute = false;
        //If the mainLaneLocator is not on the route, but on some other road, we take an arbitrary road instead
        if (locatedObject.mainLaneLocator.size() > 0)
        {
            mainLaneLocator = locatedObject.mainLaneLocator.cbegin()->second;
        }
    }
    else
    {
        mainLaneLocator = locatedObject.mainLaneLocator.at(mainRoad->roadId);
        IncorporateRoadDirection(route, mainLaneLocator);
    }

    GlobalRoadPosition referencePoint;
    const auto& referenceRoad = std::find_if(route.roads.cbegin(), route.roads.cend(),
                                        [&](const auto& routeElement){return locatedObject.referencePoint.count(routeElement.roadId) > 0;});

    if (referenceRoad == route.roads.cend())
    {
        //If the referencePoint is not on the route, but on some other road, we take an arbitrary road instead
        if (locatedObject.referencePoint.empty())
        {
            isOnRoute = false;
        }
        else
        {
            referencePoint = locatedObject.referencePoint.cbegin()->second;
        }
    }
    else
    {
        referencePoint = locatedObject.referencePoint.at(referenceRoad->roadId);
        IncorporateRoadDirection(route, referencePoint);
    }

    Remainder remainder{remainder_left, remainder_right};

    ObjectPosition position{referencePoint, mainLaneLocator, touchedRoads};

    Result result(position,
                  remainder,
                  isOnRoute,
                  touchedLaneIds.size() > 1,
                  touchedLaneIds,
                  touchedLaneIds);

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

Result Localizer::Locate(const polygon_t& boundingBox, OWL::Interfaces::WorldObject& object, const Route& route) const
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
    if (object.Is<OWL::MovingObject>())
    {
        const auto& movingObject = object.As<OWL::MovingObject>();
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

    auto result = BuildResult(locatedObject, route);
    CreateLaneAssignments(object, locatedObject.laneIntersections);

    return result;
}

void Localizer::Unlocate(OWL::Interfaces::WorldObject& object) const
{
    object.ClearLaneAssignments();
}

}
}
