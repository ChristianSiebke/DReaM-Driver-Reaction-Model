#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
set (CMAKE_CXX_STANDARD 11)


set(BOOST_COMPONENTS locale)


find_package(Boost COMPONENTS ${BOOST_COMPONENTS} QUIET)
IF(WIN32)
      add_definitions("-DBOOST_ALL_NO_LIB")
      add_definitions("-DBOOST_ALL_DYN_LINK")
ENDIF(WIN32)
INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIR})

ADD_SUBDIRECTORY(Common)
ADD_SUBDIRECTORY(Components)
ADD_SUBDIRECTORY(CoreFramework)
ADD_SUBDIRECTORY(CoreModules)
if(BUILD_PCM)
ADD_SUBDIRECTORY(Common_PCM)
ADD_SUBDIRECTORY(Components_PCM)
ADD_SUBDIRECTORY(CoreModules_PCM)
endif(BUILD_PCM)