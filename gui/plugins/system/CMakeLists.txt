################################################################################
# Copyright (c) 2019 Volkswagen Group of America
#               2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

project(openPASS-System)

cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# include QtWidgets package
find_package(Qt5Widgets REQUIRED)

# definitions --> must be in some global file TODO
add_definitions(-DSUBDIR_COMPONENTS="/components")

# set include directories
include_directories(.)
include_directories(Interfaces)
include_directories(../project/Interfaces)
include_directories(../window/Interfaces)
include_directories(../component/Interfaces)
include_directories(../../application/Interfaces)
include_directories(${Qt5Widgets_INCLUDE_DIRS})

# include interfaces, models and views
add_subdirectory(Models)
add_subdirectory(Views)

qt5_add_resources(openPASS-System.qrc)

add_openpass_target(
    NAME System TYPE library LINKAGE shared COMPONENT gui

    HEADERS
        Interfaces/openPASS-System/SystemComponentInputItemInterface.h
        Interfaces/openPASS-System/SystemComponentInputMapInterface.h
        Interfaces/openPASS-System/SystemComponentItemInterface.h
        Interfaces/openPASS-System/SystemComponentManagerInterface.h
        Interfaces/openPASS-System/SystemComponentMapInterface.h
        Interfaces/openPASS-System/SystemComponentOutputItemInterface.h
        Interfaces/openPASS-System/SystemComponentOutputMapInterface.h
        Interfaces/openPASS-System/SystemComponentParameterItemInterface.h
        Interfaces/openPASS-System/SystemComponentParameterMapInterface.h
        Interfaces/openPASS-System/SystemComponentScheduleInterface.h
        Interfaces/openPASS-System/SystemConnectionMapInterface.h
        Interfaces/openPASS-System/SystemConnectionItemInterface.h
        Interfaces/openPASS-System/SystemInterface.h
        Interfaces/openPASS-System/SystemItemInterface.h
        Interfaces/openPASS-System/SystemMapInterface.h 
        SystemPlugin.h

    SOURCES
        SystemPlugin.cpp

    LIBRARIES
        Qt5::Widgets
        Window
        Component
        Project
        ${PROJECT_NAME}_Models
        ${PROJECT_NAME}_Views
)