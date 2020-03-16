#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES CollisionManipulator.cpp
			 ComponentStateChangeManipulator.cpp
			 LaneChangeManipulator.cpp
			 ManipulatorCommonBase.cpp
			 ManipulatorExport.cpp
			 RemoveAgentsManipulator.cpp)
set(HEADERS CollisionManipulator.h
			 ComponentStateChangeManipulator.h
			 LaneChangeManipulator.h
			 ManipulatorCommonBase.h
			 ManipulatorExport.h
			 ManipulatorGlobal.h
			 RemoveAgentsManipulator.h)


add_definitions(-DMANIPULATOR_LIBRARY)
include_directories(../../Common ../../Interfaces)

OPENPASS_ADD_MODULE(Manipulator "")
