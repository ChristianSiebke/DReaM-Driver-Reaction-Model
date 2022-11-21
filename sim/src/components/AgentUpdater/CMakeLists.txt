################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME AgentUpdater)

add_compile_definitions(AGENT_UPDATER_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    agentUpdater.h
    src/agentUpdaterImpl.h

  SOURCES
    agentUpdater.cpp
    src/agentUpdaterImpl.cpp

  LIBRARIES
    Qt5::Core
)
