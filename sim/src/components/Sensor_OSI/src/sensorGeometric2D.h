/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
* Copyright (c) 2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  SensorGeometric2D.h
*	\brief This file models a sensor which only detects agents in a 2D area (x/y) in front of the agent
*/
//-----------------------------------------------------------------------------

#pragma once

#include "core/slave/modules/World_OSI/WorldData.h"
#include "objectDetectorBase.h"
#include "osi3/osi_sensorview.pb.h"
#include "osi3/osi_sensordata.pb.h"

struct ObjectView
{
    long objectId;
    double minAngle;
    double maxAngle;
    double distance;
};

struct SensorDetectionResults
{
    std::vector<osi3::MovingObject> visibleMovingObjects;
    std::vector<osi3::MovingObject> detectedMovingObjects;
    std::vector<osi3::StationaryObject> visibleStationaryObjects;
    std::vector<osi3::StationaryObject> detectedStationaryObjects;
};

//-----------------------------------------------------------------------------
/** \brief This file models a sensor which only detects agents in a 2D area (x/y) in front of the agent
* 	\details This sensor does not consider height.
*
* 	\ingroup SensorObjectDetector
*/
//-----------------------------------------------------------------------------
class SensorGeometric2D : public ObjectDetectorBase
{
public:
    SensorGeometric2D(
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

    SensorGeometric2D(const SensorGeometric2D&) = delete;
    SensorGeometric2D(SensorGeometric2D&&) = delete;
    SensorGeometric2D& operator=(const SensorGeometric2D&) = delete;
    SensorGeometric2D& operator=(SensorGeometric2D&&) = delete;
    ~SensorGeometric2D() = default;

    void UpdateInput(int, const std::shared_ptr<SignalInterface const> &, int);
    void Trigger(int time);

    /**
     * \brief Calculate which objects are inside the detection field
     *
     * For further explanation of the calculation see the [documentation](\ref dev_agent_modules_geometric2d)
    */
    SensorDetectionResults DetectObjects();

protected:
    /*!
    * \brief Generates the SensorViewConfiguration to send the world data for filtering of the SensorView
    *
    * @return SensorViewConfiguration for parametrization of Sensorview
    */
    osi3::SensorViewConfiguration GenerateSensorViewConfiguration() override;

private:
    void Observe(const int time, const SensorDetectionResults& results);
    SensorDetectionResults ApplyLatencyToResults(const int time, const SensorDetectionResults& results);
    /**
     * \brief Calculate if objects in the detection field visually obstruct other objects and remove those whose visibility percentage is below the threshold
     *
     * For further explanation of the calculation see the [documentation](\ref dev_agent_modules_geometric2d_obstruction)
     *
     * \param movingObjects         list of moving objects in the detection field for which visual obstruction should be applied
     * \param stationaryObjects     list of stationary objects in the detection field for which visual obstruction should be applied
     * \param sensorPositionGlobal  sensor postion in global coordinates
    */
    template<typename T>
    std::pair<std::vector<T>, std::vector<T>> CalcVisualObstruction(const std::vector<const T*>& objects,
                                                                                  const multi_polygon_t& brightArea);

    /**
     * Calculate the polygon to approximate the detection area
     *
     * \returns bright area polygon
    */
    polygon_t CalcInitialBrightArea(point_t sensorPosition);

    /**
     * Calculate the shadow drawn by an object with the sensorPosition as light source
     *
     * \param boundingBox  boundingBox of the objects which draws the shadow
     * \param sensorPosition Position of the sensor (light source)
     * \returns shadow polygon
    */
    multi_polygon_t CalcObjectShadow(const polygon_t& boundingBox, point_t sensorPosition);

    /**
     * Calculate how many percent of an object are inside the bright area
     * \param boundingBox boundingBox of the object
     * \param brightArea
     * \returns percentage of the visible area of the object
     */
    static double CalcObjectVisibilityPercentage(const polygon_t& boundingBox, const multi_polygon_t &brightArea);
    
    /*!
     * \brief Returns true if opening angle is smaller than pi
     */
    bool OpeningAngleWithinHalfCircle() const;

    /*!
     * \brief Returns true if opening angle is smaller than two pi
     */
    bool OpeningAngleWithinFullCircle() const;

    /*!
     * \brief Creates the detection field for angles smaller than pi
     */
    polygon_t CreateFourPointDetectionField() const;

    /*!
     * \brief Creates the detection field for angles between pi and two pi
     */
    polygon_t CreateFivePointDetectionField() const;

    std::pair<point_t, polygon_t> CreateSensorDetectionField(const osi3::MovingObject* hostVehicle) const;

    template<typename T>
    void ApplyVisualObstructionToDetectionArea(multi_polygon_t& brightArea,
                                               const point_t& sensorPositionGlobal,
                                               const std::vector<const T*>& objects);
    template<typename T>
    bool ObjectIsInDetectionArea(const T& object,
                                 const point_t& sensorPositionGlobal,
                                 const polygon_t& detectionField) const;

    std::pair<std::vector<const osi3::MovingObject*>, std::vector<const osi3::StationaryObject*>> GetObjectsInDetectionAreaFromSensorView(const osi3::SensorView& sensorView,
                                                                                                                                          const point_t& sensorPositionGlobal,
                                                                                                                                          const polygon_t& detectionField) const;

    std::string CreateAgentIdListString(const std::set<OWL::Id>& owlIds) const;

    bool enableVisualObstruction = false;
    double requiredPercentageOfVisibleArea = 0.001;
    double detectionRange;
    double openingAngleH;
    std::map<int, SensorDetectionResults> latentSensorDetectionResultsBuffer;

    static constexpr double MIN_VISIBLE_UNOBSTRUCTED_PERCENTAGE = 0.0001;
    static constexpr double WARNING_THRESHOLD_SCALE = 1e6;
};

