/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2018-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  SensorFusionQuery.h
//! @brief This file provides methods to retrieve structured basic information
//!        from sensorData provided by SensorFusionImplementation::Output().
//-----------------------------------------------------------------------------

#pragma once

#include <map>
#include <vector>

#include "osi3/osi_sensordata.pb.h"

namespace SensorFusionHelperFunctions
{
    std::vector<osi3::DetectedMovingObject> RetrieveMovingObjectsBySensorId(std::vector<int> sensorIds, const osi3::SensorData &sensorData)
    {
        std::vector<osi3::DetectedMovingObject> result;
        auto detectedMovingObjects = sensorData.moving_object();
        for (const auto& object : detectedMovingObjects)
        {
            for (auto sensorId : object.header().sensor_id())
            {
                if(std::count(sensorIds.cbegin(), sensorIds.cend(), sensorId.value()) > 0)
                {
                    result.push_back(object);
                    break;
                }
            }
        }
        return result;
    }

    std::vector<osi3::DetectedStationaryObject> RetrieveStationaryObjectsBySensorId(std::vector<int> sensorIds, const osi3::SensorData &sensorData)
    {
        std::vector<osi3::DetectedStationaryObject> result;
        auto detectedStationaryObjects = sensorData.stationary_object();
        for (const auto& object : detectedStationaryObjects)
        {
            for (auto sensorId : object.header().sensor_id())
            {
                if(std::count(sensorIds.cbegin(), sensorIds.cend(), sensorId.value()) > 0)
                {
                    result.push_back(object);
                    break;
                }
            }
        }
        return result;
    }
};
