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

namespace openpass::type {

template <typename T, typename Tag>
struct NamedType
{
    T value;

    NamedType(T value) :
        value(value)
    {
    }

    operator T() const
    {
        return value;
    }
};

} // namespace openpass::type
