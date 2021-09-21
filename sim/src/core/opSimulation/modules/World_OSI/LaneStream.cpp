/*******************************************************************************
 * Copyright (c) 2021, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "LaneStream.h"

LaneStream::LaneStream(std::vector<LaneStreamElement> elements) :
    elements(elements)
{
}


StreamPosition LaneStream::GetStreamPosition(const GlobalRoadPosition &roadPosition) const
{
    StreamPosition streamPosition {-1, 0};

    for (const auto& element : elements)
    {
        if (element.lane->GetRoad().GetId() == roadPosition.roadId && element.lane->GetOdId() == roadPosition.laneId)
        {
            streamPosition.s = element.GetStreamPosition(roadPosition.roadPosition.s);
            streamPosition.t = roadPosition.roadPosition.t * (element.inStreamDirection ? 1 : -1);
        }
    }
    return streamPosition;
}

GlobalRoadPosition LaneStream::GetRoadPosition(const StreamPosition &streamPosition) const
{
    GlobalRoadPosition globalRoadPosition;

    for (const auto& element : elements)
    {
        if (element.StartS() <= streamPosition.s && element.EndS() >= streamPosition.s)
        {
            globalRoadPosition.roadId = element.lane->GetRoad().GetId();
            globalRoadPosition.laneId = element.lane->GetOdId();
            globalRoadPosition.roadPosition.s = element.GetElementPosition(streamPosition.s);
            globalRoadPosition.roadPosition.t = streamPosition.t * (element.inStreamDirection ? 1 : -1);
        }
    }

    return globalRoadPosition;
}

std::vector<const AgentInterface *> LaneStream::GetAgentsInRange(const StreamPosition &start, const StreamPosition &end) const
{
    AgentInterfaces foundObjects;
    for (const auto& laneStreamElement : elements)
    {
        if (laneStreamElement.EndS() < start.s) //Ignore lanes ending before startDistance
        {
            continue;
        }
        if (laneStreamElement.StartS() > end.s) //Stop if reached endDistance
        {
            break;
        }
        const auto streamDirection = laneStreamElement.inStreamDirection;

        for (const auto& [laneOverlap, object] : laneStreamElement.lane->GetWorldObjects(streamDirection))
        {
            const auto s_min = streamDirection ? laneOverlap.s_min : laneOverlap.s_max;
            const auto s_max = streamDirection ? laneOverlap.s_max : laneOverlap.s_min;

            auto streamPositionStart = laneStreamElement.GetStreamPosition(s_min - laneStreamElement.lane->GetDistance(OWL::MeasurementPoint::RoadStart));
            if (streamPositionStart > end.s)
            {
                break;
            }

            auto streamPositionEnd = laneStreamElement.GetStreamPosition(s_max - laneStreamElement.lane->GetDistance(OWL::MeasurementPoint::RoadStart));
            if (dynamic_cast<const OWL::Interfaces::MovingObject*>(object) && streamPositionEnd >= start.s)
            {
                if (std::find(foundObjects.crbegin(), foundObjects.crend(), object->GetLink<AgentInterface>()) == foundObjects.crend())
                {
                    foundObjects.push_back(object->GetLink<AgentInterface>());
                }
            }
        }
    }
    return foundObjects;
}

std::vector<const WorldObjectInterface *> LaneStream::GetObjectsInRange(const StreamPosition &start, const StreamPosition &end) const
{
    std::vector<const WorldObjectInterface*> foundObjects;
    for (const auto& laneStreamElement : elements)
    {
        if (laneStreamElement.EndS() < start.s) //Ignore lanes ending before startDistance
        {
            continue;
        }
        if (laneStreamElement.StartS() > end.s) //Stop if reached endDistance
        {
            break;
        }
        const auto streamDirection = laneStreamElement.inStreamDirection;

        for (const auto& [laneOverlap, object] : laneStreamElement.lane->GetWorldObjects(streamDirection))
        {
            const auto s_min = streamDirection ? laneOverlap.s_min : laneOverlap.s_max;
            const auto s_max = streamDirection ? laneOverlap.s_max : laneOverlap.s_min;

            auto streamPositionStart = laneStreamElement.GetStreamPosition(s_min - laneStreamElement.lane->GetDistance(OWL::MeasurementPoint::RoadStart));
            if (streamPositionStart > end.s)
            {
                break;
            }

            auto streamPositionEnd = laneStreamElement.GetStreamPosition(s_max - laneStreamElement.lane->GetDistance(OWL::MeasurementPoint::RoadStart));
            if (streamPositionEnd >= start.s)
            {
                if (std::find(foundObjects.crbegin(), foundObjects.crend(), object->GetLink<AgentInterface>()) == foundObjects.crend())
                {
                    foundObjects.push_back(object->GetLink<WorldObjectInterface>());
                }
            }
        }
    }
    return foundObjects;
}

std::optional<StreamPosition> LaneStream::GetStreamPosition(const WorldObjectInterface *object, const MeasurementPoint &mp) const
{
    auto& objectPosition = object->GetObjectPosition();
    for (auto element : elements)
    {
        if (mp == MeasurementPoint::Reference)
        {
            auto referencePointPosition = helper::map::query(objectPosition.referencePoint, element.lane->GetRoad().GetId());
            if(referencePointPosition.has_value()
                    && referencePointPosition.value().laneId == element.lane->GetOdId()
                    && element.lane->GetSection().Covers(referencePointPosition.value().roadPosition.s))
            {
                const auto s = element.GetStreamPosition(referencePointPosition.value().roadPosition.s - element.lane->GetDistance(OWL::MeasurementPoint::RoadStart));
                const auto t = referencePointPosition.value().roadPosition.t;
                return StreamPosition{s,t};
            }
        }
        else if (mp == MeasurementPoint::Front)
        {
            auto mainLocatePointPosition = helper::map::query(objectPosition.mainLocatePoint, element.lane->GetRoad().GetId());
            if(mainLocatePointPosition.has_value()
                    && mainLocatePointPosition.value().laneId == element.lane->GetOdId()
                    && element.lane->GetSection().Covers(mainLocatePointPosition.value().roadPosition.s))
            {
                const auto s = element.GetStreamPosition(mainLocatePointPosition.value().roadPosition.s - element.lane->GetDistance(OWL::MeasurementPoint::RoadStart));
                const auto t = mainLocatePointPosition.value().roadPosition.t;
                return StreamPosition{s,t};
            }
        }
        else if (mp == MeasurementPoint::Rear)
        {
            auto touchedRoads = helper::map::query(objectPosition.touchedRoads, element.lane->GetRoad().GetId());
            if(touchedRoads.has_value())
            {
                if (std::find(touchedRoads.value().lanes.cbegin(), touchedRoads.value().lanes.cend(), element.lane->GetOdId()) == touchedRoads.value().lanes.cend())
                {
                    continue;
                }
                const auto sStart = element.inStreamDirection ? touchedRoads.value().sStart : touchedRoads.value().sEnd;
                if (!element.lane->GetSection().Covers(sStart))
                {
                    continue;
                }
                const auto s = element.GetStreamPosition(sStart- element.lane->GetDistance(OWL::MeasurementPoint::RoadStart));
                const auto t = 0.; //Not supported
                return StreamPosition{s,t};
            }
        }
    }
    return std::nullopt;
}

double LaneStream::GetLength() const
{
    return elements.back().EndS();
}

std::vector<std::pair<double, LaneType>> LaneStream::GetLaneTypes() const
{
    LaneType lastLaneType{LaneType::Undefined};
    std::vector<std::pair<double, LaneType>> laneTypes;
    for (const auto& element : elements)
    {
        if(element.lane->GetLaneType() != lastLaneType)
        {
            laneTypes.emplace_back(element.StartS(), element.lane->GetLaneType());
            lastLaneType = element.lane->GetLaneType();
        }
    }
    return laneTypes;
}
