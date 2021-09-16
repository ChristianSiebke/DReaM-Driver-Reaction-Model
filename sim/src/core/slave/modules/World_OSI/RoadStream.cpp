/*******************************************************************************
 * Copyright (c) 2021, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "RoadStream.h"

const OWL::Interfaces::Section& GetSectionAtDistance(const OWL::Interfaces::Road& road, double distance)
{
    for (const auto section : road.GetSections())
    {
        if (section->Covers(distance))
        {
            return *section;
        }
    }
}

double GetLaneWidth(const OWL::Interfaces::Section& section, double laneId, double distance)
{
    for (const auto lane : section.GetLanes())
    {
        if (lane->GetOdId() == laneId)
        {
            return lane->GetWidth(distance);
        }
    }
    return 0.0;
}

StreamPosition RoadStream::GetStreamPosition(const GlobalRoadPosition &roadPosition) const
{
    StreamPosition streamPosition {-1, 0};

    for (const auto& element : elements)
    {
        if (element.road->GetId() == roadPosition.roadId)
        {
            streamPosition.s = element.GetStreamPosition(roadPosition.roadPosition.s);
            streamPosition.t = roadPosition.roadPosition.t;
            if (roadPosition.laneId < 0)
            {
                const auto& section = GetSectionAtDistance(*element.road, roadPosition.roadPosition.s);
                for (int laneId = -1; laneId > roadPosition.laneId; --laneId)
                {
                    streamPosition.t -= GetLaneWidth(section, laneId, roadPosition.roadPosition.s);
                }
                streamPosition.t -= 0.5 * GetLaneWidth(section, roadPosition.laneId, roadPosition.roadPosition.s);
            }
            if (roadPosition.laneId > 0)
            {
                const auto& section = GetSectionAtDistance(*element.road, roadPosition.roadPosition.s);
                for (int laneId = 1; laneId < roadPosition.laneId; ++laneId)
                {
                    streamPosition.t += GetLaneWidth(section, laneId, roadPosition.roadPosition.s);
                }
                streamPosition.t += 0.5 * GetLaneWidth(section, roadPosition.laneId, roadPosition.roadPosition.s);
            }
            streamPosition.t *= element.inStreamDirection ? 1 : -1;
            return streamPosition;
        }
    }
    return streamPosition;
}

GlobalRoadPosition RoadStream::GetRoadPosition(const StreamPosition &streamPosition) const
{
    GlobalRoadPosition globalRoadPosition;

    for (const auto& element : elements)
    {
        if (element.StartS() <= streamPosition.s && element.EndS() >= streamPosition.s)
        {
            globalRoadPosition.roadId = element.road->GetId();
            globalRoadPosition.roadPosition.s = element.GetElementPosition(streamPosition.s);
            const auto& section = GetSectionAtDistance(*element.road, globalRoadPosition.roadPosition.s);
            double remainingOffset = streamPosition.t * (element.inStreamDirection ? 1 : -1);
            if (element.inStreamDirection xor (streamPosition.t > 0))
            {
                int laneId = -1;
                double laneWidth = GetLaneWidth(section, laneId, globalRoadPosition.roadPosition.s);
                while (-remainingOffset > laneWidth)
                {
                    remainingOffset += laneWidth;
                    --laneId;
                    laneWidth = GetLaneWidth(section, laneId, globalRoadPosition.roadPosition.s);
                }
                globalRoadPosition.laneId = laneId;
                globalRoadPosition.roadPosition.t = remainingOffset + 0.5 * laneWidth;
            }
            else
            {
                int laneId = 1;
                double laneWidth = GetLaneWidth(section, laneId, globalRoadPosition.roadPosition.s);
                while (remainingOffset > laneWidth)
                {
                    remainingOffset -= laneWidth;
                    ++laneId;
                    laneWidth = GetLaneWidth(section, laneId, globalRoadPosition.roadPosition.s);
                }
                globalRoadPosition.laneId = laneId;
                globalRoadPosition.roadPosition.t = remainingOffset - 0.5 * laneWidth;
            }
            return globalRoadPosition;
        }
    }

    return globalRoadPosition;
}

bool LaneIsInDirection (const OWL::Interfaces::Lane* lane, bool odDirection)
{
    return odDirection ? (lane->GetOdId() < 0) : (lane->GetOdId() > 0);
}

std::vector<std::unique_ptr<LaneStreamInterface>> RoadStream::GetAllLaneStreams() const
{
    std::vector<std::unique_ptr<LaneStreamInterface>> laneStreams;
    OWL::Interfaces::Lanes const* lastLanes{nullptr};
    for (auto element : elements)
    {
        OWL::Interfaces::Sections sections = element.road->GetSections();
        if (!element.inStreamDirection)
        {
            sections.reverse();
        }
        for(auto section : sections)
        {
            for(auto lane : section->GetLanes())
            {
                if (!LaneIsInDirection(lane, element.inStreamDirection))
                {
                    continue;
                }
                bool hasPredecessorInStream = false;
                if(lastLanes)
                {
                    auto predecessors = element.inStreamDirection ? lane->GetPrevious() : lane->GetNext();

                    for (const auto& lastLane : *lastLanes)
                    {
                        if (std::find(predecessors.cbegin(), predecessors.cend(), lastLane->GetId()) != predecessors.cend())
                        {
                            hasPredecessorInStream = true;
                            break;
                        }
                    }
                }
                if (hasPredecessorInStream)
                {
                    continue;
                }
                StreamPosition start{element.GetStreamPosition(lane->GetDistance(OWL::MeasurementPoint::RoadStart)), 0.};
                laneStreams.push_back(GetLaneStream(start, lane->GetOdId()));
            }
            lastLanes = &section->GetLanes();
        }
    }
    return laneStreams;
}

std::unique_ptr<LaneStreamInterface> RoadStream::GetLaneStream(const StreamPosition &startPosition, const int laneId) const
{
    return std::make_unique<LaneStream>(CreateLaneStream(startPosition, laneId));
}

std::vector<LaneStreamElement> RoadStream::CreateLaneStream(const StreamPosition &startPosition, const int laneId) const
{
    std::vector<LaneStreamElement> laneStreamElements;
    for (auto element = elements.begin(); element != elements.cend(); ++element)
    {
        if (element->StartS() > startPosition.s || element->EndS() < startPosition.s)
        {
            continue;
        }
        OWL::Interfaces::Sections sections = element->road->GetSections();
        if (!element->inStreamDirection)
        {
            sections.reverse();
        }
        for (auto section = sections.begin(); section != sections.cend(); ++ section)
        {
            if (!(*section)->Covers(element->GetElementPosition(startPosition.s)))
            {
                continue;
            }
            for (const auto& lane : (*section)->GetLanes())
            {
                if (lane->GetOdId() != laneId)
                {
                    continue;
                }

                for (auto currentLane = lane; currentLane;)
                {
                    LaneStreamElement newElement;
                    newElement.lane = currentLane;
                    newElement.inStreamDirection = element->inStreamDirection;
                    newElement.sOffset = element->GetStreamPosition(currentLane->GetDistance(OWL::MeasurementPoint::RoadStart));
                    laneStreamElements.push_back(newElement);

                    const auto successors = element->inStreamDirection ? currentLane->GetNext() : currentLane->GetPrevious();
                    ++section;
                    if (section == sections.cend())
                    {
                        ++element;
                        if(element == elements.cend())
                        {
                            break;
                        }
                        sections = element->road->GetSections();
                        if (!element->inStreamDirection)
                        {
                            sections.reverse();
                        }
                        section = sections.begin();
                    }
                    for (const auto& nextLane : (*section)->GetLanes())
                    {
                        if (std::find(successors.cbegin(), successors.cend(), nextLane->GetId()) != successors.cend())
                        {
                            currentLane = nextLane;
                            break;
                        }
                    }
                }
                return laneStreamElements;
            }
        }
    }
    return laneStreamElements;
}

double RoadStream::GetLength() const
{
    return elements.back().EndS();
}
