#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES CollisionDetector.cpp
			 ConditionalEventDetector.cpp
			 EventDetectorCommonBase.cpp
			 EventDetectorExport.cpp)
set(HEADERS CollisionDetector.h
			 ConditionalEventDetector.h
			 EventDetectorCommonBase.h
			 EventDetectorExport.h
			 EventDetectorGlobal.h
			 Conditions/ConditionCommonBase.h
			 Conditions/ReachPositionCondition.h
			 Conditions/SimulationTimeCondition.h)


add_definitions(-DEVENT_DETECTOR_LIBRARY)
include_directories(../../Common ../../Interfaces)

OPENPASS_ADD_MODULE(EventDetector "")