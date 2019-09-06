/*******************************************************************************
* Copyright (c) 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#pragma once

#include "Common/vector2d.h"
#include "OWL/Primitives.h"

namespace OWL {
namespace Primitive {

struct LaneGeometryJoint
{
    /// \brief Tripple describing the sampled points of the lane
    /// \note Left/Right refers to the direction of the road
    struct Points
    {
        Common::Vector2d left;
        Common::Vector2d reference;
        Common::Vector2d right;
    } points;

    double curvature;              //!< \brief Curvature of the reference line at this joint
    double sOffset;     //!< \brief Offset with respect to the start of the road
    double sHdg;        //!< \brief Heading of the s projection axis
};

} // namespace Primitive
} // namespace OWL
