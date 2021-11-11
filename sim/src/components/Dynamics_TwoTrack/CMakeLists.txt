################################################################################
# Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME Dynamics_TwoTrack)

add_compile_definitions(DYNAMICS_TWOTRACK_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    dynamics_twotrack.h
    dynamics_twotrack_implementation.h
    dynamics_twotrack_global.h
    src/dynamics_twotrack_vehicle.h
    src/dynamics_twotrack_tire.h
    ../../common/pcm/controlSignal.h

  SOURCES
    dynamics_twotrack.cpp
    dynamics_twotrack_implementation.cpp
    src/dynamics_twotrack_vehicle.cpp
    src/dynamics_twotrack_tire.cpp
    ../../common/pcm/controlSignal.cpp

  INCDIRS
    src
    ../../common/pcm

  LIBRARIES
    Qt5::Core
)
