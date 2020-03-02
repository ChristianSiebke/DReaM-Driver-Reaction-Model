#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

SET (SOURCES algorithm_cruisecontrolbydistance.cpp
			 algorithm_cruisecontrolbydistance_implementation.cpp)
SET (HEADERS algorithm_cruisecontrolbydistance.h
			 algorithm_cruisecontrolbydistance_global.h
			 algorithm_cruisecontrolbydistance_implementation.h)


add_definitions(-DALGORITHM_CRUISECONTROLBYDISTANCE_LIBRARY)

OPENPASS_add_component(Algorithm_CruiseControlByDistance "")