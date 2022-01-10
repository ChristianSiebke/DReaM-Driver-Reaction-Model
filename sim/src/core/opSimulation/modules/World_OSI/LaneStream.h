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
//! @file  LaneStream.h
//! @brief
//-----------------------------------------------------------------------------

#pragma once

#include "include/streamInterface.h"
#include "OWL/DataTypes.h"

//! This class represents one element of a LaneStream.
struct LaneStreamElement
{
    const OWL::Interfaces::Lane* lane;  //!< lane represented by this object
    double sOffset;         //!< S Offset of the start point of the element from the beginning of the stream
    bool inStreamDirection; //!< Specifies whether the direction of the element is the same as the direction of the stream

    LaneStreamElement() = default;

    LaneStreamElement(const OWL::Interfaces::Lane* lane, double sOffset, bool inStreamDirection) :
        lane(lane),
        sOffset(sOffset),
        inStreamDirection(inStreamDirection)
    {}

    const OWL::Interfaces::Lane& operator()() const
    {
        return *lane;
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

    //! Returns the stream position of the start of the lane
    double StartS() const
    {
        return sOffset - (inStreamDirection ? 0 : lane->GetLength());
    }

    //! Returns the stream position of the end of the lane
    double EndS() const
    {
        return sOffset + (inStreamDirection ? lane->GetLength() : 0);
    }
};

class LaneStream : public LaneStreamInterface
{
public:
    LaneStream(std::vector<LaneStreamElement> elements);

    virtual StreamPosition GetStreamPosition(const GlobalRoadPosition& roadPosition) const override;

    virtual GlobalRoadPosition GetRoadPosition(const StreamPosition& streamPosition) const override;

    virtual AgentInterfaces GetAgentsInRange(const StreamPosition& start, const StreamPosition& end) const override;

    virtual std::vector<const WorldObjectInterface*> GetObjectsInRange(const StreamPosition& start, const StreamPosition& end) const override;

    virtual std::optional<StreamPosition> GetStreamPosition(const WorldObjectInterface* object, const MeasurementPoint& mp) const override;

    virtual double GetLength() const override;

    virtual std::vector<std::pair<double, LaneType>> GetLaneTypes() const override;

private:
    const std::vector<LaneStreamElement> elements;
};
