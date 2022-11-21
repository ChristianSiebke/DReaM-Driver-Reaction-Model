/********************************************************************************
 * Copyright (c) 2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include "include/worldObjectInterface.h"

using OpenDriveId = std::string;

class TrafficObjectInterface : public virtual WorldObjectInterface
{
public:
    TrafficObjectInterface() = default;
    TrafficObjectInterface(const TrafficObjectInterface&) = delete;
    TrafficObjectInterface(TrafficObjectInterface&&) = delete;
    TrafficObjectInterface& operator=(const TrafficObjectInterface&) = delete;
    TrafficObjectInterface& operator=(TrafficObjectInterface&&) = delete;
    virtual ~TrafficObjectInterface() = default;

    virtual OpenDriveId GetOpenDriveId() const = 0;
};
