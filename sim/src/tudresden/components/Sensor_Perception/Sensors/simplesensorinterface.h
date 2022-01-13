/******************************************************************************
 * Copyright (c) 2021 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *****************************************************************************/
#ifndef SIMPLESENSORINTERFACE_H
#define SIMPLESENSORINTERFACE_H

#include "Objects/observedworldobject.h"
#include "include/agentInterface.h"

class SimpleSensorInterface {
public:
    SimpleSensorInterface(const AgentInterface *egoAgent, WorldInterface *world) : egoAgent(egoAgent), world(world) {
    }
    SimpleSensorInterface(const SimpleSensorInterface &) = delete;
    SimpleSensorInterface(SimpleSensorInterface &&) = delete;
    SimpleSensorInterface &operator=(const SimpleSensorInterface &) = delete;
    SimpleSensorInterface &operator=(SimpleSensorInterface &&) = delete;
    virtual ~SimpleSensorInterface() = default;

protected:
    const AgentInterface *egoAgent;
    WorldInterface *world;
};

#endif // SIMPLESENSORINTERFACE_H
