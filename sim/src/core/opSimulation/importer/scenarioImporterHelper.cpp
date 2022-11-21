/********************************************************************************
 * Copyright (c) 2019-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "scenarioImporterHelper.h"

#include "importerLoggingHelper.h"
#include "oscImporterCommon.h"

namespace TAG = openpass::importer::xml::scenarioImporter::tag;
namespace ATTRIBUTE = openpass::importer::xml::scenarioImporter::attribute;
namespace RULE = openpass::importer::xml::openScenario::rule;

namespace DYNAMICSSHAPE = openpass::importer::xml::openScenario::dynamicsShape;
namespace DYNAMICSDIMENSION = openpass::importer::xml::openScenario::dynamicsDimension;
namespace SPEEDTARGETVALUETYPE = openpass::importer::xml::openScenario::speedTargetValueType;

using namespace Importer;

template <typename T>
T ParseAttributeHelper(const QDomElement& element, const char attributeName[], openScenario::Parameters& parameters, T&)
{
    return Importer::ParseAttribute<T>(element, attributeName, parameters);
}

namespace openScenario
{

const std::map<std::string, Shape> shapeConversionMap = {{DYNAMICSSHAPE::linear, Shape::Linear},
                                                         {DYNAMICSSHAPE::step, Shape::Step}};

const std::map<std::string, SpeedTargetValueType> speedTargetValueTypeConversionMap = {{SPEEDTARGETVALUETYPE::delta, SpeedTargetValueType::Delta},
                                                                                       {SPEEDTARGETVALUETYPE::factor, SpeedTargetValueType::Factor}};

Position ScenarioImporterHelper::ImportPosition(QDomElement root, Parameters& parameters)
{
    QDomElement positionElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(root, TAG::position, positionElement),
                 root, std::string("Tag ") + TAG::position + " is missing.");
    QDomElement childOfPositionElement;
    if(SimulationCommon::GetFirstChildElement(positionElement, TAG::lanePosition, childOfPositionElement))
    {
        return ImportLanePosition(childOfPositionElement, parameters);
    }
    else if (SimulationCommon::GetFirstChildElement(positionElement, TAG::relativeLanePosition, childOfPositionElement))
    {
        return ImportRelativeLanePosition(childOfPositionElement, parameters);
    }
    else if (SimulationCommon::GetFirstChildElement(positionElement, TAG::roadPosition, childOfPositionElement))
    {
        return ImportRoadPosition(childOfPositionElement, parameters);
    }
    else if (SimulationCommon::GetFirstChildElement(positionElement, TAG::worldPosition, childOfPositionElement))
    {
        return ImportWorldPosition(childOfPositionElement, parameters);
    }
    else if (SimulationCommon::GetFirstChildElement(positionElement, TAG::relativeObjectPosition, childOfPositionElement))
    {
        return ImportRelativeObjectPosition(childOfPositionElement, parameters);
    }
    else if (SimulationCommon::GetFirstChildElement(positionElement, TAG::relativeWorldPosition, childOfPositionElement))
    {
        return ImportRelativeWorldPosition(childOfPositionElement, parameters);
    }
    else
    {
        LogErrorAndThrow(std::string("Position type not supported. Currently supported are: ") + TAG::lanePosition + ", " + TAG::roadPosition + ", " + TAG::worldPosition + ".");
    }
}

GlobalAction ScenarioImporterHelper::ImportGlobalAction(QDomElement globalActionElement, Parameters& parameters)
{
    QDomElement childOfGlobalActionElement;
    if (SimulationCommon::GetFirstChildElement(globalActionElement, TAG::entityAction, childOfGlobalActionElement))
    {
        return ImportEntityAction(childOfGlobalActionElement, parameters);
    }
    else if (SimulationCommon::GetFirstChildElement(globalActionElement, TAG::environmentAction, childOfGlobalActionElement))
    {
        return ImportEnvironmentAction(childOfGlobalActionElement, parameters);
    }
    else
    {
        LogErrorAndThrow("Invalid GlobalAction type.");
    }
}

PrivateAction ScenarioImporterHelper::ImportPrivateAction(QDomElement privateActionElement, Parameters& parameters, const std::string catalogPath)
{
    QDomElement childOfPrivateActionElement;
    if  (SimulationCommon::GetFirstChildElement(privateActionElement, TAG::lateralAction, childOfPrivateActionElement))
    {
        return ImportLateralAction(childOfPrivateActionElement, parameters);
    }
    else if  (SimulationCommon::GetFirstChildElement(privateActionElement, TAG::longitudinalAction, childOfPrivateActionElement))
    {
        return ImportLongitudinalAction(childOfPrivateActionElement, parameters);
    }
    else if (SimulationCommon::GetFirstChildElement(privateActionElement, TAG::teleportAction, childOfPrivateActionElement))
    {
        return ImportTeleportAction(childOfPrivateActionElement, parameters);
    }
    else if (SimulationCommon::GetFirstChildElement(privateActionElement, TAG::routingAction, childOfPrivateActionElement))
    {
        return ImportRoutingAction(childOfPrivateActionElement, parameters, catalogPath);
    }
    else if (SimulationCommon::GetFirstChildElement(privateActionElement, TAG::visibilityAction, childOfPrivateActionElement))
    {
        return ImportVisibilityAction(childOfPrivateActionElement, parameters);
    }
    else
    {
        LogErrorAndThrow(std::string("PrivateAction type invalid."));
    }
}

UserDefinedAction ScenarioImporterHelper::ImportUserDefinedAction(QDomElement userDefinedActionElement)
{
    QDomElement userDefinedChildElement;
       ThrowIfFalse(SimulationCommon::GetFirstChildElement(userDefinedActionElement, TAG::customCommandAction, userDefinedChildElement),
                    userDefinedActionElement, "Tag " + std::string(TAG::customCommandAction) + " is missing.");

    std::string command = userDefinedChildElement.text().toStdString();
    boost::algorithm::trim(command);

    return CustomCommandAction {command};
}

LanePosition ScenarioImporterHelper::ImportLanePosition(QDomElement positionElement, Parameters& parameters)
{
    LanePosition lanePosition;
    lanePosition.s = ParseAttribute<double>(positionElement, ATTRIBUTE::s, parameters);
    const auto laneIdString = ParseAttribute<std::string>(positionElement, ATTRIBUTE::laneId, parameters);
    try
    {
        lanePosition.laneId = std::stoi(laneIdString);
    }
    catch(std::invalid_argument)
    {
        ThrowIfFalse(false, positionElement, "LaneId must be integer");
    }
    catch(std::out_of_range)
    {
        ThrowIfFalse(false, positionElement, "LaneId is out of range");
    }
    lanePosition.roadId = ParseAttribute<std::string>(positionElement, ATTRIBUTE::roadId, parameters);

    lanePosition.offset = ParseOptionalAttribute<double>(positionElement, ATTRIBUTE::offset, parameters);

    //Parse optional stochastics
    QDomElement stochasticElement;
    SimulationCommon::GetFirstChildElement(positionElement, TAG::stochastics, stochasticElement);
    while (!stochasticElement.isNull())
    {
        const auto& stochasticInformation = ImportStochastics(stochasticElement, parameters);

        if (stochasticInformation.first == "offset")
        {
            ThrowIfFalse(lanePosition.offset.has_value(), stochasticElement, "The offset attribute is required in order to use stochastic offsets.");

            lanePosition.stochasticOffset = stochasticInformation.second;
            lanePosition.stochasticOffset->mean = lanePosition.offset.value();
        }
        else if (stochasticInformation.first == "s")
        {
            lanePosition.stochasticS = stochasticInformation.second;
            lanePosition.stochasticS->mean = lanePosition.s;
        }
        stochasticElement = stochasticElement.nextSiblingElement(TAG::stochastics);
    }

    QDomElement orientationElement;
    if(SimulationCommon::GetFirstChildElement(positionElement, TAG::orientation, orientationElement))
    {
        lanePosition.orientation = ImportOrientation(orientationElement, parameters);
    }

    return lanePosition;
}

RelativeLanePosition ScenarioImporterHelper::ImportRelativeLanePosition(QDomElement positionElement, Parameters& parameters)
{
    RelativeLanePosition relativeLanePosition;
    relativeLanePosition.entityRef = ParseAttribute<std::string>(positionElement, ATTRIBUTE::entityRef, parameters);
    relativeLanePosition.dLane = ParseAttribute<int>(positionElement, ATTRIBUTE::dLane, parameters);
    relativeLanePosition.ds = ParseAttribute<double>(positionElement, ATTRIBUTE::ds, parameters);
    relativeLanePosition.offset = ParseOptionalAttribute<double>(positionElement, ATTRIBUTE::offset, parameters);

    QDomElement orientationElement;
    if(SimulationCommon::GetFirstChildElement(positionElement, TAG::orientation, orientationElement))
    {
        relativeLanePosition.orientation = ImportOrientation(orientationElement, parameters);
    }

    return relativeLanePosition;
}

RoadPosition ScenarioImporterHelper::ImportRoadPosition(QDomElement positionElement, Parameters& parameters)
{
    RoadPosition roadPosition;
    roadPosition.s = ParseAttribute<double>(positionElement, ATTRIBUTE::s, parameters);
    roadPosition.t = ParseAttribute<double>(positionElement, ATTRIBUTE::t, parameters);
    roadPosition.roadId = ParseAttribute<std::string>(positionElement, ATTRIBUTE::roadId, parameters);

    QDomElement orientationElement;
    if(SimulationCommon::GetFirstChildElement(positionElement, TAG::orientation, orientationElement))
    {
        roadPosition.orientation = ImportOrientation(orientationElement, parameters);
    }

    return roadPosition;
}

WorldPosition ScenarioImporterHelper::ImportWorldPosition(QDomElement positionElement, Parameters& parameters)
{
    WorldPosition worldPosition;
    worldPosition.x = ParseAttribute<double>(positionElement, ATTRIBUTE::x, parameters);
    worldPosition.y= ParseAttribute<double>(positionElement, ATTRIBUTE::y, parameters);

    worldPosition.z = ParseOptionalAttribute<double>(positionElement, ATTRIBUTE::z, parameters);
    worldPosition.h = ParseOptionalAttribute<double>(positionElement, ATTRIBUTE::h, parameters);
    worldPosition.p = ParseOptionalAttribute<double>(positionElement, ATTRIBUTE::p, parameters);
    worldPosition.r = ParseOptionalAttribute<double>(positionElement, ATTRIBUTE::r, parameters);

    return worldPosition;
}

Orientation ScenarioImporterHelper::ImportOrientation(QDomElement& orientationElement, Parameters& parameters)
{
    Orientation orientation;

    std::string type = ParseAttribute<std::string>(orientationElement, ATTRIBUTE::type, parameters);
    ThrowIfFalse(type == "relative", orientationElement, "Scenario Importer: only relative orientation is allowed.");
    orientation.type = OrientationType::Relative;

    double heading = ParseAttribute<double>(orientationElement, ATTRIBUTE::h, parameters);
    orientation.h = heading;

    return orientation;
}

std::pair<std::string, StochasticAttribute> ScenarioImporterHelper::ImportStochastics(QDomElement& stochasticsElement, Parameters& parameters)
{
    std::string attributeName = ParseAttribute<std::string>(stochasticsElement, ATTRIBUTE::value, parameters);

    StochasticAttribute stochasticAttribute;
    stochasticAttribute.stdDeviation = ParseAttribute<double>(stochasticsElement, ATTRIBUTE::stdDeviation, parameters);
    stochasticAttribute.lowerBoundary = ParseAttribute<double>(stochasticsElement, ATTRIBUTE::lowerBound, parameters);
    stochasticAttribute.upperBoundary = ParseAttribute<double>(stochasticsElement, ATTRIBUTE::upperBound, parameters);

    return {attributeName, stochasticAttribute};
}

EntityAction ScenarioImporterHelper::ImportEntityAction(QDomElement entityActionElement, Parameters& parameters)
{
    const auto entityRef = ParseAttribute<std::string>(entityActionElement, ATTRIBUTE::entityRef, parameters);

    EntityActionType type;
    QDomElement childOfEntityAction;
    if (SimulationCommon::GetFirstChildElement(entityActionElement, TAG::addEntityAction, childOfEntityAction))
    {
        type = EntityActionType::Add;
    }
    else if (SimulationCommon::GetFirstChildElement(entityActionElement, TAG::deleteEntityAction, childOfEntityAction))
    {
       type = EntityActionType::Delete;
    }
    else
    {
        LogErrorAndThrow(std::string("Invalid ") + TAG::entityAction + " type.");
    }

    return {entityRef, type};
}

EnvironmentAction ScenarioImporterHelper::ImportEnvironmentAction(QDomElement environmentActionElement, Parameters& parameters)
{
    EnvironmentAction environmentAction;

    QDomElement environmentElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(environmentActionElement, TAG::environment, environmentElement),
                 environmentActionElement, "Tag " + std::string(TAG::environment) + " is missing.");

    QDomElement weatherElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(environmentElement, TAG::weather, weatherElement),
                 environmentElement, "Tag " + std::string(TAG::weather) + " is missing.");

    auto cloudState = ParseAttribute<std::string>(weatherElement, ATTRIBUTE::cloudState, parameters);
    if (cloudState == "skyOff")
    {
        environmentAction.weather.cloudState = Weather::CloudState::skyOff;
    }
    else if (cloudState == "free")
    {
        environmentAction.weather.cloudState = Weather::CloudState::free;
    }
    else if (cloudState == "cloudy")
    {
        environmentAction.weather.cloudState = Weather::CloudState::cloudy;
    }
    else if (cloudState == "overcast")
    {
        environmentAction.weather.cloudState = Weather::CloudState::overcast;
    }
    else
    {
        ThrowIfFalse(cloudState == "rainy", environmentElement, "Unknown cloudState " + cloudState);
        environmentAction.weather.cloudState = Weather::CloudState::rainy;
    }

    QDomElement fogElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(weatherElement, TAG::fog, fogElement),
                 weatherElement, "Tag " + std::string(TAG::fog) + " is missing.");
    environmentAction.weather.fog.visualRange = ParseAttribute<double>(fogElement, ATTRIBUTE::visualRange, parameters);

    QDomElement sunElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(weatherElement, TAG::sun, sunElement),
                 weatherElement, "Tag " + std::string(TAG::sun) + " is missing.");
    environmentAction.weather.sun.azimuth = ParseAttribute<double>(sunElement, ATTRIBUTE::azimuth, parameters);
    environmentAction.weather.sun.elevation = ParseAttribute<double>(sunElement, ATTRIBUTE::elevation, parameters);
    environmentAction.weather.sun.intensity = ParseAttribute<double>(sunElement, ATTRIBUTE::intensity, parameters);

    QDomElement precipitationElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(weatherElement, TAG::precipitation, precipitationElement),
                 weatherElement, "Tag " + std::string(TAG::precipitation) + " is missing.");
    environmentAction.weather.precipitation.intensity = ParseAttribute<double>(precipitationElement, ATTRIBUTE::intensity, parameters);
    auto precipitationType = ParseAttribute<std::string>(precipitationElement, ATTRIBUTE::precipitationType, parameters);
    if (precipitationType == "dry")
    {
        environmentAction.weather.precipitation.type = Precipitation::Type::dry;
    }
    else if (precipitationType == "rain")
    {
        environmentAction.weather.precipitation.type = Precipitation::Type::rain;
    }
    else
    {
        ThrowIfFalse(precipitationType == "snow", precipitationElement, "Unknown precipitation type " + precipitationType);
        environmentAction.weather.precipitation.type = Precipitation::Type::snow;
    }

    return  environmentAction;
}

LateralAction ScenarioImporterHelper::ImportLateralAction(QDomElement lateralActionElement, Parameters& parameters)
{

    QDomElement lateralChildElement;
    if (SimulationCommon::GetFirstChildElement(lateralActionElement, TAG::laneChangeAction, lateralChildElement))
    {
        QDomElement dynamicsElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(lateralChildElement, TAG::laneChangeActionDynamics, dynamicsElement),
                    lateralChildElement, "Tag " + std::string(TAG::laneChangeActionDynamics) + " is missing.");


        double transitionDynamicsValue = ParseAttribute<double>(dynamicsElement, ATTRIBUTE::value, parameters);
        std::string dynamicsDimension = ParseAttribute<std::string>(dynamicsElement, ATTRIBUTE::dynamicsDimension, parameters);

        openScenario::LaneChangeParameter::DynamicsType dynamicsType;

        if (dynamicsDimension == "time")
        {
            dynamicsType = openScenario::LaneChangeParameter::DynamicsType::Time;
        }
        else if (dynamicsDimension == "distance")
        {
            dynamicsType = openScenario::LaneChangeParameter::DynamicsType::Distance;
        }
        else
        {
            LogErrorAndThrow("Invalid " + std::string(ATTRIBUTE::dynamicsDimension) + " in " + std::string(TAG::laneChangeActionDynamics) + " .");
        }

        std::string shape = ParseAttribute<std::string>(dynamicsElement, ATTRIBUTE::dynamicsShape, parameters);
        ThrowIfFalse(shape == "sinusoidal", dynamicsElement, "Currently only shape sinusoidal supported for LaneChangeAction");

        QDomElement targetElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(lateralChildElement, TAG::laneChangeTarget, targetElement),
                    lateralChildElement, "Tag " + std::string(TAG::laneChangeTarget) + " is missing.");

        QDomElement typeElement;
        if (SimulationCommon::GetFirstChildElement(targetElement, TAG::relativeTargetLane, typeElement))
        {
            std::string entityRef = ParseAttribute<std::string>(typeElement, ATTRIBUTE::entityRef, parameters);
            int value = ParseAttribute<int>(typeElement, ATTRIBUTE::value, parameters);

            LaneChangeParameter laneChangeParameter {openScenario::LaneChangeParameter::Type::Relative,
                                                     value,
                                                     entityRef,
                                                     transitionDynamicsValue,
                                                     dynamicsType};

            return LaneChangeAction{laneChangeParameter};
        }
        else
        {
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(targetElement, TAG::absoluteTargetLane, typeElement),
                         targetElement, "Tag " + std::string(TAG::absoluteTargetLane) + " is missing.");

            const auto value = ParseAttribute<int>(typeElement, ATTRIBUTE::value, parameters);

            LaneChangeParameter laneChangeParameter {openScenario::LaneChangeParameter::Type::Absolute,
                                                     value,
                                                     "",
                                                     transitionDynamicsValue,
                                                     dynamicsType};

            return LaneChangeAction {laneChangeParameter};
        }
    }
    else
    {
        LogErrorAndThrow("Invalid LateralAction Type.");
    }
}

LongitudinalAction ScenarioImporterHelper::ImportLongitudinalAction(QDomElement longitudinalActionElement, Parameters& parameters)
{
    QDomElement childOfLongitudinalActionElement;
    if (SimulationCommon::GetFirstChildElement(longitudinalActionElement, TAG::speedAction, childOfLongitudinalActionElement))
    {
        SpeedAction speedAction;

        QDomElement dynamicsElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(childOfLongitudinalActionElement, TAG::speedActionDynamics, dynamicsElement),
                     childOfLongitudinalActionElement, std::string("Error while importing ") + TAG::speedAction + ". Tag" + TAG::speedActionDynamics + " is missing.");

        speedAction.transitionDynamics.value = ParseAttribute<double>(dynamicsElement, ATTRIBUTE::value, parameters);
        ThrowIfFalse(speedAction.transitionDynamics.value >= 0.0,
                     dynamicsElement, "TransitionDynamics value must greater or equal to 0.");

        // DynamicsShape
        const auto shape = ParseAttribute<std::string>(dynamicsElement, ATTRIBUTE::dynamicsShape, parameters);

        if (shapeConversionMap.find(shape) == shapeConversionMap.cend())
        {
            LogErrorAndThrow(std::string("Error while importing ") + TAG::speedActionDynamics + ". " + ATTRIBUTE::dynamicsShape + ": " + shape + " not supported.");
        }

        speedAction.transitionDynamics.shape = shapeConversionMap.at(shape);

        // DynamicsDimensions
        speedAction.transitionDynamics.dimension = ParseAttribute<std::string>(dynamicsElement, ATTRIBUTE::dynamicsDimension, parameters);

        if(speedAction.transitionDynamics.dimension != DYNAMICSDIMENSION::rate)
        {
            LogErrorAndThrow(std::string("Error while importing ") + TAG::speedActionDynamics + ". " + ATTRIBUTE::dynamicsDimension + ": " + speedAction.transitionDynamics.dimension + " not supported.");
        }

        // Handle <Target> attributes
        QDomElement targetElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(childOfLongitudinalActionElement, TAG::speedActionTarget, targetElement),
                     childOfLongitudinalActionElement, std::string("Error while importing ") + TAG::speedAction + ". Tag" + TAG::speedActionTarget + " is missing.");

        // Handle <Target> internal tags - currently ignoring <Relative> tags
        QDomElement targetSpeedElement;
        if (SimulationCommon::GetFirstChildElement(targetElement, TAG::absoluteTargetSpeed, targetSpeedElement))
        {
            if (SimulationCommon::HasAttribute(targetSpeedElement, ATTRIBUTE::value))
            {
                AbsoluteTargetSpeed absoluteTargetSpeed {ParseAttribute<double>(targetSpeedElement, ATTRIBUTE::value, parameters)};
                speedAction.target = absoluteTargetSpeed;
            }
        }
        else if (SimulationCommon::GetFirstChildElement(targetElement, TAG::relativeTargetSpeed, targetSpeedElement))
        {
            RelativeTargetSpeed relativeTargetSpeed;
            relativeTargetSpeed.entityRef = ParseAttribute<std::string>(targetSpeedElement, ATTRIBUTE::entityRef, parameters);
            relativeTargetSpeed.value = ParseAttribute<double>(targetSpeedElement, ATTRIBUTE::value, parameters);

            const auto speedTargetValueType = ParseAttribute<std::string>(targetSpeedElement, ATTRIBUTE::speedTargetValueType, parameters);

            ThrowIfFalse(speedTargetValueTypeConversionMap.find(speedTargetValueType) != speedTargetValueTypeConversionMap.cend(),
                         targetSpeedElement, std::string("Error while importing ") + ATTRIBUTE::speedTargetValueType + ". Invalid choice.");

            relativeTargetSpeed.speedTargetValueType = speedTargetValueTypeConversionMap.at(speedTargetValueType);

            relativeTargetSpeed.continuous = ParseAttribute<bool>(targetSpeedElement, ATTRIBUTE::continuous, parameters);

            speedAction.target = relativeTargetSpeed;
        }
        else
        {
            LogErrorAndThrow(std::string("Error while importing ") + TAG::speedActionTarget + ". Invalid choice.");
        }

        // Parse stochastics if available
        QDomElement stochasticElement;
        SimulationCommon::GetFirstChildElement(childOfLongitudinalActionElement, TAG::stochastics, stochasticElement);
        while (!stochasticElement.isNull())
        {
            const auto& [attributeName, stochasticInformation] = ImportStochastics(stochasticElement, parameters);
            if (attributeName == "velocity")
            {
                speedAction.stochasticValue = stochasticInformation;
                speedAction.stochasticValue.value().mean = std::get<AbsoluteTargetSpeed>(speedAction.target).value;
            }
            else if (attributeName == "rate")
            {
                speedAction.stochasticDynamics = stochasticInformation;
                speedAction.stochasticDynamics.value().mean = speedAction.transitionDynamics.value;
            }

           stochasticElement = stochasticElement.nextSiblingElement(TAG::stochastics);
        }

        return speedAction;
    }
    else
    {
        LogErrorAndThrow("Invalid LongitudinalAction Type.");
    }
}

TeleportAction ScenarioImporterHelper::ImportTeleportAction(QDomElement teleportActionElement, Parameters& parameters)
{
    return ImportPosition(teleportActionElement, parameters);
}

RoutingAction ScenarioImporterHelper::ImportRoutingAction(QDomElement routingActionElement, Parameters& parameters, const std::string catalogPath)
{

    QDomElement childOfRoutingActionElement;
    if(SimulationCommon::GetFirstChildElement(routingActionElement, TAG::assignRouteAction, childOfRoutingActionElement))
    {
        AssignRouteAction assignRouteAction;

        QDomElement routeElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(childOfRoutingActionElement, TAG::route, routeElement),
                     childOfRoutingActionElement, "Tag " + std::string(TAG::route) + " is missing.");

        QDomElement waypointElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(routeElement, TAG::waypoint, waypointElement),
                     routeElement, "Tag " + std::string(TAG::waypoint) + " is missing.");

        while (!waypointElement.isNull())
        {
            const auto position = ScenarioImporterHelper::ImportPosition(waypointElement, parameters);

            if (std::holds_alternative<RoadPosition>(position))
            {
                assignRouteAction.push_back(std::get<RoadPosition>(position));
            }
            else
            {
                LogErrorAndThrow(std::string(TAG::assignRouteAction) + " currently only supports " + TAG::roadPosition + ".");
            }

            waypointElement = waypointElement.nextSiblingElement(TAG::waypoint);
        }

        return assignRouteAction;
    }
    else if (SimulationCommon::GetFirstChildElement(routingActionElement, TAG::followTrajectoryAction, childOfRoutingActionElement))
    {
        Trajectory trajectory;

        Parameters defaultParameters;
        Parameters assignedParameters;

        QDomElement trajectoryElement;
        if(!SimulationCommon::GetFirstChildElement(childOfRoutingActionElement, TAG::trajectory, trajectoryElement))
        {
            QDomElement catalogReferenceElement;
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(childOfRoutingActionElement, TAG::catalogReference, catalogReferenceElement),
                         childOfRoutingActionElement, "Tag " + std::string(TAG::trajectory) + " or " + std::string(TAG::catalogReference) + " is missing.");
            std::string catalogName = ParseAttribute<std::string>(catalogReferenceElement, ATTRIBUTE::catalogName, parameters);
            std::string entryName = ParseAttribute<std::string>(catalogReferenceElement, ATTRIBUTE::entryName, parameters);
            trajectoryElement = GetTrajectoryElementFromCatalog(catalogName, catalogPath, entryName, parameters);

            QDomElement parameterDeclarationElement;
            SimulationCommon::GetFirstChildElement(trajectoryElement, TAG::parameterDeclarations, parameterDeclarationElement);
            if (!parameterDeclarationElement.isNull())
            {
                ImportParameterDeclarationElement(parameterDeclarationElement, defaultParameters);
            }
            QDomElement parameterAssignmentsElement;
            SimulationCommon::GetFirstChildElement(catalogReferenceElement, TAG::parameterAssignments, parameterAssignmentsElement);

            QDomElement parameterAssignmentElement;
            SimulationCommon::GetFirstChildElement(parameterAssignmentsElement, TAG::parameterAssignment, parameterAssignmentElement);
            while (!parameterAssignmentElement.isNull())
            {
                auto parameterName = ParseAttribute<std::string>(parameterAssignmentElement, ATTRIBUTE::parameterRef, parameters);
                auto foundParameter = defaultParameters.find(parameterName);
                ThrowIfFalse(foundParameter != defaultParameters.cend(), parameterAssignmentElement, "Can not assign parameter \"" + parameterName + "\". No parameter with this name declared.");
                std::visit([&](auto& value){
                    assignedParameters.insert({parameterName, ParseAttributeHelper(parameterAssignmentElement, ATTRIBUTE::value, parameters, value)});
                    },
                    foundParameter->second);
                parameterAssignmentElement = parameterAssignmentElement.nextSiblingElement(TAG::parameterAssignment);
            }
        }
        else
        {
            defaultParameters = parameters;
        }
        trajectory.name = ParseAttribute<std::string>(trajectoryElement, ATTRIBUTE::name, defaultParameters, assignedParameters);

        QDomElement timeReferenceElement;
        if(SimulationCommon::GetFirstChildElement(childOfRoutingActionElement, TAG::timeReference, timeReferenceElement))
        {
            QDomElement timingElement;
            if(SimulationCommon::GetFirstChildElement(timeReferenceElement, TAG::timing, timingElement)) {
                TrajectoryTimeReference timeReference;

                ThrowIfFalse(SimulationCommon::HasAttribute(timingElement, ATTRIBUTE::domainAbsoluteRelative),
                             timingElement,"Attribute " + std::string(ATTRIBUTE::domainAbsoluteRelative) + " is missing.");
                ThrowIfFalse(SimulationCommon::HasAttribute(timingElement, ATTRIBUTE::scale),
                             timingElement,"Attribute " + std::string(ATTRIBUTE::scale) + " is missing.");
                ThrowIfFalse(SimulationCommon::HasAttribute(timingElement, ATTRIBUTE::offset),
                             timingElement,"Attribute " + std::string(ATTRIBUTE::offset) + " is missing.");

                timeReference.domainAbsoluteRelative = ParseAttribute<std::string>(timingElement, ATTRIBUTE::domainAbsoluteRelative, parameters);
                timeReference.scale = ParseAttribute<double>(timingElement, ATTRIBUTE::scale, parameters);
                timeReference.offset = ParseAttribute<double>(timingElement, ATTRIBUTE::offset, parameters);

                trajectory.timeReference = timeReference;
            }
        }

        QDomElement shapeElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(trajectoryElement, TAG::shape, shapeElement),
                             trajectoryElement, "Tag " + std::string(TAG::shape) + " is missing.");

        QDomElement polylineElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(shapeElement, TAG::polyline, polylineElement),
                             shapeElement, "Tag " + std::string(TAG::polyline) + " is missing.");

        QDomElement vertexElement;
        ThrowIfFalse(SimulationCommon::GetFirstChildElement(polylineElement, TAG::vertex, vertexElement),
                     trajectoryElement, "Tag " + std::string(TAG::vertex) + " is missing.");
        while (!vertexElement.isNull())
        {
            const auto position = openScenario::ScenarioImporterHelper::ImportPosition(vertexElement, parameters);

            if(std::holds_alternative<openScenario::WorldPosition>(position))
            {
                const auto worldPosition = std::get<openScenario::WorldPosition>(position);

                openScenario::TrajectoryPoint trajectoryPoint;
                trajectoryPoint.time = ParseAttribute<double>(vertexElement, ATTRIBUTE::time, defaultParameters, assignedParameters);
                trajectoryPoint.x = worldPosition.x;
                trajectoryPoint.y = worldPosition.y;

                if (worldPosition.h.has_value())
                {
                    trajectoryPoint.yaw = worldPosition.h.value();
                }
                else
                {
                    LogErrorAndThrow("TrajectoryPoint requires WorldPosition to contain heading.");
                }
                trajectory.points.push_back(trajectoryPoint);

                vertexElement = vertexElement.nextSiblingElement(TAG::vertex);
            }
            else
            {
                LogErrorAndThrow("FollowTrajectoryAction only supports WorldPositions in Vertexes.");
            }
        }

        return FollowTrajectoryAction{trajectory};
    }
    else if (SimulationCommon::GetFirstChildElement(routingActionElement, TAG::acquirePositionAction, childOfRoutingActionElement))
    {
        AcquirePositionAction acquirePositionAction{ScenarioImporterHelper::ImportPosition(childOfRoutingActionElement, parameters)};
        return acquirePositionAction;
    }
    else
    {
        LogErrorAndThrow("Invalid RoutingAction type.");
    }
}

VisibilityAction ScenarioImporterHelper::ImportVisibilityAction(QDomElement visibilityActionElement, Parameters& parameters)
{
    VisibilityAction visibilityAction;
    visibilityAction.graphics = ParseAttribute<bool>(visibilityActionElement, ATTRIBUTE::graphics, parameters);
    visibilityAction.traffic = ParseAttribute<bool>(visibilityActionElement, ATTRIBUTE::traffic, parameters);
    visibilityAction.sensors = ParseAttribute<bool>(visibilityActionElement, ATTRIBUTE::sensors, parameters);

    return visibilityAction;
}

QDomElement ScenarioImporterHelper::GetTrajectoryElementFromCatalog(const std::string& catalogName, const std::string& catalogPath, const std::string& entryName, openScenario::Parameters& parameters)
{
    std::locale::global(std::locale("C"));

    QFile xmlFile(QString::fromStdString(catalogPath)); // automatic object will be closed on destruction
    ThrowIfFalse(xmlFile.open(QIODevice::ReadOnly), "Could not open TrajectoryCatalog (" + catalogName + ")");

    QByteArray xmlData(xmlFile.readAll());
    QDomDocument document;
    QString errorMsg;
    int errorLine;
    ThrowIfFalse(document.setContent(xmlData, &errorMsg, &errorLine), "Invalid xml format (" + catalogName + ") in line " + std::to_string(errorLine) + ": " + errorMsg.toStdString());

    QDomElement documentRoot = document.documentElement();
    ThrowIfFalse(!documentRoot.isNull(), "TrajectoryCatalog has no document root");

    QDomElement catalogElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(documentRoot, TAG::catalog, catalogElement),
                 documentRoot, "Tag " + std::string(TAG::catalog) + " is missing.");

    QDomElement trajectoryElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(catalogElement, TAG::trajectory, trajectoryElement),
                 catalogElement, "Tag " + std::string(TAG::trajectory) + " is missing.");
    while (!trajectoryElement.isNull())
    {
        std::string name = ParseAttribute<std::string>(trajectoryElement, ATTRIBUTE::name, parameters);
        if (name == entryName)
        {
            return trajectoryElement;
        }
        trajectoryElement = trajectoryElement.nextSiblingElement(TAG::trajectory);
    }

    LogErrorAndThrow("Entry " + entryName + " not found in TrajectoryCatalog " + catalogName);
}

RelativeWorldPosition ScenarioImporterHelper::ImportRelativeWorldPosition(const QDomElement &positionElement, Parameters &parameters)
{
    RelativeWorldPosition relativeWorldPosition;

    relativeWorldPosition.entityRef = ParseAttribute<std::string>(positionElement, ATTRIBUTE::entityRef, parameters);
    relativeWorldPosition.dx = ParseAttribute<double>(positionElement, ATTRIBUTE::dx, parameters);
    relativeWorldPosition.dy = ParseAttribute<double>(positionElement, ATTRIBUTE::dy, parameters);
    relativeWorldPosition.dz = ParseOptionalAttribute<double>(positionElement, ATTRIBUTE::dz, parameters);

    QDomElement orientationElement;
    if (SimulationCommon::GetFirstChildElement(positionElement, TAG::orientation, orientationElement))
    {
        relativeWorldPosition.orientation = ImportOrientation(orientationElement, parameters);
    }

    return relativeWorldPosition;
}

RelativeObjectPosition ScenarioImporterHelper::ImportRelativeObjectPosition(const QDomElement &positionElement, Parameters &parameters)
{
    RelativeObjectPosition relativeObjectPosition;

    relativeObjectPosition.entityRef = ParseAttribute<std::string>(positionElement, ATTRIBUTE::entityRef, parameters);
    relativeObjectPosition.dx = ParseAttribute<double>(positionElement, ATTRIBUTE::dx, parameters);
    relativeObjectPosition.dy = ParseAttribute<double>(positionElement, ATTRIBUTE::dy, parameters);
    relativeObjectPosition.dz = ParseOptionalAttribute<double>(positionElement, ATTRIBUTE::dz, parameters);

    QDomElement orientationElement;
    if (SimulationCommon::GetFirstChildElement(positionElement, TAG::orientation, orientationElement))
    {
        relativeObjectPosition.orientation = ImportOrientation(orientationElement, parameters);
    }

    return relativeObjectPosition;
}

} // namespace openScenario
