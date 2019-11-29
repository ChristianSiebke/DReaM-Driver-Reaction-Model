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

#include "OWL/DataTypes.h"
#include "Common/vector2d.h"

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
using DiscretePoint = bg::model::point<int, 2, bg::cs::cartesian>;
using CoarseBoundingBox = bg::model::box<DiscretePoint>;

namespace World {
namespace Localization {

//! Returns a discrete search box for the rTree query in millimeters
static CoarseBoundingBox GetSearchBox(std::vector<Common::Vector2d> corners)
{
    auto x_min = std::numeric_limits<int>::max();
    auto x_max = std::numeric_limits<int>::min();
    auto y_min = std::numeric_limits<int>::max();
    auto y_max = std::numeric_limits<int>::min();

    //searchBox is in millimeters
    for (const auto& point: corners)
    {
        x_min = std::min(x_min, static_cast<int>(std::round(point.x * 1000)));
        x_max = std::max(x_max, static_cast<int>(std::round(point.x * 1000)));
        y_min = std::min(y_min, static_cast<int>(std::round(point.y * 1000)));
        y_max = std::max(y_max, static_cast<int>(std::round(point.y * 1000)));
    }
    return CoarseBoundingBox{DiscretePoint{x_min, y_min}, DiscretePoint{x_max, y_max}};
}

//! This struct contains all the information about a LaneGeometryElement that is needed by the Localizer
struct LocalizationElement
{
    LocalizationElement(const OWL::Primitive::LaneGeometryElement& element) :
        laneGeometryElement{element},
        lane{element.lane},
        polygon{element.joints.current.points.left, element.joints.next.points.left, element.joints.next.points.right, element.joints.current.points.right},
        search_box {GetSearchBox(polygon)},
        referenceVector{element.joints.next.points.reference - element.joints.current.points.reference},
        referenceScale{(element.joints.next.sOffset - element.joints.current.sOffset) / referenceVector.Length()},
        tAxisCenter{CommonHelper::CalculateIntersection(element.joints.current.points.left, element.joints.current.points.right - element.joints.current.points.left,
                                                        element.joints.next.points.left, element.joints.next.points.right - element.joints.next.points.left)}
    {
    }

    LocalizationElement(const LocalizationElement& other) :
        laneGeometryElement(other.laneGeometryElement),
        lane(other.lane),
        polygon(other.polygon),
        search_box(other.search_box),
        referenceVector(other.referenceVector),
        referenceScale(other.referenceScale),
        tAxisCenter(other.tAxisCenter)
    {}

    const OWL::Primitive::LaneGeometryElement& laneGeometryElement;
    const OWL::Interfaces::Lane* lane;
    std::vector<Common::Vector2d> polygon;
    const CoarseBoundingBox search_box; //!< Box for r-tree in millimeters
    Common::Vector2d referenceVector; //!< Vector from reference point of current joint to reference point of next joint
    double referenceScale; //!< Factor between the actual length of the referenceVector and the s coordinate distance
    std::optional<Common::Vector2d> tAxisCenter; //!< Intersection point of all t-axis or nullopt if t-axis are paralel
};

} // namespace Localization
} // namespace World
