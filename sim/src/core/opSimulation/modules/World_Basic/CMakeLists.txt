################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME World_Basic)

add_compile_definitions(WORLD_BASIC_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    agentAdapter.h
    agentNetwork.h
    world_basic.h
    world_basic_implementation.h
    world_basic_global.h

  SOURCES
    agentAdapter.cpp
    agentNetwork.cpp
    world_basic.cpp
    world_basic_implementation.cpp

  LIBRARIES
    Qt5::Core
    Qt5::Xml
    Common
)
