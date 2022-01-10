################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME Sensor_OSI)

add_compile_definitions(SENSOR_OSI_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    sensorOSI.h
    src/objectDetectorBase.h
    src/sensorGeometric2D.h

  SOURCES
    sensorOSI.cpp
    src/objectDetectorBase.cpp
    src/sensorGeometric2D.cpp

  INCDIRS
    ../../core/opSimulation/modules/World_OSI

  LIBRARIES
    Qt5::Core
    Common

  LINKOSI
)
