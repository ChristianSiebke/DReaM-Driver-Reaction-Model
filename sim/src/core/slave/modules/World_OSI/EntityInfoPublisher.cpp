/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "EntityInfoPublisher.h"

#include "include/dataStoreInterface.h"

namespace openpass::publisher {

void EntityInfoPublisher::Publish(openpass::type::EntityId entityId, const openpass::type::EntityInfo &entityInfo, bool persistent)
{
    const auto keyPrefix = std::string(persistent ? PERSISTENT_ENTITIES_ROOT : NONPERSISTENT_ENTITIES_ROOT) + "/" + std::to_string(entityId.value) + "/";

    dataStore->PutStatic(keyPrefix + "Source", entityInfo.source, persistent);
    for (const auto &[key, value] : entityInfo.parameter)
    {
        dataStore->PutStatic(keyPrefix + "Metainfo/" + key, value, persistent);
    }
}

} // namespace openpass::publisher