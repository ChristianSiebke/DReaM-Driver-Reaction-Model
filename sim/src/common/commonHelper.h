/********************************************************************************
 * Copyright (c) 2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <map>
#include <unordered_map>
#include <vector>

#include "math.h"

//-----------------------------------------------------------------------------
//! @brief defines common helper functions like conversion from and to enums.
//-----------------------------------------------------------------------------
namespace CommonHelper
{
static constexpr double EPSILON = 0.001; //!Treat values smaller than epsilon as zero for geometric calculations

[[maybe_unused]] static inline constexpr bool DoubleEquality(double value1, double value2, double epsilon = EPSILON)
{
    return std::abs(value1 - value2) <= epsilon;
}

//! Returns the same angle but within the range [-PI, PI]
[[maybe_unused]] static inline double SetAngleToValidRange(double angel)
{
    return std::fmod(angel + 3 * M_PI, 2 * M_PI) - M_PI;
}

//-----------------------------------------------------------------------------
//! @brief Calculate linear interpolated points with constant spacing.
//!
//! @param[in] start            Start of interval
//! @param[in] end              End of interval
//! @param[in] totalPoints      Total number of points returned (includes start
//!                             and end point)
//!
//! @return                     Vector of interpolated points.
//-----------------------------------------------------------------------------
[[maybe_unused]] static std::vector<double> InterpolateLinear(const double start, const double end, const int totalPoints)
{
    std::vector<double> elements;

    for(int i = 0; i < totalPoints; ++i)
    {
        elements.push_back(start + (i * ((end-start) / (totalPoints-1))));
        //elements.push_back(start + (i * ((end-start+1) / (totalPoints))));  //not correct implemented yet
    }
    return elements;
}

template <typename T>
[[maybe_unused]] static inline constexpr T PerHourToPerSecond(T value) noexcept
{
    static_assert(std::is_floating_point_v<T>, "the value must be a floating point data type");
    return value/static_cast<T>(3600.0);
}

template <typename T>
[[maybe_unused]] static inline constexpr T KilometerPerHourToMeterPerSecond(T kmH) noexcept
{
    static_assert(std::is_floating_point_v<T>, "kmH must be a floating point data type");
    return kmH/static_cast<T>(3.6);
}
}; // end namespace CommonHelper
