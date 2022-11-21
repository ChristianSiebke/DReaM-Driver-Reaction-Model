################################################################################
# Copyright (c) 2021 ITK Engineering GmbH
#               2019-2021 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

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
    opSimulation_Tests \
    openScenarioActions_Tests \
    scheduler_Tests \
    sensorDriver_Tests \
    sensorAggregation_Tests \
    sensorFusionErrorless_Tests \
    sensorOSI_Tests \
    signalPrioritizer_Tests \
    spawnerScenario_Tests \
    spawnerPreRunCommon_Tests \
    spawnerRuntimeCommon_Tests \
    spawnerWorldAnalyzer_Tests \
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
    $$PWD/core/opsimulation/modules/BasicDataBuffer/basicDataBuffer_Tests.pro

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
    $$PWD/core/opsimulation/modules/EventDetector/eventDetector_Tests.pro

limiterAccelerationVehiclecomponents_Tests.file = \
    $$PWD/components/LimiterAccVehComp/limiterAccVehComp_Tests.pro

observationLog_Tests.file = \
    $$PWD/core/opsimulation/modules/Observation_Log/observationLog_Tests.pro

opSimulation_Tests.file = \
    $$PWD/core/opsimulation/opSimulation_Tests.pro

openScenarioActions_Tests.file = \
    $$PWD/components/OpenScenarioActions/openScenarioActions_Tests.pro

scheduler_Tests.file = \
    $$PWD/core/opsimulation/Scheduler/scheduler_Tests.pro

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

spawnerScenario_Tests.file = \
    $$PWD/core/opsimulation/modules/SpawnerScenario/spawnerScenario_Tests.pro

spawnerPreRunCommon_Tests.file = \
    $$PWD/core/opsimulation/modules/SpawnerPreRunCommon/spawnerPreRunCommon_Tests.pro

spawnerRuntimeCommon_Tests.file = \
    $$PWD/core/opsimulation/modules/SpawnerRuntimeCommon/spawnerRuntimeCommon_Tests.pro

spawnerWorldAnalyzer_Tests.file = \
    $$PWD/core/opsimulation/modules/SpawnerWorldAnalyzer/spawnerWorldAnalyzer_Tests.pro

worldOSI_Tests.file = \
    $$PWD/core/opsimulation/modules/World_OSI/worldOSI_Tests.pro

common_Tests.file = \
    $$PWD/common/Common_Tests.pro
