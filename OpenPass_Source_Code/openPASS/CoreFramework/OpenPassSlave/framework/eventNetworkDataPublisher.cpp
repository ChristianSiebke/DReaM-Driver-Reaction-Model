/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "eventNetworkDataPublisher.h"

#include "scheduler/timeKeeper.h"

namespace openpass::publisher {

void EventNetworkDataPublisher::Publish(const openpass::narrator::EventBase &event)
{
    dataStore->PutAcyclic(openpass::scheduling::TimeKeeper::Now(), openpass::type::CoreId, event.name, Acyclic(event));
}

} // namespace openpass::publisher
