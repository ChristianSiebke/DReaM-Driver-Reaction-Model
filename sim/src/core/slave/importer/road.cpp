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
#include "common/commonHelper.h"

extern "C"
{
    extern int fresnl(double xxa, double *ssa, double *cca);
}

namespace
{
const double SQRT_PI = std::sqrt(M_PI);

} // namespace

RoadLane::~RoadLane()
{
    for (RoadLaneWidth *item : widths)
    {
        delete item;
    }

    for (RoadLaneWidth *item : borders)
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

bool RoadLane::AddBorder(double sOffset, double a, double b, double c, double d)
{
    RoadLaneWidth *laneWidth = new (std::nothrow) RoadLaneWidth(sOffset, a, b, c, d);
    if (!laneWidth)
    {
        return false;
    }

    borders.push_back(laneWidth);

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
    if (sOffset > length)
    {
        if (!CommonHelper::DoubleEquality(sOffset, length))
        {
            LOG_INTERN(LogLevel::Warning)
                << "cummulative s-offset exceeds length of line geometry by " << (sOffset - length) << " m. "
                << "Setting sOffset to length.";
        }
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
    if (sOffset > length)
    {
        if (!CommonHelper::DoubleEquality(sOffset, length))
        {
            LOG_INTERN(LogLevel::Warning)
                << "cummulative s-offset exceeds length of arc geometry by " << (sOffset - length) << " m. "
                << "Setting sOffset to length.";
        }

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

RoadGeometrySpiral::RoadGeometrySpiral(double s, double x, double y, double hdg, double length, double curvStart, double curvEnd)
    : RoadGeometry{s, x, y, hdg, length}, c_start{curvStart}, c_end{curvEnd}
{
    if (length != 0.0)
    {
        c_dot = (c_end - c_start) / length;
    }
    else
    {
        c_dot = 0.0;
    }

    if (c_dot != 0.0)
    {
        l_start = c_start / c_dot;
    }
    else
    {
        l_start = 0.0;
    }

    double l_end = l_start + length;
    double rl;   // helper constant R * L

    if (c_start != 0.0)
    {
        rl = l_start / c_start;
    }
    else if (c_end != 0.0)
    {
        rl = l_end / c_end;
    }
    else
    {
        t_start = 0.0;
        a = 0.0;
        sign = 0.0;
        return;
    }

    t_start = 0.5 * l_start * curvStart;
    a = std::sqrt(std::abs(rl));
    sign = std::signbit(rl) ? -1.0 : 1.0;
}

Common::Vector2d RoadGeometrySpiral::FullCoord(double sOffset, double tOffset) const
{
    // curvature of the spiral at sOffset
    double curvAtsOffet = c_start + c_dot * sOffset;

    // start and end coordinates of spiral
    Common::Vector2d start, end;

    // calculate x and y of spiral start, assuming sOffset = 0 means start of spiral with curvature curvStart
    (void)fresnl(l_start / a / SQRT_PI, &start.y, &start.x);
    start.Scale(a * SQRT_PI);
    start.y *= sign;

    // calculate x and y of spiral end, assuming l_start + sOffset means end of spiral with curvature curvEnd
    (void)fresnl((l_start + sOffset) / a / SQRT_PI, &end.y, &end.x);
    end.Scale(a * SQRT_PI);
    end.y *= sign;

    // delta x, y from start of spiral to end of spiral
    auto diff = end - start;

    // tangent angle at end of spiral
    double t_end = (l_start + sOffset) * curvAtsOffet / 2.0;

    // heading change of spiral
    double dHdg = t_end - t_start;

    // rotate delta x, y to match starting direction and origin heading
    diff.Rotate(-t_start+hdg);

    // heading at end of spiral
    auto endHdg = hdg + dHdg;

    // calculate t-offset to spiral
    Common::Vector2d unit{1.0,0};
    unit.Rotate(endHdg + M_PI_2);
    unit.Scale(tOffset);

    return diff + unit + Common::Vector2d(x, y);
}

double RoadGeometrySpiral::FullCurvature(double sOffset) const
{
    return c_start + c_dot * sOffset;
}

double RoadGeometrySpiral::FullDir(double sOffset) const
{
    // tangent_angle = L / (2 * R) = 0.5 * L * curv
    // direction change in spiral = tangent_end - tangent_start

    double curvEnd = FullCurvature(sOffset);
    return hdg + 0.5 * (curvEnd * (l_start + sOffset) - c_start * l_start);
}

Common::Vector2d RoadGeometrySpiral::GetCoord(double sOffset, double tOffset) const
{
    if (0.0 == c_start && 0.0 == c_end)
    {
        return GetCoordLine(sOffset, tOffset);
    }

    if (std::abs(c_start - c_end) < 1e-6 /* assumed to be equal */)
    {
        return GetCoordArc(sOffset, tOffset, c_start);
    }

    return FullCoord(sOffset, tOffset);
}

double RoadGeometrySpiral::GetDir(double sOffset) const
{
    if (0.0 == c_start && 0.0 == c_end)
    {
        return GetDirLine(sOffset);
    }

    if (std::abs(c_start - c_end) < 1e-6 /* assumed to be equal */)
    {
        return GetDirArc(sOffset, c_start);
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
                   ContactPointType contactPoint)
{
    RoadLink *roadLink = new (std::nothrow) RoadLink(type, elementType, elementId, contactPoint);
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
