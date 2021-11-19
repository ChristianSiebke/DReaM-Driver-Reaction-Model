################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME Parameters_Vehicle)

add_compile_definitions(PARAMETERS_VEHICLE_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    parameters_vehicle.h
    src/parameters_vehicleImpl.h

  SOURCES
    parameters_vehicle.cpp
    src/parameters_vehicleImpl.cpp

  LIBRARIES
    Qt5::Core
)
