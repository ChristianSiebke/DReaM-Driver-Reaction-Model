#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

SET (SOURCES spawnpoint_start.cpp
			 spawnpoint_start_implementation.cpp)
SET (HEADERS spawnpoint_start.h
			 spawnpoint_start_implementation.h
			 spawnpoint_start_global.h)


add_definitions(-DSPAWNPOINT_START_LIBRARY)

add_library(SpawnPoint_Start SHARED ${SOURCES} ${HEADERS})
target_link_libraries(SpawnPoint_Start Common)

qt5_use_modules(SpawnPoint_Start Xml)

install(TARGETS SpawnPoint_Start
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" )
		
install(FILES ${HEADERS} DESTINATION "${INSTALL_INC_DIR}")
