################################################################################
# Copyright (c) 2020 Uwe Woessner
# Copyright (c) 2020 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
#
# Global cmake file for the openPASS build process
#

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# NOTE: undocumented feature of cmake
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

cmake_policy(SET CMP0020 NEW)   # Automatically link Qt executables to qtmain target on Windows. OLD behavior may be removed in a future version.
cmake_policy(SET CMP0043 NEW)   # Ignore COMPILE_DEFINITIONS_<Config> properties. OLD behavior may be removed in a future version.
cmake_policy(SET CMP0054 NEW)   # Only interpret if() arguments as variables or keywords when unquoted. OLD behavior may be removed in a future version.

if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
  set(CMAKE_C_STANDARD_REQUIRED ON)
  set(CMAKE_C_EXTENSIONS ON)
endif()

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if(USE_CCACHE)
  set(CMAKE_C_COMPILER_LAUNCHER ccache)
  set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
endif()

if(MINGW)
  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    # this avoids string table overflow errors during compilation
    add_compile_options(-O1)
  endif()

  # required by some openpass link targets
  # and (almost) all test executables
  add_compile_options(-Wa,-mbig-obj)
endif()

set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON)
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "generated")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(Protobuf 2.6.1 REQUIRED)
add_compile_definitions(PROTOBUF_USE_DLLS)

find_package(OSI REQUIRED)

if(MINGW)
  # Bug in boost-install 1.72.0
  # setting boost mingw version manually
  # https://github.com/boostorg/boost_install/issues/33
  string(REGEX MATCHALL "[0-9]+" _CVER_COMPONENTS ${CMAKE_CXX_COMPILER_VERSION})
  list(GET _CVER_COMPONENTS 0 _CVER_MAJOR)
  list(GET _CVER_COMPONENTS 1 _CVER_MINOR)
  set(Boost_COMPILER "mgw${_CVER_MAJOR}${_CVER_MINOR}")
endif()
set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost COMPONENTS filesystem REQUIRED)

find_package(Qt5 COMPONENTS Concurrent Core Widgets Xml)
find_package(FMILibrary)

option(WITH_SIMCORE "Build OSI based scenario simulation" ON)
option(WITH_GUI "Build GUI" OFF)
option(WITH_TESTS "Build unit tests" ON)
option(WITH_EXTENDED_OSI "Assume an extended version of OSI is available" OFF)
option(WITH_PROTOBUF_ARENA "Make use of protobuf arena allocation" ON)
option(WITH_DEBUG_POSTFIX "Use 'd' binary postfix on Windows platform" ON)

if(WITH_TESTS)
  find_package(GTest)
  # as GMock currently doesn't provide a find_package config, gmock file location is derived from gtest in HelperMacros.cmake
  #find_package(GMock)
endif()

if(WITH_EXTENDED_OSI)
  add_compile_definitions(USE_EXTENDED_OSI)
endif()

if(WITH_PROTOBUF_ARENA)
  add_compile_definitions(USE_PROTOBUF_ARENA)
endif()

if(WIN32)
  set(CMAKE_INSTALL_PREFIX "C:/OpenPASS" CACHE PATH "Destination directory")
  add_compile_definitions(WIN32)
  add_compile_definitions(BOOST_ALL_NO_LIB)
  add_compile_definitions(BOOST_ALL_DYN_LINK)
  add_compile_definitions(_USE_MATH_DEFINES)
  option(OPENPASS_ADJUST_OUTPUT "Adjust output directory" ON)
  if(WITH_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX "d")
  endif()
else()
  set(CMAKE_INSTALL_PREFIX "/OpenPASS" CACHE PATH "Destination directory")
  add_compile_definitions(unix)
  add_link_options(LINKER:-z,defs)
  option(OPENPASS_ADJUST_OUTPUT "Adjust output directory" OFF)
endif()

add_compile_definitions($<IF:$<CONFIG:Debug>,DEBUG_POSTFIX="${CMAKE_DEBUG_POSTFIX}",DEBUG_POSTFIX=\"\">)

set(CMAKE_BUILD_RPATH \$ORIGIN)

include(HelperMacros)

#######################################

set(INSTALL_BIN_DIR "bin" CACHE PATH "Installation directory for executables")
set(INSTALL_LIB_DIR "lib" CACHE PATH "Installation directory for libraries")
set(SUBDIR_LIB_GUI "gui" CACHE PATH "Installation directory for GUI libraries")
set(SUBDIR_LIB_SIM "lib" CACHE PATH "Installation directory for simulation libraries")
set(SUBDIR_LIB_COMPONENTS "lib" CACHE PATH "Installation directory for core components")
set(SUBDIR_XML_COMPONENTS "components" CACHE PATH "Installation directory for core components")
set(INSTALL_INC_DIR "include" CACHE PATH "Installation directory for headers")
set(INSTALL_CFG_DIR "cfg" CACHE PATH "Installation directory for config files")
set(INSTALL_MAN_DIR "share/man" CACHE PATH "Installation directory for manual pages")
set(INSTALL_PKGCONFIG_DIR "share/pkgconfig" CACHE PATH "Installation directory for pkgconfig (.pc) files")

if(OPENPASS_ADJUST_OUTPUT)
set(OPENPASS_DESTDIR ${CMAKE_BINARY_DIR}/OpenPASS CACHE PATH "Destination directory")
file(MAKE_DIRECTORY ${OPENPASS_DESTDIR})
file(MAKE_DIRECTORY ${OPENPASS_DESTDIR}${SUBDIR_LIB_SIM})
file(MAKE_DIRECTORY ${OPENPASS_DESTDIR}${SUBDIR_LIB_GUI})
file(MAKE_DIRECTORY ${OPENPASS_DESTDIR}${SUBDIR_LIB_COMPONENTS})
file(MAKE_DIRECTORY ${OPENPASS_DESTDIR}${SUBDIR_LIB_COMPONENTS})
endif()

add_compile_definitions(SUBDIR_LIB_SIM="${SUBDIR_LIB_SIM}")
add_compile_definitions(SUBDIR_LIB_GUI="${SUBDIR_LIB_GUI}")
add_compile_definitions(SUBDIR_LIB_COMPONENTS="${SUBDIR_LIB_COMPONENTS}")
add_compile_definitions(SUBDIR_XML_COMPONENTS="${SUBDIR_XML_COMPONENTS}")

if(MSVC)
  if(WITH_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX "d")
  endif()
  add_compile_definitions(_CRT_SECURE_NO_DEPRECATE)
  add_compile_definitions(_CRT_NONSTDC_NO_DEPRECATE)

  # get rid of annoying template needs to have dll-interface warnings on VisualStudio
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4251 -wd4335")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4250")
endif()

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
include(InstallRequiredSystemLibraries)
