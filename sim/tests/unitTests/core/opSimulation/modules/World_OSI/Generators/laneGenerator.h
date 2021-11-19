/********************************************************************************
 * Copyright (c) 2018 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "OWL/DataTypes.h"
#include "laneGeometryElementGenerator.h"

namespace OWL {
namespace Testing {

struct LaneGeometryElementsGenerator {

    std::list<OWL::Interfaces::LaneGeometryElements*> laneGeometryElementsContainer;

    const OWL::Interfaces::LaneGeometryElements& SimpleLane(
            Common::Vector2d origin,
            double width,
            int numberOfElements,
            double elementLength,
            double hdg = 0.0
            )
    {
        auto laneGeometryElements = new OWL::Interfaces::LaneGeometryElements();

        auto element = LaneGeometryElementGenerator::RectangularLaneGeometryElement(origin, width, elementLength, hdg);
        (double &)element.joints.current.projectionAxes.sOffset = 0;

        laneGeometryElements->push_back(new OWL::Primitive::LaneGeometryElement(element));
        auto currentOrigin = element.joints.next.points.reference;

        for(int i = 1; i < numberOfElements; ++i)
        {
            auto element = LaneGeometryElementGenerator::RectangularLaneGeometryElement(currentOrigin, width, elementLength, hdg);
            (double &)element.joints.current.projectionAxes.sOffset = i * elementLength;
            (double &)element.joints.next.projectionAxes.sOffset = (i + 1) * elementLength;
            laneGeometryElements->push_back(new OWL::Primitive::LaneGeometryElement(element));
            currentOrigin = element.joints.next.points.reference;
        }

        laneGeometryElementsContainer.push_back(laneGeometryElements);
        return *laneGeometryElements;
    }

    ~LaneGeometryElementsGenerator()
    {
        for(auto laneGeometryElements : laneGeometryElementsContainer) {
            for(auto element : *laneGeometryElements) {
                delete element;
            }
            delete laneGeometryElements;
        }
    }
};

} // namespace Testing
} // namespace OWL
