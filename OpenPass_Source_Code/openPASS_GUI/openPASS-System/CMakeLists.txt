# /*********************************************************************
# * Copyright (c) 2019 Volkswagen Group of America.
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

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
include_directories(../openPASS-Project/Interfaces)
include_directories(../openPASS-Window/Interfaces)
include_directories(../openPASS-Component/Interfaces)
include_directories(../openPASS/Interfaces)
include_directories(${Qt5Widgets_INCLUDE_DIRS})

# include interfaces, models and views
add_subdirectory(Interfaces)
add_subdirectory(Models)
add_subdirectory(Views)

# include headers and sources of this plugin
set(SOURCES SystemPlugin.cpp)
set(HEADERS SystemPlugin.h)
qt5_add_resources(openPASS-System.qrc)

# declare target library
add_library(System SHARED ${SOURCES} ${HEADERS} )

# link with Qt libraries, models, views and Interfaces
target_link_libraries(System PRIVATE
                      Qt5::Widgets
                      ${PROJECT_NAME}_Models
                      ${PROJECT_NAME}_Views
                      ${PROJECT_NAME}_Interfaces)

# target directories. Needs to be defined in some global file TODO
set_target_properties(System PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY "C:/OpenPASS/bin/gui"
                      PREFIX "")
