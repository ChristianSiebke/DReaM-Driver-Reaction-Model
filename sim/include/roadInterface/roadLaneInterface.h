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

//-----------------------------------------------------------------------------
//! @file  roadLaneInterface.h
//! @brief This file contains a interface representing a road lane.
//-----------------------------------------------------------------------------

#pragma once

#include "roadInterface.h"
#include "roadLaneInterface.h"
#include "roadLaneSectionInterface.h"
#include "roadLaneRoadMark.h"
//-----------------------------------------------------------------------------
//! Class representing a road lane.
//-----------------------------------------------------------------------------
class RoadLaneWidth;

using LaneIds = std::vector<int>;
using RoadLaneWidths = std::vector<RoadLaneWidth*>;
using RoadLaneRoadMarks = std::vector<RoadLaneRoadMark*>;

class RoadLaneInterface
{
public:
    RoadLaneInterface() = default;
    RoadLaneInterface(const RoadLaneInterface&) = delete;
    RoadLaneInterface(RoadLaneInterface&&) = delete;
    RoadLaneInterface& operator=(const RoadLaneInterface&) = delete;
    RoadLaneInterface& operator=(RoadLaneInterface&&) = delete;
    virtual ~RoadLaneInterface() = default;

    //-----------------------------------------------------------------------------
    //! Adds a new polynomial calculating the width of a lane to a road lane.
    //!
    //! @param[in]  sOffset             Offset relative to the preceding lane section
    //! @param[in]  a                   Constant factor from the polynomial
    //! @param[in]  b                   Linear factor from the polynomial
    //! @param[in]  c                   Quadratic factor from the polynomial
    //! @param[in]  d                   Cubic factor from the polynomial
    //!
    //! @return                         False if an error occurred, true otherwise
    //-----------------------------------------------------------------------------
    virtual bool AddWidth(double sOffset,
                  double a,
                  double b,
                  double c,
                  double d) = 0;

    //-----------------------------------------------------------------------------
    //! Adds a new polynomial calculating the border of a lane to a road lane.
    //!
    //! @param[in]  sOffset             Offset relative to the preceding lane section
    //! @param[in]  a                   Constant factor from the polynomial
    //! @param[in]  b                   Linear factor from the polynomial
    //! @param[in]  c                   Quadratic factor from the polynomial
    //! @param[in]  d                   Cubic factor from the polynomial
    //!
    //! @return                         False if an error occurred, true otherwise
    //-----------------------------------------------------------------------------
    virtual bool AddBorder(double sOffset,
                  double a,
                  double b,
                  double c,
                  double d) = 0;

    //-----------------------------------------------------------------------------
    //! Adds a new roadmark to a road lane.
    //!
    //! @param[in]  sOffset             Offset relative to the preceding lane section
    //! @param[in]  roadMark            type of the road mark
    //! @param[in]  color               color of the road mark
    //! @param[in]  laneChange          allowed laneChange across the road mark
    //! @param[in]  weight              weight of the road mark
    //!
    //! @return                         False if an error occurred, true otherwise
    //-----------------------------------------------------------------------------
    virtual bool AddRoadMark(double sOffset,
                             RoadLaneRoadDescriptionType descType,
                             RoadLaneRoadMarkType roadMark,
                             RoadLaneRoadMarkColor color,
                             RoadLaneRoadMarkLaneChange laneChange,
                             RoadLaneRoadMarkWeight weight) = 0;

    //-----------------------------------------------------------------------------
    //! Adds the ID of a successor lane to a road lane.
    //!
    //! @param[in]  id                  ID of a successor lane
    //!
    //! @return                         False if an error occurred, true otherwise
    //-----------------------------------------------------------------------------
    virtual bool AddSuccessor(int id) = 0;

    //-----------------------------------------------------------------------------
    //! Adds the ID of a predecessor lane to a road lane.
    //!
    //! @param[in]  id                  ID of a predecessor lane
    //!
    //! @return                         False if an error occurred, true otherwise
    //-----------------------------------------------------------------------------
    virtual bool AddPredecessor(int id) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the ID of the road lane.
    //!
    //! @return                         ID of the road lane
    //-----------------------------------------------------------------------------
    virtual int GetId() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the RoadLaneTypeType of the road lane.
    //!
    //! @return                         RoadLaneTypeType of the road lane
    //-----------------------------------------------------------------------------
    virtual RoadLaneType GetType() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the widths of the road lane.
    //!
    //! @return                         RoadLaneTypeType of the road lane
    //-----------------------------------------------------------------------------
    virtual const RoadLaneWidths &GetWidths() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the widths of the road lane.
    //!
    //! @return                         RoadLaneTypeType of the road lane
    //-----------------------------------------------------------------------------
    virtual const RoadLaneWidths &GetBorders() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the successors of a road lane, can have at most one element.
    //!
    //! @return                         Successors of the road lane
    //-----------------------------------------------------------------------------
    virtual const LaneIds &GetSuccessor() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the predecessors of a road lane, can have at most one element.
    //!
    //! @return                         Predecessors of the road lane
    //-----------------------------------------------------------------------------
    virtual const LaneIds &GetPredecessor() const = 0;

    //-----------------------------------------------------------------------------
    //! Sets the flag, if the road lane is in the reference direction or not.
    //!
    //! @param[in]  inDirection         Flag, if the road lane is in the reference
    //!                                 direction or not
    //-----------------------------------------------------------------------------
    virtual void SetInDirection(bool inDirection) = 0;

    //-----------------------------------------------------------------------------
    //! Returns if the road lane is in the reference direction or not.
    //!
    //! @return                         Flag, if the road lane is in the reference
    //!                                 direction or not
    //-----------------------------------------------------------------------------
    virtual bool GetInDirection() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the RoadLaneSection to which the road lane belongs.
    //!
    //! @return                         RoadLaneSection to which the road lane belongs
    //-----------------------------------------------------------------------------
    virtual RoadLaneSectionInterface *GetLaneSection() const = 0;

    //! Returns the roadmarktype of the road lane.
    //!
    //! @return                         RoadLaneRoadMarkType
    //-----------------------------------------------------------------------------

    virtual const RoadLaneRoadMarks &GetRoadMarks() const = 0;

};


