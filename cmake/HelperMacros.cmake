# @file HelperMacros.cmake
#
# @author Blasius Czink
# @author Reinhard Biegel, in-tech GmbH
#
# Provides helper macros for build control:
#
#  ADD_OPENPASS_LIBRARY(targetname [STATIC | SHARED | MODULE] [EXCLUDE_FROM_ALL]
#              source1 source2 ... sourceN)
#    - OPENPASS specific wrapper macro of add_library. Please use this macro for OPENPASS libraries.
#
#  ADD_OPENPASS_EXECUTABLE(targetname [WIN32] [MACOSX_BUNDLE] [EXCLUDE_FROM_ALL]
#              source1 source2 ... sourceN)
#    - OPENPASS specific wrapper macro of add_executable. Please use this macro for OPENPASS executables.
#      Note: The variables SOURCES and HEADERS are added automatically.
#
#  ADD_OPENPASS_MODULE(targetname [WIN32] [MACOSX_BUNDLE] [EXCLUDE_FROM_ALL]
#              source1 source2 ... sourceN)
#    - OPENPASS specific wrapper macro of add_library. Please use this macro for OPENPASS modules.
#      Note: The variables SOURCES and HEADERS are added automatically.
#
#  OPENPASS_INSTALL_TARGET(targetname)
#    - OPENPASS specific wrapper macro of INSTALL(TARGETS ...) Please use this macro for installing OPENPASS targets.
#      You should use the cmake INSTALL() command for anything else but OPENPASS-targets.
#
#  ADD_OPENPASS_COMPILE_FLAGS(targetname flags)
#    - add additional compile_flags to the given target
#      Example: ADD_OPENPASS_COMPILE_FLAGS(coJS "-fPIC;-fno-strict-aliasing")
#
#  REMOVE_OPENPASS_COMPILE_FLAGS(targetname flags)
#    - remove compile flags from target
#      Example: REMOVE_OPENPASS_COMPILE_FLAGS(coJS "-fPIC")
#
#  ADD_OPENPASS_LINK_FLAGS(targetname flags)
#    - add additional link flags to the given target
#
#  REMOVE_OPENPASS_LINK_FLAGS(targetname flags)
#    - remove link flags from target
#
#  --------------------------------------------------------------------------------------
#
#  OPENPASS_RESET (varname)
#    - sets a variable to "not found"
#
#  OPENPASS_TEST_FEATURE (feature_dest_var feature_test_name my_output)
#    - Compiles a small program given in "feature_test_name" and sets the variable "feature_dest_var"
#      if the compile/link process was successful
#      The full output from the compile/link process is returned in "my_output"
#      This macro expects the "feature-test"-files in CM_FEATURE_TESTS_DIR which is preset to
#      ${OPENPASSDIR}/cmake/FeatureTests
#
#      Example: OPENPASS_TEST_FEATURE(MPI_THREADED ft_mpi_threaded.c MY_OUTPUT)
#
#  OPENPASS_COPY_TARGET_PDB(target_name pdb_inst_prefix)
#    - gets the targets .pdb file and deploys it to the given location ${pdb_inst_prefix}/lib or
#      ${pdb_inst_prefix}/bin during install
#    - only the pdb files for debug versions of a library are installed
#      (this macro is windows specific)
#
#  OPENPASS_DUMP_LIB_SETUP (basename)
#    - dumps the different library-variable contents to a file
#
#  USING(DEP1 DEP2 [optional])
#    - add dependencies DEP1 and DEP2,
#      all of them are optional, if 'optional' is present within the arguments
#
# @author Uwe Woessner
#

include(MSVC)

# helper to dump the lib-values to a simple text-file
MACRO(OPENPASS_DUMP_LIB_SETUP basename)
  SET (dump_file "${CMAKE_BINARY_DIR}/${basename}_lib_setup.txt")
  FILE(WRITE  ${dump_file} "${basename}_INCLUDE_DIR    = ${${basename}_INCLUDE_DIR}\n")
  FILE(APPEND ${dump_file} "${basename}_LIBRARY        = ${${basename}_LIBRARY}\n")
  FILE(APPEND ${dump_file} "${basename}_LIBRARY_RELESE = ${${basename}_LIBRARY_RELEASE}\n")
  FILE(APPEND ${dump_file} "${basename}_LIBRARY_DEBUG  = ${${basename}_LIBRARY_DEBUG}\n")
  FILE(APPEND ${dump_file} "${basename}_LIBRARIES      = ${${basename}_LIBRARIES}\n")
ENDMACRO(OPENPASS_DUMP_LIB_SETUP)

# helper to print the lib-values to a simple text-file
MACRO(OPENPASS_PRINT_LIB_SETUP basename)
  MESSAGE("${basename}_INCLUDE_DIR    = ${${basename}_INCLUDE_DIR}")
  MESSAGE("${basename}_LIBRARY        = ${${basename}_LIBRARY}")
  MESSAGE("${basename}_LIBRARY_RELESE = ${${basename}_LIBRARY_RELEASE}")
  MESSAGE("${basename}_LIBRARY_DEBUG  = ${${basename}_LIBRARY_DEBUG}")
  MESSAGE("${basename}_LIBRARIES      = ${${basename}_LIBRARIES}")
ENDMACRO(OPENPASS_PRINT_LIB_SETUP)


MACRO(OPENPASS_TEST_FEATURE feature_dest_var feature_test_name my_output)
  MESSAGE (STATUS "Checking for ${feature_test_name}")
  TRY_COMPILE (${feature_dest_var}
               ${CMAKE_BINARY_DIR}/CMakeTemp
               ${CM_FEATURE_TESTS_DIR}/${feature_test_name}
               CMAKE_FLAGS
               -DINCLUDE_DIRECTORIES=$ENV{OPENPASS_FT_INC}
               -DLINK_LIBRARIES=$ENV{OPENPASS_FT_LIB}
               OUTPUT_VARIABLE ${my_output}
  )
  # Feature test failed
  IF (${feature_dest_var})
     MESSAGE (STATUS "Checking for ${feature_test_name} - succeeded")
  ELSE (${feature_dest_var})
     MESSAGE (STATUS "Checking for ${feature_test_name} - feature not available")
  ENDIF (${feature_dest_var})
ENDMACRO(OPENPASS_TEST_FEATURE)


# Macro to adjust the output directories of a target
#FUNCTION(OPENPASS_ADJUST_OUTPUT_DIR targetname)
#    #MESSAGE("OPENPASS_ADJUST_OUTPUT_DIR(${targetname}) : TARGET_TYPE = ${TARGET_TYPE}, ARGV2=${ARGV2}")
#
#    SET(MYPATH_POSTFIX )
#    # optional path-postfix specified?
#    IF(NOT "${ARGV1}" STREQUAL "")
#      IF("${ARGV1}" MATCHES "^/.*")
#        SET(MYPATH_POSTFIX "${ARGV1}")
#      ELSE()
#        SET(MYPATH_POSTFIX "/${ARGV1}")
#      ENDIF()
#    ENDIF()
#
#    # adjust
#    IF(CMAKE_CONFIGURATION_TYPES)
#      # generator supports configuration types
#      FOREACH(conf_type ${CMAKE_CONFIGURATION_TYPES})
#        STRING(TOUPPER "${conf_type}" upper_conf_type_str)
#            SET_TARGET_PROPERTIES(${ARGV0} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${upper_conf_type_str} "${OPENPASS_DESTDIR}${MYPATH_POSTFIX}")
#            SET_TARGET_PROPERTIES(${ARGV0} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${upper_conf_type_str} "${OPENPASS_DESTDIR}${MYPATH_POSTFIX}")
#            SET_TARGET_PROPERTIES(${ARGV0} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${upper_conf_type_str} "${OPENPASS_DESTDIR}${MYPATH_POSTFIX}")
#      ENDFOREACH(conf_type)
#    ELSE(CMAKE_CONFIGURATION_TYPES)
#      # no configuration types - probably makefile generator
#      STRING(TOUPPER "${CMAKE_BUILD_TYPE}" upper_build_type_str)
#      SET_TARGET_PROPERTIES(${ARGV0} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${upper_build_type_str} "${OPENPASS_DESTDIR}${MYPATH_POSTFIX}")
#      SET_TARGET_PROPERTIES(${ARGV0} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${upper_build_type_str} "${OPENPASS_DESTDIR}${MYPATH_POSTFIX}")
#      SET_TARGET_PROPERTIES(${ARGV0} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${upper_build_type_str} "${OPENPASS_DESTDIR}${MYPATH_POSTFIX}")
#    ENDIF(CMAKE_CONFIGURATION_TYPES)
#ENDFUNCTION(OPENPASS_ADJUST_OUTPUT_DIR)

##
# Macro to add openPASS libraries, executables and tests
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
#
#   For building libraries:
#     add_openpass_target(NAME <target> TYPE libraries LINKAGE <static|shared>
#                         SOURCES <sourcefiles>
#                         [HEADERS <headerfiles>]
#                         [INCDIRS <include-directories>]
#                         [LIBRARIES <libraries>]
#                         [UIS <qt_uis>]
#                         [LINKOSI])
#
#   For building tests:
#     add_openpass_target(NAME <target> TYPE test
#                         SOURCES <sourcefiles>
#                         [HEADERS <headerfiles>]
#                         [INCDIRS <include-directories>]
#                         [LIBRARIES <libraries>]
#                         [UIS <qt_uis>]
#                         [LINKOSI]
#                         [DEFAULT_MAIN]
#                         [SIMCORE_DEPS <dependencies>]
#                         [RESOURCES <directories>])
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
# DEFAULT_MAIN   Links a simple main() implementation for running GTest
# SIMCORE_DEPS   Adds dependencies on simulation core targets to a test
# RESOURCES      List of directories to be copied to the test executable's location before test execution
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
  cmake_parse_arguments(PARSED_ARG "LINKOSI;DEFAULT_MAIN" "NAME;TYPE;LINKAGE" "HEADERS;SOURCES;INCDIRS;LIBRARIES;UIS;SIMCORE_DEPS;RESOURCES" ${ARGN})

  if(TARGET ${PARSED_ARG_NAME})
    message(STATUS "Target '${PARSED_ARG_NAME}' already defined. Skipping.")
  else()

    if("${PARSED_ARG_TYPE}" STREQUAL "library")

      set(VALID_LINKAGES shared static)

      if(NOT "${PARSED_ARG_LINKAGE}" IN_LIST VALID_LINKAGES)
        message(FATAL_ERROR "Target type 'library' requires either 'shared' or 'static' LINKAGE")
      else()
        string(TOUPPER "${PARSED_ARG_LINKAGE}" PARSED_ARG_LINKAGE)
      endif()

      add_library(${PARSED_ARG_NAME} ${PARSED_ARG_LINKAGE} ${PARSED_ARG_HEADERS} ${PARSED_ARG_SOURCES} ${PARSED_ARG_UIS})
      install(FILES $<TARGET_FILE:${PARSED_ARG_NAME}> DESTINATION "${INSTALL_LIB_DIR}")

      if(WIN32)
        set_target_properties(${PARSED_ARG_NAME} PROPERTIES PREFIX "")
      endif()

    elseif("${PARSED_ARG_TYPE}" STREQUAL "executable")

      if(DEFINED PARSED_ARG_LINKAGE)
        message(WARNING "LINKAGE parameter isn't used by target type 'executable'")
      endif()

      add_executable(${PARSED_ARG_NAME} ${PARSED_ARG_HEADERS} ${PARSED_ARG_SOURCES} ${PARSED_ARG_UIS})
      install(TARGETS ${PARSED_ARG_NAME} RUNTIME DESTINATION "${INSTALL_BIN_DIR}")

    elseif("${PARSED_ARG_TYPE}" STREQUAL "test")

      if(DEFINED PARSED_ARG_LINKAGE)
        message(WARNING "LINKAGE parameter isn't used by target type 'executable'")
      endif()

      set(ADDITIONAL_TEST_ARGS "")

      if(${PARSED_ARG_DEFAULT_MAIN})
        set(ADDITIONAL_TEST_ARGS "${ADDITIONAL_TEST_ARGS} --default-xml")

        list(APPEND PARSED_ARG_HEADERS
          ${TEST_PATH}/common/gtest/mainHelper.h
        )

        list(APPEND PARSED_ARG_SOURCES
          ${TEST_PATH}/common/gtest/mainHelper.cpp
          ${TEST_PATH}/common/gtest/unitTestMain.cpp
        )
      endif()

      add_executable(${PARSED_ARG_NAME} EXCLUDE_FROM_ALL ${PARSED_ARG_HEADERS} ${PARSED_ARG_SOURCES} ${PARSED_ARG_UIS})

      target_include_directories(${PARSED_ARG_NAME}
        SYSTEM PRIVATE
        ${GTEST_INCLUDE_DIR}
      )

      # currently not provided by FindGTest
      get_filename_component(GMOCK_LIBRARY_PATH ${GTEST_LIBRARY} DIRECTORY)
      set(GMOCK_LIBRARY ${GMOCK_LIBRARY_PATH}/libgmock.a)

      target_link_libraries(${PARSED_ARG_NAME}
        ${GTEST_LIBRARY}
        ${GMOCK_LIBRARY}
        pthread
      )

      add_test(NAME ${PARSED_ARG_NAME}_build COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${PARSED_ARG_NAME})
      add_test(NAME ${PARSED_ARG_NAME} COMMAND ${PARSED_ARG_NAME} ${ADDITIONAL_TEST_ARGS})
      set_tests_properties(${PARSED_ARG_NAME} PROPERTIES DEPENDS ${PARSED_ARG_NAME}_build)

    else()

      message(FATAL_ERROR "Target type '${PARSED_TARGET_TYPE}' is not supported.")

    endif()

    set_target_properties(${PARSED_ARG_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
    set_target_properties(${PARSED_ARG_NAME} PROPERTIES PROJECT_LABEL "${PARSED_ARG_NAME}")
    set_target_properties(${PARSED_ARG_NAME} PROPERTIES OUTPUT_NAME "${PARSED_ARG_NAME}")

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

    if(DEFINED PARSED_ARG_SIMCORE_DEPS)
      if("${PARSED_ARG_TYPE}" STREQUAL "test")
        foreach(DEP IN LISTS PARSED_ARG_SIMCORE_DEPS)
          # build dependency
          add_dependencies(${PARSED_ARG_NAME} ${DEP})

          # test run dependency
          get_property(DEP_PATH TARGET ${DEP} PROPERTY BINARY_DIR)
          set_tests_properties(${PARSED_ARG_NAME} PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=${DEP_PATH}")
        endforeach()
      else()
        message(WARNING "SIMCORE_DEPS argument is only used for 'test' openpass targets")
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


# Macro to add OPENPASS modules (executables with a module-category)
#MACRO(ADD_OPENPASS_PLUGIN targetname component)
#  ADD_LIBRARY(${targetname} SHARED ${ARGN} ${SOURCES} ${HEADERS} ${UIS})
#  TARGET_LINK_LIBRARIES(${targetname} ${EXTRA_LIBS})
#
#  # SET_TARGET_PROPERTIES(${targetname} PROPERTIES PROJECT_LABEL "${targetname}")
#  SET_TARGET_PROPERTIES(${targetname} PROPERTIES OUTPUT_NAME "${targetname}")
#
#  #OPENPASS_ADJUST_OUTPUT_DIR(${targetname} ${component})
#
#  # set additional OPENPASS_COMPILE_FLAGS
#  #SET_TARGET_PROPERTIES(${targetname} PROPERTIES COMPILE_FLAGS "${OPENPASS_COMPILE_FLAGS}")
#  # set additional OPENPASS_LINK_FLAGS
#  #SET_TARGET_PROPERTIES(${targetname} PROPERTIES LINK_FLAGS "${OPENPASS_LINK_FLAGS}")
#
#  # SET_TARGET_PROPERTIES(${targetname} PROPERTIES DEBUG_OUTPUT_NAME "${targetname}${CMAKE_DEBUG_POSTFIX}")
#
#  OPENPASS_INSTALL_TARGET(${targetname})
#
#  #UNSET(SOURCES)
#  #UNSET(HEADERS)
#  #UNSET(UIS)
#ENDMACRO(ADD_OPENPASS_PLUGIN)

#MACRO(OPENPASS_GUI_add_plugin targetname category)  
#   ADD_OPENPASS_PLUGIN(${targetname} "lib/gui" ${ARGN})
#   IF("${category}" STREQUAL "")
#       set_target_properties(${targetname} PROPERTIES FOLDER "GUI")
#   ELSE("${category}" STREQUAL "")
#       set_target_properties(${targetname} PROPERTIES FOLDER "${category}/GUI")
#   ENDIF("${category}" STREQUAL "")
#   target_link_libraries(${targetname} Common)
#   qt5_use_modules(${targetname} Core Gui Xml Widgets)
#
#ENDMACRO(OPENPASS_GUI_add_plugin)

#MACRO(OPENPASS_add_module targetname category)
#   ADD_OPENPASS_PLUGIN(${targetname} lib ${ARGN})
#   IF("${category}" STREQUAL "")
#       set_target_properties(${targetname} PROPERTIES FOLDER "CoreModules")
#   ELSE("${category}" STREQUAL "")
#       set_target_properties(${targetname} PROPERTIES FOLDER "${category}/CoreModules")
#   ENDIF("${category}" STREQUAL "")
#   target_link_libraries(${targetname} Common)
#   qt5_use_modules(${targetname} Xml)
#
#ENDMACRO(OPENPASS_add_module)

#MACRO(add_openpass_component targetname category)
#   ADD_OPENPASS_PLUGIN(${targetname} "lib" ${ARGN})
#   IF("${category}" STREQUAL "")
#       set_target_properties(${targetname} PROPERTIES FOLDER "Component")
#   ELSE("${category}" STREQUAL "")
#       set_target_properties(${targetname} PROPERTIES FOLDER "${category}/Component")
#   ENDIF("${category}" STREQUAL "")
#   target_link_libraries(${targetname} Common)
#   qt5_use_modules(${targetname} Xml)
#
#ENDMACRO(OPENPASS_add_component)

# Macro to install and export
macro(add_openpass_install_target targetname)
  install(TARGETS ${targetname}
          RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
          ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
          LIBRARY DESTINATION "${INSTALL_LIB_DIR}")

  install(FILES ${HEADERS} DESTINATION "${INSTALL_INC_DIR}")
endmacro()

# Macro to install an OpenOPENPASS_GUI plugin
#MACRO(OPENPASS_GUI_INSTALL_PLUGIN targetname)
#  INSTALL(TARGETS ${ARGV} EXPORT OPENPASS-targets
#          RUNTIME DESTINATION ${INSTALL_BIN_DIR}/bin/gui
#          LIBRARY DESTINATION ${INSTALL_LIB_DIR}
#          ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
#          COMPONENT guiplugins
#  )
#ENDMACRO(OPENPASS_GUI_INSTALL_PLUGIN)

# Macro to install headers
#MACRO(OPENPASS_INSTALL_HEADERS dirname)
#  INSTALL(FILES ${ARGN} DESTINATION include/OPENPASS/${dirname})
#ENDMACRO(OPENPASS_INSTALL_HEADERS)
