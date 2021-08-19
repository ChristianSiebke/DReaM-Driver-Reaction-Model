#/*******************************************************************************
#* Copyright (c) 2020 ITK Engineering GmbH
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(COMPONENT_NAME Dynamics_RegularTwoTrack)

add_compile_definitions(DYNAMICS_REGULAR_TWOTRACK_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    dynamics_regularTwoTrack.h
    src/dynamics_regularTwoTrackImpl.h
    src/dynamics_twotrack_tire.h
    src/dynamics_twotrack_vehicle.h

  SOURCES
    dynamics_regularTwoTrack.cpp
    src/dynamics_regularTwoTrackImpl.cpp
    src/dynamics_twotrack_tire.cpp
    src/dynamics_twotrack_vehicle.cpp

  LIBRARIES
    Qt5::Core

  INCDIRS
    src
)
