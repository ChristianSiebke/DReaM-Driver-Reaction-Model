################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME opSimulationManager)

set(CMAKE_AUTOMOC ON)

add_compile_definitions(SimulationCore_EXPORTS)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE executable COMPONENT bin

  HEADERS
    framework/processManager.h
    framework/config.h
    framework/simulationConfig.h
    importer/configImporter.h
    opSimulationManager.h
    ../common/log.h

  SOURCES
    framework/main.cpp
    framework/processManager.cpp
    importer/configImporter.cpp
    ../common/log.cpp

  INCDIRS
    framework
    importer
    ../../../..

  LIBRARIES
    Qt5::Concurrent
    Qt5::Core
    Qt5::Widgets
    Qt5::Xml
    CoreCommon
)

set_target_properties(${COMPONENT_NAME} PROPERTIES FOLDER "core")
