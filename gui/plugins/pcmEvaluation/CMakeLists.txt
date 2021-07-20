# /*********************************************************************
# * Copyright (c) 2021 ITK Engineering GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

project(openPASS-pcmEvaluation)

cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# include QtWidgets package
find_package(Qt5 COMPONENTS Core Widgets Xml REQUIRED)

# set include directories
include_directories(.)
include_directories(Interfaces)
include_directories(Models)
include_directories(Presenters)
include_directories(Views)
include_directories(../window/Interfaces)
include_directories(../../application/Interfaces)
include_directories(../../common)
include_directories(../../common/pcm/PCM_Data)
include_directories(../../common/pcm/PCM_Importer)
include_directories(../../../sim/src/common)
include_directories(../../../sim/src)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Xml_INCLUDE_DIRS})

# include interfaces, models and views
add_subdirectory(Interfaces)
add_subdirectory(Models)
add_subdirectory(Presenters)
add_subdirectory(Views)

# include headers and sources of this plugin
set(SOURCES PluginPcm_Eval.cpp)
set(HEADERS PluginPcm_Eval.h)

# declare target library
add_library(Pcm_eval SHARED ${SOURCES} ${HEADERS} )

# link with Qt libraries, models, views and Interfaces
target_link_libraries(Pcm_eval PRIVATE
                      Qt5::Widgets
                      Qt5::Core
                      Qt5::Xml
                      ${PROJECT_NAME}_Models
                      ${PROJECT_NAME}_Views
                      ${PROJECT_NAME}_Interfaces
                      ${PROJECT_NAME}_Presenters)

install(TARGETS Pcm_eval
        DESTINATION ${SUBDIR_LIB_GUI})
