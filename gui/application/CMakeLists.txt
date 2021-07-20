# /*********************************************************************
# * Copyright (c) 2019 Volkswagen Group of America.
# * Copyright (c) 2021 ITK Engineering GmbH.
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

project(openPASS)

cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

# add definitions: this needs to be in a certain gloabl file TODO
add_definitions(-DAPPLICATION_NAME="openPASS" -DAPPLICATION_VERSION="0.7")
add_definitions(${Qt5Widgets_DEFINITIONS})

# include QtWidgets package
find_package(Qt5Widgets REQUIRED)

# set include directories
include_directories(.)
include_directories(Interfaces)
include_directories(${Qt5Widgets_INCLUDE_DIRS})

# include interfaces, models and views
add_subdirectory(Interfaces)
add_subdirectory(Models)

# include sources and headers
set(SOURCES openPASS.cpp)
set(RC_ICONS openPASS.rc)


# declare target library
add_executable(openPASS WIN32 ${SOURCES} ${RC_ICONS})

# link with Qt libraries, models, views and Interfaces
target_link_libraries(openPASS PRIVATE
                      Qt5::Widgets
                      ${PROJECT_NAME}_Models
                      ${PROJECT_NAME}_Interfaces)

install(TARGETS openPASS
        DESTINATION ${INSTALL_BIN_DIR})
