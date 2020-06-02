/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <vector>

namespace  RamerDouglasPeucker {

constexpr double ERROR_THRESHOLD = 0.05;

//! Uses the Ramer-Douglas-Peucker algorithm to reduce the number of joints while guaranteeing a maximum lateral error
//! See https://en.wikipedia.org/wiki/Ramer-Douglas-Peucker_algorithm
template<typename T>
std::vector<T> Simplify (const typename std::vector<T>::const_iterator& begin, const typename std::vector<T>::const_iterator& end)
{
    double maxSquaredError{-1};
    typename std::vector<T>::const_iterator jointWithMaxError{begin};
    assert(begin < end);
    for (auto joint = begin; joint <= end; joint++)
    {
        auto squaredError = joint->GetSquaredError(*begin, *end);
        if (squaredError > maxSquaredError)
        {
            maxSquaredError = squaredError;
            jointWithMaxError = joint;
        }
    }
    if (maxSquaredError > ERROR_THRESHOLD * ERROR_THRESHOLD)
    {
        auto firstJoints = Simplify<T>(begin, jointWithMaxError);
        auto secondJoints = Simplify<T>(jointWithMaxError, end);

        firstJoints.insert(firstJoints.end(), secondJoints.cbegin() + 1, secondJoints.cend());
        return firstJoints;
    }
    else
    {
        return {*begin, *end};
    }
}

//! Uses the Ramer-Douglas-Peucker algorithm to reduce the number of joints while guaranteeing a maximum lateral error
//! See https://en.wikipedia.org/wiki/Ramer-Douglas-Peucker_algorithm
template<typename T>
std::vector<T> Simplify (const std::vector<T>& joints)
{
    return Simplify<T>(joints.cbegin(), joints.cend() - 1);
}
}
