/********************************************************************************
 * Copyright (c) 2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#ifndef JUNCTION_H
#define JUNCTION_H


#include "include/roadInterface/junctionInterface.h"
#include "connection.h"

#include <list>
#include <map>
#include <stdio.h>


class Junction : public JunctionInterface
{
public:
    Junction(const std::string &id)
    {
        this->id = id;
    }
    ~Junction() override;

    virtual ConnectionInterface* AddConnection(std::string id,std::string incomingRoadId, std::string connectingRoadId, ContactPointType contactPoint) override;

    virtual void AddPriority(Priority priority) override;

    virtual std::map<std::string, ConnectionInterface*> GetConnections() const override;

    virtual const std::vector<Priority>& GetPriorities() const override;

    std::string GetId() const override;

private:
    std::string id;
    std::map<std::string, ConnectionInterface*> connections;
    std::vector<Priority> priorities;
};

#endif // JUNCTION_H
