/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#pragma once

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
