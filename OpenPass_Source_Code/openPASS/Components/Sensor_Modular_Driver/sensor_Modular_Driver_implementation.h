/******************************************************************************
* Copyright (c) 2019 TU Dresden
* Copyright (c) 2019 AMFD GmbH
*
* This program and the accompanying materials are made available under the
* terms of the Eclipse Public License 2.0 which is available at
* https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*****************************************************************************/

//-----------------------------------------------------------------------------
//! @file  sensor_Modular_Driver_implementation.h
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief provide all for the driver relevant information
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_ModularDriver
//-----------------------------------------------------------------------------


#ifndef SENSOR_MODULAR_DRIVER_IMPLEMENTATION_H
#define SENSOR_MODULAR_DRIVER_IMPLEMENTATION_H

#include <math.h>
#include "modelInterface.h"
#include "observationInterface.h"
#include "complexsignals.h"
#include "primitiveSignals.h"
#include "WorldData.h"
#include "ContainerStructures.h"

/*!
 * \addtogroup Components_Basic openPASS components basic
 * @{
 * \addtogroup Sensor_Modular_Driver
 * @{
 * \brief basic module to sense the current state of an agent
 *
 * This module provides information about the Test of the agent to the agent in front
 * and the current velocity of the agent.
 *
 * \section Sensor_Modular_Driver_Inputs Inputs
 * none
 *
 * \section Sensor_Modular_Driver_InitInputs Init Inputs
 * none
 *
 * \section Sensor_Modular_Driver_Outputs Outputs
 * Output variables:
 * name | meaning
 * -----|------
 * distance2NextAgent             | The distance between this agent and agent in front
 * out_agentVelocity              | The velocity of the agent
 *
 * Output channel IDs:
 * Output Id | signal class | contained variables
 * ------------|--------------|-------------
 * 0 | DoubleSignal | distance2NextAgent
 * 1 | DoubleSignal | out_agentVelocity
 *
 * \section Sensor_Modular_Driver_ExternalParameters External parameters
 * none
 *
 * \section Sensor_Modular_Driver_ConfigParameters Parameters to be specified in agentConfiguration.xml
 * none
 *
 * \section Sensor_Modular_Driver_InternalParameters Internal paramters
 * none
 *
 * @}
 * @} */

/*!
* \brief This module is a basic module to sense the current state of an agent.
*
* This module provides information about the distance of the agent to the agent in front
* and the current velocity of the agent.
*
* \ingroup Sensor_Distance
*/


template<class T>
class structSignal;

class Sensor_Modular_Driver_Implementation : public SensorInterface
{
public:
    const std::string COMPONENTNAME = "Sensor_Modular_Driver";

    Sensor_Modular_Driver_Implementation(std::string componentName,
                                   bool isInit,
                                   int priority,
                                   int offsetTime,
                                   int responseTime,
                                   int cycleTime,
                                   StochasticsInterface *stochastics,
                                   WorldInterface *world,
                                   const ParameterInterface *parameters,
                                   const std::map<int, ObservationInterface *> *observations,
                                   const CallbackInterface *callbacks,
                                   AgentInterface *agent);
    virtual ~Sensor_Modular_Driver_Implementation() = default;

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
    virtual void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data,
                             int time);

    /*!
     * \brief Update outputs.
     *
     * Function is called by framework when this component has to deliver a signal over
     * a channel to another component (scheduler calls update task of this component).
     *
     * Refer to module description for output channels and output ids.
     *
     * \param[in]     localLinkId    Corresponds to "id" of "ComponentOutput"
     * \param[out]    data           Referenced signal (copied by this component)
     * \param[in]     time           Current scheduling time
     */
    virtual void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time);

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

    void SetEgoData(AgentInterface *agent);

    void SetMovingObjects(AgentInterface *agent, const std::map<int, AgentInterface*> *Agents);

    void SetEnvironment(AgentInterface *agent);

    //! \brief Get sensor data containing traffic rule information.
    virtual void GetTrafficRuleInformation();

    //! \brief Get traffic rule sensor data from the ego lane.
    LaneInformationTrafficRules GetTrafficRuleLaneInformationEgo();

    //! \brief Get traffic rule sensor data from the lane left of ego.
    LaneInformationTrafficRules GetTrafficRuleLaneInformationLeft();

    //! \brief Get traffic rule sensor data from the lane right of ego.
    LaneInformationTrafficRules GetTrafficRuleLaneInformationRight();

    //! \brief Get lane geometry sensor data.
    virtual void GetRoadGeometry();

    //! \brief Get lane geometry sensor data from the ego lane.
    void GetGeometryLaneInformationEgo(LaneInformation &laneInformation);

    //! \brief Get lane geometry sensor data from the lane left of ego.
    void GetGeometryLaneInformationLeft(LaneInformation &laneInformation);

    //! \brief Get lane geometry sensor data from the lane right of ego.
    void GetGeometryLaneInformationRight(LaneInformation &laneInformation);

    void SetMentalModelGeometryLaneInformation();

    //!
    //! \brief ConvertRoadToMMLane
    //! Checks special lane to save it with its geometrical description
    //! \param lane     OWL-Lane with all of its information
    //! \param OdId     openDriveId
    //!
    void ConvertRoadToMMLane(OWL::Lane* lane, int64_t OdId);

    //!
    //! \brief GetCurrentOsiLaneId
    //! Transforms the laneID from openDrive2OWL
    //! \param odRoadId     Id of Road in openDrive-Format
    //! \return             Id of Road in OWL-Format
    //!
    uint64_t GetCurrentOsiLaneId(std::string odRoadId)
    {
        std::unordered_map<uint64_t, std::string> RoadMap = static_cast<OWL::WorldData*>(GetWorld()->GetWorldData())->GetRoadIdMapping();
        for (auto it = RoadMap.begin(); it!=RoadMap.end(); ++it)
        {
            if (it->second == odRoadId)
                return it->first;
        }
    }

    struct DistanceComparator
    {
        bool operator ()(SurroundingMovingObjectsData &Obj1, SurroundingMovingObjectsData &Obj2)
        {
            return *Obj1.GetDistanceToEgo() < *Obj2.GetDistanceToEgo();
        }
    };

private:

    StaticEnvironmentData StaticEnvironment;
    egoData Ego;
    std::list<SurroundingMovingObjectsData> SurroundingMovingObjects;

    std::map<int, std::vector<MentalModelLane>> lanes;
    OWL::Road* _currentRoad = nullptr;

};

#endif // SENSOR_MODULAR_DRIVER_IMPLEMENTATION_H
