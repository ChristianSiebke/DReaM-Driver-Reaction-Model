set(COMPONENT_TEST_NAME OpenPassSlave_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/CoreFramework/OpenPassSlave)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    # AgentSampler
    agentSampler_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/framework/dynamicAgentTypeGenerator.cpp
    ${COMPONENT_SOURCE_DIR}/framework/dynamicParametersSampler.cpp
    ${COMPONENT_SOURCE_DIR}/framework/dynamicProfileSampler.cpp
    ${COMPONENT_SOURCE_DIR}/importer/systemConfig.cpp
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
    ${COMPONENT_SOURCE_DIR}/framework/eventNetworkDataPublisher.cpp
    ${COMPONENT_SOURCE_DIR}/importer/eventDetectorImporter.cpp

    # ManipulatorImporter
    manipulatorImporter_Tests.cpp

    # ParameterImporter
    parameterImporter_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/importer/parameterImporter.cpp

    # ProfilesImporter
    profilesImporter_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/importer/profiles.cpp
    ${COMPONENT_SOURCE_DIR}/importer/profilesImporter.cpp

    # Sampler
    sampler_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/framework/sampler.cpp

    # ScenarioImporter
    scenarioImporter_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/importer/oscImporterCommon.cpp
    ${COMPONENT_SOURCE_DIR}/importer/scenario.cpp
    ${COMPONENT_SOURCE_DIR}/importer/scenarioImporter.cpp
    ${COMPONENT_SOURCE_DIR}/importer/scenarioImporterHelper.cpp

    # SceneryImporter
    sceneryImporter_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/importer/connection.cpp
    ${COMPONENT_SOURCE_DIR}/importer/junction.cpp
    ${COMPONENT_SOURCE_DIR}/importer/road.cpp
    ${COMPONENT_SOURCE_DIR}/importer/road/roadObject.cpp
    ${COMPONENT_SOURCE_DIR}/importer/road/roadSignal.cpp
    ${COMPONENT_SOURCE_DIR}/importer/sceneryImporter.cpp

    # SlaveConfigImporter
    slaveConfigImporter_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/importer/slaveConfig.cpp
    ${COMPONENT_SOURCE_DIR}/importer/slaveConfigImporter.cpp

    # VehicleModelsImporter
    vehicleModelsImporter_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/importer/vehicleModelsImporter.cpp

  HEADERS
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
    ${COMPONENT_SOURCE_DIR}/framework/eventNetworkDataPublisher.h
    ${COMPONENT_SOURCE_DIR}/importer/eventDetectorImporter.h

    # ManipulatorImporter

    # ParameterImporter
    ${COMPONENT_SOURCE_DIR}/importer/parameterImporter.h

    # ProfilesImporter
    ${COMPONENT_SOURCE_DIR}/importer/profiles.h
    ${COMPONENT_SOURCE_DIR}/importer/profilesImporter.h

    # Sampler
    ${COMPONENT_SOURCE_DIR}/framework/sampler.h

    # ScenarioImporter
    ${COMPONENT_SOURCE_DIR}/importer/oscImporterCommon.h
    ${COMPONENT_SOURCE_DIR}/importer/scenario.h
    ${COMPONENT_SOURCE_DIR}/importer/scenarioImporter.h
    ${COMPONENT_SOURCE_DIR}/importer/scenarioImporterHelper.h

    # SceneryImporter
    ${COMPONENT_SOURCE_DIR}/importer/connection.h
    ${COMPONENT_SOURCE_DIR}/importer/junction.h
    ${COMPONENT_SOURCE_DIR}/importer/road.h
    ${COMPONENT_SOURCE_DIR}/importer/road/roadObject.h
    ${COMPONENT_SOURCE_DIR}/importer/road/roadSignal.h
    ${COMPONENT_SOURCE_DIR}/importer/sceneryImporter.h

    # SlaveConfigImporter
    ${COMPONENT_SOURCE_DIR}/importer/slaveConfig.h
    ${COMPONENT_SOURCE_DIR}/importer/slaveConfigImporter.h

    # VehicleModelsImporter
    ${COMPONENT_SOURCE_DIR}/importer/vehicleModelsImporter.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}
    ${COMPONENT_SOURCE_DIR}/../CoreShare
    ${COMPONENT_SOURCE_DIR}/framework
    ${COMPONENT_SOURCE_DIR}/importer
    ${COMPONENT_SOURCE_DIR}/modelElements
    ${COMPONENT_SOURCE_DIR}/modelInterface

  LIBRARIES
    Qt5::Core
    Common
    CoreShare
)
