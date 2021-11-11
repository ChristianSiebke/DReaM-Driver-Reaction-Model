/********************************************************************************
 * Copyright (c) 2018 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include "OWL/DataTypes.h"

namespace OWL {
namespace Testing {

struct LaneGeometryElementGenerator
{

static Primitive::LaneGeometryElement RectangularLaneGeometryElement(
        Common::Vector2d origin,
        double width,
        double length,
        double hdg = 0.0,
        OWL::Interfaces::Lane* lane = nullptr)
{
    Common::Vector2d current_left {
        origin.x - width/2 * std::sin(hdg),
        origin.y + width/2 * std::cos(hdg)
    };

    Common::Vector2d current_reference = origin;

    Common::Vector2d current_right {
        origin.x + width/2 * std::sin(hdg),
        origin.y - width/2 * std::cos(hdg)
    };

    Common::Vector2d next_left {
        origin.x + length*std::cos(hdg) - width/2 * std::sin(hdg),
        origin.y + length*std::sin(hdg) + width/2 * std::cos(hdg)
    };

    Common::Vector2d next_reference {
        origin.x + length*std::cos(hdg),
        origin.y + length*std::sin(hdg)
    };

    Common::Vector2d next_right {
        origin.x + length*std::cos(hdg) + width/2 * std::sin(hdg),
        origin.y + length*std::sin(hdg) - width/2 * std::cos(hdg)
    };

    OWL::Primitive::LaneGeometryJoint current
    {
        {
            current_left,
            current_reference,
            current_right
        },
        0.0,
        0.0,
        hdg
    };

    OWL::Primitive::LaneGeometryJoint next
    {
        {
            next_left,
            next_reference,
            next_right
        },
        0.0,
        length,
        hdg
    };

    return OWL::Primitive::LaneGeometryElement(current, next, lane);
}

static Primitive::LaneGeometryElement RectangularLaneGeometryElementWithCurvature(
        Common::Vector2d origin,
        double width,
        double length,
        double curvatureStart,
        double curvatureEnd,
        double hdg = 0.0)
{
    Common::Vector2d current_left {
        origin.x - width/2 * std::sin(hdg),
        origin.y + width/2 * std::cos(hdg)
    };

    Common::Vector2d current_reference = origin;

    Common::Vector2d current_right {
        origin.x + width/2 * std::sin(hdg),
        origin.y - width/2 * std::cos(hdg)
    };

    Common::Vector2d next_left {
        origin.x + length*std::cos(hdg) - width/2 * std::sin(hdg),
        origin.y + length*std::sin(hdg) + width/2 * std::cos(hdg)
    };

    Common::Vector2d next_reference {
        origin.x + length*std::cos(hdg),
        origin.y + length*std::sin(hdg)
    };

    Common::Vector2d next_right {
        origin.x + length*std::cos(hdg) + width/2 * std::sin(hdg),
        origin.y + length*std::sin(hdg) - width/2 * std::cos(hdg)
    };

    OWL::Primitive::LaneGeometryJoint current
    {
        {
            current_left,
            current_reference,
            current_right
        },
        curvatureStart,
        0.0,
        hdg
    };

    OWL::Primitive::LaneGeometryJoint next
    {
        {
            next_left,
            next_reference,
            next_right
        },
        curvatureEnd,
        length,
        hdg
    };

    return OWL::Primitive::LaneGeometryElement(current, next, nullptr);
}

static Primitive::LaneGeometryElement TriangularLaneGeometryElement(
        Common::Vector2d origin,
        double width,
        double length,
        double hdg = 0.0)
{
    Common::Vector2d current_left = origin;
    Common::Vector2d current_reference = origin;
    Common::Vector2d current_right = origin;

    Common::Vector2d next_left {
        origin.x + length*std::cos(hdg) - width/2 * std::sin(hdg),
        origin.y + length*std::sin(hdg) + width/2 * std::cos(hdg)
    };

    Common::Vector2d next_reference {
        origin.x + length*std::cos(hdg),
        origin.y + length*std::sin(hdg)
    };

    Common::Vector2d next_right {
        origin.x + length*std::cos(hdg) + width/2 * std::sin(hdg),
        origin.y + length*std::sin(hdg) - width/2 * std::cos(hdg)
    };

    OWL::Primitive::LaneGeometryJoint current
    {
        {
            current_left,
            current_reference,
            current_right
        },
        0.0,
        0.0,
        hdg
    };

    OWL::Primitive::LaneGeometryJoint next
    {
        {
            next_left,
            next_reference,
            next_right
        },
        0.0,
        length,
        hdg
    };

    return OWL::Primitive::LaneGeometryElement(current, next, nullptr);
}

static Primitive::LaneGeometryElement CurvedLaneGeometryElement(
        Common::Vector2d origin,
        double width,
        double length,
        double sDistance,
        double radius)
{
    double openingAngle = length / radius;

    Common::Vector2d current_left {
        origin.x,
        origin.y + width/2
    };

    Common::Vector2d current_reference = origin;

    Common::Vector2d current_right {
        origin.x,
        origin.y - width/2
    };

    Common::Vector2d next_left {
        origin.x + radius * std::sin(openingAngle) - width/2 * std::sin(openingAngle),
        origin.y + radius * (1 - std::cos(openingAngle)) + width/2 * std::cos(openingAngle)
    };

    Common::Vector2d next_reference {
        origin.x + radius * std::sin(openingAngle),
        origin.y + radius * (1 - std::cos(openingAngle))
    };

    Common::Vector2d next_right {
        origin.x + radius * std::sin(openingAngle) + width/2 * std::sin(openingAngle),
        origin.y + radius * (1 - std::cos(openingAngle)) - width/2 * std::cos(openingAngle)
    };

    OWL::Primitive::LaneGeometryJoint current
    {
        {
            current_left,
            current_reference,
            current_right
        },
        1.0 / radius,
        0.0,
        0.0
    };

    OWL::Primitive::LaneGeometryJoint next
    {
        {
            next_left,
            next_reference,
            next_right
        },
        1.0 / radius,
        sDistance,
        0.0
    };

    return OWL::Primitive::LaneGeometryElement(current, next, nullptr);
}

};

} // namespace Testing
} // namespace OW
