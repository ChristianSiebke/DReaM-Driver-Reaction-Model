#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES algorithm_longitudinal.cpp
			 algorithm_longitudinalCalculations.cpp
			 algorithm_longitudinalImplementation.cpp)
set(HEADERS algorithm_longitudinal.h
			 algorithm_longitudinalCalculations.h
			 algorithm_longitudinalGlobal.h
			 algorithm_longitudinalImplementation.h)


add_definitions(-DALGORITHM_LONGITUDINAL_LIBRARY)

OPENPASS_add_component(Algorithm_Longitudinal "")