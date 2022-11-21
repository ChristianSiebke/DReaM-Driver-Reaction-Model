################################################################################
# Copyright (c) 2020-2021 ITK Engineering GmbH
#               2019 Volkswagen Group of America
#               2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

project(Gui)
cmake_minimum_required(VERSION 3.14)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(application)
add_subdirectory(common)
#add_subdirectory(plugins/agentConfiguration)
add_subdirectory(plugins/component)
add_subdirectory(plugins/pcmEvaluation)
add_subdirectory(plugins/pcmSimulation)
add_subdirectory(plugins/project)
add_subdirectory(plugins/statistics)
add_subdirectory(plugins/system)
add_subdirectory(plugins/timePlot)
#add_subdirectory(plugins/trafficSimulation)
add_subdirectory(plugins/window)
