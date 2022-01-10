/********************************************************************************
 * Copyright (c) 2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include "LocalizationElement.h"

namespace World {
namespace Localization {

class WorldToRoadCoordinateConverter
{
    const LocalizationElement& element;

public:
    WorldToRoadCoordinateConverter(const LocalizationElement& localizationElement) :
        element{localizationElement}
    {}

    bool IsConvertible(const Common::Vector2d& point) const;

    RoadPosition GetRoadCoordinate(const Common::Vector2d& point, double hdg) const;

    double GetS(const Common::Vector2d& point) const;

private:
    Common::Vector2d GetIntersectionPoint(const Common::Vector2d& point) const;
    double CalcS(const Common::Vector2d& intersectionPoint) const;
    double CalcT(const Common::Vector2d& point, const Common::Vector2d& intersectionPoint) const;
    double CalcYaw(double hdg) const;
    Common::Vector2d ProjectOntoReferenceAxis(const Common::Vector2d& point) const;

    bool IsLeftOfReferenceAxis(const Common::Vector2d& vector) const;

};

} // namespace Localization
} // namespace World

