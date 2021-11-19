################################################################################
# Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME Algorithm_RouteControl)

add_compile_definitions(ALGORITHM_ROUTECONTROL_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    algorithm_RouteControl.h
    algorithm_RouteControl_implementation.h
    algorithm_RouteControl_global.h
    routeControl.h
    ../../common/pcm/controlSignal.h

  SOURCES
    algorithm_RouteControl.cpp
    algorithm_RouteControl_implementation.cpp
    routeControl.cpp
    ../../common/pcm/controlSignal.cpp

  INCDIRS
    ../../common/pcm

  LIBRARIES
    Qt5::Core
)
