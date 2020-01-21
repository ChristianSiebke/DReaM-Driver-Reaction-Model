/*******************************************************************************
 * Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
 *               2016, 2017, 2018 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *******************************************************************************/

#include "road.h"
#include "importerLoggingHelper.h"

extern "C"
{
    extern int fresnl(double xxa, double *ssa, double *cca);
}

namespace
{
const double SQRT_PI_2 = std::sqrt(M_PI_2);

} // namespace

RoadLane::~RoadLane()
{
    for (RoadLaneWidth *item : widths)
    {
        delete item;
    }

    for (auto roadMark : roadMarks)
    {
        delete roadMark;
    }
}

bool RoadLane::AddWidth(double sOffset, double a, double b, double c, double d)
{
    RoadLaneWidth *laneWidth = new (std::nothrow) RoadLaneWidth(sOffset, a, b, c, d);
    if (!laneWidth)
    {
        return false;
    }

    widths.push_back(laneWidth);

    return true;
}

bool RoadLane::AddSuccessor(int id)
{
    ThrowIfFalse(successor.empty(), "added more than one successor to road lane.");
    successor.push_back(id);

    return true;
}

bool RoadLane::AddPredecessor(int id)
{
    ThrowIfFalse(predecessor.empty(), "added more than one predecessor to road lane.");
    predecessor.push_back(id);

    return true;
}

bool RoadLane::AddRoadMark(double sOffset, RoadLaneRoadDescriptionType type, RoadLaneRoadMarkType roadMark,
                           RoadLaneRoadMarkColor color, RoadLaneRoadMarkLaneChange laneChange,
                           RoadLaneRoadMarkWeight weight)
{
    RoadLaneRoadMark *laneRoadMark =
        new (std::nothrow) RoadLaneRoadMark(sOffset, type, roadMark, color, laneChange, weight);
    if (!laneRoadMark)
    {
        return false;
    }

    for (auto roadMark : roadMarks)
    {
        roadMark->LimitSEnd(sOffset);
    }
    roadMarks.push_back(laneRoadMark);

    return true;
}

RoadLaneSection::~RoadLaneSection()
{
    for (auto &item : lanes)
    {
        delete item.second;
    }
}

RoadLane *RoadLaneSection::AddRoadLane(int id, RoadLaneType type)
{
    RoadLane *lane = new (std::nothrow) RoadLane(this, id, type);

    if (!lanes.insert({id, lane}).second)
    {
        delete lane;
        return nullptr;
    }

    return lane;
}

Common::Vector2d RoadGeometry::GetCoordLine(double sOffset, double tOffset) const
{
    if (length < sOffset)
    {
        LOG_INTERN(LogLevel::Warning) << "exceeding length of geometry";
        sOffset = length;
    }

    // unrotated road initially pointing to east
    Common::Vector2d offset(sOffset, tOffset);

    offset.Rotate(hdg);

    offset.x += x;
    offset.y += y;

    return offset;
}

double RoadGeometry::GetDirLine(double sOffset) const
{
    Q_UNUSED(sOffset);
    return hdg;
}

Common::Vector2d RoadGeometry::GetCoordArc(double sOffset, double tOffset, double curvature) const
{
    if (length < sOffset)
    {
        LOG_INTERN(LogLevel::Warning) << "exceeding length of geometry";
        sOffset = length;
    }

    double radius = 1 / curvature;
    double circumference = 2 * M_PI / curvature;

    // account for sOffset beyond circumference
    // fractionRad = fractionCircumference * 2 * PI / circumference
    double fractionRad = fmod(sOffset, circumference) * curvature;

    // shift by radius to rotate around center at origin
    Common::Vector2d offset(0, -radius + tOffset);
    offset.Rotate(fractionRad);

    // shift back
    offset.y += radius;

    offset.Rotate(hdg);

    offset.x += x;
    offset.y += y;

    return offset;
}

double RoadGeometry::GetDirArc(double sOffset, double curvature) const
{
    double circumference = 2 * M_PI / curvature;

    // account for sOffset beyond circumference
    // fractionRad = fractionCircumference * 2 * PI / circumference
    double fractionRad = fmod(sOffset, circumference) * curvature;

    return hdg + fractionRad;
}

Common::Vector2d RoadGeometryLine::GetCoord(double sOffset, double tOffset) const
{
    return GetCoordLine(sOffset, tOffset);
}

double RoadGeometryLine::GetDir(double sOffset) const
{
    return GetDirLine(sOffset);
}

Common::Vector2d RoadGeometryArc::GetCoord(double sOffset, double tOffset) const
{
    if (0.0 == curvature)
    {
        return GetCoordLine(sOffset, tOffset);
    }

    return GetCoordArc(sOffset, tOffset, curvature);
}

double RoadGeometryArc::GetDir(double sOffset) const
{
    if (0.0 == curvature)
    {
        return GetDirLine(sOffset);
    }

    return GetDirArc(sOffset, curvature);
}

Common::Vector2d RoadGeometrySpiral::HalfCoord(double sOffset, double tOffset) const
{
    double _curvStart = curvStart;
    double _curvEnd = curvEnd;

    assert(_curvStart != _curvEnd);
    assert((0.0 <= _curvStart && 0.0 <= _curvEnd) || (0.0 >= _curvStart && 0.0 >= _curvEnd));

    if (length < sOffset)
    {
        LOG_INTERN(LogLevel::Warning) << "exceeding length of geometry";
        sOffset = length;
    }

    if (0.0 <= _curvStart && 0.0 <= _curvEnd)
    {
        if (_curvStart < _curvEnd)
        {
            assert(0.0 != _curvEnd);

            double radiusEnd = 1.0 / _curvEnd;

            // 1. equation (definition of clothoid): const = radiusStart * distanceStart = radiusEnd * distanceEnd
            // 2. equation: length = distanceEnd - distanceStart
            // -> distanceEnd = radiusStart * length / (radiusStart - radiusEnd)
            // -> formed such that equation copes with _curvStart==0 (infinite radiusStart)
            double distanceEnd = length / (1 - radiusEnd * _curvStart);
            assert(length <= distanceEnd);

            double distanceStart = distanceEnd - length;
            double a = std::sqrt(2 * radiusEnd * distanceEnd);

            Common::Vector2d start;
            (void)fresnl(distanceStart / a / SQRT_PI_2, &start.y, &start.x);
            start.Scale(a * SQRT_PI_2);

            double distanceOffset = distanceStart + sOffset;
            Common::Vector2d offset;
            (void)fresnl(distanceOffset / a / SQRT_PI_2, &offset.y, &offset.x);
            offset.Scale(a * SQRT_PI_2);
            offset.Sub(start);

            double tangentAngle = distanceOffset * distanceOffset / a / a;
            if (0 > _curvEnd)
            {
                tangentAngle = -tangentAngle;
            }

            double normAngle = tangentAngle + M_PI_2;
            normAngle = std::fmod(normAngle, 2 * M_PI);

            // get perpendicular vector
            Common::Vector2d norm(1, 0);
            norm.Rotate(normAngle);
            norm.Scale(tOffset);

            offset.Add(norm);
            offset.Rotate(hdg);

            return Common::Vector2d(x + offset.x, y + offset.y);
        }
        else // _curvStart > _curvEnd ("curStart != _curvEnd" guaranteed by checks in caller)
        {
            std::swap(_curvStart, _curvEnd);
            sOffset = length - sOffset;

            assert(0.0 != _curvEnd);

            double radiusEnd = 1.0 / _curvEnd;

            // 1. equation (definition of clothoid): const = radiusStart * distanceStart = radiusEnd * distanceEnd
            // 2. equation: length = distanceEnd - distanceStart
            // -> distanceEnd = radiusStart * length / (radiusStart - radiusEnd)
            // -> formed such that equation copes with _curvStart==0 (infinite radiusStart)
            double distanceEnd = length / (1 - radiusEnd * _curvStart);
            assert(length <= distanceEnd);

            double distanceStart = distanceEnd - length;
            double a = std::sqrt(2 * radiusEnd * distanceEnd);

            Common::Vector2d start;
            (void)fresnl(distanceStart / a / SQRT_PI_2, &start.y, &start.x);
            start.Scale(a * SQRT_PI_2);

            double distanceOffset = distanceStart + sOffset;
            Common::Vector2d offset;
            (void)fresnl(distanceOffset / a / SQRT_PI_2, &offset.y, &offset.x);
            offset.Scale(a * SQRT_PI_2);
            offset.Sub(start);

            double tangentAngle = distanceOffset * distanceOffset / a / a;
            if (0 > _curvEnd)
            {
                tangentAngle = -tangentAngle;
            }

            double normAngle = tangentAngle + M_PI_2;
            normAngle = std::fmod(normAngle, 2 * M_PI);

            // get perpendicular vector
            Common::Vector2d norm(1, 0);
            norm.Rotate(normAngle);
            norm.Scale(tOffset);

            offset.Add(norm);

            // calculate end point
            Common::Vector2d endOffset;
            (void)fresnl(distanceEnd / a / SQRT_PI_2, &endOffset.y, &endOffset.x);
            endOffset.Scale(a * SQRT_PI_2);
            endOffset.Sub(start);

            // compensate for inverted curvatures
            double tangentAngleEnd = distanceEnd * distanceEnd / a / a;
            if (0 > _curvEnd)
            {
                tangentAngleEnd = -tangentAngleEnd;
            }
            tangentAngleEnd = -tangentAngleEnd + M_PI;

            offset.Sub(endOffset);
            offset.y = -offset.y;
            offset.Rotate(hdg - tangentAngleEnd);

            return Common::Vector2d(x + offset.x, y + offset.y);
        }
    }
    else // 0.0 >= _curvStart && 0.0 >= _curvEnd
    {
        _curvStart = -_curvStart;
        _curvEnd = -_curvEnd;

        if (_curvStart < _curvEnd)
        {
            assert(0.0 != _curvEnd);

            double radiusEnd = 1.0 / _curvEnd;

            // 1. equation (definition of clothoid): const = radiusStart * distanceStart = radiusEnd * distanceEnd
            // 2. equation: length = distanceEnd - distanceStart
            // -> distanceEnd = radiusStart * length / (radiusStart - radiusEnd)
            // -> formed such that equation copes with _curvStart==0 (infinite radiusStart)
            double distanceEnd = length / (1 - radiusEnd * _curvStart);
            assert(length <= distanceEnd);

            double distanceStart = distanceEnd - length;
            double a = std::sqrt(2 * radiusEnd * distanceEnd);

            Common::Vector2d start;
            (void)fresnl(distanceStart / a / SQRT_PI_2, &start.y, &start.x);
            start.Scale(a * SQRT_PI_2);

            double distanceOffset = distanceStart + sOffset;
            Common::Vector2d offset;
            (void)fresnl(distanceOffset / a / SQRT_PI_2, &offset.y, &offset.x);
            offset.Scale(a * SQRT_PI_2);
            offset.Sub(start);

            double tangentAngle = distanceOffset * distanceOffset / a / a;
            if (0 > _curvEnd)
            {
                tangentAngle = -tangentAngle;
            }

            double normAngle = tangentAngle + M_PI_2;
            normAngle = std::fmod(normAngle, 2 * M_PI);

            // get perpendicular vector
            Common::Vector2d norm(-1, 0);
            norm.Rotate(normAngle);
            norm.Scale(tOffset);

            offset.Add(norm);
            offset.y = -offset.y;
            offset.Rotate(hdg);

            return Common::Vector2d(x + offset.x, y + offset.y);
        }
        else // _curvStart > _curvEnd ("curStart != _curvEnd" guaranteed by checks in caller)
        {
            std::swap(_curvStart, _curvEnd);
            sOffset = length - sOffset;

            assert(0.0 != _curvEnd);

            double radiusEnd = 1.0 / _curvEnd;

            // 1. equation (definition of clothoid): const = radiusStart * distanceStart = radiusEnd * distanceEnd
            // 2. equation: length = distanceEnd - distanceStart
            // -> distanceEnd = radiusStart * length / (radiusStart - radiusEnd)
            // -> formed such that equation copes with _curvStart==0 (infinite radiusStart)
            double distanceEnd = length / (1 - radiusEnd * _curvStart);
            assert(length <= distanceEnd);

            double distanceStart = distanceEnd - length;
            double a = std::sqrt(2 * radiusEnd * distanceEnd);

            Common::Vector2d start;
            (void)fresnl(distanceStart / a / SQRT_PI_2, &start.y, &start.x);
            start.Scale(a * SQRT_PI_2);

            double distanceOffset = distanceStart + sOffset;
            Common::Vector2d offset;
            (void)fresnl(distanceOffset / a / SQRT_PI_2, &offset.y, &offset.x);
            offset.Scale(a * SQRT_PI_2);
            offset.Sub(start);

            double tangentAngle = distanceOffset * distanceOffset / a / a;
            if (0 > _curvEnd)
            {
                tangentAngle = -tangentAngle;
            }

            double normAngle = tangentAngle + M_PI_2;
            normAngle = std::fmod(normAngle, 2 * M_PI);

            // get perpendicular vector
            Common::Vector2d norm(-1, 0);
            norm.Rotate(normAngle);
            norm.Scale(tOffset);

            offset.Add(norm);

            // calculate end point
            Common::Vector2d endOffset;
            (void)fresnl(distanceEnd / a / SQRT_PI_2, &endOffset.y, &endOffset.x);
            endOffset.Scale(a * SQRT_PI_2);
            endOffset.Sub(start);

            // compensate for inverted curvatures
            double tangentAngleEnd = distanceEnd * distanceEnd / a / a;
            if (0 > _curvEnd)
            {
                tangentAngleEnd = -tangentAngleEnd;
            }
            tangentAngleEnd = tangentAngleEnd - M_PI;

            offset.Sub(endOffset);
            offset.Rotate(hdg - tangentAngleEnd);

            return Common::Vector2d(x + offset.x, y + offset.y);
        }
    }
}

Common::Vector2d RoadGeometrySpiral::FullCoord(double sOffset, double tOffset) const
{
    if ((0.0 <= curvStart && 0.0 <= curvEnd) || (0.0 >= curvStart && 0.0 >= curvEnd))
    {
        return HalfCoord(sOffset, tOffset);
    }

    assert((0.0 > curvStart && 0.0 < curvEnd) || (0.0 < curvStart && 0.0 > curvEnd));

    // one degree of freedom: start position/end position can not be determined
    LOG_INTERN(LogLevel::Warning) << "could not calculate spiral coordinate";

    return Common::Vector2d();
}

double RoadGeometrySpiral::HalfCurvature(double sOffset) const
{
    double _curvStart = curvStart;
    double _curvEnd = curvEnd;

    assert(_curvStart != _curvEnd);
    assert((0.0 <= _curvStart && 0.0 <= _curvEnd) || (0.0 >= _curvStart && 0.0 >= _curvEnd));

    if (length < sOffset)
    {
        LOG_INTERN(LogLevel::Warning) << "exceeding length of geometry";
        sOffset = length;
    }

    if (0.0 <= _curvStart && 0.0 <= _curvEnd)
    {
        if (_curvStart < _curvEnd)
        {
            assert(0.0 != _curvEnd);

            double radiusEnd = 1.0 / _curvEnd;

            // 1. equation (definition of clothoid): const = radiusStart * distanceStart = radiusEnd * distanceEnd
            // 2. equation: length = distanceEnd - distanceStart
            // -> distanceEnd = radiusStart * length / (radiusStart - radiusEnd)
            // -> formed such that equation copes with _curvStart==0 (infinite radiusStart)
            double distanceEnd = length / (1 - radiusEnd * _curvStart);
            assert(length <= distanceEnd);

            double distanceStart = distanceEnd - length;
            double distanceOffset = distanceStart + sOffset;

            // equation const = radiusEnd * distanceEnd = radiusOffset * distanceOffset
            // -> curvatureOffset = 1 / radiusOffset = distanceOffset / (radiusEnd * distanceEnd)
            return distanceOffset / radiusEnd / distanceEnd;
        }
        else // _curvStart > _curvEnd ("curStart != _curvEnd" guaranteed by checks in caller)
        {
            std::swap(_curvStart, _curvEnd);
            sOffset = length - sOffset;

            assert(0.0 != _curvEnd);

            double radiusEnd = 1.0 / _curvEnd;

            // 1. equation (definition of clothoid): const = radiusStart * distanceStart = radiusEnd * distanceEnd
            // 2. equation: length = distanceEnd - distanceStart
            // -> distanceEnd = radiusStart * length / (radiusStart - radiusEnd)
            // -> formed such that equation copes with _curvStart==0 (infinite radiusStart)
            double distanceEnd = length / (1 - radiusEnd * _curvStart);
            assert(length <= distanceEnd);

            double distanceStart = distanceEnd - length;
            double distanceOffset = distanceStart + sOffset;

            // equation const = radiusEnd * distanceEnd = radiusOffset * distanceOffset
            // -> curvatureOffset = 1 / radiusOffset = distanceOffset / (radiusEnd * distanceEnd)
            return distanceOffset / radiusEnd / distanceEnd;
        }
    }
    else // 0.0 >= _curvStart && 0.0 >= _curvEnd
    {
        _curvStart = -_curvStart;
        _curvEnd = -_curvEnd;

        if (_curvStart < _curvEnd)
        {
            assert(0.0 != _curvEnd);

            double radiusEnd = 1.0 / _curvEnd;

            // 1. equation (definition of clothoid): const = radiusStart * distanceStart = radiusEnd * distanceEnd
            // 2. equation: length = distanceEnd - distanceStart
            // -> distanceEnd = radiusStart * length / (radiusStart - radiusEnd)
            // -> formed such that equation copes with _curvStart==0 (infinite radiusStart)
            double distanceEnd = length / (1 - radiusEnd * _curvStart);
            assert(length <= distanceEnd);

            double distanceStart = distanceEnd - length;
            double distanceOffset = distanceStart + sOffset;

            // equation const = radiusEnd * distanceEnd = radiusOffset * distanceOffset
            // -> curvatureOffset = 1 / radiusOffset = distanceOffset / (radiusEnd * distanceEnd)
            return -distanceOffset / radiusEnd / distanceEnd;
        }
        else // _curvStart > _curvEnd ("curStart != _curvEnd" guaranteed by checks in caller)
        {
            std::swap(_curvStart, _curvEnd);
            sOffset = length - sOffset;

            assert(0.0 != _curvEnd);

            double radiusEnd = 1.0 / _curvEnd;

            // 1. equation (definition of clothoid): const = radiusStart * distanceStart = radiusEnd * distanceEnd
            // 2. equation: length = distanceEnd - distanceStart
            // -> distanceEnd = radiusStart * length / (radiusStart - radiusEnd)
            // -> formed such that equation copes with _curvStart==0 (infinite radiusStart)
            double distanceEnd = length / (1 - radiusEnd * _curvStart);
            assert(length <= distanceEnd);

            double distanceStart = distanceEnd - length;
            double distanceOffset = distanceStart + sOffset;

            // equation const = radiusEnd * distanceEnd = radiusOffset * distanceOffset
            // -> curvatureOffset = 1 / radiusOffset = distanceOffset / (radiusEnd * distanceEnd)
            return -distanceOffset / radiusEnd / distanceEnd;
        }
    }
}

double RoadGeometrySpiral::FullCurvature(double sOffset) const
{
    if ((0.0 <= curvStart && 0.0 <= curvEnd) || (0.0 >= curvStart && 0.0 >= curvEnd))
    {
        return HalfCurvature(sOffset);
    }

    assert((0.0 > curvStart && 0.0 < curvEnd) || (0.0 < curvStart && 0.0 > curvEnd));

    // one degree of freedom: start position/end position can not be determined
    LOG_INTERN(LogLevel::Warning) << "could not calculate spiral curvature";

    return 0.0;
}

double RoadGeometrySpiral::HalfDir(double sOffset) const
{
    double _curvStart = curvStart;
    double _curvEnd = curvEnd;

    assert(_curvStart != _curvEnd);
    assert((0.0 <= _curvStart && 0.0 <= _curvEnd) || (0.0 >= _curvStart && 0.0 >= _curvEnd));

    if (length < sOffset)
    {
        LOG_INTERN(LogLevel::Warning) << "exceeding length of geometry";
        sOffset = length;
    }

    if (0.0 <= _curvStart && 0.0 <= _curvEnd)
    {
        if (_curvStart < _curvEnd)
        {
            assert(0.0 != _curvEnd);

            double radiusEnd = 1.0 / _curvEnd;

            // 1. equation (definition of clothoid): const = radiusStart * distanceStart = radiusEnd * distanceEnd
            // 2. equation: length = distanceEnd - distanceStart
            // -> distanceEnd = radiusStart * length / (radiusStart - radiusEnd)
            // -> formed such that equation copes with _curvStart==0 (infinite radiusStart)
            double distanceEnd = length / (1 - radiusEnd * _curvStart);
            assert(length <= distanceEnd);

            double distanceStart = distanceEnd - length;
            double a = std::sqrt(2 * radiusEnd * distanceEnd);

            double distanceOffset = distanceStart + sOffset;
            double tangentAngle = distanceOffset * distanceOffset / a / a;

            return hdg + tangentAngle;
        }
        else // _curvStart > _curvEnd ("curStart != _curvEnd" guaranteed by checks in caller)
        {
            std::swap(_curvStart, _curvEnd);
            sOffset = length - sOffset;

            assert(0.0 != _curvEnd);

            double radiusEnd = 1.0 / _curvEnd;

            // 1. equation (definition of clothoid): const = radiusStart * distanceStart = radiusEnd * distanceEnd
            // 2. equation: length = distanceEnd - distanceStart
            // -> distanceEnd = radiusStart * length / (radiusStart - radiusEnd)
            // -> formed such that equation copes with _curvStart==0 (infinite radiusStart)
            double distanceEnd = length / (1 - radiusEnd * _curvStart);
            assert(length <= distanceEnd);

            double distanceStart = distanceEnd - length;
            double a = std::sqrt(2 * radiusEnd * distanceEnd);

            double distanceOffset = distanceStart + sOffset;

            double tangentAngle = distanceOffset * distanceOffset / a / a;

            // compensate for inverted curvatures
            double tangentAngleEnd = distanceEnd * distanceEnd / a / a;

            return hdg + tangentAngleEnd - tangentAngle;
        }
    }
    else // 0.0 >= _curvStart && 0.0 >= _curvEnd
    {
        _curvStart = -_curvStart;
        _curvEnd = -_curvEnd;

        if (_curvStart < _curvEnd)
        {
            assert(0.0 != _curvEnd);

            double radiusEnd = 1.0 / _curvEnd;

            // 1. equation (definition of clothoid): const = radiusStart * distanceStart = radiusEnd * distanceEnd
            // 2. equation: length = distanceEnd - distanceStart
            // -> distanceEnd = radiusStart * length / (radiusStart - radiusEnd)
            // -> formed such that equation copes with _curvStart==0 (infinite radiusStart)
            double distanceEnd = length / (1 - radiusEnd * _curvStart);
            assert(length <= distanceEnd);

            double distanceStart = distanceEnd - length;
            double a = std::sqrt(2 * radiusEnd * distanceEnd);

            double distanceOffset = distanceStart + sOffset;

            double tangentAngle = distanceOffset * distanceOffset / a / a;

            return hdg - tangentAngle;
        }
        else // _curvStart > _curvEnd ("curStart != _curvEnd" guaranteed by checks in caller)
        {
            std::swap(_curvStart, _curvEnd);
            sOffset = length - sOffset;

            assert(0.0 != _curvEnd);

            double radiusEnd = 1.0 / _curvEnd;

            // 1. equation (definition of clothoid): const = radiusStart * distanceStart = radiusEnd * distanceEnd
            // 2. equation: length = distanceEnd - distanceStart
            // -> distanceEnd = radiusStart * length / (radiusStart - radiusEnd)
            // -> formed such that equation copes with _curvStart==0 (infinite radiusStart)
            double distanceEnd = length / (1 - radiusEnd * _curvStart);
            assert(length <= distanceEnd);

            double distanceStart = distanceEnd - length;
            double a = std::sqrt(2 * radiusEnd * distanceEnd);

            double distanceOffset = distanceStart + sOffset;

            double tangentAngle = distanceOffset * distanceOffset / a / a;

            // compensate for inverted curvatures
            double tangentAngleEnd = distanceEnd * distanceEnd / a / a;

            return hdg - (tangentAngleEnd - tangentAngle);
        }
    }
}

double RoadGeometrySpiral::FullDir(double sOffset) const
{
    if ((0.0 <= curvStart && 0.0 <= curvEnd) || (0.0 >= curvStart && 0.0 >= curvEnd))
    {
        return HalfDir(sOffset);
    }

    assert((0.0 > curvStart && 0.0 < curvEnd) || (0.0 < curvStart && 0.0 > curvEnd));

    // one degree of freedom: start position/end position can not be determined
    LOG_INTERN(LogLevel::Warning) << "could not calculate spiral curvature";

    return 0.0;
}

Common::Vector2d RoadGeometrySpiral::GetCoord(double sOffset, double tOffset) const
{
    if (0.0 == curvStart && 0.0 == curvEnd)
    {
        return GetCoordLine(sOffset, tOffset);
    }

    if (std::abs(curvStart - curvEnd) < 1e-6 /* assumed to be equal */)
    {
        return GetCoordArc(sOffset, tOffset, curvStart);
    }

    return FullCoord(sOffset, tOffset);
}

double RoadGeometrySpiral::GetDir(double sOffset) const
{
    if (0.0 == curvStart && 0.0 == curvEnd)
    {
        return GetDirLine(sOffset);
    }

    if (std::abs(curvStart - curvEnd) < 1e-6 /* assumed to be equal */)
    {
        return GetDirArc(sOffset, curvStart);
    }

    return FullDir(sOffset);
}

Common::Vector2d RoadGeometryPoly3::GetCoord(double sOffset, double tOffset) const
{
    if (0.0 == a && 0.0 == b && 0.0 == c && 0.0 == d)
    {
        return GetCoordLine(sOffset, tOffset);
    }

    double s = 0.0;
    Common::Vector2d lastPos;
    Common::Vector2d delta;
    Common::Vector2d pos(0.0, a);

    while (s < sOffset)
    {
        lastPos = pos;
        pos.x += 1.0;
        pos.y = a + b * pos.x + c * pos.x * pos.x + d * pos.x * pos.x * pos.x;

        delta = pos - lastPos;
        double deltaLength = delta.Length();

        if (0.0 == deltaLength)
        {
            LOG_INTERN(LogLevel::Warning) << "could not calculate road geometry correctly";
            return Common::Vector2d();
        }

        if (s + deltaLength > sOffset)
        {
            // rescale last step
            double scale = (sOffset - s) / deltaLength;

            delta.Scale(scale);
            deltaLength = sOffset - s;
        }

        s += deltaLength;
    }

    Common::Vector2d offset(lastPos + delta);

    Common::Vector2d norm;
    if (0 < sOffset)
    {
        norm = delta;
    }
    else // account for start point
    {
        norm.x = 1.0;
    }

    norm.Rotate(-M_PI_2); // pointing to right side
    if (!norm.Norm())
    {
        LOG_INTERN(LogLevel::Error) << "division by 0";
    }

    offset.Add(norm * -tOffset);

    offset.Rotate(hdg);

    offset.x += x;
    offset.y += y;
    return offset;
}

double RoadGeometryPoly3::GetDir(double sOffset) const
{
    if (0.0 == a && 0.0 == b && 0.0 == c && 0.0 == d)
    {
        return GetDirLine(sOffset);
    }

    double s = 0.0;
    Common::Vector2d lastPos;
    Common::Vector2d delta;
    Common::Vector2d pos(0.0, a);

    while (s < sOffset)
    {
        lastPos = pos;
        pos.x += 1.0;
        pos.y = a + b * pos.x + c * pos.x * pos.x + d * pos.x * pos.x * pos.x;

        delta = pos - lastPos;
        double deltaLength = delta.Length();

        if (0.0 == deltaLength)
        {
            LOG_INTERN(LogLevel::Warning) << "could not calculate road geometry correctly";
            return 0.0;
        }

        if (s + deltaLength > sOffset)
        {
            // rescale last step
            double scale = (sOffset - s) / deltaLength;

            delta.Scale(scale);
            deltaLength = sOffset - s;
        }

        s += deltaLength;
    }

    Common::Vector2d direction;
    if (0 < sOffset)
    {
        direction = delta;
    }
    else // account for start point
    {
        direction.x = 1.0;
    }

    direction.Rotate(hdg);
    if (!direction.Norm())
    {
        LOG_INTERN(LogLevel::Error) << "division by 0";
    }

    if (1.0 < direction.y)
    {
        direction.y = 1.0;
    }

    if (-1.0 > direction.y)
    {
        direction.y = -1.0;
    }

    double angle = std::asin(direction.y);

    if (0.0 <= direction.x)
    {
        return angle;
    }
    else
    {
        return M_PI - angle;
    }
}
Common::Vector2d RoadGeometryParamPoly3::GetCoord(double sOffset, double tOffset) const
{
    if (0.0 == parameters.aV && 0.0 == parameters.bV && 0.0 == parameters.cV && 0.0 == parameters.dV)
    {
        return GetCoordLine(sOffset, tOffset);
    }

    double s = 0.0;
    Common::Vector2d lastPos;
    Common::Vector2d delta;
    double p = 0.0;
    Common::Vector2d pos(parameters.aU, parameters.aV);

    while (s < sOffset)
    {
        lastPos = pos;
        p += 1 / length;
        pos.x = parameters.aU + parameters.bU * p + parameters.cU * p * p + parameters.dU * p * p * p;
        pos.y = parameters.aV + parameters.bV * p + parameters.cV * p * p + parameters.dV * p * p * p;

        delta = pos - lastPos;
        double deltaLength = delta.Length();

        if (0.0 == deltaLength)
        {
            LOG_INTERN(LogLevel::Warning) << "could not calculate road geometry correctly";
            return Common::Vector2d();
        }

        if (s + deltaLength > sOffset)
        {
            // rescale last step
            double scale = (sOffset - s) / deltaLength;

            delta.Scale(scale);
            deltaLength = sOffset - s;
        }

        s += deltaLength;
    }

    Common::Vector2d offset(lastPos + delta);

    Common::Vector2d norm;
    if (0 < sOffset)
    {
        norm = delta;
    }
    else // account for start point
    {
        norm.x = 1.0;
    }

    norm.Rotate(-M_PI_2); // pointing to right side
    if (!norm.Norm())
    {
        LOG_INTERN(LogLevel::Error) << "division by 0";
    }

    offset.Add(norm * -tOffset);

    offset.Rotate(hdg);

    offset.x += x;
    offset.y += y;
    return offset;
}

double RoadGeometryParamPoly3::GetDir(double sOffset) const
{
    if (0.0 == parameters.aV && 0.0 == parameters.bV && 0.0 == parameters.cV && 0.0 == parameters.dV)
    {
        return GetDirLine(sOffset);
    }

    double s = 0.0;
    Common::Vector2d lastPos;
    Common::Vector2d delta;
    double p = 0.0;
    Common::Vector2d pos(parameters.aU, parameters.aV);

    while (s < sOffset)
    {
        lastPos = pos;
        p += 1 / length;
        pos.x = parameters.aU + parameters.bU * p + parameters.cU * p * p + parameters.dU * p * p * p;
        pos.y = parameters.aV + parameters.bV * p + parameters.cV * p * p + parameters.dV * p * p * p;

        delta = pos - lastPos;
        double deltaLength = delta.Length();

        if (0.0 == deltaLength)
        {
            LOG_INTERN(LogLevel::Warning) << "could not calculate road geometry correctly";
            return 0.0;
        }

        if (s + deltaLength > sOffset)
        {
            // rescale last step
            double scale = (sOffset - s) / deltaLength;

            delta.Scale(scale);
            deltaLength = sOffset - s;
        }

        s += deltaLength;
    }

    Common::Vector2d direction;
    if (0 < sOffset)
    {
        direction = delta;
    }
    else // account for start point
    {
        direction.x = 1.0;
    }

    direction.Rotate(hdg);
    if (!direction.Norm())
    {
        LOG_INTERN(LogLevel::Error) << "division by 0";
    }

    if (1.0 < direction.y)
    {
        direction.y = 1.0;
    }

    if (-1.0 > direction.y)
    {
        direction.y = -1.0;
    }

    double angle = std::asin(direction.y);

    if (0.0 <= direction.x)
    {
        return angle;
    }
    else
    {
        return M_PI - angle;
    }
}

Road::~Road()
{
    for (RoadElevation *item : elevations)
    {
        delete item;
    }

    for (RoadLaneOffset *item : laneOffsets)
    {
        delete item;
    }

    for (RoadGeometryInterface *item : geometries)
    {
        delete item;
    }

    for (RoadLinkInterface *item : links)
    {
        delete item;
    }

    for (RoadLaneSectionInterface *item : laneSections)
    {
        delete item;
    }

    for (RoadSignalInterface *item : roadSignals)
    {
        delete item;
    }

    for (RoadObjectInterface *item : roadObjects)
    {
        delete item;
    }
}

bool Road::AddGeometryLine(double s, double x, double y, double hdg, double length)
{
    RoadGeometry *roadGeometry = new (std::nothrow) RoadGeometryLine(s, x, y, hdg, length);
    if (!roadGeometry)
    {
        return false;
    }

    geometries.push_back(roadGeometry);

    return true;
}

bool Road::AddGeometryArc(double s, double x, double y, double hdg, double length, double curvature)
{
    RoadGeometry *roadGeometry = new (std::nothrow) RoadGeometryArc(s, x, y, hdg, length, curvature);
    if (!roadGeometry)
    {
        return false;
    }

    geometries.push_back(roadGeometry);

    return true;
}

bool Road::AddGeometrySpiral(double s, double x, double y, double hdg, double length, double curvStart, double curvEnd)
{
    RoadGeometry *roadGeometry = new (std::nothrow) RoadGeometrySpiral(s, x, y, hdg, length, curvStart, curvEnd);
    if (!roadGeometry)
    {
        return false;
    }

    geometries.push_back(roadGeometry);

    return true;
}

bool Road::AddGeometryPoly3(double s, double x, double y, double hdg, double length, double a, double b, double c,
                            double d)
{
    RoadGeometry *roadGeometry = new (std::nothrow) RoadGeometryPoly3(s, x, y, hdg, length, a, b, c, d);
    if (!roadGeometry)
    {
        return false;
    }

    geometries.push_back(roadGeometry);

    return true;
}

bool Road::AddGeometryParamPoly3(double s, double x, double y, double hdg, double length,
                                 ParamPoly3Parameters parameters)
{
    RoadGeometry *roadGeometry = new (std::nothrow) RoadGeometryParamPoly3(s, x, y, hdg, length, parameters);
    if (!roadGeometry)
    {
        return false;
    }

    geometries.push_back(roadGeometry);

    return true;
}

bool Road::AddElevation(double s, double a, double b, double c, double d)
{
    RoadElevation *roadElevation = new (std::nothrow) RoadElevation(s, a, b, c, d);
    if (!roadElevation)
    {
        return false;
    }

    elevations.push_back(roadElevation);

    return true;
}

bool Road::AddLaneOffset(double s, double a, double b, double c, double d)
{
    RoadLaneOffset *roadLaneOffset = new (std::nothrow) RoadLaneOffset(s, a, b, c, d);
    if (!roadLaneOffset)
    {
        return false;
    }

    laneOffsets.push_back(roadLaneOffset);

    return true;
}

bool Road::AddLink(RoadLinkType type, RoadLinkElementType elementType, const std::string &elementId,
                   ContactPointType contactPoint, RoadLinkDirectionType direction, RoadLinkSideType side)
{
    RoadLink *roadLink = new (std::nothrow) RoadLink(type, elementType, elementId, contactPoint, direction, side);
    if (!roadLink)
    {
        return false;
    }

    links.push_back(roadLink);

    return true;
}

RoadLaneSection *Road::AddRoadLaneSection(double start)
{
    RoadLaneSection *laneSection = new (std::nothrow) RoadLaneSection(this, start);
    laneSections.push_back(laneSection);

    return laneSection;
}

void Road::AddRoadSignal(const RoadSignalSpecification &signal)
{
    RoadSignal *roadSignal = new (std::nothrow) RoadSignal(this, signal);
    roadSignals.push_back(roadSignal);
}

void Road::AddRoadObject(const RoadObjectSpecification &object)
{
    RoadObject *roadObject = new (std::nothrow) RoadObject(this, object);
    roadObjects.push_back(roadObject);
}

void Road::AddRoadType(const RoadTypeSpecification &info)
{
    roadTypes.push_back(info);
}

RoadTypeInformation Road::GetRoadType(double start) const
{
    for (RoadTypeSpecification roadTypeSpec : roadTypes)
    {
        if (std::abs(roadTypeSpec.s - start) < 1e-6 /* assumed to be equal*/)
        {
            return roadTypeSpec.roadType;
        }
    }

    return RoadTypeInformation::Undefined;
}
