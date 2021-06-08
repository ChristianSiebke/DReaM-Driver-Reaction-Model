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

#include <string>

#include "common/openPassTypes.h"

namespace openpass::type {

/**
 * @brief Data capsule for publishing entity metainfo into the databuffer
 */
struct EntityInfo
{
    std::string source;                      //!< Source of entity, such as OpenDRIVE
    openpass::type::FlatParameter parameter; //!< Arbitrary payload using key/value pairs
};

} // namespace openpass::type