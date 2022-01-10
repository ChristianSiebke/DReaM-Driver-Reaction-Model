################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_NAME Manipulator)

add_compile_definitions(MANIPULATOR_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    CollisionManipulator.h
    DefaultCustomCommandAction.h
    ComponentStateChangeManipulator.h
    LaneChangeManipulator.h
    ManipulatorCommonBase.h
    ManipulatorExport.h
    ManipulatorGlobal.h
    NoOperationManipulator.h
    RemoveAgentsManipulator.h
    SpeedActionManipulator.h
    TrajectoryManipulator.h
    AcquirePositionManipulator.h
    ../../../../common/vector2d.h
    srcCollisionPostCrash/polygon.h
    srcCollisionPostCrash/collisionDetection_Impact_implementation.h
    ../../../../core/common/coreDataPublisher.h

  SOURCES
    CollisionManipulator.cpp
    DefaultCustomCommandAction.cpp
    ComponentStateChangeManipulator.cpp
    LaneChangeManipulator.cpp
    ManipulatorCommonBase.cpp
    ManipulatorExport.cpp
    NoOperationManipulator.cpp
    RemoveAgentsManipulator.cpp
    SpeedActionManipulator.cpp
    TrajectoryManipulator.cpp
    AcquirePositionManipulator.cpp
    srcCollisionPostCrash/polygon.cpp
    srcCollisionPostCrash/collisionDetection_Impact_implementation.cpp

  LIBRARIES
    Qt5::Core
    CoreCommon

  INCDIRS
    ../../../../common
    srcCollisionPostCrash
    core/common
)
