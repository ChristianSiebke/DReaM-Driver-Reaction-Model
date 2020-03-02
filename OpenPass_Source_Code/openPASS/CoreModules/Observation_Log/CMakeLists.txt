#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

SET (SOURCES observationCyclics.cpp
			 observationFileHandler.cpp
			 observation_log.cpp
			 observation_logImplementation.cpp
			 runStatistic.cpp
			 runStatisticCalculation.cpp)
SET (HEADERS observationCyclics.h
			 observationFileHandler.h
			 observationLogConstants.h
			 observationLogGlobal.h
			 observation_log.h
			 observation_logImplementation.h
			 runStatistic.h
			 runStatisticCalculation.h)


add_definitions(-DOBSERVATION_LOG_LIBRARY)

OPENPASS_ADD_MODULE(Observation_Log "")
