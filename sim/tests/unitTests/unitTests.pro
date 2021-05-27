#/*******************************************************************************
#* Copyright (c) 2019, 2020, 2021 in-tech GmbH
#* Copyright (c) 2021 ITK Engineering GmbH
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

TEMPLATE = subdirs

DEFINES += USE_PROTOBUF_ARENA

SUBDIRS = \
    algorithmAEB_Tests \
    algorithmAgentFollowingDriverModel_Tests \
    algorithmFmuWrapper_Tests \
    algorithmLateral_Tests \
    algorithmLongitudinal_Tests \
    Algorithm_ECU \
    Algorithm_RouteControl \
    Algorithm_Switch \
    basicDataBuffer_Tests \
    componentController_Tests \
    Dynamics_Chassis \
    dynamicsCollision_Tests \
    dynamicsTrajectoryFollower_Tests \
    Dynamics_TwoTrack \
    eventDetector_Tests \
    limiterAccelerationVehiclecomponents_Tests \
    observationLog_Tests \
    \ #observationLogNG_Tests \
    openPassSlave_Tests \
    openScenarioActions_Tests \
    scheduler_Tests \
    sensorDriver_Tests \
    sensorAggregation_Tests \
    sensorFusionErrorless_Tests \
    sensorOSI_Tests \
    signalPrioritizer_Tests \
    spawnPointScenario_Tests \
    spawnPointPreRunCommon_Tests \
    spawnPointRuntimeCommon_Tests \
    spawnPointWorldAnalyzer_Tests \
    worldOSI_Tests \
    common_Tests

Algorithm_ECU.file = \
    $$PWD/components/Algorithm_ECU/Algorithm_ECU.pro

algorithmAEB_Tests.file = \
    $$PWD/components/AlgorithmAEB/algorithmAEB_Tests.pro

algorithmAgentFollowingDriverModel_Tests.file = \
    $$PWD/components/Algorithm_AFDM/Afdm_Tests.pro

algorithmFmuWrapper_Tests.file = \
    $$PWD/components/Algorithm_FmuWrapper/FmuWrapper_Tests.pro

algorithmLateral_Tests.file = \
    $$PWD/components/Algorithm_Lateral/algorithmLateral_Tests.pro

algorithmLongitudinal_Tests.file = \
    $$PWD/components/Algorithm_Longitudinal/algorithmLongitudinal_Tests.pro

Algorithm_ECU.file = \
    $$PWD/components/Algorithm_ECU/Algorithm_ECU.pro

Algorithm_RouteControl.file = \
    $$PWD/components/Algorithm_RouteControl/Algorithm_RouteControl.pro

Algorithm_Switch.file = \
    $$PWD/components/Algorithm_Switch/Algorithm_Switch.pro

basicDataBuffer_Tests.file = \
    $$PWD/core/slave/modules/BasicDataBuffer/basicDataBuffer_Tests.pro

componentController_Tests.file = \
    $$PWD/components/ComponentController/componentController_Tests.pro

dynamicsCollision_Tests.file = \
    $$PWD/components/Dynamics_Collision/dynamicsCollision_Tests.pro

dynamicsTrajectoryFollower_Tests.file = \
    $$PWD/components/Dynamics_TF/dynamicsTF_Tests.pro

Dynamics_Chassis.file = \
    $$PWD/components/Dynamics_Chassis/Dynamics_Chassis.pro

Dynamics_TwoTrack.file = \
    $$PWD/components/Dynamics_TwoTrack/Dynamics_TwoTrack.pro

eventDetector_Tests.file = \
    $$PWD/core/slave/modules/EventDetector/eventDetector_Tests.pro

limiterAccelerationVehiclecomponents_Tests.file = \
    $$PWD/components/LimiterAccVehComp/limiterAccVehComp_Tests.pro

observationLog_Tests.file = \
    $$PWD/core/slave/modules/Observation_Log/observationLog_Tests.pro

openPassSlave_Tests.file = \
    $$PWD/core/slave/openPassSlave_Tests.pro

openScenarioActions_Tests.file = \
    $$PWD/components/OpenScenarioActions/openScenarioActions_Tests.pro

scheduler_Tests.file = \
    $$PWD/core/slave/Scheduler/scheduler_Tests.pro

sensorDriver_Tests.file = \
    $$PWD/components/Sensor_Driver/sensorDriver_Tests.pro

sensorAggregation_Tests.file = \
    $$PWD/components/SensorAggregation_OSI/sensorAggregationOSI_Tests.pro

sensorFusionErrorless_Tests.file = \
    $$PWD/components/SensorFusionErrorless_OSI/sensorFusionErrorless_Tests.pro

sensorOSI_Tests.file = \
    $$PWD/components/Sensor_OSI/sensorOSI_Tests.pro

signalPrioritizer_Tests.file = \
    $$PWD/components/SignalPrioritizer/signalPrioritizer_Tests.pro

spawnPointScenario_Tests.file = \
    $$PWD/core/slave/modules/SpawnPointScenario/spawnPointScenario_Tests.pro

spawnPointPreRunCommon_Tests.file = \
    $$PWD/core/slave/modules/SpawnPointPreRunCommon/spawnPointPreRunCommon_Tests.pro

spawnPointRuntimeCommon_Tests.file = \
    $$PWD/core/slave/modules/SpawnPointRuntimeCommon/spawnPointRuntimeCommon_Tests.pro

spawnPointWorldAnalyzer_Tests.file = \
    $$PWD/core/slave/modules/SpawnPointWorldAnalyzer/spawnPointWorldAnalyzer_Tests.pro

worldOSI_Tests.file = \
    $$PWD/core/slave/modules/World_OSI/worldOSI_Tests.pro

common_Tests.file = \
    $$PWD/common/Common_Tests.pro
