/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "coreDataPublisher.h"

#include "timeKeeper.h"

namespace openpass::publisher {

void CoreDataPublisher::Publish(const openpass::datastore::Key &key, const openpass::publisher::LogEntryBase &event)
{
    dataStore->PutAcyclic(openpass::scheduling::TimeKeeper::Now(), CORE_ID, key, event);
}

} // namespace openpass::publisher