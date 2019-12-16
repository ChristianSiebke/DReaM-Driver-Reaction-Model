/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
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

#include "WorldData.h"
#include "objectDetectorBase.h"
#include "osi/osi_sensorview.pb.h"
#include "osi/osi_sensordata.pb.h"

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
            const std::map<int, ObservationInterface*> *observations,
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
    static multi_polygon_t CalcObjectShadow(const polygon_t& boundingBox, point_t sensorPosition);

    /**
     * Calculate how many percent of an object are inside the bright area
     * \param boundingBox boundingBox of the object
     * \param brightArea
     * \returns percentage of the visible area of the object
     */
    static double CalcObjectVisibilityPercentage(const polygon_t& boundingBox, const multi_polygon_t &brightArea);
    
    /*!
     * \brief Adds the information of a detected moving object as DetectedMovingObject to the sensor data
     *
     * \param object            detected moving object
     * \param ownVelocity       velocity of own vehicle in global coordinates
     * \param ownAcceleration   acceleration of own vehicle in global coordinates
     * \param ownPosition       position of own vehicle in global coordinates
     * \param yaw               yaw of own vehicle in global coordinates
     * \param yawRate           yawRate of own vehicle in global coordinates
     */
    void AddMovingObjectToSensorData (osi3::MovingObject object, point_t ownVelocity, point_t ownAcceleration, point_t ownPosition, double yaw, double yawRate);

    /*!
     * \brief Adds the information of a detected stationary object as DetectedStationaryObject to the sensor data
     *
     * \param object            stationary moving object
     * \param ownPosition       position of own vehicle in global coordinates
     * \param yaw               yaw of own vehicle in global coordinates
     */
    void AddStationaryObjectToSensorData (osi3::StationaryObject object, point_t ownPosition, double yaw);
    
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

    point_t GetHostVehiclePosition(const osi3::MovingObject* hostVehicle) const;

    std::pair<point_t, polygon_t> CreateSensorDetectionField(const osi3::MovingObject* hostVehicle) const;
    template<typename T>
    static void ApplyVisualObstructionToDetectionArea(multi_polygon_t& brightArea,
                                                      const point_t& sensorPositionGlobal,
                                                      const std::vector<const T*>& objects);
    template<typename T>
    bool ObjectIsInDetectionArea(const T& object,
                                 const point_t& sensorPositionGlobal,
                                 const polygon_t& detectionField) const;

    std::pair<std::vector<const osi3::MovingObject*>, std::vector<const osi3::StationaryObject*>> GetObjectsInDetectionAreaFromSensorView(const osi3::SensorView& sensorView,
                                                                                                                                          const point_t& sensorPositionGlobal,
                                                                                                                                          const polygon_t& detectionField) const;

    static const osi3::MovingObject* FindHostVehicleInSensorView(const osi3::SensorView& sensorView);
    std::string CreateAgentIdListString(const std::vector<OWL::Id>& owlIds) const;

    bool enableVisualObstruction = false;
    double requiredPercentageOfVisibleArea = 0.001;
    double detectionRange;
    double openingAngleH;
    std::map<int, SensorDetectionResults> latentSensorDetectionResultsBuffer;

    static constexpr double MIN_VISIBLE_UNOBSTRUCTED_PERCENTAGE = 0.0001;
};

