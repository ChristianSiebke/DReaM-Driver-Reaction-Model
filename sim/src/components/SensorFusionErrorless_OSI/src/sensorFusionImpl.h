/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

/** \addtogroup SensorFusionErrorless
* @{
* \brief This file models the SensorFusionErrorless.
*
* \details This file models the SensorFusionErrorless which can be part of an agent.
*          This module gets OSI SensorData of the SensorAggregation and combines all
*          object with the same id into one.
*
* \section MODULENAME_Inputs Inputs
* Input variables:
* name | meaning
* -----|------
* sensorData | SensorData of a single sensor.
*
* Input channel IDs:
* Input Id | signal class | contained variables
* ----------|--------------|-------------
*  0 		| SensorDataSignal  | sensorData
*
* \section MODULENAME_Outputs Outputs
* Output variables:
* name | meaning
* -----|------
* out_sensorData | Combined SensorData from all sensors.
*
* Output channel IDs:
* Output Id | signal class | contained variables
* ----------|--------------|-------------
*  0 		| SensorDataSignal  | out_sensorData
*
* @} */

#pragma once

#include "include/modelInterface.h"
#include "common/sensorDataSignal.h"
#include "osi3/osi_sensordata.pb.h"

//-----------------------------------------------------------------------------
/** \brief This class is the SensorFusionErrorless module.
* 	\details This class contains all logic regarding the sensor fusion.
*
* 	\ingroup SensorFusionErrorless
*/
//-----------------------------------------------------------------------------
class SensorFusionErrorlessImplementation : public UnrestrictedModelInterface
{
public:
    const std::string COMPONENTNAME = "SensorFusion";

    SensorFusionErrorlessImplementation(
            std::string componentName,
            bool isInit,
            int priority,
            int offsetTime,
            int responseTime,
            int cycleTime,
            StochasticsInterface *stochastics,
            WorldInterface *world,
            const ParameterInterface *parameters,
            PublisherInterface * const publisher,
            const CallbackInterface *callbacks,
            AgentInterface *agent);

    SensorFusionErrorlessImplementation(const SensorFusionErrorlessImplementation&) = delete;
    SensorFusionErrorlessImplementation(SensorFusionErrorlessImplementation&&) = delete;
    SensorFusionErrorlessImplementation& operator=(const SensorFusionErrorlessImplementation&) = delete;
    SensorFusionErrorlessImplementation& operator=(SensorFusionErrorlessImplementation&&) = delete;
    virtual ~SensorFusionErrorlessImplementation() = default;

    virtual void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time);

    void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time);

    virtual void Trigger(int time);

private:

    void MergeSensorData(const osi3::SensorData& in_SensorData);

    osi3::SensorData out_sensorData;
};


