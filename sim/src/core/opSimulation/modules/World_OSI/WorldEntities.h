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

#include "EntityInfo.h"

namespace openpass::utils {

static constexpr char ENTITY_SOURCE[]{"OpenSCENARIO"};

/**
 * @brief Adapter function for publishing arbitrary OpenSCENARIO entities
 * 
 * @param metainfo The metainfo, which shall be published with the entity
 * @return The entity info 
 */
[[nodiscard]] const openpass::type::EntityInfo GetEntityInfo(const openpass::type::FlatParameter &metainfo)
{
    return {
        ENTITY_SOURCE,
        std::move(metainfo)};
}

} // namespace openpass::utils