#/*******************************************************************************
#* Copyright (c) 2019 in-tech GmbH
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

TEMPLATE = subdirs

SUBDIRS = \
    algorithmAEB_Tests \
    algorithmAgentFollowingDriverModel_Tests \
    algorithmLateral_Tests \
    algorithmLongitudinal_Tests \
    componentController_Tests \
    dynamicsCollision_Tests \
    eventDetector_Tests \
    limiterAccelerationVehicleComponents_Tests \
    observationLog_Tests \
    openPassSlave_Tests \
    scheduler_Tests \
    sensorDriver_Tests \
    sensorFusionOSI_Tests \
    sensorOSI_Tests \
    signalPrioritizer_Tests \
    spawnPoint_Tests \
    worldOSI_Tests

algorithmAEB_Tests.file = \
    $$PWD/Components/AlgorithmAEB/algorithmAEB_Tests.pro

algorithmAgentFollowingDriverModel_Tests.file = \
    $$PWD/Components/Algorithm_AFDM/Afdm_Tests.pro

algorithmLateral_Tests.file = \
    $$PWD/Components/Algorithm_Lateral/algorithmLateral_Tests.pro

algorithmLongitudinal_Tests.file = \
    $$PWD/Components/Algorithm_Longitudinal/algorithmLongitudinal_Tests.pro

componentController_Tests.file = \
    $$PWD/Components/ComponentController/componentController_Tests.pro

dynamicsCollision_Tests.file = \
    $$PWD/Components/Dynamics_Collision/dynamicsCollision_Tests.pro

eventDetector_Tests.file = \
    $$PWD/CoreModules/EventDetector/eventDetector_Tests.pro

limiterAccelerationVehicleComponents_Tests.file = \
    $$PWD/Components/LimiterAccVehComp/limiterAccVehComp_Tests.pro

observationLog_Tests.file = \
    $$PWD/CoreModules/Observation_Log/observationLog_Tests.pro

openPassSlave_Tests.file = \
    $$PWD/CoreFramework/OpenPassSlave/openPassSlave_Tests.pro

scheduler_Tests.file = \
    $$PWD/CoreFramework/OpenPassSlave/Scheduler/scheduler_Tests.pro

sensorDriver_Tests.file = \
    $$PWD/Components/Sensor_Driver/sensorDriver_Tests.pro

sensorFusionOSI_Tests.file = \
    $$PWD/Components/SensorFusion_OSI/sensorFusionOSI_Tests.pro

sensorOSI_Tests.file = \
    $$PWD/Components/Sensor_OSI/sensorOSI_Tests.pro

signalPrioritizer_Tests.file = \
    $$PWD/Components/SignalPrioritizer/signalPrioritizer_Tests.pro

spawnPoint_Tests.file = \
    $$PWD/CoreModules/SpawnPoint_OSI/spawnPoint_Tests.pro

worldOSI_Tests.file = \
    $$PWD/CoreModules/World_OSI/worldOSI_Tests.pro
