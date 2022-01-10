################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME Observation_Ttc)

add_compile_definitions(OBSERVATION_TTC_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    observation_ttc.h
    observation_ttc_implementation.h
    observation_ttc_global.h

  SOURCES
    observation_ttc.cpp
    observation_ttc_implementation.cpp

  INCDIRS
    ../../../Common
    ../../../../Common

  LIBRARIES
    Qt5::Core
)
