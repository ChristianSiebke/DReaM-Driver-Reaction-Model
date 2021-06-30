# /*********************************************************************
# * Copyright (c) 2019 Volkswagen Group of America.
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
add_subdirectory(Interfaces)
add_subdirectory(Models)

# include sources and headers
set(SOURCES ComponentPlugin.cpp)
set(HEADERS ComponentPlugin.h)

# declare target library
add_library(Component SHARED ${SOURCES} ${HEADERS})

# link with Qt libraries, models, views and Interfaces
target_link_libraries(Component PRIVATE
                      Qt5::Widgets
                      ${PROJECT_NAME}_Models
                      ${PROJECT_NAME}_Interfaces)

# target directories. Needs to be defined in some global file TODO
set_target_properties(Component PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/${SUBDIR_LIB_GUI}"
    PREFIX "")
