/********************************************************************************
 * Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  RoadStream.h
//! @brief
//-----------------------------------------------------------------------------

#pragma once

#include "include/streamInterface.h"
#include "OWL/DataTypes.h"
#include "LaneStream.h"

//! This class represents one element of a RoadStream.
struct RoadStreamElement
{
    const OWL::Interfaces::Road* road;  //!< road represented by this object
    double sOffset;         //!< S Offset of the start point of the element from the beginning of the stream
    bool inStreamDirection; //!< Specifies whether the direction of the element is the same as the direction of the stream

    RoadStreamElement() = default;

    RoadStreamElement(const OWL::Interfaces::Road* road, double sOffset, bool inStreamDirection) :
        road(road),
        sOffset(sOffset),
        inStreamDirection(inStreamDirection)
    {}

    const OWL::Interfaces::Road& operator()() const
    {
        return *road;
    }

    //! Transform the s coordinate on the element to the s coordinate on the stream
    //!
    //! \param elementPosition position relative to the start of the element
    //! \return position relative to the start of the stream
    double GetStreamPosition(double elementPosition) const
    {
        return sOffset + (inStreamDirection ? elementPosition : -elementPosition);
    }

    //! Transform the s coordinate on the stream to the s coordinate on the element
    //!
    //! \param streamPosition position relative to the start of the stream
    //! \return position relative to the start of the element
    double GetElementPosition(double streamPosition) const
    {
        return inStreamDirection ? streamPosition - sOffset : sOffset - streamPosition;
    }

    //! Returns the stream position of the start of the road
    double StartS() const
    {
        return sOffset - (inStreamDirection ? 0 : road->GetLength());
    }

    //! Returns the stream position of the end of the road
    double EndS() const
    {
        return sOffset + (inStreamDirection ? road->GetLength() : 0);
    }
};

class RoadStream : public RoadStreamInterface
{
public:
    RoadStream(const std::vector<RoadStreamElement>&& elements) :
        elements(elements)
    {}

    virtual StreamPosition GetStreamPosition(const GlobalRoadPosition& roadPosition) const override;

    virtual GlobalRoadPosition GetRoadPosition(const StreamPosition& streamPosition) const override;

    virtual std::vector<std::unique_ptr<LaneStreamInterface>> GetAllLaneStreams() const override;

    virtual std::unique_ptr<LaneStreamInterface> GetLaneStream(const StreamPosition& startPosition, const int laneId) const override;

    //Public for testing
    std::vector<LaneStreamElement> CreateLaneStream(const StreamPosition& startPosition, const int laneId) const;

    virtual double GetLength() const override;

private:
    const std::vector<RoadStreamElement> elements;
};
