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

#include "OWL/DataTypes.h"
#include "Common/vector2d.h"

namespace World {
namespace Localization {

class PointQuery
{
public:
    /// Check if point lies within (or at the boundaries) of a given geometry element
    ///
    /// \param[in] element  element object
    /// \param[in] point    2D point
    /// \return true, if point is within (or at the boundaries) of the element
    ///
    static bool IsWithin(const OWL::Primitive::LaneGeometryElement& element, const Common::Vector2d& point);

    /*
       currentJoint        |    nextJoint
                          |
              A           |
    left      #-------____|__      B
             /   P        |  ------#
            /    #        |         \
           /              |          \
    right #---------------|-----------#
          C               |            D
                          |
    */
    static bool IsWithin(const Common::Vector2d& A,
                         const Common::Vector2d& B,
                         const Common::Vector2d& C,
                         const Common::Vector2d& D,
                         const Common::Vector2d& P);

private:
    static bool WithinBarycentricCoords(double dot00,
                                        double dot02,
                                        double dot01,
                                        double dot11,
                                        double dot12);

    static bool OnEdge(const Common::Vector2d& A,
                       const Common::Vector2d& B,
                       const Common::Vector2d& P);
};

} // namespace Localization
} // namespace World
