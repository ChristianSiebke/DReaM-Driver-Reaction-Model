# /*********************************************************************
# * Copyright (c) 2019 Volkswagen Group of America.
# * Copyright (c) 2020 ITK Engineering GmbH.
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

project(Gui)
cmake_minimum_required(VERSION 3.14)

add_subdirectory(application)
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
