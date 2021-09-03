/*******************************************************************************
* Copyright (c) 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "coreDataPublisher.h"

namespace openpass::publisher {

void CoreDataPublisher::Publish(const openpass::databuffer::Key &key, const openpass::publisher::LogEntryBase &event)
{
    dataBuffer->PutAcyclic(CORE_ID, key, event);
}

} // namespace openpass::publisher
