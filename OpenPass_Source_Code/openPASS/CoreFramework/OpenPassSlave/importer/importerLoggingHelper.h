/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "log.h"

namespace openpass::importer::xml::openScenario::rule
{
    constexpr char greaterThan[] {"greaterThan"};
    constexpr char lessThan[] {"lessThan"};
    constexpr char equalTo[] {"equalTo"};
}

namespace openpass::importer::xml::openScenario::dynamicsShape
{
    constexpr char linear[] {"linear"};
    constexpr char step[] {"step"};
}

namespace openpass::importer::xml::openScenario::speedTargetValueType
{
    constexpr char delta[] {"delta"};
    constexpr char factor[] {"factor"};
}

namespace openpass::importer::xml::openScenario::dynamicsDimension
{
    constexpr char rate[] {"rate"};
}

namespace openpass::importer::xml::eventDetectorImporter::tag
{
    constexpr char byEntityCondition[] {"ByEntityCondition"};
    constexpr char byValueCondition[] {"ByValueCondition"};
    constexpr char condition[] {"Condition"};
    constexpr char conditionGroup[] {"ConditionGroup"};
    constexpr char entityRef[] {"EntityRef"};
    constexpr char entityCondition[] {"EntityCondition"};
    constexpr char position[] {"Position"};
    constexpr char reachPositionCondition[] {"ReachPositionCondition"};
    constexpr char relativeLanePosition[] {"RelativeLanePosition"};
    constexpr char relativeSpeedCondition[] {"RelativeSpeedCondition"};
    constexpr char roadPosition[] {"RoadPosition"};
    constexpr char simulationTimeCondition[] {"SimulationTimeCondition"};
    constexpr char startTrigger[] {"StartTrigger"};
    constexpr char timeToCollisionConditionTarget[] {"TimeToCollisionConditionTarget"};
    constexpr char timeHeadwayCondition[] {"TimeHeadwayCondition"};
    constexpr char timeToCollisionCondition[] {"TimeToCollisionCondition"};
    constexpr char triggeringEntities[] {"TriggeringEntities"};
}

namespace openpass::importer::xml::eventDetectorImporter::attribute
{
    constexpr char alongRoute[] {"alongRoute"};
    constexpr char entityRef[] {"entityRef"};
    constexpr char freespace[] {"freespace"};
    constexpr char name[] {"name"};
    constexpr char rule[] {"rule"};
    constexpr char tolerance[] {"tolerance"};
    constexpr char value[] {"value"};
}

namespace openpass::importer::xml::manipulatorImporter::tag
{
    constexpr char addEntityAction[] {"AddEntityAction"};
    constexpr char catalogReference[] {"CatalogReference"};
    constexpr char position[] {"Position"};
    constexpr char routingAction[] {"RoutingAction"};
    constexpr char worldPosition[] {"WorldPosition"};
}

namespace openpass::importer::xml::manipulatorImporter::attribute
{
    constexpr char catalogName[] {"catalogName"};
    constexpr char distance[] {"distance"};
    constexpr char dynamicsDimension[] {"dynamicsDimension"};
    constexpr char entityRef[] {"entityRef"};
    constexpr char entryName[] {"entryName"};
    constexpr char h[] {"h"};
    constexpr char name[] {"name"};
    constexpr char object[] {"object"};
    constexpr char dynamicsShape[] {"dynamicsShape"};
    constexpr char time[] {"time"};
    constexpr char value[] {"value"};
    constexpr char x[] {"x"};
    constexpr char y[] {"y"};
}

namespace openpass::importer::xml::parameterImporter::tag
{
    constexpr char Bool[] {"Bool"};
    constexpr char Double[] {"Double"};
    constexpr char DoubleVector[] {"DoubleVector"};
    constexpr char Int[] {"Int"};
    constexpr char IntVector[] {"IntVector"};
    constexpr char String[] {"String"};
    constexpr char StringVector[] {"StringVector"};

    constexpr char NormalDistribution[] {"NormalDistribution"};
    constexpr char LogNormalDistribution[] {"LogNormalDistribution"};
    constexpr char UniformDistribution[] {"UniformDistribution"};
    constexpr char ExponentialDistribution[] {"ExponentialDistribution"};

    constexpr char List[] {"List"};
    constexpr char ListItem[] {"ListItem"};
    constexpr char Reference[] {"Reference"};
}

namespace openpass::importer::xml::parameterImporter::attribute
{
    constexpr char key[] {"Key"};
    constexpr char lambda[] {"Lambda"};
    constexpr char max[] {"Max"};
    constexpr char mean[] {"Mean"};
    constexpr char min[] {"Min"};
    constexpr char mu[] {"Mu"};
    constexpr char name[] {"Name"};
    constexpr char sd[] {"SD"};
    constexpr char sigma[] {"Sigma"};
    constexpr char type[] {"Type"};
    constexpr char value[] {"Value"};
}

namespace openpass::importer::xml::profilesImporter::tag
{
    constexpr char agentProfile[] {"AgentProfile"};
    constexpr char agentProfiles[] {"AgentProfiles"};
    constexpr char component[] {"Component"};
    constexpr char components[] {"Components"};
    constexpr char driverProfile[] {"DriverProfile"};
    constexpr char driverProfiles[] {"DriverProfiles"};
    constexpr char model[] {"Model"};
    constexpr char position[] {"Position"};
    constexpr char profile[] {"Profile"};
    constexpr char profiles[] {"Profiles"};
    constexpr char profileGroup[] {"ProfileGroup"};
    constexpr char sensor[] {"Sensor"};
    constexpr char sensors[] {"Sensors"};
    constexpr char sensorLink[] {"SensorLink"};
    constexpr char sensorLinks[] {"SensorLinks"};
    constexpr char system[] {"System"};
    constexpr char vehicleProfile[] {"VehicleProfile"};
    constexpr char vehicleProfiles[] {"VehicleProfiles"};
}

namespace openpass::importer::xml::profilesImporter::attribute
{
    constexpr char file[] {"File"};
    constexpr char height[] {"Height"};
    constexpr char id[] {"Id"};
    constexpr char inputId[] {"InputId"};
    constexpr char lateral[] {"Lateral"};
    constexpr char longitudinal[] {"Longitudinal"};
    constexpr char name[] {"Name"};
    constexpr char pitch[] {"Pitch"};
    constexpr char roll[] {"Roll"};
    constexpr char schemaVersion[] {"SchemaVersion"};
    constexpr char sensorId[] {"SensorId"};
    constexpr char type[] {"Type"};
    constexpr char vehicleModel[] {"VehicleModel"};
    constexpr char yaw[] {"Yaw"};
    constexpr char latency[] {"Latency"};
}

namespace openpass::importer::xml::scenarioImporter::tag
{
    constexpr char absoluteTargetLane[] {"AbsoluteTargetLane"};
    constexpr char absoluteTargetSpeed[] {"AbsoluteTargetSpeed"};
    constexpr char act[] {"Act"};
    constexpr char action[] {"Action"};
    constexpr char addEntityAction[] {"AddEntityAction"};
    constexpr char privateAction[] {"PrivateAction"};
    constexpr char actions[] {"Actions"};
    constexpr char actors[] {"Actors"};
    constexpr char byValueCondition[] {"ByValueCondition"};
    constexpr char catalog[] {"Catalog"};
    constexpr char catalogLocations[] {"CatalogLocations"};
    constexpr char catalogReference[] {"CatalogReference"};
    constexpr char condition[] {"Condition"};
    constexpr char conditionGroup[] {"ConditionGroup"};
    constexpr char customCommandAction[] {"CustomCommandAction"};
    constexpr char deleteEntityAction[] {"DeleteEntityAction"};
    constexpr char directory[] {"Directory"};
    constexpr char followTrajectoryAction[] {"FollowTrajectoryAction"};
    constexpr char speedActionDynamics[] {"SpeedActionDynamics"};
    constexpr char entities[] {"Entities"};
    constexpr char entityAction[] {"EntityAction"};
    constexpr char entityRef[] {"EntityRef"};
    constexpr char event[] {"Event"};
    constexpr char assignRouteAction[] {"AssignRouteAction"};
    constexpr char globalAction [] {"GlobalAction"};
    constexpr char init[] {"Init"};
    constexpr char laneChangeAction[] {"LaneChangeAction"};
    constexpr char laneChangeActionDynamics[] {"LaneChangeActionDynamics"};
    constexpr char laneChangeTarget[] {"LaneChangeTarget"};
    constexpr char lanePosition[] {"LanePosition"};
    constexpr char lateralAction[] {"LateralAction"};
    constexpr char logicFile[] {"LogicFile"};
    constexpr char longitudinalAction[] {"LongitudinalAction"};
    constexpr char maneuver[] {"Maneuver"};
    constexpr char maneuverGroup[] {"ManeuverGroup"};
    constexpr char members[] {"Members"};
    constexpr char scenarioObject[] {"ScenarioObject"};
    constexpr char orientation[] {"Orientation"};
    constexpr char parameterAssignment[] {"ParameterAssignment"};
    constexpr char parameterAssignments[] {"ParameterAssignments"};
    constexpr char parameterDeclaration[] {"ParameterDeclaration"};
    constexpr char parameterDeclarations[] {"ParameterDeclarations"};
    constexpr char pedestrianCatalog[] {"PedestrianCatalog"};
    constexpr char polyline[] {"Polyline"};
    constexpr char position[] {"Position"};
    constexpr char Private[] {"Private"};
    constexpr char relativeLanePosition[] {"RelativeLanePosition"};
    constexpr char relativeTargetLane[] {"RelativeTargetLane"};
    constexpr char relativeTargetSpeed[] {"RelativeTargetSpeed"};
    constexpr char roadPosition[] {"RoadPosition"};
    constexpr char roadNetwork[] {"RoadNetwork"};
    constexpr char route[] {"Route"};
    constexpr char routingAction[] {"RoutingAction"};
    constexpr char entitySelection[] {"EntitySelection"};
    constexpr char shape[] {"Shape"};
    constexpr char speedAction[] {"SpeedAction"};
    constexpr char stochastics[] {"Stochastics"};
    constexpr char stopTrigger[] {"StopTrigger"};
    constexpr char story[] {"Story"};
    constexpr char storyboard[] {"Storyboard"};
    constexpr char speedActionTarget[] {"SpeedActionTarget"};
    constexpr char trajectory[] {"Trajectory"};
    constexpr char trajectoryCatalog[] {"TrajectoryCatalog"};
    constexpr char teleportAction[] {"TeleportAction"};
    constexpr char userDefinedAction[] {"UserDefinedAction"};
    constexpr char vehicleCatalog[] {"VehicleCatalog"};
    constexpr char visibilityAction[] {"VisibilityAction"};
    constexpr char vertex[] {"Vertex"};
    constexpr char waypoint[] {"Waypoint"};
    constexpr char worldPosition[] {"WorldPosition"};
}

namespace openpass::importer::xml::scenarioImporter::attribute
{
    constexpr char catalogName[] {"catalogName"};
    constexpr char delay[] {"delay"};
    constexpr char dLane[] {"dLane"};
    constexpr char ds[] {"ds"};
    constexpr char conditionEdge[] {"conditionEdge"};
    constexpr char continuous[] {"continuous"};
    constexpr char dynamicsDimension[] {"dynamicsDimension"};
    constexpr char dynamicsShape[] {"dynamicsShape"};
    constexpr char entityRef[] {"entityRef"};
    constexpr char entryName[] {"entryName"};
    constexpr char filepath[] {"filepath"};
    constexpr char graphics[] {"graphics"};
    constexpr char h[] {"h"};
    constexpr char laneId[] {"laneId"};
    constexpr char lowerBound[] {"lowerBound"};
    constexpr char maximumExecutionCount[] {"maximumExecutionCount"};
    constexpr char name[] {"name"};
    constexpr char offset[] {"offset"};
    constexpr char p[] {"p"};
    constexpr char parameterRef[] {"parameterRef"};
    constexpr char parameterType[] {"parameterType"};
    constexpr char path[] {"path"};
    constexpr char r[] {"r"};
    constexpr char rate[] {"rate"};
    constexpr char reference[] {"reference"};
    constexpr char roadId[] {"roadId"};
    constexpr char rule[] {"rule"};
    constexpr char s[] {"s"};
    constexpr char selectTriggeringEntities[] {"selectTriggeringEntities"};
    constexpr char sensors[] {"sensors"};
    constexpr char speedTargetValueType[] {"speedTargetValueType"};
    constexpr char stdDeviation[] {"stdDeviation"};
    constexpr char t[] {"t"};
    constexpr char time[] {"time"};
    constexpr char traffic[] {"traffic"};
    constexpr char type[] {"type"};
    constexpr char upperBound[] {"upperBound"};
    constexpr char value[] {"value"};
    constexpr char x[] {"x"};
    constexpr char y[] {"y"};
    constexpr char z[] {"z"};
}

namespace openpass::importer::xml::sceneryImporter::tag
{
    constexpr char arc[] {"arc"};
    constexpr char center[] {"center"};
    constexpr char connection[] {"connection"};
    constexpr char dependency[] {"dependency"};
    constexpr char elevation[] {"elevation"};
    constexpr char elevationProfile[] {"elevationProfile"};
    constexpr char geometry[] {"geometry"};
    constexpr char junction[] {"junction"};
    constexpr char lane[] {"lane"};
    constexpr char laneLink[] {"laneLink"};
    constexpr char laneOffset[] {"laneOffset"};
    constexpr char lanes[] {"lanes"};
    constexpr char laneSection[] {"laneSection"};
    constexpr char left[] {"left"};
    constexpr char line[] {"line"};
    constexpr char link[] {"link"};
    constexpr char neighbor[] {"neighbor"};
    constexpr char object[] {"object"};
    constexpr char objects[] {"objects"};
    constexpr char paramPoly3[] {"paramPoly3"};
    constexpr char planView[] {"planView"};
    constexpr char poly3[] {"poly3"};
    constexpr char predecessor[] {"predecessor"};
    constexpr char priority[] {"priority"};
    constexpr char repeat[] {"repeat"};
    constexpr char right[] {"right"};
    constexpr char road[] {"road"};
    constexpr char roadMark[] {"roadMark"};
    constexpr char signal[] {"signal"};
    constexpr char Signals[] {"signals"};
    constexpr char spiral[] {"spiral"};
    constexpr char successor[] {"successor"};
    constexpr char type[] {"type"};
    constexpr char validity[] {"validity"};
    constexpr char width[] {"width"};
}

namespace openpass::importer::xml::sceneryImporter::attribute
{
    constexpr char a[] {"a"};
    constexpr char aU[] {"aU"};
    constexpr char aV[] {"aV"};
    constexpr char b[] {"b"};
    constexpr char bU[] {"bU"};
    constexpr char bV[] {"bV"};
    constexpr char c[] {"c"};
    constexpr char color[] {"color"};
    constexpr char connectingRoad[] {"connectingRoad"};
    constexpr char contactPoint[] {"contactPoint"};
    constexpr char country[] {"country"};
    constexpr char cU[] {"cU"};
    constexpr char curvature[] {"curvature"};
    constexpr char curvEnd[] {"curvEnd"};
    constexpr char curvStart[] {"curvStart"};
    constexpr char cV[] {"cV"};
    constexpr char d[] {"d"};
    constexpr char direction[] {"direction"};
    constexpr char distance[] {"distance"};
    constexpr char ds[] {"ds"};
    constexpr char dU[] {"dU"};
    constexpr char dV[] {"dV"};
    constexpr char dynamic[] {"dynamic"};
    constexpr char elementId[] {"elementId"};
    constexpr char elementType[] {"elementType"};
    constexpr char end[] {"end"};
    constexpr char from[] {"from"};
    constexpr char fromLane[] {"fromLane"};
    constexpr char hdg[] {"hdg"};
    constexpr char height[] {"height"};
    constexpr char high[] {"high"};
    constexpr char hOffset[] {"hOffset"};
    constexpr char id[] {"id"};
    constexpr char incomingRoad[] {"incomingRoad"};
    constexpr char junction[] {"junction"};
    constexpr char left[] {"left"};
    constexpr char length[] {"length"};
    constexpr char low[] {"low"};
    constexpr char name[] {"name"};
    constexpr char opposite[] {"opposite"};
    constexpr char orientation[] {"orientation"};
    constexpr char pitch[] {"pitch"};
    constexpr char right[] {"right"};
    constexpr char road[] {"road"};
    constexpr char roll[] {"roll"};
    constexpr char s[] {"s"};
    constexpr char same[] {"same"};
    constexpr char side[] {"side"};
    constexpr char sOffset[] {"sOffset"};
    constexpr char start[] {"start"};
    constexpr char subtype[] {"subtype"};
    constexpr char t[] {"t"};
    constexpr char text[] {"text"};
    constexpr char to[] {"to"};
    constexpr char toLane[] {"toLane"};
    constexpr char type[] {"type"};
    constexpr char unit[] {"unit"};
    constexpr char validLength[] {"validLength"};
    constexpr char value[] {"value"};
    constexpr char weight[] {"weight"};
    constexpr char width[] {"width"};
    constexpr char x[] {"x"};
    constexpr char y[] {"y"};
    constexpr char zOffset[] {"zOffset"};
}

namespace openpass::importer::xml::slaveConfigImporter::tag
{
    constexpr char environment[] {"Environment"};
    constexpr char experiment[] {"Experiment"};
    constexpr char friction[] {"Friction"};
    constexpr char frictions[] {"Frictions"};
    constexpr char homogenities[] {"Homogenities"};
    constexpr char homogenity[] {"Homogenity"};
    constexpr char libraries[] {"Libraries"};
    constexpr char library[] {"Library"};
    constexpr char observation[] {"Observation"};
    constexpr char observations[] {"Observations"};
    constexpr char parameters[] {"Parameters"};
    constexpr char priority[] {"Priority"};
    constexpr char scenario[] {"Scenario"};
    constexpr char spawner[] {"Spawner"};
    constexpr char spawners[] {"Spawners"};
    constexpr char timeOfDay[] {"TimeOfDay"};
    constexpr char timeOfDays[] {"TimeOfDays"};
    constexpr char type[] {"Type"};
    constexpr char visibilityDistance[] {"VisibilityDistance"};
    constexpr char visibilityDistances[] {"VisibilityDistances"};
    constexpr char weather[] {"Weather"};
    constexpr char weathers[] {"Weathers"};
}

namespace openpass::importer::xml::slaveConfigImporter::attribute
{
    constexpr char schemaVersion[] {"SchemaVersion"};
}

namespace openpass::importer::xml::systemConfigImporter::tag
{
    constexpr char component[] {"component"};
    constexpr char components[] {"components"};
    constexpr char connection[] {"connection"};
    constexpr char connections[] {"connections"};
    constexpr char id[] {"id"};
    constexpr char input[] {"input"};
    constexpr char output[] {"output"};
    constexpr char parameter[] {"parameter"};
    constexpr char parameters[] {"parameters"};
    constexpr char source[] {"source"};
    constexpr char system[] {"system"};
    constexpr char target[] {"target"};
}

namespace openpass::importer::xml::trajectoryImporter::tag
{
    constexpr char trajectoryCoordinate[] {"TrajectoryCoordinate"};
}

namespace openpass::importer::xml::trajectoryImporter::attribute
{
    constexpr char hdg[] {"Hdg"};
    constexpr char s[] {"S"};
    constexpr char t[] {"T"};
    constexpr char time[] {"Time"};
    constexpr char x[] {"X"};
    constexpr char y[] {"Y"};
    constexpr char yaw[] {"Yaw"};
}

namespace openpass::importer::xml::vehicleModelsImporter::tag
{
    constexpr char axles[] {"Axles"};
    constexpr char boundingBox[] {"BoundingBox"};
    constexpr char catalog[] {"Catalog"};
    constexpr char center[] {"Center"};
    constexpr char dimensions[] {"Dimensions"};
    constexpr char frontAxle[] {"FrontAxle"};
    constexpr char parameterDeclarations[] {"ParameterDeclarations"};
    constexpr char pedestrian[] {"Pedestrian"};
    constexpr char performance[] {"Performance"};
    constexpr char properties[] {"Properties"};
    constexpr char property[] {"Property"};
    constexpr char rearAxle[] {"RearAxle"};
    constexpr char vehicle[] {"Vehicle"};
}

namespace openpass::importer::xml::vehicleModelsImporter::attribute
{
    constexpr char vehicleCategory[] {"vehicleCategory"};
    constexpr char height[] {"height"};
    constexpr char length[] {"length"};
    constexpr char mass[] {"mass"};
    constexpr char maxSpeed[] {"maxSpeed"};
    constexpr char maxSteering[] {"maxSteering"};
    constexpr char name[] {"name"};
    constexpr char pedestrianCategory[] {"pedestrianCategory"};
    constexpr char positionX[] {"positionX"};
    constexpr char trackWidth[] {"trackWidth"};
    constexpr char value[] {"value"};
    constexpr char wheelDiameter[] {"wheelDiameter"};
    constexpr char width[] {"width"};
    constexpr char x[] {"x"};
    constexpr char y[] {"y"};
    constexpr char z[] {"z"};
}
