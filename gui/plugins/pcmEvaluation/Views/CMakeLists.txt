################################################################################
# Copyright (c) 2021 ITK Engineering GmbH
#               2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

set(SOURCES
    ViewPcm_Eval.cpp
    TrajectoryGraphicsItem.cpp
    TrafficScene.cpp
)

set(HEADERS
    ViewPcm_Eval.h
    TrajectoryGraphicsItem.h
    TrafficScene.h
)

set(UI
    ViewPcm_Eval.ui
)

add_library(${PROJECT_NAME}_Views STATIC ${SOURCES} ${HEADERS} ${UI})