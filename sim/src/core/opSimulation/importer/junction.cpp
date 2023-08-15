/********************************************************************************
 * Copyright (c) 2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "junction.h"



Junction::~Junction()
{

}

ConnectionInterface *Junction::AddConnection(std::string id, std::string incomingRoadId, std::string connectingRoadId, ContactPointType contactPoint)
{
    std::shared_ptr<ConnectionInterface> connection = std::make_shared<Connection>(id, incomingRoadId, connectingRoadId, contactPoint);

    if(!connection)
    {
        return nullptr;
    }

    connections.insert({id, connection});

    return connection.get();
}

void Junction::AddPriority(Priority priority)
{
    priorities.emplace_back(priority);
}

std::map<std::string, std::shared_ptr<ConnectionInterface>> Junction::GetConnections() const {
    return connections;
}

const std::vector<Priority> &Junction::GetPriorities() const
{
    return  priorities;
}

std::string Junction::GetId() const
{
    return id;
}

