# /*********************************************************************
# * Copyright (c) 2019 Volkswagen Group of America.
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

project(openPASS-Window)

cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# increase path length
set(CMAKE_OBJECT_PATH_MAX 300)

# include QtWidgets package
find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)

# set include directories
include_directories(.)
include_directories(Interfaces)
include_directories(../../application/Interfaces)
include_directories(${Qt5Widgets_INCLUDE_DIRS})

# include interfaces, models and views
add_subdirectory(Interfaces)
add_subdirectory(Models)
add_subdirectory(Presenters)
add_subdirectory(Views)

# include headers and sources of this plugin
set(SOURCES WindowPlugin.cpp)
set(HEADERS WindowPlugin.h)
set(RESOURCES openPASS-Window.qrc)

# declare target library
add_library(Window SHARED ${SOURCES} ${HEADERS} ${RESOURCES})

# link with Qt libraries, models, views and Interfaces
target_link_libraries(Window PRIVATE
                      Qt5::Widgets
                      ${PROJECT_NAME}_Models
                      ${PROJECT_NAME}_Views
                      ${PROJECT_NAME}_Presenters
                      ${PROJECT_NAME}_Interfaces)

# target directories. Needs to be defined in some global file TODO
set_target_properties(Window PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY "${OPENPASS_DESTDIR}/${SUBDIR_LIB_GUI}"
                      PREFIX "")
