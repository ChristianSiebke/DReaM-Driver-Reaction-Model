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
//! @file  roadGeometryInterface.h
//! @brief This file contains a abstract class representing the geometry of a
//!        road, i.e. the layout of the road's reference line in the in the x/y
//!        plane.
//-----------------------------------------------------------------------------

#pragma once

#include "Common/vector2d.h"
#include "Interfaces/roadInterface/roadInterface.h"

//-----------------------------------------------------------------------------
//! Abstract class representing the geometry of a road, i.e. the layout of the
//! road's reference line in the in the x/y plane.
//-----------------------------------------------------------------------------
class RoadGeometryInterface
{
public:
    RoadGeometryInterface() = default;
    RoadGeometryInterface(const RoadGeometryInterface&) = delete;
    RoadGeometryInterface(RoadGeometryInterface&&) = delete;
    RoadGeometryInterface& operator=(const RoadGeometryInterface&) = delete;
    RoadGeometryInterface& operator=(RoadGeometryInterface&&) = delete;
    virtual ~RoadGeometryInterface() = default;

    //-----------------------------------------------------------------------------
    //! Calculates the x/y coordinates as vector.
    //!
    //! @param[in]  sOffset    s offset within geometry section
    //! @param[in]  tOffset    offset to the left
    //! @return                coordinates with the x/y coordinates
    //-----------------------------------------------------------------------------
    virtual Common::Vector2d GetCoord(double geometryOffset, double laneOffset) const = 0;

    //-----------------------------------------------------------------------------
    //! Calculates the heading.
    //!
    //! @param[in]  sOffset    s offset within geometry section
    //! @return                heading
    //-----------------------------------------------------------------------------
    virtual double GetDir(double geometryOffset) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the offset within the OpenDrive road.
    //!
    //! @return                         offset
    //-----------------------------------------------------------------------------
    virtual double GetS() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the initial direction of the geometry section
    //!
    //! @return                         initial direction of geometry
    //-----------------------------------------------------------------------------
    virtual double GetHdg() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the length of the geometry section
    //!
    //! @return                         length of geometry section
    //-----------------------------------------------------------------------------
    virtual double GetLength() const = 0;
};
