/********************************************************************************
 * Copyright (c) 2018-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include <algorithm>

#include "OWL/Primitives.h"
#include "OWL/LaneGeometryJoint.h"
#include "common/commonTools.h"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

namespace OWL {

namespace Interfaces {
class Lane;
}

namespace Primitive {

namespace bg = boost::geometry;

using point_t = bg::model::d2::point_xy<double>;
using polygon_t = bg::model::polygon<point_t>;

using point_i_t = bg::model::point<int, 2, bg::cs::cartesian>;
using box_i_t = bg::model::box<point_i_t>;


struct LaneGeometryElement
{
    LaneGeometryElement(const LaneGeometryJoint current,
                        const LaneGeometryJoint next,
                        const OWL::Interfaces::Lane* lane) :
        joints
        {
            current,
            next
        },
      lane(lane)
    {

    }

    struct Joints
    {
        LaneGeometryJoint current;
        LaneGeometryJoint next;
    } const joints;

    const OWL::Interfaces::Lane* lane;
};

} // namespace Primitive
} // namespace OWL
