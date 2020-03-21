#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

#
# Root-file for the OpenPASS build process
#
#

# requirements to cmake version

# 2.8.7 is too old for identifying clang
CMAKE_MINIMUM_REQUIRED(VERSION 3.15.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)


# allow only out-of-source builds
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" in_source_build)
if(in_source_build)
  message(FATAL_ERROR "A separate build directory is required, please create one and run cmake from this directory")
endif(in_source_build)


# project
PROJECT(OpenPASS CXX C)

if(POLICY CMP0020)
       #qt policy Automatically link Qt executables to qtmain target on Windows.
       cmake_policy(SET CMP0020 NEW)
endif()
cmake_policy(SET CMP0043 NEW)
cmake_policy(SET CMP0054 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#### Microsoft Visual Studio specific flags

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
   # CMAKE does not yet (3.17) support  CMAKE_CXX_STANDARD 17 for MSVC
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /wd4251")
   # get rid of annoying template needs to have dll-interface warnings on VisualStudio
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4251 -wd4335")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4250")
   set(CMAKE_DEBUG_POSTFIX "d")
   add_compile_definitions(_CRT_SECURE_NO_DEPRECATE)
   add_compile_definitions(_CRT_NONSTDC_NO_DEPRECATE)
   include_directories(${CMAKE_CURRENT_SOURCE_DIR})
endif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

add_compile_definitions($<IF:$<CONFIG:Debug>,DEBUG_POSTFIX=\"${CMAKE_DEBUG_POSTFIX}\",DEBUG_POSTFIX=\"\">)

# this is used to override some of cmake's original find-modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# default locations for 3rd party libraries
set(CMAKE_PREFIX_PATH $ENV{EXTERNLIBS}/boost ${CMAKE_PREFIX_PATH})
set(CMAKE_PREFIX_PATH $ENV{EXTERNLIBS}/osi ${CMAKE_PREFIX_PATH})
set(CMAKE_PREFIX_PATH $ENV{EXTERNLIBS}/protobuf ${CMAKE_PREFIX_PATH})
set(CMAKE_PREFIX_PATH $ENV{EXTERNLIBS}/gtest ${CMAKE_PREFIX_PATH})
set(CMAKE_PREFIX_PATH $ENV{EXTERNLIBS}/qt5 ${CMAKE_PREFIX_PATH})

find_package(OSI)
find_package(Protobuf 2.6.1 REQUIRED)
add_compile_definitions(PROTOBUF_USE_DLLS)
find_package(GTest)
find_package(Qt5Core)
find_package(Qt5Network)
find_package(Qt5Xml)
find_package(Qt5Widgets)
find_package(Qt5OpenGL)
find_package(Qt5Gui)
find_package(Qt5Svg)
find_package(Qt5PrintSupport)
find_package(Qt5UiTools)
find_package(Qt5Script)
find_package(Qt5ScriptTools)

option(BUILD_PCM "Build PCM support" OFF)
option(BUILD_BASIC "Build Basic support" OFF)
option(BUILD_OSI "Build OSI support" ON)

# this is used to override some of cmake's original find-modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

set(OPENPASS_DESTDIR ${CMAKE_BINARY_DIR}/OpenPASS CACHE PATH "Destination directory")
file(MAKE_DIRECTORY ${OPENPASS_DESTDIR})
file(MAKE_DIRECTORY ${OPENPASS_DESTDIR}/bin)
file(MAKE_DIRECTORY ${OPENPASS_DESTDIR}/bin/plugins)
file(MAKE_DIRECTORY ${OPENPASS_DESTDIR}/bin/coreModules)
file(MAKE_DIRECTORY ${OPENPASS_DESTDIR}/bin/gui)

add_compile_definitions(SUBDIR_LIB_SIM="/lib" SUBDIR_LIB_PLUGIN="plugins" SUBDIR_LIB_GUI="/bin/gui"  SUBDIR_COMPONENTS="/components")

include(HelperMacros)
#######################################

if(WIN32)
  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/windows;${CMAKE_MODULE_PATH}")
endif()
if(UNIX)
  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/unix;${CMAKE_MODULE_PATH}")
endif()
if(APPLE)
  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/apple;${CMAKE_MODULE_PATH}")
endif()
if(MINGW)
  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/mingw;${CMAKE_MODULE_PATH}")
endif(MINGW)

set(INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Installation directory for executables")
set(INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "Installation directory for libraries")
set(INSTALL_INC_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Installation directory for headers")
set(INSTALL_CFG_DIR "${CMAKE_INSTALL_PREFIX}/cfg" CACHE PATH "Installation directory for config files")
set(INSTALL_MAN_DIR "${CMAKE_INSTALL_PREFIX}/share/man" CACHE PATH "Installation directory for manual pages")
set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_PREFIX}/share/pkgconfig" CACHE PATH "Installation directory for pkgconfig (.pc) files")


include_directories(${PROJECT_SOURCE_DIR}/OpenPass_Source_Code/openPASS)

include_directories(${GTEST_INCLUDE_DIRS})

# descend to sources
add_subdirectory(OpenPass_Source_Code)
