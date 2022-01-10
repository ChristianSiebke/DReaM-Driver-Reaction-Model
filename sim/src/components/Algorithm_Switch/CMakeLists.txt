################################################################################
# Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME Algorithm_Switch)

add_compile_definitions(ALGORITHM_SWITCH_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    algorithm_Switch.h
    algorithm_Switch_implementation.h
    algorithm_Switch_global.h
    switch.h
    ../../common/pcm/controlSignal.h

  SOURCES
    algorithm_Switch.cpp
    algorithm_Switch_implementation.cpp
    switch.cpp
    ../../common/pcm/controlSignal.cpp

  INCDIRS
    ../../common/pcm

  LIBRARIES
    Qt5::Core
)
