/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2020 BMW AG
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

namespace EventDefinitions {

enum struct EventCategory
{
    OpenPASS,
    OpenSCENARIO
};

namespace utils {

static constexpr std::array<const char *, 2> EventCategoryMapping{
    "OpenPASS",
    "OpenSCENARIO"};

constexpr auto GetAsString(EventCategory category)
{
    return EventCategoryMapping[static_cast<size_t>(category)];
}

} // namespace utils

} //namespace EventDefinitions
