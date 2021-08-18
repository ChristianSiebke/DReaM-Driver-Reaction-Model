################################################################################
# Copyright (c) 2020, 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
# Copyright (c) 2020 HLRS, University of Stuttgart
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

# @file HelperMacros.cmake
#
# @author Reinhard Biegel, in-tech GmbH
# @author René Paris, in-tech GmbH
# @author Uwe Woessner, HLRS
#
# Provides helper macros for defining build targets in the openPASS build ecosystem
#

# Macro to adjust the output directories of a target
function(openpass_adjust_output_dir targetname)
    #MESSAGE("openpass_adjust_output_dir(${targetname}) : OPENPASS_DESTDIR = ${OPENPASS_DESTDIR}, ARGV1=${ARGV1}")

    SET(MYPATH_POSTFIX )
    # optional path-postfix specified?
    IF(NOT "${ARGV1}" STREQUAL "")
      IF("${ARGV1}" MATCHES "^/.*")
        SET(MYPATH_POSTFIX "${ARGV1}")
      ELSE()
        SET(MYPATH_POSTFIX "/${ARGV1}")
      ENDIF()
    ENDIF()

    # adjust
    IF(CMAKE_CONFIGURATION_TYPES)
      # generator supports configuration types
      FOREACH(conf_type ${CMAKE_CONFIGURATION_TYPES})
        STRING(TOUPPER "${conf_type}" upper_conf_type_str)
            SET_TARGET_PROPERTIES(${ARGV0} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${upper_conf_type_str} "${OPENPASS_DESTDIR}${MYPATH_POSTFIX}")
            SET_TARGET_PROPERTIES(${ARGV0} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${upper_conf_type_str} "${OPENPASS_DESTDIR}${MYPATH_POSTFIX}")
            SET_TARGET_PROPERTIES(${ARGV0} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${upper_conf_type_str} "${OPENPASS_DESTDIR}${MYPATH_POSTFIX}")
      ENDFOREACH(conf_type)
    ELSE(CMAKE_CONFIGURATION_TYPES)
      # no configuration types - probably makefile generator
      STRING(TOUPPER "${CMAKE_BUILD_TYPE}" upper_build_type_str)
      SET_TARGET_PROPERTIES(${ARGV0} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${upper_build_type_str} "${OPENPASS_DESTDIR}${MYPATH_POSTFIX}")
      SET_TARGET_PROPERTIES(${ARGV0} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${upper_build_type_str} "${OPENPASS_DESTDIR}${MYPATH_POSTFIX}")
      SET_TARGET_PROPERTIES(${ARGV0} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${upper_build_type_str} "${OPENPASS_DESTDIR}${MYPATH_POSTFIX}")
    ENDIF(CMAKE_CONFIGURATION_TYPES)
endfunction(openpass_adjust_output_dir)

##
# WITH_COVERAGE extension
#
# When set, add compiler flags and make additional test targets available
#
# @author René Paris, in-tech GmbH
#
if(WITH_COVERAGE)
  if(CMAKE_COMPILER_IS_GNUCXX)
    include(TestCoverage)
    set_coverage_compiler_flags()
  else()
    message(FATAL_ERROR "Generating code coverage is only supported for GNU gcc")
  endif()
endif()


##
# Macro to add openPASS libraries, executables and tests
#
# @author Reinhard Biegel
# @author René Paris
#
# Usages:
#   For building executables:
#     add_openpass_target(NAME <target> TYPE executable
#                         SOURCES <sourcefiles>
#                         [HEADERS <headerfiles>]
#                         [INCDIRS <include-directories>]
#                         [LIBRARIES <libraries>]
#                         [UIS <qt_uis>]
#                         [LINKOSI])
#                         [LINKGUI]
#                         [FOLDER <category>]
#                         [COMPONENT <gui|sim|core|bin|module>])
#
#   For building libraries:
#     add_openpass_target(NAME <target> TYPE library
#                         LINKAGE <static|shared>
#                         SOURCES <sourcefiles>
#                         [HEADERS <headerfiles>]
#                         [INCDIRS <include-directories>]
#                         [LIBRARIES <libraries>]
#                         [UIS <qt_uis>]
#                         [LINKOSI]
#                         [LINKGUI]
#                         [FOLDER <category>]
#                         [COMPONENT <gui|sim|core|bin|module>])
#
#   For building tests:
#     add_openpass_target(NAME <target> TYPE test
#                         SOURCES <sourcefiles>
#                         [HEADERS <headerfiles>]
#                         [INCDIRS <include-directories>]
#                         [LIBRARIES <libraries>]
#                         [UIS <qt_uis>]
#                         [LINKOSI]
#                         [LINKGUI]
#                         [DEFAULT_MAIN]
#                         [SIMCORE_DEPS <dependencies>]
#                         [RESOURCES <directories>]
#                         [FOLDER <category>]
#                         [COMPONENT <gui|sim|core|bin|module>])
#
# NAME           Specifies the target name, has to be unique
# TYPE           Type of the target to build (executable, library or test)
# LINKAGE        Specifies static or shared linkage for library targets
# SOURCES        Source files
# HEADERS        Header files
# UIS            Qt UI files
# INCDIRS        Additional include directories
# LIBRARIES      Additional libraries to link
# LINKOSI        Shortcut for adding OSI include directories and libraries (incl. protobuf)
# LINKGUI        Shortcut for adding GUI Libraries
# DEFAULT_MAIN   Links a simple main() implementation for running GTest
# SIMCORE_DEPS   Adds dependencies on simulation core targets to a test
# RESOURCES      List of directories to be copied to the test executable's location before test execution
# FOLDER         The target will be sorted into that folder in your development environment
#                tests will be sorted into tests/<category>
#
# In addtion to the parameters above:
#   - For 'test' targets:
#    - A test for building the test is added and the test will depend on that build target
#    - gmock/gtest headers and include directories are added
#    - gtest/gmock/pthread libraries are linked
#    - Tests are excluded form the 'all' target
#    - If DEFAULT_MAIN argument is provided, adds '--default-xml' to test executable command line arguments
#  - General:
#    - Target properties PROJECT_LABEL and OUTPUT_NAME are set to the target's name
#    - Target property DEBUG_POSTFIX is set to CMAKE_DEBUG_POSTFIX
##
function(add_openpass_target)
  cmake_parse_arguments(PARSED_ARG "LINKGUI;LINKOSI;DEFAULT_MAIN" "NAME;TYPE;LINKAGE" "HEADERS;SOURCES;INCDIRS;LIBRARIES;UIS;SIMCORE_DEPS;RESOURCES;FOLDER;COMPONENT" ${ARGN})

  if(TARGET ${PARSED_ARG_NAME})
    message(STATUS "Target '${PARSED_ARG_NAME}' already defined. Skipping.")
  else()

# TODO: different categories of libraries can be placed in different directories
#       GUI Components are installed in their own gui directory as an example
    if("${PARSED_ARG_COMPONENT}" STREQUAL "gui")
      set (DESTDIR ${SUBDIR_LIB_GUI})
    elseif("${PARSED_ARG_COMPONENT}" STREQUAL "core")
      set (DESTDIR ${SUBDIR_LIB_COMPONENTS})
    elseif("${PARSED_ARG_COMPONENT}" STREQUAL "bin")
      set (DESTDIR ${INSTALL_BIN_DIR})
    elseif("${PARSED_ARG_COMPONENT}" STREQUAL "sim")
      set (DESTDIR ${SUBDIR_LIB_SIM})
    elseif("${PARSED_ARG_COMPONENT}" STREQUAL "module")
      set (DESTDIR ${SUBDIR_LIB_SIM})
    else()
      message(FATAL_ERROR "Unknown COMPONENT '${PARSED_ARG_COMPONENT}' please use any one of 'gui', 'core', 'bin', 'sim', or 'module'.")
    endif()

    if("${PARSED_ARG_TYPE}" STREQUAL "library")

      set(VALID_LINKAGES shared static)

      if(NOT "${PARSED_ARG_LINKAGE}" IN_LIST VALID_LINKAGES)
        message(FATAL_ERROR "Target type 'library' requires either 'shared' or 'static' LINKAGE")
      else()
        string(TOUPPER "${PARSED_ARG_LINKAGE}" PARSED_ARG_LINKAGE)
      endif()

      add_library(${PARSED_ARG_NAME} ${PARSED_ARG_LINKAGE} ${PARSED_ARG_HEADERS} ${PARSED_ARG_SOURCES} ${PARSED_ARG_UIS})
      set_target_properties(${PARSED_ARG_NAME} PROPERTIES INSTALL_RPATH "\$ORIGIN;\$ORIGIN/..")
      install(FILES $<TARGET_FILE:${PARSED_ARG_NAME}> DESTINATION "${DESTDIR}")
      install(TARGETS ${PARSED_ARG_NAME} RUNTIME DESTINATION "${DESTDIR}")
      add_to_global_target_list(lib_target_list ${PARSED_ARG_NAME})

      if(OPENPASS_ADJUST_OUTPUT)
        openpass_adjust_output_dir(${PARSED_ARG_NAME} ${DESTDIR})
      endif()

      if(WIN32)
        set_target_properties(${PARSED_ARG_NAME} PROPERTIES PREFIX "")
      endif()

    elseif("${PARSED_ARG_TYPE}" STREQUAL "executable")

      if(DEFINED PARSED_ARG_LINKAGE)
        message(WARNING "LINKAGE parameter isn't used by target type 'executable'")
      endif()

      add_executable(${PARSED_ARG_NAME} ${PARSED_ARG_HEADERS} ${PARSED_ARG_SOURCES} ${PARSED_ARG_UIS})
      set_target_properties(${PARSED_ARG_NAME} PROPERTIES INSTALL_RPATH "\$ORIGIN;\$ORIGIN/lib")
      install(TARGETS ${PARSED_ARG_NAME} RUNTIME DESTINATION "${DESTDIR}")
      add_to_global_target_list(exe_target_list ${PARSED_ARG_NAME})

      if(OPENPASS_ADJUST_OUTPUT)
        openpass_adjust_output_dir(${PARSED_ARG_NAME} ${DESTDIR})
      endif()

    elseif("${PARSED_ARG_TYPE}" STREQUAL "test")

      if(DEFINED PARSED_ARG_LINKAGE)
        message(WARNING "LINKAGE parameter isn't used by target type 'executable'")
      endif()

      set(ADDITIONAL_TEST_ARGS)

      if(${PARSED_ARG_DEFAULT_MAIN})
        list(APPEND ADDITIONAL_TEST_ARGS "--default-xml")

        list(APPEND PARSED_ARG_HEADERS
          ${TEST_PATH}/common/gtest/mainHelper.h
        )

        list(APPEND PARSED_ARG_SOURCES
          ${TEST_PATH}/common/gtest/mainHelper.cpp
          ${TEST_PATH}/common/gtest/unitTestMain.cpp
        )
      endif()

      add_executable(${PARSED_ARG_NAME} EXCLUDE_FROM_ALL ${PARSED_ARG_HEADERS} ${PARSED_ARG_SOURCES} ${PARSED_ARG_UIS})

      target_link_libraries(${PARSED_ARG_NAME}
        GTest::gtest
        GTest::gmock
        pthread
      )

      add_test(NAME ${PARSED_ARG_NAME}_build COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${PARSED_ARG_NAME})
      add_test(NAME ${PARSED_ARG_NAME} COMMAND ${PARSED_ARG_NAME} ${ADDITIONAL_TEST_ARGS})
      set_tests_properties(${PARSED_ARG_NAME} PROPERTIES DEPENDS ${PARSED_ARG_NAME}_build)

      if(WITH_COVERAGE)
        add_test_coverage_fastcov(NAME ${PARSED_ARG_NAME})
      endif()

    else()
      message(FATAL_ERROR "Target type '${PARSED_ARG_TYPE}' is not supported.")
    endif()

    set_target_properties(${PARSED_ARG_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
    set_target_properties(${PARSED_ARG_NAME} PROPERTIES PROJECT_LABEL "${PARSED_ARG_NAME}")
    set_target_properties(${PARSED_ARG_NAME} PROPERTIES OUTPUT_NAME "${PARSED_ARG_NAME}")

    if("${PARSED_ARG_FOLDER}" STREQUAL "")
      set(PARSED_ARG_FOLDER ${FOLDER})
    endif()

    if("${PARSED_ARG_TYPE}" STREQUAL "test")
      set_target_properties(${PARSED_ARG_NAME} PROPERTIES FOLDER "tests/${PARSED_ARG_FOLDER}")
    else()
      set_target_properties(${PARSED_ARG_NAME} PROPERTIES FOLDER "${PARSED_ARG_FOLDER}")
    endif()

    target_include_directories(${PARSED_ARG_NAME} PRIVATE
      ${PARSED_ARG_INCDIRS}
    )

    target_link_libraries(${PARSED_ARG_NAME}
      ${PARSED_ARG_LIBRARIES}
      Qt5::Core
      Boost::headers
    )

    if(${PARSED_ARG_LINKOSI})
      target_include_directories(${PARSED_ARG_NAME} PRIVATE
        ${OSI_INCLUDE_DIR}
        protobuf::libprotobuf
      )

      target_link_libraries(${PARSED_ARG_NAME}
        ${OSI_LIBRARIES}
        protobuf::libprotobuf
      )
    endif()

    target_compile_options(${PARSED_ARG_NAME} PRIVATE
      $<$<CXX_COMPILER_ID:MSVC>:
          -wd4251 -wd4335 -wd4250>)

    if(${PARSED_ARG_LINKGUI})
      qt5_use_modules(${PARSED_ARG_NAME} Core Gui Xml Widgets)
    endif()

    # locate shared library dependencies for test execution and add
    # their location to PATH or LD_LIBRARY_PATH environment variable
    if("${PARSED_ARG_TYPE}" STREQUAL "test")
      set(DEPS)
      set(DEP_PATHS)
      if(DEFINED PARSED_ARG_SIMCORE_DEPS)
        list(APPEND DEPS ${PARSED_ARG_SIMCORE_DEPS})
      endif()
      if(DEFINED PARSED_ARG_LIBRARIES)
        list(APPEND DEPS ${PARSED_ARG_LIBRARIES})
      endif()
      if(${PARSED_ARG_LINKOSI})
        list(APPEND DEPS "${OSI_LIBRARIES}" protobuf::libprotobuf)
      endif()

      message(DEBUG "Locating shared library test dependencies...")
      foreach(DEP IN LISTS DEPS)
        if(TARGET ${DEP})
          set(DEP_PATH "")
          message(DEBUG "Target dependency: ${DEP}")
          # build dependencies
          add_dependencies(${PARSED_ARG_NAME} ${DEP})

          # test run dependencies
          get_property(DEP_TARGET_TYPE TARGET ${DEP} PROPERTY TYPE)
          if("${DEP_TARGET_TYPE}" STREQUAL "INTERFACE_LIBRARY")
            message(DEBUG "Dependency is interface libarary. Ignoring.")
            continue()
          endif()

          get_property(DEP_PATH TARGET ${DEP} PROPERTY IMPORTED_LOCATION)
          if("${DEP_PATH}" STREQUAL "")
            string(TOUPPER "${CMAKE_BUILD_TYPE}" TYPE)
            get_property(DEP_PATH TARGET ${DEP} PROPERTY "IMPORTED_LOCATION_${TYPE}")
          endif()
          if("${DEP_PATH}" STREQUAL "")
            get_property(DEP_PATH TARGET ${DEP} PROPERTY IMPORTED_LOCATION_RELEASE)
          endif()
          if("${DEP_PATH}" STREQUAL "")
            get_property(DEP_PATH TARGET ${DEP} PROPERTY IMPORTED_LOCATION_DEBUG)
          endif()

          if("${DEP_PATH}" STREQUAL "")
            get_property(DEP_PATH TARGET ${DEP} PROPERTY BINARY_DIR)
            message(DEBUG "no IMPORTED_LOCATION defined, got BINARY_DIR: ${DEP_PATH}")
          else()
            message(DEBUG "IMPORTED_LOCATION: ${DEP_PATH}")
            if(WIN32)
              get_filename_component(DEP_EXT ${DEP_PATH} EXT)
              if("${DEP_EXT}" STREQUAL ".dll.a")
                string(REGEX REPLACE "\.a$" "" DEP_PATH_NO_A "${DEP_PATH}")
                if(NOT EXISTS ${DEP_PATH_NO_A})
                  string(REGEX REPLACE "\/lib\/" "/bin/" DEP_PATH "${DEP_PATH_NO_A}")
                endif()
              endif()
              if(EXISTS "${DEP_PATH}")
                message(DEBUG "located shared library: ${DEP_PATH}")
                get_filename_component(DEP_PATH ${DEP_PATH} DIRECTORY)
              else()
                message(DEBUG "unable to locate shared library")
                continue()
              endif()
            endif()
          endif()
        else()
          message(DEBUG "No target dependency: ${DEP}")
          set(DEP_PATH "${DEP}")
          if(WIN32)
            get_filename_component(DEP_EXT "${DEP_PATH}" EXT)
            if("${DEP_EXT}" STREQUAL ".dll.a")
              string(REGEX REPLACE "\\.a$" "" DEP_PATH_NO_A "${DEP_PATH}")
              if(EXISTS ${DEP_PATH_NO_A})
                set(DEP_PATH "${DEP_PATH_NO_A}")
              else()
                string(REGEX REPLACE "/lib/" "/bin/" DEP_PATH "${DEP_PATH_NO_A}")
              endif()
            endif()
          endif()
          if(EXISTS "${DEP_PATH}")
            message(DEBUG "located shared library: ${DEP_PATH}")
            get_filename_component(DEP_PATH ${DEP_PATH} DIRECTORY)
          else()
            message(DEBUG "unable to locate shared library")
            continue()
          endif()
        endif()
        if(WIN32)
          string(REGEX REPLACE "/" "\\\\" DEP_PATH "${DEP_PATH}")
        endif()
        list(APPEND DEP_PATHS "${DEP_PATH}")
      endforeach()

      if(WIN32)
        list(JOIN DEP_PATHS "\\;" ADDITIONAL_PATHS)
        set(CURRENT_PATH "$ENV{PATH}")
        string(REGEX REPLACE "\;" "\\\;" CURRENT_PATH "${CURRENT_PATH}")
        set_tests_properties(${PARSED_ARG_NAME} PROPERTIES ENVIRONMENT "PATH=${ADDITIONAL_PATHS}\;${CURRENT_PATH}")
      else()
        list(JOIN DEP_PATHS ":" ADDITIONAL_PATHS)
        set_tests_properties(${PARSED_ARG_NAME} PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=${ADDITIONAL_PATHS}:$ENV{LD_LIBRARY_PATH}")
      endif()
    endif()

    if(DEFINED PARSED_ARG_RESOURCES)
      if("${PARSED_ARG_TYPE}" STREQUAL "test")
        foreach(RES IN LISTS PARSED_ARG_RESOURCES)
          add_custom_command(TARGET ${PARSED_ARG_NAME}
                             POST_BUILD
                             COMMAND ${CMAKE_COMMAND} -E copy_directory
                               ${CMAKE_CURRENT_SOURCE_DIR}/${RES}
                               ${CMAKE_CURRENT_BINARY_DIR}/${RES}
          )
        endforeach()
      else()
        message(WARNING "RESOURCES only used for 'test' openpass targets")
      endif()
    endif()
  endif()
endfunction()

##
# Function to add a target's destination file to a global list
#
# Usage:
#   add_to_global_target_list(<target-list> <target-name>)
#
#  target-list: the global property to append the file to
#  target-name: the name of the target to be added to the list
#
# NOTE:
# The generator expression won't be evaluated until usage of the target-list inside a scope that is able
# to parse the expression. Thus, the cmake policy CMP0087 has to be set to NEW for that scope.
#
function(add_to_global_target_list target_list target_name)
  get_property(tmp GLOBAL PROPERTY ${target_list})
  list(APPEND tmp $<TARGET_FILE:${target_name}>)
  set_property(GLOBAL PROPERTY ${target_list} ${tmp})
endfunction()

