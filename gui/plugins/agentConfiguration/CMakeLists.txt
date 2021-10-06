################################################################################
# Copyright (c) 2019 Volkswagen Group of America
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

project(openPASS-AgentConfiguration)

cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# increase path length
set(CMAKE_OBJECT_PATH_MAX 600)

# include QtWidgets package
find_package(Qt5Widgets REQUIRED)

# set include directories
include_directories(.)
include_directories(Interfaces)
include_directories(../project/Interfaces)
include_directories(../window/Interfaces)
include_directories(../component/Interfaces)
include_directories(../system/Interfaces)
include_directories(../../application/Interfaces)
include_directories(../../common)
include_directories(${Qt5Widgets_INCLUDE_DIRS})

# include interfaces, models and views
add_subdirectory(Interfaces)
add_subdirectory(Models)
add_subdirectory(Presenters)
add_subdirectory(Views)

# include headers and sources of this plugin
set(SOURCES AgentConfigurationPlugin.cpp)
set(HEADERS AgentConfigurationPlugin.h)

# declare target library
add_library(AgentConfiguration SHARED ${SOURCES} ${HEADERS})

# link with Qt libraries, models, views and Interfaces
target_link_libraries(AgentConfiguration PRIVATE
                      Qt5::Widgets
                      ${PROJECT_NAME}_Models
                      ${PROJECT_NAME}_Views
                      ${PROJECT_NAME}_Presenters
                      ${PROJECT_NAME}_Interfaces)

if(WIN32)
    install(TARGETS AgentConfiguration
        RUNTIME DESTINATION ${SUBDIR_LIB_GUI}) 
elseif(UNIX)
    install(TARGETS AgentConfiguration
        LIBRARY DESTINATION ${SUBDIR_LIB_GUI}) 
endif()