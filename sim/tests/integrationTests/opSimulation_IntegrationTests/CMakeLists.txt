################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_TEST_NAME opSimulation_IntegrationTests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/core/opSimulation)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN
  LINKOSI

  SOURCES
    ${OPENPASS_SIMCORE_DIR}/core/common/callbacks.cpp
    ${OPENPASS_SIMCORE_DIR}/core/common/log.cpp

    # Parameter
    ${COMPONENT_SOURCE_DIR}/importer/parameterImporter.cpp

    # Scenario
    ScenarioImporter_IntegrationTests.cpp
    ${COMPONENT_SOURCE_DIR}/importer/eventDetectorImporter.cpp
    ${COMPONENT_SOURCE_DIR}/importer/oscImporterCommon.cpp
    ${COMPONENT_SOURCE_DIR}/importer/scenario.cpp
    ${COMPONENT_SOURCE_DIR}/importer/scenarioImporter.cpp
    ${COMPONENT_SOURCE_DIR}/importer/scenarioImporterHelper.cpp
    ${COMPONENT_SOURCE_DIR}/importer/sceneryDynamics.cpp

    # Scenery
    SceneryImporter_IntegrationTests.cpp
    ${COMPONENT_SOURCE_DIR}/importer/connection.cpp
    ${COMPONENT_SOURCE_DIR}/importer/junction.cpp
    ${COMPONENT_SOURCE_DIR}/importer/road.cpp
    ${COMPONENT_SOURCE_DIR}/importer/road/roadObject.cpp
    ${COMPONENT_SOURCE_DIR}/importer/road/roadSignal.cpp
    ${COMPONENT_SOURCE_DIR}/importer/scenery.cpp
    ${COMPONENT_SOURCE_DIR}/importer/sceneryImporter.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/Stochastics/stochastics_implementation.cpp

    # SimulationConfig
    SimulationConfigImporter_IntegrationTests.cpp
    ${COMPONENT_SOURCE_DIR}/framework/directories.cpp
    ${COMPONENT_SOURCE_DIR}/importer/parameterImporter.cpp
    ${COMPONENT_SOURCE_DIR}/importer/simulationConfig.cpp
    ${COMPONENT_SOURCE_DIR}/importer/simulationConfigImporter.cpp

    # SystemConfig
    SystemConfigImporter_IntegrationTests.cpp
    ${COMPONENT_SOURCE_DIR}/modelElements/agentBlueprint.cpp
    ${COMPONENT_SOURCE_DIR}/modelElements/agentType.cpp
    ${COMPONENT_SOURCE_DIR}/modelElements/componentType.cpp
    ${COMPONENT_SOURCE_DIR}/importer/systemConfig.cpp
    ${COMPONENT_SOURCE_DIR}/importer/systemConfigImporter.cpp

    # VehicleModels
    VehicleModelsImporter_IntegrationTests.cpp
    ${COMPONENT_SOURCE_DIR}/importer/vehicleModels.cpp
    ${COMPONENT_SOURCE_DIR}/importer/vehicleModelsImporter.cpp

    # World
    ${COMPONENT_SOURCE_DIR}/bindings/worldBinding.cpp
    ${COMPONENT_SOURCE_DIR}/bindings/worldLibrary.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/OWL/DataTypes.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/OWL/OpenDriveTypeMapper.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/AgentAdapter.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/LaneStream.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/Localization.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/RoadStream.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/WorldData.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/WorldDataQuery.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/WorldDataException.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/WorldObjectAdapter.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/WorldToRoadCoordinateConverter.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/egoAgent.cpp

  HEADERS
    ${OPENPASS_SIMCORE_DIR}/core/common/log.h

    # Parameter
    ${COMPONENT_SOURCE_DIR}/importer/parameterImporter.h

    # Scenario
    ${COMPONENT_SOURCE_DIR}/importer/eventDetectorImporter.h
    ${COMPONENT_SOURCE_DIR}/importer/oscImporterCommon.h
    ${COMPONENT_SOURCE_DIR}/importer/scenario.h
    ${COMPONENT_SOURCE_DIR}/importer/scenarioImporter.h
    ${COMPONENT_SOURCE_DIR}/importer/scenarioImporterHelper.h
    ${COMPONENT_SOURCE_DIR}/importer/sceneryDynamics.h

    # Scenery
    ${COMPONENT_SOURCE_DIR}/importer/connection.h
    ${COMPONENT_SOURCE_DIR}/importer/junction.h
    ${COMPONENT_SOURCE_DIR}/importer/road.h
    ${COMPONENT_SOURCE_DIR}/importer/road/roadObject.h
    ${COMPONENT_SOURCE_DIR}/importer/road/roadSignal.h
    ${COMPONENT_SOURCE_DIR}/importer/scenery.h
    ${COMPONENT_SOURCE_DIR}/importer/sceneryImporter.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/Stochastics/stochastics_implementation.h

    # SimulationConfig
    ${COMPONENT_SOURCE_DIR}/framework/directories.h
    ${COMPONENT_SOURCE_DIR}/importer/parameterImporter.h
    ${COMPONENT_SOURCE_DIR}/importer/simulationConfig.h
    ${COMPONENT_SOURCE_DIR}/importer/simulationConfigImporter.h

    # SystemConfig
    ${COMPONENT_SOURCE_DIR}/modelElements/agentType.h
    ${COMPONENT_SOURCE_DIR}/modelElements/componentType.h
    ${COMPONENT_SOURCE_DIR}/importer/systemConfig.h
    ${COMPONENT_SOURCE_DIR}/importer/systemConfigImporter.h

    # VehicleModels
    ${COMPONENT_SOURCE_DIR}/importer/vehicleModels.h
    ${COMPONENT_SOURCE_DIR}/importer/vehicleModelsImporter.h

    # World
    ${COMPONENT_SOURCE_DIR}/bindings/world.h
    ${COMPONENT_SOURCE_DIR}/bindings/worldBinding.h
    ${COMPONENT_SOURCE_DIR}/bindings/worldLibrary.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/OWL/DataTypes.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/OWL/OpenDriveTypeMapper.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/AgentAdapter.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/Localization.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/WorldData.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/WorldDataQuery.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/WorldDataException.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/WorldObjectAdapter.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/WorldToRoadCoordinateConverter.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/egoAgent.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}
    ${COMPONENT_SOURCE_DIR}/..
    ${COMPONENT_SOURCE_DIR}/framework
    ${COMPONENT_SOURCE_DIR}/importer/road
    ${COMPONENT_SOURCE_DIR}/modelElements
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/Localization
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/OWL

  LIBRARIES
    Qt5::Core
    Qt5::Xml
    Qt5::XmlPatterns
    Common
    CoreCommon

  SIMCORE_DEPS
    World_OSI

  RESOURCES
    Resources
)

