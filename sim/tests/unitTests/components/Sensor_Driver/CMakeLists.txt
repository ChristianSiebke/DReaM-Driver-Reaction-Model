################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_TEST_NAME SensorDriver_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/components/Sensor_Driver/src)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT module
  DEFAULT_MAIN

  SOURCES
    sensorDriver_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/sensor_driverCalculations.cpp
    ${COMPONENT_SOURCE_DIR}/sensor_driverImpl.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/sensor_driverCalculations.h
    ${COMPONENT_SOURCE_DIR}/sensor_driverImpl.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/RoutePlanning

  LIBRARIES
    Qt5::Core
)

