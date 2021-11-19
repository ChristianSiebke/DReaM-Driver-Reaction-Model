################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME Observation_State)

add_compile_definitions(OBSERVATION_STATE_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    observation_state.h
    observation_state_implementation.h
    observation_state_global.h
    observationtypes.h

  SOURCES
    observation_state.cpp
    observation_state_implementation.cpp

  LIBRARIES
    Qt5::Core
)
