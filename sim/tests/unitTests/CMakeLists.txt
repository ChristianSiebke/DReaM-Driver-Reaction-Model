################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
add_subdirectory(common)

add_subdirectory(components/AlgorithmAEB)
add_subdirectory(components/Algorithm_AFDM)
add_subdirectory(components/Algorithm_FmuWrapper)
add_subdirectory(components/Algorithm_Lateral)
add_subdirectory(components/Algorithm_Longitudinal)
add_subdirectory(components/ComponentController)
add_subdirectory(components/Dynamics_Collision)
add_subdirectory(components/Dynamics_TF)
add_subdirectory(components/LimiterAccVehComp)
add_subdirectory(components/OpenScenarioActions)
add_subdirectory(components/SensorAggregation_OSI)
add_subdirectory(components/SensorFusionErrorless_OSI)
add_subdirectory(components/Sensor_Driver)
add_subdirectory(components/Sensor_OSI)
add_subdirectory(components/SignalPrioritizer)

add_subdirectory(core/opSimulation)
add_subdirectory(core/opSimulation/modules/BasicDataBuffer)
add_subdirectory(core/opSimulation/modules/EventDetector)
add_subdirectory(core/opSimulation/modules/Observation_Log)
add_subdirectory(core/opSimulation/modules/SpawnerPreRunCommon)
add_subdirectory(core/opSimulation/modules/SpawnerRuntimeCommon)
add_subdirectory(core/opSimulation/modules/SpawnerScenario)
add_subdirectory(core/opSimulation/modules/SpawnerWorldAnalyzer)
add_subdirectory(core/opSimulation/modules/World_OSI)
add_subdirectory(core/opSimulation/Scheduler)
