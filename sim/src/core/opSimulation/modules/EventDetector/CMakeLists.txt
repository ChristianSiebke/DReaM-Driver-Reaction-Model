################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME EventDetector)

add_compile_definitions(EVENT_DETECTOR_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    CollisionDetector.h
    ConditionalEventDetector.h
    EventDetectorCommonBase.h
    EventDetectorExport.h
    EventDetectorGlobal.h
    Conditions/ConditionCommonBase.h
    Conditions/ReachPositionCondition.h
    Conditions/SimulationTimeCondition.h

  SOURCES
    CollisionDetector.cpp
    ConditionalEventDetector.cpp
    EventDetectorCommonBase.cpp
    EventDetectorExport.cpp

  LIBRARIES
    Qt5::Core
    Common
)
