/*******************************************************************************
* Copyright (c) 2021, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  streamInterface.h
//! @brief
//-----------------------------------------------------------------------------

#pragma once

#include "common/worldDefinitions.h"

class AgentInterface;
class WorldObjectInterface;

#include <cmath>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

//! Defines the position in a RoadStream or LaneStream
struct StreamPosition
{
    double s; //!< Longitudinal position in the stream (relative to the start)
    double t; //!< Lateral position in stream (relative to middle of the road/lane)

    double operator==(const StreamPosition& other) const
    {
        return std::abs(s - other.s) < EQUALITY_BOUND
                && std::abs(t - other.t) < EQUALITY_BOUND;
    }
};

//! Represents a connected sequence of lanes in the road network
class LaneStreamInterface
{
public:
    LaneStreamInterface() = default;
    LaneStreamInterface(const LaneStreamInterface &) = delete;
    LaneStreamInterface(LaneStreamInterface &&) = delete;
    LaneStreamInterface &operator=(const LaneStreamInterface &) = delete;
    LaneStreamInterface &operator=(LaneStreamInterface &&) = delete;
    virtual ~LaneStreamInterface() = default;

    //! Converts a OpenDRIVE position into a StreamPosition
    //! (or {-1,0}) if the OpenDRIVE position is not on the stream)
    //!
    //! \param roadPosition  position in OpenDRIVE
    //! \return  position in the stream
    virtual StreamPosition GetStreamPosition(const GlobalRoadPosition& roadPosition) const = 0;

    //! Converts a StreamPosition into a position in OpenDRIVE
    //!
    //! \param roadPosition  position in the stream
    //! \return  position in OpenDRIVE
    virtual GlobalRoadPosition GetRoadPosition(const StreamPosition& streamPosition) const = 0;

    //! Returns all agents inside the given range on the stream
    //!
    //! \param start  start of range (t is ignored)
    //! \param end    end of range (t is ignored)
    //! \return agents in range
    virtual std::vector<const AgentInterface*> GetAgentsInRange(const StreamPosition& start, const StreamPosition& end) const = 0;

    //! Returns all objects inside the given range on the stream
    //!
    //! \param start  start of range (t is ignored)
    //! \param end    end of range (t is ignored)
    //! \return objects in range
    virtual std::vector<const WorldObjectInterface*> GetObjectsInRange(const StreamPosition& start, const StreamPosition& end) const = 0;

    //! Returns the position of an object on the stream
    //!
    //! \param object   object on the stream
    //! \param mp       point of the object from which to take the position
    //! \return object position on the stream
    virtual std::optional<StreamPosition> GetStreamPosition(const WorldObjectInterface* object, const MeasurementPoint& mp) const = 0;

    //! Calculates a list of all LaneTypes with their starting s on the stream
    //! Each LaneType is valid until the s position of the next element in the list
    //!
    //! \return list of laneTypes and where they start
    virtual std::vector<std::pair<double, LaneType>> GetLaneTypes() const = 0;

    //! Returns the length of the stream
    virtual double GetLength() const = 0;
};

//! Represents a connected sequence of roads in the road network
class RoadStreamInterface
{
public:
    RoadStreamInterface() = default;
    RoadStreamInterface(const RoadStreamInterface &) = delete;
    RoadStreamInterface(RoadStreamInterface &&) = delete;
    RoadStreamInterface &operator=(const RoadStreamInterface &) = delete;
    RoadStreamInterface &operator=(RoadStreamInterface &&) = delete;
    virtual ~RoadStreamInterface() = default;

    //! Converts a OpenDRIVE position into a StreamPosition
    //! (or {-1,0}) if the OpenDRIVE position is not on the stream)
    //!
    //! \param roadPosition  position in OpenDRIVE
    //! \return  position in the stream
    virtual StreamPosition GetStreamPosition(const GlobalRoadPosition& roadPosition) const = 0;

    //! Converts a StreamPosition into a position in OpenDRIVE
    //!
    //! \param roadPosition  position in the stream
    //! \return  position in OpenDRIVE
    virtual GlobalRoadPosition GetRoadPosition(const StreamPosition& streamPosition) const = 0;

    //! Return all LaneStreams that are part of this RoadStream
    virtual std::vector<std::unique_ptr<LaneStreamInterface>> GetAllLaneStreams() const = 0;

    //! Returns the LaneStream corresponding to this RoadStream starting at the given position
    virtual std::unique_ptr<LaneStreamInterface> GetLaneStream(const StreamPosition& startPosition, const int laneId) const = 0;

    //! Returns the length of the stream
    virtual double GetLength() const = 0;
};
