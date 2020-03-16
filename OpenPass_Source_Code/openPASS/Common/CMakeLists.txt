#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES vector2d.cpp
			 vector3d.cpp
			 eventDetectorDefinitions.cpp)

set(HEADERS accelerationSignal.h
			 agentBasedManipulatorEvent.h
			 agentCompToCompCtrlSignal.h
			 basicEvent.h
			 boostGeometryCommon.h
			 collisionEvent.h
			 commonTools.h
			 compCtrlToAgentCompSignal.h
			 componentStateChangeEvent.h
			 componentWarningEvent.h
			 dynamicsSignal.h
			 eventDetectorDefinitions.h
			 eventTypes.h
			 globalDefinitions.h
			 laneChangeEvent.h
			 lateralSignal.h
			 longitudinalSignal.h
			 observationTypes.h
			 openScenarioDefinitions.h
			 parametersVehicleSignal.h
			 primitiveSignals.h
			 secondaryDriverTasksSignal.h
			 sensorDataSignal.h
			 spawnPointDefinitions.h
			 steeringSignal.h
			 vector2d.h
			 vector3d.h
			 vectorSignals.h
			 vehicleComponentEvent.h
			 worldDefinitions.h
			 opExport.h)
			 

openpass_add_library(Common SHARED)
