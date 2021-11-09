/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <QString>

struct OutputAttributes
{
    const QString FRAMEWORKVERSION = "FrameworkVersion";
    const QString SCHEMAVERSION = "SchemaVersion";
    const QString RUNID = "RunId";
    const QString FAILUREPROBABITLITY = "FailureProbability";
    const QString LATENCY = "Latency";
    const QString OPENINGANGLEH = "OpeningAngleH";
    const QString OPENINGANGLEV = "OpeningAngleV";
    const QString MOUNTINGPOSITIONLONGITUDINAL = "MountingPosLongitudinal";
    const QString MOUNTINGPOSITIONLATERAL = "MountingPosLateral";
    const QString MOUNTINGPOSITIONHEIGHT = "MountingPosHeight";
    const QString ORIENTATIONYAW = "OrientationYaw";
    const QString ORIENTATIONPITCH = "OrientationPitch";
    const QString ORIENTATIONROLL = "OrientationRoll";
    const QString DETECTIONRANGE = "DetectionRange";
    const QString VEHICLEMODELTYPE = "VehicleModelType";
    const QString DRIVERPROFILENAME = "DriverProfileName";
    const QString AGENTTYPEGROUPNAME = "AgentTypeGroupName";
    const QString AGENTTYPENAME = "AgentTypeName";
    const QString TIME = "Time";
    const QString TYPE = "Type";
    const QString NAME = "Name";
    const QString KEY = "Key";
    const QString VALUE = "Value";
    const QString ID = "Id";
    const QString SOURCE = "Source";
    const QString WIDTH = "Width";
    const QString LENGTH = "Length";
    const QString HEIGHT = "Height";
    const QString LONGITUDINALPIVOTOFFSET = "LongitudinalPivotOffset";
};

struct OutputTags
{
    const QString RUNRESULTS = "RunResults";
    const QString RUNRESULT = "RunResult";
    const QString RUNSTATISTICS = "RunStatistics";
    const QString SIMULATIONOUTPUT = "SimulationOutput";
    const QString EVENTS = "Events";
    const QString EVENT = "Event";
    const QString EVENTPARAMETER = "EventParameter";
    const QString AGENTS = "Agents";
    const QString AGENT = "Agent";
    const QString SENSORS = "Sensors";
    const QString SENSOR = "Sensor";
    const QString CYCLICS = "Cyclics";
    const QString CYCLICSFILE = "CyclicsFile";
    const QString HEADER = "Header";
    const QString SAMPLES = "Samples";
    const QString SAMPLE = "Sample";
    const QString SCENERYFILE = "SceneryFile";
    const QString VEHICLEATTRIBUTES = "VehicleAttributes";
    const QString TRIGGERINGENTITIES = "TriggeringEntities";
    const QString AFFECTEDENTITIES = "AffectedEntities";
};

namespace output::tag {
static constexpr char ENTITY[] = "Entity";
static constexpr char PARAMETERS[] = "Parameters";
static constexpr char PARAMETER[] = "Parameter";
} // namespace output::tag

namespace output::attribute {
static constexpr char ID[] = "Id";
static constexpr char TIME[] = "Time";
static constexpr char TYPE[] = "Type";
static constexpr char NAME[] = "Name";
static constexpr char KEY[] = "Key";
static constexpr char VALUE[] = "Value";
} // namespace output::attribute

namespace output::event::tag {
static constexpr char EVENTS[] = "NewEvents";
static constexpr char EVENT[] = "Event";
static constexpr char TRIGGERING_ENTITIES[] = "TriggeringEntities";
static constexpr char AFFECTED_ENTITIES[] = "AffectedEntities";
} // namespace output::event::tag
