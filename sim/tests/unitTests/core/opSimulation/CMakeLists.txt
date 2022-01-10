################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_TEST_NAME opSimulation_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/core/opSimulation)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    ${OPENPASS_SIMCORE_DIR}/core/common/log.cpp

    # AgentSampler
    agentSampler_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/framework/dynamicAgentTypeGenerator.cpp
    ${COMPONENT_SOURCE_DIR}/framework/dynamicParametersSampler.cpp
    ${COMPONENT_SOURCE_DIR}/framework/dynamicProfileSampler.cpp
    ${COMPONENT_SOURCE_DIR}/modelElements/agentType.cpp
    ${COMPONENT_SOURCE_DIR}/modelElements/componentType.cpp

    # CommandLineParser
    commandLineParser_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/framework/commandLineParser.cpp

    # Directories
    directories_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/framework/directories.cpp

    # EventNetwork
    eventNetwork_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/framework/eventNetwork.cpp
    ${COMPONENT_SOURCE_DIR}/importer/eventDetectorImporter.cpp

    # ImporterCommon
    ${COMPONENT_SOURCE_DIR}/importer/importerCommon.cpp

    # ManipulatorImporter
    manipulatorImporter_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/importer/oscImporterCommon.cpp

    # ParameterImporter
    parameterImporter_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/importer/parameterImporter.cpp

    # ProfilesImporter
    profilesImporter_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/importer/profiles.cpp
    ${COMPONENT_SOURCE_DIR}/importer/profilesImporter.cpp

    # Publishers
    agentDataPublisher_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/framework/agentDataPublisher.cpp
    ${OPENPASS_SIMCORE_DIR}/core/common/coreDataPublisher.cpp

    # Sampler
    sampler_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/framework/sampler.cpp

    # ScenarioImporter
    scenarioImporter_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/importer/oscImporterCommon.cpp
    ${COMPONENT_SOURCE_DIR}/importer/scenario.cpp
    ${COMPONENT_SOURCE_DIR}/importer/scenarioImporter.cpp
    ${COMPONENT_SOURCE_DIR}/importer/scenarioImporterHelper.cpp
    ${COMPONENT_SOURCE_DIR}/importer/sceneryDynamics.cpp

    # SceneryImporter
    roadGeometry_Tests.cpp
    sceneryImporter_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/importer/connection.cpp
    ${COMPONENT_SOURCE_DIR}/importer/junction.cpp
    ${COMPONENT_SOURCE_DIR}/importer/road.cpp
    ${COMPONENT_SOURCE_DIR}/importer/road/roadObject.cpp
    ${COMPONENT_SOURCE_DIR}/importer/road/roadSignal.cpp
    ${COMPONENT_SOURCE_DIR}/importer/sceneryImporter.cpp

    # SimulationConfigImporter
    simulationConfigImporter_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/importer/simulationConfig.cpp
    ${COMPONENT_SOURCE_DIR}/importer/simulationConfigImporter.cpp

    # SystemConfigImporter
    systemConfigImporter_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/importer/systemConfig.cpp
    ${COMPONENT_SOURCE_DIR}/importer/systemConfigImporter.cpp

    # VehicleModelsImporter
    vehicleModelsImporter_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/importer/vehicleModelsImporter.cpp

  HEADERS
    ${OPENPASS_SIMCORE_DIR}/core/common/log.h

    # AgentSampler
    ${COMPONENT_SOURCE_DIR}/framework/dynamicAgentTypeGenerator.h
    ${COMPONENT_SOURCE_DIR}/framework/dynamicParametersSampler.h
    ${COMPONENT_SOURCE_DIR}/framework/dynamicProfileSampler.h
    ${COMPONENT_SOURCE_DIR}/importer/systemConfig.h
    ${COMPONENT_SOURCE_DIR}/modelElements/agentType.h
    ${COMPONENT_SOURCE_DIR}/modelElements/componentType.h

    # CommandLineParser
    ${COMPONENT_SOURCE_DIR}/framework/commandLineParser.h

    # Directories
    ${COMPONENT_SOURCE_DIR}/framework/directories.h

    # EventNetwork
    ${COMPONENT_SOURCE_DIR}/framework/eventNetwork.h
    ${COMPONENT_SOURCE_DIR}/importer/eventDetectorImporter.h

    # ImporterCommon
    ${COMPONENT_SOURCE_DIR}/importer/importerCommon.h

    # ManipulatorImporter
    ${COMPONENT_SOURCE_DIR}/importer/oscImporterCommon.h

    # ParameterImporter
    ${COMPONENT_SOURCE_DIR}/importer/parameterImporter.h

    # ProfilesImporter
    ${COMPONENT_SOURCE_DIR}/importer/profiles.h
    ${COMPONENT_SOURCE_DIR}/importer/profilesImporter.h

    # Publishers
    ${COMPONENT_SOURCE_DIR}/framework/agentDataPublisher.h
    ${OPENPASS_SIMCORE_DIR}/core/common/coreDataPublisher.h

    # Sampler
    ${COMPONENT_SOURCE_DIR}/framework/sampler.h

    # ScenarioImporter
    ${COMPONENT_SOURCE_DIR}/importer/oscImporterCommon.h
    ${COMPONENT_SOURCE_DIR}/importer/scenario.h
    ${COMPONENT_SOURCE_DIR}/importer/scenarioImporter.h
    ${COMPONENT_SOURCE_DIR}/importer/scenarioImporterHelper.h
    ${COMPONENT_SOURCE_DIR}/importer/sceneryDynamics.h

    # SceneryImporter
    ${COMPONENT_SOURCE_DIR}/importer/connection.h
    ${COMPONENT_SOURCE_DIR}/importer/junction.h
    ${COMPONENT_SOURCE_DIR}/importer/road.h
    ${COMPONENT_SOURCE_DIR}/importer/road/roadObject.h
    ${COMPONENT_SOURCE_DIR}/importer/road/roadSignal.h
    ${COMPONENT_SOURCE_DIR}/importer/sceneryImporter.h

    # SimulationConfigImporter
    ${COMPONENT_SOURCE_DIR}/importer/simulationConfig.h
    ${COMPONENT_SOURCE_DIR}/importer/simulationConfigImporter.h

    # VehicleModelsImporter
    ${COMPONENT_SOURCE_DIR}/importer/vehicleModelsImporter.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}
    ${COMPONENT_SOURCE_DIR}/..
    ${COMPONENT_SOURCE_DIR}/framework
    ${COMPONENT_SOURCE_DIR}/importer
    ${COMPONENT_SOURCE_DIR}/modelElements
    ${COMPONENT_SOURCE_DIR}/modelInterface

  LIBRARIES
    Qt5::Core
    Qt5::XmlPatterns
    Common
    CoreCommon
)
