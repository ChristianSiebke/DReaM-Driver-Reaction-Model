################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME Algorithm_CruiseControlByDistance)

add_compile_definitions(ALGORITHM_CRUISECONTROLBYDISTANCE_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    algorithm_cruisecontrolbydistance.h
    algorithm_cruisecontrolbydistance_global.h
    algorithm_cruisecontrolbydistance_implementation.h

  SOURCES
    algorithm_cruisecontrolbydistance.cpp
    algorithm_cruisecontrolbydistance_implementation.cpp

  LIBRARIES
    Qt5::Core
)
