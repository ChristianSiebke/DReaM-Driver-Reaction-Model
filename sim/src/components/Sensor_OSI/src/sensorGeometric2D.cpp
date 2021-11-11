/********************************************************************************
 * Copyright (c) 2019-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** \brief SensorGeometric2D.cpp */
//-----------------------------------------------------------------------------

#include "sensorGeometric2D.h"
#include <QtGlobal>
#include <numeric>
#include "common/boostGeometryCommon.h"
#include "include/parameterInterface.h"
#include "common/osiUtils.h"

SensorGeometric2D::SensorGeometric2D(
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
        AgentInterface *agent) :
    ObjectDetectorBase(
        componentName,
        isInit,
        priority,
        offsetTime,
        responseTime,
        cycleTime,
        stochastics,
        world,
        parameters,
        publisher,
        callbacks,
        agent)
{
    try
    {
        openingAngleH = parameters->GetParametersDouble().at("OpeningAngleH");
        enableVisualObstruction = parameters->GetParametersBool().at("EnableVisualObstruction");
        detectionRange = parameters->GetParametersDouble().at("DetectionRange");

        if (parameters->GetParametersDouble().count("RequiredPercentageOfVisibleArea") == 1)
        {
            requiredPercentageOfVisibleArea = parameters->GetParametersDouble().at("RequiredPercentageOfVisibleArea");
        }
    }
    catch (const std::out_of_range& e)
    {
        std::string msg{"Unable to retrieve sensor parameter: "};
        msg += e.what();
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }
}

void SensorGeometric2D::Trigger(int time)
{
    sensorData = {};
    osi3::utils::SetTimestamp(sensorData, time + latencyInMs);
    osi3::utils::SetVersion(sensorData);
    SensorDetectionResults results = DetectObjects();
    sensorData = ApplyLatency(time, sensorData);
    sensorData.mutable_moving_object_header()->set_data_qualifier(osi3::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE);

    Observe(time, ApplyLatencyToResults(time, results));
}

void SensorGeometric2D::UpdateInput(int, const std::shared_ptr<SignalInterface const> &, int)
{
}

void SensorGeometric2D::Observe(const int time, const SensorDetectionResults& results)
{
    std::set<OWL::Id> visibleIds;

    std::transform(results.visibleMovingObjects.begin(),
                   results.visibleMovingObjects.end(),
                   std::inserter(visibleIds, visibleIds.end()),
                   [](const auto object) -> OWL::Id
    {
        return object.id().value();
    });

    std::transform(results.visibleStationaryObjects.begin(),
                   results.visibleStationaryObjects.end(),
                   std::inserter(visibleIds, visibleIds.end()),
                   [](const auto object) -> OWL::Id
    {
        return object.id().value();
    });

    std::set<OWL::Id> detectedIds;

    std::transform(results.detectedMovingObjects.begin(),
                   results.detectedMovingObjects.end(),
                   std::inserter(detectedIds, detectedIds.end()),
                   [](const auto object) -> OWL::Id
    {
        return object.id().value();
    });

    std::transform(results.detectedStationaryObjects.begin(),
                   results.detectedStationaryObjects.end(),
                   std::inserter(detectedIds, detectedIds.end()),
                   [](const auto object) -> OWL::Id
    {
        return object.id().value();
    });

    GetPublisher()->Publish("Sensor" + std::to_string(id) + "_VisibleAgents", CreateObjectIdListString(visibleIds));
    GetPublisher()->Publish("Sensor" + std::to_string(id) + "_DetectedAgents", CreateObjectIdListString(detectedIds));
}

std::string SensorGeometric2D::CreateObjectIdListString(const std::set<OWL::Id>& owlIds) const
{
    return std::accumulate(owlIds.begin(),
                           owlIds.end(),
                           std::string(""),
                           [](const auto& first, const auto& second) -> std::string
    {
        return first + ';' + std::to_string(second);
    }).erase(0,1);
}

SensorDetectionResults SensorGeometric2D::ApplyLatencyToResults(const int time, const SensorDetectionResults& results)
{
    latentSensorDetectionResultsBuffer.emplace(time + latencyInMs, results);

    // because maps are sorted, find the first instance of a time in the buffer after the time requested by the parameters of the function
    auto latentResultsIterForTime = std::find_if_not(latentSensorDetectionResultsBuffer.begin(),
                                                     latentSensorDetectionResultsBuffer.end(),
                                                     [time](const auto& timeToResultsPair) -> bool
    {
        return timeToResultsPair.first <= time;
    });

    // if it exists, get the previous element of the map - the most recent latent result to the time specified in the parameters
    SensorDetectionResults resultsByLatency;
    if (std::prev(latentResultsIterForTime) != latentSensorDetectionResultsBuffer.end())
    {
        resultsByLatency = std::prev(latentResultsIterForTime)->second;
        latentSensorDetectionResultsBuffer.erase(latentSensorDetectionResultsBuffer.begin(), std::prev(latentResultsIterForTime));
    }

    return resultsByLatency;
}

bool SensorGeometric2D::OpeningAngleWithinHalfCircle() const
{
    return openingAngleH < M_PI;
}

bool SensorGeometric2D::OpeningAngleWithinFullCircle() const
{
    return openingAngleH < 2 * M_PI;
}

polygon_t SensorGeometric2D::CreateFourPointDetectionField() const
{
    polygon_t detectionField;

    double cosResult = std::cos(openingAngleH/2.0);
    double upperPointX = detectionRange * cosResult ;
    double upperPointY = detectionRange * std::sin(openingAngleH/2);
    double frontPointX = detectionRange / cosResult;

    bg::append(detectionField, point_t{0, 0});
    bg::append(detectionField, point_t{upperPointX, upperPointY});
    bg::append(detectionField, point_t{frontPointX, 0});
    bg::append(detectionField, point_t{upperPointX, -upperPointY});
    bg::append(detectionField, point_t{0, 0});

    return detectionField;
}

polygon_t SensorGeometric2D::CreateFivePointDetectionField() const
{
    polygon_t detectionField;

    double leftUpperPointX = detectionRange * std::cos(openingAngleH/2.0);
    double leftUpperPointY = detectionRange * std::sin(openingAngleH/2.0);
    double righttUpperPointY = detectionRange * std::tan(openingAngleH/4.0);

    bg::append(detectionField, point_t{0, 0});
    bg::append(detectionField, point_t{leftUpperPointX, leftUpperPointY});
    bg::append(detectionField, point_t{detectionRange, righttUpperPointY});
    bg::append(detectionField, point_t{detectionRange, -righttUpperPointY});
    bg::append(detectionField, point_t{leftUpperPointX, -leftUpperPointY});
    bg::append(detectionField, point_t{0, 0});

    return detectionField;
}

std::pair<point_t, polygon_t> SensorGeometric2D::CreateSensorDetectionField(const osi3::MovingObject* hostVehicle) const
{
    polygon_t detectionField;

    if (OpeningAngleWithinHalfCircle())
    {
        detectionField = CreateFourPointDetectionField();
    }
    else if (OpeningAngleWithinFullCircle())
    {
        detectionField = CreateFivePointDetectionField();
    }
    else
    {
        bg::append(detectionField, point_t{0, 0});
    }

    const auto ownPosition = GetHostVehiclePosition(hostVehicle);
    double yaw = hostVehicle->base().orientation().yaw();

    point_t sensorPositionGlobal = CalculateGlobalSensorPosition(ownPosition, yaw);
    detectionField = TransformPolygonToGlobalCoordinates(detectionField, sensorPositionGlobal, yaw);

    return std::make_pair(sensorPositionGlobal, detectionField);
}

SensorDetectionResults SensorGeometric2D::DetectObjects()
{
    SensorDetectionResults results;
    osi3::SensorViewConfiguration sensorViewConfig = GenerateSensorViewConfiguration();
    auto sensorView = static_cast<OWL::Interfaces::WorldData*>(world->GetWorldData())->GetSensorView(sensorViewConfig, GetAgent()->GetId());

    sensorData.add_sensor_view()->CopyFrom(*sensorView);

    const auto hostVehicle = FindHostVehicleInSensorView(*sensorView);
    sensorData.mutable_host_vehicle_location()->CopyFrom(hostVehicle->base());

    const auto [sensorPositionGlobal, detectionField] = CreateSensorDetectionField(hostVehicle);
    const auto [movingObjectsInDetectionField, stationaryObjectsInDetectionField] = GetObjectsInDetectionAreaFromSensorView(*sensorView,
                                                                                                                            sensorPositionGlobal,
                                                                                                                            detectionField);
    if (enableVisualObstruction)
    {
        multi_polygon_t brightArea{CalcInitialBrightArea(sensorPositionGlobal)};
        bg::correct(brightArea);

        //Remove shadows from brightArea
        ApplyVisualObstructionToDetectionArea(brightArea,
                                              sensorPositionGlobal,
                                              movingObjectsInDetectionField);
        ApplyVisualObstructionToDetectionArea(brightArea,
                                              sensorPositionGlobal,
                                              stationaryObjectsInDetectionField);

        std::tie(results.visibleMovingObjects, results.detectedMovingObjects) = CalcVisualObstruction(movingObjectsInDetectionField, brightArea);
        std::tie(results.visibleStationaryObjects, results.detectedStationaryObjects) = CalcVisualObstruction(stationaryObjectsInDetectionField, brightArea);
    }
    else
    {
        std::transform(movingObjectsInDetectionField.begin(),
                       movingObjectsInDetectionField.end(),
                       std::back_inserter(results.visibleMovingObjects),
                       [](const auto movingObject) -> osi3::MovingObject
        {
            return *movingObject;
        });
        results.detectedMovingObjects = results.visibleMovingObjects;
        std::transform(stationaryObjectsInDetectionField.begin(),
                       stationaryObjectsInDetectionField.end(),
                       std::back_inserter(results.visibleStationaryObjects),
                       [](const auto stationaryObject) -> osi3::StationaryObject
        {
            return *stationaryObject;
        });
        results.detectedStationaryObjects = results.visibleStationaryObjects;
    }

    const auto ownPosition = GetHostVehiclePosition(hostVehicle);
    const auto yaw = hostVehicle->base().orientation().yaw();
    const auto yawRate = hostVehicle->base().orientation_rate().yaw();
    const point_t ownVelocity{hostVehicle->base().velocity().x(), hostVehicle->base().velocity().y()};
    const point_t ownAcceleration{hostVehicle->base().acceleration().x(), hostVehicle->base().acceleration().y()};

    for (const auto& object : results.detectedMovingObjects)
    {
        if(HasDetectionError())
        {
            continue;
        }
        AddMovingObjectToSensorData(object, ownVelocity, ownAcceleration, ownPosition, yaw, yawRate);
    }
    for (const auto& object : results.detectedStationaryObjects)
    {
        if(HasDetectionError())
        {
            continue;
        }
        AddStationaryObjectToSensorData(object, ownPosition, yaw);
    }

    return results;
}

std::pair<std::vector<const osi3::MovingObject*>, std::vector<const osi3::StationaryObject*>> SensorGeometric2D::GetObjectsInDetectionAreaFromSensorView(const osi3::SensorView& sensorView,
                                                                                                                                                         const point_t& sensorPositionGlobal,
                                                                                                                                                         const polygon_t& detectionField) const
{
    std::vector<const osi3::MovingObject*> movingObjectsInDetectionField;
    for (const auto& movingObject : sensorView.global_ground_truth().moving_object())
    {
        if(ObjectIsInDetectionArea(movingObject, sensorPositionGlobal, detectionField) && movingObject.id().value() != sensorView.host_vehicle_id().value())
        {
            movingObjectsInDetectionField.emplace_back(&movingObject);
        }
    }

    std::vector<const osi3::StationaryObject*> stationaryObjectsInDetectionField;
    for (const auto& stationaryObject : sensorView.global_ground_truth().stationary_object())
    {
        if (ObjectIsInDetectionArea(stationaryObject, sensorPositionGlobal, detectionField))
        {
            stationaryObjectsInDetectionField.emplace_back(&stationaryObject);
        }
    }

    return std::make_pair(movingObjectsInDetectionField, stationaryObjectsInDetectionField);
}

template<typename T>
bool SensorGeometric2D::ObjectIsInDetectionArea(const T& object,
                                                const point_t& sensorPositionGlobal,
                                                const polygon_t& detectionField) const
{
    polygon_t objectBoundingBox = CalculateBoundingBox(object.base().dimension(), object.base().position(),
                                                  object.base().orientation());

    double distanceToObjectBoundary = bg::distance(sensorPositionGlobal, objectBoundingBox);

    return distanceToObjectBoundary <= detectionRange &&
           (openingAngleH >= 2 * M_PI || bg::intersects(detectionField, objectBoundingBox));
}

osi3::SensorViewConfiguration SensorGeometric2D::GenerateSensorViewConfiguration()
{
    // The sensor has to specify an area which contains all objects of interest. To take objects on the edge of this area into account,
    // an arbitrary margin is added deliberately, as no other specification is available at the moment.
    constexpr double rangeBuffer = 20.0;
    constexpr double fieldOfViewBuffer = 0.2;

    osi3::SensorViewConfiguration viewConfiguration = ObjectDetectorBase::GenerateSensorViewConfiguration();
    viewConfiguration.set_field_of_view_horizontal(openingAngleH + fieldOfViewBuffer);
    viewConfiguration.set_range(detectionRange + rangeBuffer);

    return viewConfiguration;
}

template<typename T>
void SensorGeometric2D::ApplyVisualObstructionToDetectionArea(multi_polygon_t& brightArea,
                                                              const point_t& sensorPositionGlobal,
                                                              const std::vector<const T*>& objects)
{
    for (const auto object : objects)
    {
        polygon_t objectBoundingBoxGlobal = CalculateBoundingBox(object->base().dimension(),
                                                                 object->base().position(),
                                                                 object->base().orientation());
        auto temporaryShadow = CalcObjectShadow(objectBoundingBoxGlobal, sensorPositionGlobal);
        multi_polygon_t newBrightArea;
        bg::difference(brightArea, temporaryShadow, newBrightArea);
        brightArea = newBrightArea;
    }
}

template<typename T>
std::pair<std::vector<T>, std::vector<T>> SensorGeometric2D::CalcVisualObstruction(const std::vector<const T*>& objects,
                                                                                   const multi_polygon_t &brightArea)
{
    std::vector<T> visibleObjects;
    std::vector<T> detectedObjects;
    for (const auto object : objects)
    {
        polygon_t objectBoundingBoxGlobal = CalculateBoundingBox(object->base().dimension(),
                                                                 object->base().position(),
                                                                 object->base().orientation());

        const auto visiblePercent = CalcObjectVisibilityPercentage(objectBoundingBoxGlobal, brightArea);
        if (visiblePercent >= MIN_VISIBLE_UNOBSTRUCTED_PERCENTAGE)
        {
            visibleObjects.emplace_back(*object);
        }
        if (visiblePercent >= requiredPercentageOfVisibleArea)
        {
            detectedObjects.emplace_back(*object);
        }
    }

    return std::make_pair(visibleObjects, detectedObjects);
}

polygon_t SensorGeometric2D::CalcInitialBrightArea(point_t sensorPosition)
{
    const double stepSize = 0.1;
    double sensorX = sensorPosition.x();
    double sensorY = sensorPosition.y();
    polygon_t brightArea;
    bg::append(brightArea, sensorPosition);

    double angle = position.yaw + GetAgent()->GetYaw() - 0.5 * openingAngleH;
    double maxAngle = position.yaw + GetAgent()->GetYaw() + 0.5 * openingAngleH;

    while (angle < maxAngle)
    {
        double x = sensorX + detectionRange * std::cos(angle);
        double y = sensorY + detectionRange * std::sin(angle);
        bg::append(brightArea, point_t{x,y});
        angle += stepSize;
    }

    double x = sensorX + detectionRange * std::cos(maxAngle);
    double y = sensorY + detectionRange * std::sin(maxAngle);
    bg::append(brightArea, point_t{x,y});

    bg::append(brightArea, sensorPosition);
    return brightArea;
}

multi_polygon_t SensorGeometric2D::CalcObjectShadow(const polygon_t& boundingBox, point_t sensorPosition)
{
    //get an arbitrary point of the object as reference point
    point_t referencePoint = boundingBox.outer()[0];
    point_t referenceVector{referencePoint.x() - sensorPosition.x(), referencePoint.y() - sensorPosition.y()};
    double referenceVectorLength = std::hypot(referenceVector.x(), referenceVector.y());
    double normalX = -referenceVector.y();

    //using this reference point, calculate the leftmost and rightmost point of the object
    double maxLeftAngle = 0;
    point_t leftPoint = referencePoint;
    point_t leftVector = referenceVector; //vector from the sensor to the leftPoint
    double maxRightAngle = 0;
    point_t rightPoint = referencePoint;
    point_t rightVector = referenceVector; //vector from the sensor to the rightPoint
    for (const point_t &point : boundingBox.outer())
    {
        point_t vector{point.x() - sensorPosition.x(), point.y() - sensorPosition.y()};
        double vectorLength = std::hypot(vector.x(), vector.y());
        double scalarProduct = referenceVector.x() * vector.x() + referenceVector.y() * vector.y();
        double angle = std::acos(scalarProduct / (referenceVectorLength * vectorLength));
        double perpendicularFootX = sensorPosition.x() + referenceVector.x() * scalarProduct / (referenceVectorLength * referenceVectorLength);
        double perpendicularX = point.x() - perpendicularFootX;
        bool isLeft = (perpendicularX / normalX) > 0; //point is left, if the perpendicular points in the same direction as the normal
        if (isLeft && (angle > maxLeftAngle))
        {
            maxLeftAngle = angle;
            leftPoint = point;
            leftVector = vector;
        }
        if (!isLeft && (angle > maxRightAngle))
        {
            maxRightAngle = angle;
            rightPoint = point;
            rightVector = vector;
        }
    }

    // The outer points are obtained by scaling the leftVector and rightVector, 
    // so that the calculated shadow just covers the detection range
    // For a detailed description of the scaling factor please refer to the developer documentation.
    const auto leftVectorLength = std::hypot(leftVector.x(), leftVector.y());
    const auto rightVectorLength = std::hypot(rightVector.x(), rightVector.y());
    const auto height = std::min(leftVectorLength, rightVectorLength) * std::cos(0.5 * (maxRightAngle + maxLeftAngle));
    const auto scale = detectionRange / height;
    
    if(scale > WARNING_THRESHOLD_SCALE)
    {
       LOG(CbkLogLevel::Warning, "Scaling factor for shadow casting exceeds internal threshold: numeric issues could cause detection to fail.");
    }

    point_t leftOuterPoint{sensorPosition.x() + scale * leftVector.x(), sensorPosition.y() + scale * leftVector.y()};
    point_t rightOuterPoint{sensorPosition.x() + scale * rightVector.x(), sensorPosition.y() + scale * rightVector.y()};

    //build the shadow polygon
    polygon_t shadow;
    bg::append (shadow, leftPoint);
    bg::append (shadow, leftOuterPoint);
    bg::append (shadow, rightOuterPoint);
    bg::append (shadow, rightPoint);
    bg::append (shadow, leftPoint);

    //remove the object boundingBox from its own shadow
    multi_polygon_t shadowM;
    polygon_t tmpBBox = boundingBox;
    bg::correct(tmpBBox);
    bg::difference (shadow, tmpBBox, shadowM);

    return shadowM;
}

double SensorGeometric2D::CalcObjectVisibilityPercentage(const polygon_t &boundingBox, const multi_polygon_t &brightArea)
{
    polygon_t tmpBBox = boundingBox;
    bg::correct(tmpBBox);
    double totalArea = bg::area(tmpBBox);
    multi_polygon_t visibleBoundingBox;
    bg::intersection(tmpBBox, brightArea, visibleBoundingBox);
    double visibleArea = bg::area(visibleBoundingBox);
    return visibleArea / totalArea;
}
