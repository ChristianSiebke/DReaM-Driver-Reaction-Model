#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

SET (SOURCES action_longitudinalDriver.cpp
			 action_longitudinalDriverImplementation.cpp)
SET (HEADERS action_longitudinalDriver.h
			 action_longitudinalDriverGlobal.h
			 action_longitudinalDriverImplementation.h)

add_definitions(-DACTION_LONGITUDINAL_DRIVER_LIBRARY)

OPENPASS_add_component(Action_LongitudinalDriver "")
