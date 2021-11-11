################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME ComponentController)

add_compile_definitions(COMPONENT_CONTROLLER_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    componentController.h
    src/componentControllerCommon.h
    src/componentControllerImpl.h
    src/componentStateInformation.h
    src/condition.h
    src/stateManager.h

  SOURCES
    componentController.cpp
    src/componentControllerImpl.cpp
    src/componentStateInformation.cpp
    src/condition.cpp
    src/stateManager.cpp

  LIBRARIES
    Qt5::Core
)
