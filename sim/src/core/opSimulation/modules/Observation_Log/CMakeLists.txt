################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME Observation_Log)

add_compile_definitions(OBSERVATION_LOG_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    observationCyclics.h
    observationFileHandler.h
    observationLogConstants.h
    observation_LogGlobal.h
    observation_log.h
    observation_logImplementation.h
    runStatistic.h
    runStatisticCalculation.h

  SOURCES
    observationCyclics.cpp
    observationFileHandler.cpp
    observation_log.cpp
    observation_logImplementation.cpp
    runStatistic.cpp
    runStatisticCalculation.cpp

  LIBRARIES
    Qt5::Core
)
