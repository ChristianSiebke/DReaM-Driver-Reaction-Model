/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#pragma once

#include <boost/geometry/algorithms/intersection.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <iostream>

#include "AABBTree/aabbInterface.h"
#include "boost/geometry/geometries/point_xy.hpp"
#include "boost/geometry/geometries/polygon.hpp"
#include "common/vector2d.h"

typedef boost::geometry::model::d2::point_xy<double> BoostVector2d;
typedef boost::geometry::model::segment<BoostVector2d> Segment;
typedef boost::geometry::model::polygon<BoostVector2d> Polygon2d;
typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> Point2d;

enum class ObservedObjectType { None = 0, TrafficSign, TrafficLight, Building, MovingObject };

///
/// \brief The ObservedWorldObject struct
///
struct ObservedWorldObject : AABBInterface
{
    ///
    /// \brief Original Id of the object this ObservedWorldObject represents.
    ///
    int id;

    ///
    /// \brief The reference position of this object in the world.
    ///
    Common::Vector2d referencePosition;

    ///
    /// \brief Polygon representing the area of this object in 2D space.
    ///
    Polygon2d area;

    ///
    /// \brief The type of object (needs to be set!).
    ///
    ObservedObjectType objectType = ObservedObjectType::None;

    ///
    /// \brief Implementation of the interface, returns the current AABB for this object.
    /// \return The AABB
    ///
    AABB GetAABB() const override
    {
        return aabb;
    }

    // Override for the interface
    ~ObservedWorldObject() override = default;

public:
    ///
    /// \brief Recalculates the AABB by finding the min/max x and y of the stored polygon (area).
    ///
    void RecalculateAABB()
    {
        double minX = __DBL_MAX__;
        double minY = __DBL_MAX__;
        double maxX = -__DBL_MAX__;
        double maxY = -__DBL_MAX__;

        for (unsigned i = 0; i < area.outer().size() - 1; i++)
        {
            const auto &point = area.outer().at(i);
            if (point.x() > maxX)
                maxX = point.x();
            if (point.x() < minX)
                minX = point.x();
            if (point.y() > maxY)
                maxY = point.y();
            if (point.y() < minY)
                minY = point.y();
        }

        aabb = AABB(minX, minY, maxX, maxY);
    }

    ///
    /// \brief Checks if this object is hit by a given ray
    /// \param The ray to check against
    /// \return The distance to the hit point or a negative number if there is no hit.
    ///
    double IsHitByRay(const Ray &ray) {
        double length = std::numeric_limits<double>::max();
        bool hit = false;
        for (unsigned i = 0; i < area.outer().size() - 1; i++) {
            Segment edge({area.outer()[i].x(), area.outer()[i].y()}, {area.outer()[i + 1].x(), area.outer()[i + 1].y()});
            Segment raySegment({ray.origin.x, ray.origin.y}, {ray.direction.x * 1000, ray.direction.y * 1000});

            std::vector<Point2d> output;
            boost::geometry::intersection(edge, raySegment, output);

            if (!output.empty()) {
                for (auto &point : output) {
                    Common::Vector2d pV = {boost::geometry::get<0>(point), boost::geometry::get<1>(point)};
                    auto dV = pV - ray.origin;
                    double newlength = dV.Length();
                    if (length > newlength) {
                        hit = true;
                        length = newlength;
                    }
                }
            }
        }
        return hit ? length : -1.0;
    }

private:
    AABB aabb;
};
