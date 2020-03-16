#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES world_basic.cpp
			 world_basic_implementation.cpp
			 agentAdapter.cpp
			 agentNetwork.cpp)
set(HEADERS world_basic.h
			 world_basic_implementation.h
			 world_basic_global.h
			 agentAdapter.h
			 agentNetwork.h)


add_definitions(-DWORLD_BASIC_LIBRARY)

add_library(World_Basic SHARED ${SOURCES} ${HEADERS})
target_link_libraries(World_Basic Common)

qt5_use_modules(World_Basic Xml)

install(TARGETS World_Basic
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" )
		
install(FILES ${HEADERS} DESTINATION "${INSTALL_INC_DIR}")
