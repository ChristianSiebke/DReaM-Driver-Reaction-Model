/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#ifndef OBSERVEDWORLDOBJECT_H
#define OBSERVEDWORLDOBJECT_H

#include "AABBTree/aabbInterface.h"
#include "boost/geometry/geometries/point_xy.hpp"
#include "boost/geometry/geometries/polygon.hpp"
#include "common/vector2d.h"

typedef boost::geometry::model::d2::point_xy<double> BoostVector2d;
typedef boost::geometry::model::polygon<BoostVector2d> Polygon2d;

enum ObservedObjectType { TrafficSign, TrafficLight, Building, MovingObject };

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
    ObservedObjectType objectType;

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
    double IsHitByRay(const Ray &ray)
    {
        double shortest = __DBL_MAX__;
        bool hasHit = false;

        for (unsigned i = 0; i < area.outer().size() - 1; i++)
        {
            const auto &pointA = Common::Vector2d(area.outer()[i].x(), area.outer()[i].y());
            const auto &pointB = Common::Vector2d(area.outer()[i + 1].x(), area.outer()[i + 1].y());

            auto v1 = ray.origin - pointA;
            auto v2 = pointB - pointA;
            auto v3 = Common::Vector2d(-ray.direction.y, ray.direction.x);

            auto dot = v2.Dot(v3);
            if (std::abs(dot) < 0.00001)
                continue;

            auto t1 = v2.Cross(v1) / dot;
            auto t2 = v1.Dot(v3) / dot;

            // normally this would be 0 and 1 respectively but .01 is added/subtracted to account for errors in calculation
            if (t1 >= -0.01 && (t2 >= -0.01 && t2 <= 1.01))
            {
                hasHit = true;
                if (t1 < shortest)
                    shortest = t1;
            }
        }

        return hasHit ? shortest : -1.0;
    }

private:
    AABB aabb;
};

#endif // OBSERVEDWORLDOBJECT_H
