# /*********************************************************************
# * Copyright (c) 2019 Volkswagen Group of America.
# * Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/


project(openPASS-Component)

cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# include QtWidgets package
find_package(Qt5Widgets REQUIRED)

# set include directories
include_directories(.)
include_directories(Interfaces)
include_directories(../../application/Interfaces)
include_directories(${Qt5Widgets_INCLUDE_DIRS})

# include interfaces, models and views
add_subdirectory(Models)

add_openpass_target(
    NAME Component TYPE library LINKAGE shared COMPONENT gui

    HEADERS
        Interfaces/openPASS-Component/ComponentInputItemInterface.h
        Interfaces/openPASS-Component/ComponentInputMapInterface.h
        Interfaces/openPASS-Component/ComponentInterface.h
        Interfaces/openPASS-Component/ComponentItemInterface.h
        Interfaces/openPASS-Component/ComponentMapInterface.h
        Interfaces/openPASS-Component/ComponentOutputItemInterface.h
        Interfaces/openPASS-Component/ComponentOutputMapInterface.h
        Interfaces/openPASS-Component/ComponentParameterItemInterface.h
        Interfaces/openPASS-Component/ComponentParameterMapInterface.h
        Interfaces/openPASS-Component/ComponentScheduleInterface.h
        Interfaces/openPASS-Component/ComponentTypedefItemInterface.h
        Interfaces/openPASS-Component/ComponentTypedefMapInterface.h
        ComponentPlugin.h

    SOURCES
        ComponentPlugin.cpp

    LIBRARIES
        Qt5::Widgets
        ${PROJECT_NAME}_Models
)