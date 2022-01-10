################################################################################
# Copyright (c) 2021 ITK Engineering GmbH
#               2019 Volkswagen Group of America
#               2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

project(openPASS)

cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

# add definitions: this needs to be in a certain gloabl file TODO
add_definitions(-DAPPLICATION_NAME="openPASS" -DAPPLICATION_VERSION="0.8")
add_definitions(${Qt5Widgets_DEFINITIONS})

# include QtWidgets package
find_package(Qt5Widgets REQUIRED)

# set include directories
include_directories(.)
include_directories(Interfaces)
include_directories(${Qt5Widgets_INCLUDE_DIRS})

# include interfaces, models and views
add_subdirectory(Models)

set(RC_ICONS openPASS.rc)

add_openpass_target(
    NAME openPASS TYPE executable COMPONENT common

    HEADERS
        Interfaces/openPASS/PluginInterface.h
        Interfaces/openPASS/PluginManagerInterface.h
        Interfaces/openPASS/RandomInterface.h
        Interfaces/openPASS/ServiceInterface.h
        Interfaces/openPASS/ServiceManagerInterface.h
        Interfaces/openPASS/ioplugininterface.h

    SOURCES
        openPASS.cpp
        ${RC_ICONS}

    LIBRARIES
        Qt5::Widgets
        ${PROJECT_NAME}_Models
)
