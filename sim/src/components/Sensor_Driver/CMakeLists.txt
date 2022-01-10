################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME Sensor_Driver)

add_compile_definitions(SENSOR_DRIVER_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    sensor_driver.h
    src/sensor_driverCalculations.h
    src/sensor_driverImpl.h
    src/Signals/sensorDriverSignal.h
    src/Signals/sensor_driverDefinitions.h

  SOURCES
    sensor_driver.cpp
    src/sensor_driverCalculations.cpp
    src/sensor_driverImpl.cpp

  INCDIRS
    src
    ../../core/opSimulation/modules/World_OSI/RoutePlanning

  LIBRARIES
    Qt5::Core
)
