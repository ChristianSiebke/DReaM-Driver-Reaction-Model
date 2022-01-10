################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME Algorithm_Longitudinal)

add_compile_definitions(ALGORITHM_LONGITUDINAL_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    algorithm_longitudinal.h
    src/longCalcs.h
    src/algo_longImpl.h

  SOURCES
    algorithm_longitudinal.cpp
    src/longCalcs.cpp
    src/algo_longImpl.cpp

  LIBRARIES
    Qt5::Core
)
