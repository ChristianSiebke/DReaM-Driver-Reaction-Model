################################################################################
# Copyright (c) 2020 ITK Engineering GmbH
#               2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

include_directories(../../../common)

set(SOURCES
    TrafficScene.cpp
    TrajectoryGraphicsItem.cpp
    ViewStatistics.cpp
)

set(HEADERS
    TrafficScene.h
    TrajectoryGraphicsItem.h
    ViewStatistics.h
)

set(UI
    ViewStatistics.ui
)

add_library(${PROJECT_NAME}_Views STATIC ${SOURCES} ${HEADERS} ${UI})
target_link_libraries(${PROJECT_NAME}_Models PRIVATE Qt5::Widgets Gui_Common)
