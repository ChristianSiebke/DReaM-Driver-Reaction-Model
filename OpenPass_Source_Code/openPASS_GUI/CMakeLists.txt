# /*********************************************************************
# * Copyright (c) 2019 Volkswagen Group of America.
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

project(openPASS_GUI)
cmake_minimum_required(VERSION 3.14)

add_subdirectory(openPASS)
add_subdirectory(openPASS-Window)
add_subdirectory(openPASS-Project)
add_subdirectory(openPASS-Component)
add_subdirectory(openPASS-System)
add_subdirectory(openPASS-AgentConfiguration)
add_subdirectory(openPASS-TrafficSimulation)
add_subdirectory(openPASS-Visualisation)


