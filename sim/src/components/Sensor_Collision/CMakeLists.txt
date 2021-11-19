################################################################################
# Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME Sensor_Collision)

add_compile_definitions(SENSOR_COLLISION_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    sensor_collision.h
    sensor_collision_implementation.h
    sensor_collision_global.h

  SOURCES
    sensor_collision.cpp
    sensor_collision_implementation.cpp

  LIBRARIES
    Qt5::Core
)
