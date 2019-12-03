/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "log.h"

namespace openpass::importer::xml::eventDetectorImporter::tag
{
    constexpr char byEntity[] {"ByEntity"};
    constexpr char byValue[] {"ByValue"};
    constexpr char condition[] {"Condition"};
    constexpr char conditionGroup[] {"ConditionGroup"};
    constexpr char entity[] {"Entity"};
    constexpr char entityCondition[] {"EntityCondition"};
    constexpr char position[] {"Position"};
    constexpr char relativeLane[] {"RelativeLane"};
    constexpr char relativeSpeed[] {"RelativeSpeed"};
    constexpr char road[] {"Road"};
    constexpr char simulationTime[] {"SimulationTime"};
    constexpr char startConditions[] {"StartConditions"};
    constexpr char target[] {"Target"};
    constexpr char timeToCollision[] {"TimeToCollision"};
    constexpr char triggeringEntities[] {"TriggeringEntities"};
}

namespace openpass::importer::xml::eventDetectorImporter::attribute
{
    constexpr char dLane[] {"dLane"};
    constexpr char ds[] {"ds"};
    constexpr char entity[] {"entity"};
    constexpr char name[] {"name"};
    constexpr char object[] {"object"};
    constexpr char roadId[] {"roadId"};
    constexpr char rule[] {"rule"};
    constexpr char s[] {"s"};
    constexpr char tolerance[] {"tolerance"};
    constexpr char value[] {"value"};
}

namespace openpass::importer::xml::manipulatorImporter::tag
{
    constexpr char absolute[] {"Absolute"};
    constexpr char action[] {"Action"};
    constexpr char add[] {"Add"};
    constexpr char command[] {"Command"};
    constexpr char Delete[] {"Delete"};
    constexpr char entity[] {"Entity"};
    constexpr char global[] {"Global"};
    constexpr char userDefined[] {"UserDefined"};
    constexpr char laneChange[] {"LaneChange"};
    constexpr char lateral[] {"Lateral"};
    constexpr char Private[] {"Private"};
    constexpr char relative[] {"Relative"};
    constexpr char target[] {"Target"};
}

namespace openpass::importer::xml::manipulatorImporter::attribute
{
    constexpr char name[] {"name"};
    constexpr char object[] {"object"};
    constexpr char value[] {"value"};
}

namespace openpass::importer::xml::parameterImporter::tag
{
    constexpr char Bool[] {"Bool"};
    constexpr char Double[] {"Double"};
    constexpr char doubleVector[] {"DoubleVector"};
    constexpr char Int[] {"Int"};
    constexpr char intVector[] {"IntVector"};
    constexpr char list[] {"List"};
    constexpr char listItem[] {"ListItem"};
    constexpr char normalDistribution[] {"NormalDistribution"};
    constexpr char string[] {"String"};
}

namespace openpass::importer::xml::parameterImporter::attribute
{
    constexpr char key[] {"Key"};
    constexpr char max[] {"Max"};
    constexpr char mean[] {"Mean"};
    constexpr char min[] {"Min"};
    constexpr char name[] {"Name"};
    constexpr char sd[] {"SD"};
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
    constexpr char sensor[] {"Sensor"};
    constexpr char sensors[] {"Sensors"};
    constexpr char sensorLink[] {"SensorLink"};
    constexpr char sensorLinks[] {"SensorLinks"};
    constexpr char sensorProfile[] {"SensorProfile"};
    constexpr char sensorProfiles[] {"SensorProfiles"};
    constexpr char system[] {"System"};
    constexpr char vehicleComponentProfile[] {"VehicleComponentProfile"};
    constexpr char vehicleComponentProfiles[] {"VehicleComponentProfiles"};
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
}

namespace openpass::importer::xml::scenarioImporter::tag
{
    constexpr char absolute[] {"Absolute"};
    constexpr char act[] {"Act"};
    constexpr char action[] {"Action"};
    constexpr char actions[] {"Actions"};
    constexpr char actors[] {"Actors"};
    constexpr char byCondition[] {"ByCondition"};
    constexpr char byEntity[] {"ByEntity"};
    constexpr char byValue[] {"ByValue"};
    constexpr char catalogs[] {"Catalogs"};
    constexpr char catalogReference[] {"CatalogReference"};
    constexpr char condition[] {"Condition"};
    constexpr char conditionGroup[] {"ConditionGroup"};
    constexpr char directory[] {"Directory"};
    constexpr char dynamics[] {"Dynamics"};
    constexpr char endConditions[] {"EndConditions"};
    constexpr char entities[] {"Entities"};
    constexpr char entity[] {"Entity"};
    constexpr char event[] {"Event"};
    constexpr char init[] {"Init"};
    constexpr char lane[] {"Lane"};
    constexpr char logics[] {"Logics"};
    constexpr char longitudinal[] {"Longitudinal"};
    constexpr char maneuver[] {"Maneuver"};
    constexpr char members[] {"Members"};
    constexpr char object[] {"Object"};
    constexpr char orientation[] {"Orientation"};
    constexpr char parameter[] {"Parameter"};
    constexpr char parameterDeclaration[] {"ParameterDeclaration"};
    constexpr char position[] {"Position"};
    constexpr char Private[] {"Private"};
    constexpr char roadNetwork[] {"RoadNetwork"};
    constexpr char selection[] {"Selection"};
    constexpr char sequence[] {"Sequence"};
    constexpr char speed[] {"Speed"};
    constexpr char stochastics[] {"Stochastics"};
    constexpr char story[] {"Story"};
    constexpr char storyboard[] {"Storyboard"};
    constexpr char target[] {"Target"};
}

namespace openpass::importer::xml::scenarioImporter::attribute
{
    constexpr char actor[] {"actor"};
    constexpr char catalogName[] {"catalogName"};
    constexpr char delay[] {"delay"};
    constexpr char edge[] {"edge"};
    constexpr char entryName[] {"entryName"};
    constexpr char filePath[] {"filepath"};
    constexpr char heading[] {"h"};
    constexpr char laneId[] {"laneId"};
    constexpr char lowerBound[] {"lowerBound"};
    constexpr char name[] {"name"};
    constexpr char numberOfExecutions[] {"numberOfExecutions"};
    constexpr char object[] {"object"};
    constexpr char offset[] {"offset"};
    constexpr char path[] {"path"};
    constexpr char rate[] {"rate"};
    constexpr char roadId[] {"roadId"};
    constexpr char rule[] {"rule"};
    constexpr char s[] {"s"};
    constexpr char stdDeviation[] {"stdDeviation"};
    constexpr char type[] {"type"};
    constexpr char upperBound[] {"upperBound"};
    constexpr char value[] {"value"};
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
    constexpr char agentProfile[] {"AgentProfile"};
    constexpr char environmentConfig[] {"EnvironmentConfig"};
    constexpr char experimentConfig[] {"ExperimentConfig"};
    constexpr char friction[] {"Friction"};
    constexpr char frictions[] {"Frictions"};
    constexpr char homogenities[] {"Homogenities"};
    constexpr char homogenity[] {"Homogenity"};
    constexpr char libraries[] {"Libraries"};
    constexpr char loggingGroup[] {"LoggingGroup"};
    constexpr char loggingGroups[] {"LoggingGroups"};
    constexpr char platoonRate[] {"PlatoonRate"};
    constexpr char platoonRates[] {"PlatoonRates"};
    constexpr char regularLane[] {"RegularLane"};
    constexpr char rightMostLane[] {"RightMostLane"};
    constexpr char scenarioConfig[] {"ScenarioConfig"};
    constexpr char timeOfDay[] {"TimeOfDay"};
    constexpr char timeOfDays[] {"TimeOfDays"};
    constexpr char trafficConfig[] {"TrafficConfig"};
    constexpr char trafficParameter[] {"TrafficParameter"};
    constexpr char trafficVolume[] {"TrafficVolume"};
    constexpr char trafficVolumes[] {"TrafficVolumes"};
    constexpr char velocities[] {"Velocities"};
    constexpr char velocity[] {"Velocity"};
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
    constexpr char dimension[] {"Dimension"};
    constexpr char front[] {"Front"};
    constexpr char parameterDeclaration[] {"ParameterDeclaration"};
    constexpr char pedestrian[] {"Pedestrian"};
    constexpr char performance[] {"Performance"};
    constexpr char rear[] {"Rear"};
    constexpr char vehicle[] {"Vehicle"};
}

namespace openpass::importer::xml::vehicleModelsImporter::attribute
{
    constexpr char category[] {"category"};
    constexpr char height[] {"height"};
    constexpr char length[] {"length"};
    constexpr char mass[] {"mass"};
    constexpr char maxSpeed[] {"maxSpeed"};
    constexpr char maxSteering[] {"maxSteering"};
    constexpr char name[] {"name"};
    constexpr char positionX[] {"positionX"};
    constexpr char trackWidth[] {"trackWidth"};
    constexpr char wheelDiameter[] {"wheelDiameter"};
    constexpr char width[] {"width"};
    constexpr char x[] {"x"};
    constexpr char y[] {"y"};
    constexpr char z[] {"z"};
}
