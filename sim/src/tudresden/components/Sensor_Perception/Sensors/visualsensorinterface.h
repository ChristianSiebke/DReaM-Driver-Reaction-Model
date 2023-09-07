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

#include <bits/stl_list.h>
#include <bits/stl_map.h>
#include <boost/geometry.hpp>

#include "Objects/observedworldobject.h"
#include "simplesensorinterface.h"
#include "common/vector2d.h"

template <class T> class VisualSensorInterface : protected SimpleSensorInterface {
public:
    VisualSensorInterface(AgentInterface *egoAgent, WorldInterface *world) : SimpleSensorInterface(egoAgent, world) {
    }
    virtual ~VisualSensorInterface() = default;

    virtual void Trigger(int timestamp, GazeState gazeState) = 0;

    virtual std::vector<T> GetVisible() = 0;
};
