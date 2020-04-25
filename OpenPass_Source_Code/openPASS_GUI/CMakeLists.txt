#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(FOLDER "GUI")
include_directories(openPASS/Interfaces
					openPASS/Models
					openPASS-Window/Interfaces
					openPASS-Window/Models
					openPASS-Window/Presenters
					openPASS-Window/Views
					openPASS-AgentConfiguration/Interfaces
					openPASS-AgentConfiguration/Models
					openPASS-AgentConfiguration/Presenters
					openPASS-AgentConfiguration/Views
					openPASS-AgentConfiguration/SystemModels
					openPASS-AgentConfiguration
					openPASS-Project
					openPASS-Simulation
					openPASS-System
					openPASS-Component
					openPASS-Project/Interfaces
					openPASS-Simulation/Interfaces
					openPASS-System/Interfaces
					openPASS-Component/Interfaces
					openPASS
					Global
					../OpenPASS/Common
					./)
					

add_compile_definitions(APPLICATION_NAME="${CPACK_PACKAGE_NAME}")
if("${CPACK_PACKAGE_VERSION_PATCH}" STREQUAL "0")
add_compile_definitions(APPLICATION_VERSION="${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}" )
else()
add_compile_definitions(APPLICATION_VERSION="${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}" )
endif()
ADD_SUBDIRECTORY(openPASS)
ADD_SUBDIRECTORY(openPASS-AgentConfiguration)
ADD_SUBDIRECTORY(openPASS-Component)
ADD_SUBDIRECTORY(openPASS-Project)
ADD_SUBDIRECTORY(openPASS-TrafficSimulation)
ADD_SUBDIRECTORY(openPASS-System)
ADD_SUBDIRECTORY(openPASS-Window)
