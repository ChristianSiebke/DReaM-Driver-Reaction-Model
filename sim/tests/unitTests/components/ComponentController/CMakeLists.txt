################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_TEST_NAME ComponentController_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/components/ComponentController/src)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT module
  DEFAULT_MAIN

  SOURCES
    componentController_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/componentControllerImpl.cpp
    ${COMPONENT_SOURCE_DIR}/componentStateInformation.cpp
    ${COMPONENT_SOURCE_DIR}/condition.cpp
    ${COMPONENT_SOURCE_DIR}/stateManager.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/componentControllerImpl.h
    ${COMPONENT_SOURCE_DIR}/componentStateInformation.h
    ${COMPONENT_SOURCE_DIR}/condition.h
    ${COMPONENT_SOURCE_DIR}/stateManager.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
)

