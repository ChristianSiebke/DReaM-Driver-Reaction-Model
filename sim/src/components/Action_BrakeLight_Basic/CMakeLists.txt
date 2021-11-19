################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME Action_BrakeLight_Basic)

add_compile_definitions(ACTION_BRAKELIGHT_BASIC_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    action_brakelight_basic.h
    action_brakelight_basic_implementation.h
    action_brakelight_basic_global.h

  SOURCES
    action_brakelight_basic.cpp
    action_brakelight_basic_implementation.cpp

  LIBRARIES
    Qt5::Core
)
