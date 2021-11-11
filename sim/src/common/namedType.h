/********************************************************************************
 * Copyright (c) 2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

/*******************************************************************************
The concept of named types (aka strong types) goes back to
Jonathan Boccara (https://www.fluentcpp.com/) ideas, found at:

https://github.com/joboccara/NamedType (MIT License)
Even if we do not use the full functionality, it its clearly a modification:

MIT License

Copyright (c) 2017 Jonathan Boccara

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

#pragma once

#include <functional>

namespace openpass::type {

template <typename T, typename Tag>
struct NamedType
{
    T value;

    constexpr NamedType(T value) noexcept:
        value(value)
    {
    }

    constexpr operator T() const
    {
        return value;
    }

    constexpr bool operator==(NamedType<T, Tag> const& other) const noexcept
    {
        return this->value == other.value;
    }

    bool operator!=(NamedType<T, Tag> const& other) const
    {
        return this->value != other.value;
    }

    constexpr bool operator==(T const& other) const
    {
        return this->value == other;
    }

    bool operator!=(T const& other) const
    {
        return this->value != other;
    }
};

} // namespace openpass::type
