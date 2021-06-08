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

#include "EntityInfo.h"

class DataStoreWriteInterface;

namespace openpass::publisher {

static constexpr char PERSISTENT_ENTITIES_ROOT[]{"Entities/Persistent"};
static constexpr char NONPERSISTENT_ENTITIES_ROOT[]{"Entities/NonPersistent"};

/**
 * @brief This class acts as specialized interface for publishing Entity Information into the datastore
 */
class EntityInfoPublisher
{
public:
    EntityInfoPublisher(DataStoreWriteInterface *const dataStore) :
        dataStore(dataStore)
    {
    }

    /**
     * @brief Publish an entity
     * 
     * @param entityId   Unique ID
     * @param entityInfo Metainfo for of the entity
     * @param persistent If true, the information will be available for all (following) runs (e.g. used for static traffic signs).
     *                   If false, it will be cleared at the end of an invocation (e.g. used for moving objects).
     */
    void Publish(openpass::type::EntityId entityId, const openpass::type::EntityInfo &entityInfo, bool persistent);

private:
    DataStoreWriteInterface *const dataStore;
};

} // namespace openpass::publisher