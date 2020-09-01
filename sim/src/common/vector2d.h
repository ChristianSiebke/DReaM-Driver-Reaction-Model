/******************************************************************************
* Copyright (c) 2017 ITK Engineering GmbH.
*               2018, 2020 in-tech GmbH.
*               2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made available under the
* terms of the Eclipse Public License 2.0 which is available at
* https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

//! \brief This file implements a generic 2D vector.

#pragma once

#include <ostream>
#include <cmath>
#include "common/opExport.h"

namespace Common {

/*!
 * class for 2d vectors in cartesian coordinate system
 */
class OPENPASSCOMMONEXPORT Vector2d final
{
public:
    static double constexpr EPSILON = 1e-9;

    double x;
    double y;

    Vector2d(const Vector2d &) = default;
    Vector2d(Vector2d &&) = default;
    Vector2d &operator=(const Vector2d &) = default;
    Vector2d &operator=(Vector2d &&) = default;

    /*!
     * create 2d vector from pair (x,y)
     *
     * \param[in] x     x-value
     * \param[in] y     y-value
     */
    constexpr Vector2d(double x = 0, double y = 0) noexcept : x(x), y(y) {}

    /*!
     * translation of vector
     *
     * \param[in] x    x-value of displacement vector
     * \param[in] y    y-value of displacement vector
     */
    constexpr void Translate(double x, double y) noexcept
    {
        this->x += x;
        this->y += y;
    }

    /*!
     * \brief Translate
     * translation of vector via another vector
     * \param[in] translationVector vector of translation
     */
    constexpr void Translate(Vector2d translationVector) noexcept
    {
        this->x += translationVector.x;
        this->y += translationVector.y;
    }

    /*!
     * rotates vector by angle
     *
     * \param[in] angle     angle, in radians
     */
    void Rotate(double angle) noexcept
    {
        double cosAngle = std::cos(angle);
        double sinAngle = std::sin(angle);
        *this = Vector2d(x * cosAngle - y * sinAngle,
                         x * sinAngle + y * cosAngle);
    }

    /*!
     * scales vector by a factor
     *
     * \param[in] scale     scaling factor
     */
    constexpr void Scale(double scale) noexcept
    {
        x *= scale;
        y *= scale;
    }

    /*!
     * adds a vector
     *
     * \param[in] in     added 2d vector
     */
    constexpr void Add(const Vector2d &in) noexcept
    {
        x += in.x;
        y += in.y;
    }

    /*!
     * subtracts a vector
     *
     * \param[in] in     subtracted 2d vector
     */
    constexpr void Sub(const Vector2d &in) noexcept
    {
        x -= in.x;
        y -= in.y;
    }

    /*!
     * scalar product / dot product
     *
     * \param[in] in      2d vector
     * \return returns dot product of the 2 vectors
     */
    constexpr double Dot(const Vector2d &in) const noexcept
    {
        return x * in.x + y * in.y;
    }

    /*!
     * cross product with Z=0
     *
     * \param[in] in      2d vector
     * \return returns z-component of the cross product
     */
    constexpr double Cross(const Vector2d &in) const noexcept
    {
        return x * in.y - y * in.x;
    }

    /*!
     * Normalizes the 2d vector
     *
     * Each component of the vector is devided by the length of the vector.
     *
     * In case of a vector with length 0, the vector cannot be normalized and false is returned.
     *
     * \return returns true if vector could be normalized, false otherwise
     */
    bool Norm()
    {
        double length = Length();

        if (std::abs(length) < EPSILON)
        {
            return false;
        }

        x /= length;
        y /= length;

        return true;
    }

    /*!
     * returns length of the vector
     *
     * \return length of the vector
     */
    double Length() const noexcept
    {
        return std::hypot(x, y);
    }

    /*!
     * \brief Angle
     * returns the angle of the vector [-pi,+pi]
     * \return angle of vector
     */
    double Angle() const noexcept
    {
        return atan2(y, x);
    }

    constexpr Vector2d operator-(const Vector2d &in) const noexcept
    {
        return Vector2d(x - in.x, y - in.y);
    }

    constexpr Vector2d operator+(const Vector2d &in) const noexcept
    {
        return Vector2d(x + in.x, y + in.y);
    }

    constexpr Vector2d operator*(double in) const noexcept
    {
        return Vector2d(x * in, y * in);
    }

    /*!
     * \brief Comparison operator taking EPSILON of 1e-9 into account
     *
     * \param    in[in]    Vector to compare to
     *
     * \return   true if vectors are considered equal, false otherwise
     */
    constexpr bool operator==(const Vector2d &in) const noexcept
    {
        return (std::abs(x - in.x) < EPSILON) &&
               (std::abs(y - in.y) < EPSILON);
    }

    /*!
     * \brief Inequality operator taking EPSILON of 1e-9 into account
     *
     * \param    in[in]    Vector to compare to
     *
     * \return   false if vectors are considered equal, true otherwise
     */
    constexpr bool operator!=(const Vector2d &in) const noexcept
    {
        return !operator==(in);
    }

    /// \brief Overload << operator for Vector2d
    friend std::ostream& operator<<(std::ostream& os, const Vector2d& vector)
    {
        return os << "(" << vector.x << ", " << vector.y << ")";
    }

};

struct Line
{
    explicit constexpr Line(const Common::Vector2d& startPoint,  const Common::Vector2d& endPoint) noexcept :
        startPoint{startPoint},
        directionalVector{endPoint - startPoint}{}

    Common::Vector2d startPoint;
    Common::Vector2d directionalVector;
};

} // namespace Common
