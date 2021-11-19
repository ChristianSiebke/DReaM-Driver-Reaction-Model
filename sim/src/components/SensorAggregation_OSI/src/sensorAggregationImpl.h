/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

/** \addtogroup SensorAggregation
* @{
* \brief This file models the SensorAggregation.
*
* \details This file models the SensorAggregation which can be part of an agent.
*          This module gets OSI SensorData of all sensors of the vehicle and forwards a combined
*          SensorData to the driver assistance systems.
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
/** \brief This class is the SensorAggregation module.
* 	\details This class contains all logic regarding the sensor fusion.
*
* 	\ingroup SensorAggregation
*/
//-----------------------------------------------------------------------------
class SensorAggregationImplementation : public UnrestrictedModelInterface
{
public:
    const std::string COMPONENTNAME = "SensorFusion";

    SensorAggregationImplementation(
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

    SensorAggregationImplementation(const SensorAggregationImplementation&) = delete;
    SensorAggregationImplementation(SensorAggregationImplementation&&) = delete;
    SensorAggregationImplementation& operator=(const SensorAggregationImplementation&) = delete;
    SensorAggregationImplementation& operator=(SensorAggregationImplementation&&) = delete;
    virtual ~SensorAggregationImplementation() = default;

    /*!
     * \brief Update Inputs
     *
     * Function is called by framework when another component delivers a signal over
     * a channel to this component (scheduler calls update taks of other component).
     *
     * Refer to module description for input channels and input ids.
     *
     * \param[in]     localLinkId    Corresponds to "id" of "ComponentInput"
     * \param[in]     data           Referenced signal (copied by sending component)
     * \param[in]     time           Current scheduling time
     */
    virtual void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time);

    /*!
     * \brief Update outputs.
     *
     * Function is called by framework when this Component.has to deliver a signal over
     * a channel to another component (scheduler calls update task of this component).
     *
     * Refer to module description for output channels and output ids.
     *
     * \param[in]     localLinkId    Corresponds to "id" of "ComponentOutput"
     * \param[out]    data           Referenced signal (copied by this component)
     * \param[in]     time           Current scheduling time
     */
    void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time);

    /*!
     * \brief Process data within component.
     *
     * Function is called by framework when the scheduler calls the trigger task
     * of this component.
     *
     * Refer to module description for information about the module's task.
     *
     * \param[in]     time           Current scheduling time
     */
    virtual void Trigger(int time);

private:
    int previousTimeStamp {0};
    osi3::SensorData out_sensorData;
};


