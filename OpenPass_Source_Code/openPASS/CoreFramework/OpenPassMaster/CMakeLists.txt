#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES  framework/main.cpp
			  framework/processManager.cpp
			  importer/masterConfigImporter.cpp)
			  
set(HEADERS framework/processManager.h
			 framework/masterConfig.h
			 framework/slaveConfig.h
			 openPassMaster.h
			 importer/masterConfigImporter.h)
			 
openpass_add_executable(OpenPassMaster )
set_target_properties(OpenPassMaster PROPERTIES FOLDER "CoreFramework")

target_link_libraries(OpenPassMaster CoreShare)

qt5_use_modules(OpenPassMaster Widgets Xml Concurrent)