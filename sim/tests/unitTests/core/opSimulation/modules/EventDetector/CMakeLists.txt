################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_TEST_NAME EventDetector_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/EventDetector)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT core
  DEFAULT_MAIN

  SOURCES
    ConditionalEventDetector_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/ConditionalEventDetector.cpp
    ${COMPONENT_SOURCE_DIR}/EventDetectorCommonBase.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/ConditionalEventDetector.h
    ${COMPONENT_SOURCE_DIR}/EventDetectorCommonBase.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
    Common
)
