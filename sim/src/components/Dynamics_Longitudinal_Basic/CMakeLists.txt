################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME Dynamics_Longitudinal_Basic)

add_compile_definitions(DYNAMICS_LONGITUDINAL_BASIC_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    dynamics_longitudinal_basic.h
    dynamics_longitudinal_basic_implementation.h
    dynamics_longitudinal_basic_global.h

  SOURCES
    dynamics_longitudinal_basic.cpp
    dynamics_longitudinal_basic_implementation.cpp

  INCDIRS
    ../../../include
    ../../common

  LIBRARIES
    Qt5::Core
    Common

  LINKOSI
)
