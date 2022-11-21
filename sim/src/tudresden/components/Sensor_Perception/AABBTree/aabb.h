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

#include "ray.h"
#include <algorithm>
#include <cmath>

struct AABB {

  public:
    AABB() : minX(0.0), minY(0.0f), maxX(0.0f), maxY(0.0f), surfaceArea(0.0f) {}
    AABB(double minX, double minY, double maxX, double maxY) : minX(minX), minY(minY), maxX(maxX), maxY(maxY) {
        surfaceArea = CalculateSurfaceArea();
    }

    bool Contains(const AABB& other) const { return other.minX >= minX && other.maxX < maxX && other.minY >= minY && other.maxY <= maxY; }

    bool Overlaps(const AABB& other) const { return maxX > other.minX && minX < other.maxX && maxY > other.minY && minY < other.maxY; }

    // This is a simple implementation and could be improved using Tavians Branchless Ray/Bounding Box Intersections
    // (https://tavianator.com/2015/ray_box_nan.html). Since I do not want to rely on correct implementation of IEEE 754 I used the basic
    // implementation which is slower but should suffice for current workloads. -VA, Mar 2020
    bool HitsBox(const Ray& ray, double& distance) const {
        double tmin = -INFINITY;
        double tmax = INFINITY;

        if (ray.direction.x != 0.0) {
            double tx1 = (minX - ray.origin.x) / ray.direction.x;
            double tx2 = (maxX - ray.origin.x) / ray.direction.x;

            tmin = std::max(tmin, std::min(tx1, tx2));
            tmax = std::min(tmax, std::max(tx1, tx2));
        }

        if (ray.direction.y != 0.0) {
            double ty1 = (minY - ray.origin.y) / ray.direction.y;
            double ty2 = (maxY - ray.origin.y) / ray.direction.y;

            tmin = std::max(tmin, std::min(ty1, ty2));
            tmax = std::min(tmax, std::max(ty1, ty2));
        }

        distance = tmin;
        return tmax >= tmin;
    }

    AABB Merge(const AABB& other) const {
        return AABB(std::min(minX, other.minX), std::min(minY, other.minY), std::max(maxX, other.maxX), std::max(maxY, other.maxY));
    }

    AABB Intersection(const AABB& other) const {
        return AABB(std::max(minX, other.minX), std::max(minY, other.minY), std::min(maxX, other.maxX), std::min(maxY, other.maxY));
    }

    double GetWidth() const { return maxX - minX; }

    double GetHeight() const { return maxY - minY; }

  public:
    double minX;
    double minY;
    double maxX;
    double maxY;
    double surfaceArea;

  private:
    double CalculateSurfaceArea() const { return GetWidth() * GetHeight(); }
};
