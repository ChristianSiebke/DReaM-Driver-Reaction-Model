# @file OPENPASSHelperMacros.cmake
#
# @author Blasius Czink
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


FUNCTION(OPENPASS_COPY_TARGET_PDB target_name pdb_inst_prefix)
  IF(MSVC)
    GET_TARGET_PROPERTY(TARGET_LOC ${target_name} DEBUG_LOCATION)
    IF(TARGET_LOC)
      GET_FILENAME_COMPONENT(TARGET_HEAD "${TARGET_LOC}" NAME_WE)
      # GET_FILENAME_COMPONENT(FNABSOLUTE  "${TARGET_LOC}" ABSOLUTE)
      GET_FILENAME_COMPONENT(TARGET_PATH "${TARGET_LOC}" PATH)
      SET(TARGET_PDB "${TARGET_PATH}/${TARGET_HEAD}.pdb")
      # MESSAGE(STATUS "PDB-File of ${target_name} is ${TARGET_PDB}")
      GET_TARGET_PROPERTY(TARGET_TYPE ${target_name} TYPE)
      IF(TARGET_TYPE)
        SET(pdb_dest )
        IF(TARGET_TYPE STREQUAL "STATIC_LIBRARY")
          SET(pdb_dest lib)
        ELSE(TARGET_TYPE STREQUAL "STATIC_LIBRARY")
          SET(pdb_dest bin)
        ENDIF(TARGET_TYPE STREQUAL "STATIC_LIBRARY")
        # maybe an optional category path given?
        IF(NOT "${ARGN}" STREQUAL "")
          SET(category_path "${ARGV2}")
        ENDIF(NOT "${ARGN}" STREQUAL "")
        INSTALL(FILES ${TARGET_PDB} DESTINATION "${pdb_inst_prefix}/${pdb_dest}${category_path}" CONFIGURATIONS "Debug") 
      ENDIF(TARGET_TYPE)
    ENDIF(TARGET_LOC)
  ENDIF(MSVC)
ENDFUNCTION(OPENPASS_COPY_TARGET_PDB)

MACRO(OPENPASS_MSVC_RUNTIME_OPTION)
  IF(MSVC)
    OPTION (MSVC_USE_STATIC_RUNTIME "Use static MS-Runtime (/MT, /MTd)" OFF)
    IF(MSVC_USE_STATIC_RUNTIME)
      FOREACH(FLAG_VAR CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
                        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
      IF(${FLAG_VAR} MATCHES "/MD")
        STRING(REGEX REPLACE "/MD" "/MT" BCMSVC_${FLAG_VAR} "${${FLAG_VAR}}")
        SET(${FLAG_VAR} ${BCMSVC_${FLAG_VAR}} CACHE STRING "" FORCE)
      ENDIF(${FLAG_VAR} MATCHES "/MD")
      ENDFOREACH(FLAG_VAR)
    ELSE(MSVC_USE_STATIC_RUNTIME)
      FOREACH(FLAG_VAR CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
                        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
      IF(${FLAG_VAR} MATCHES "/MT")
        STRING(REGEX REPLACE "/MT" "/MD" BCMSVC_${FLAG_VAR} "${${FLAG_VAR}}")
        SET(${FLAG_VAR} ${BCMSVC_${FLAG_VAR}} CACHE STRING "" FORCE)
      ENDIF(${FLAG_VAR} MATCHES "/MT")
      ENDFOREACH(FLAG_VAR)
    ENDIF(MSVC_USE_STATIC_RUNTIME)
  ENDIF(MSVC)
ENDMACRO(OPENPASS_MSVC_RUNTIME_OPTION)


# Macro to adjust the output directories of a target
FUNCTION(OPENPASS_ADJUST_OUTPUT_DIR targetname)
    #MESSAGE("OPENPASS_ADJUST_OUTPUT_DIR(${targetname}) : TARGET_TYPE = ${TARGET_TYPE}, ARGV2=${ARGV2}")

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
ENDFUNCTION(OPENPASS_ADJUST_OUTPUT_DIR)

# Macro to add OPENPASS libraries
MACRO(ADD_OPENPASS_LIBRARY targetname)
  ADD_LIBRARY(${ARGV} ${SOURCES} ${HEADERS} ${UIS})
  TARGET_LINK_LIBRARIES(${targetname} ${EXTRA_LIBS})
  # SET_TARGET_PROPERTIES(${targetname} PROPERTIES PROJECT_LABEL "${targetname}")
  SET_TARGET_PROPERTIES(${targetname} PROPERTIES OUTPUT_NAME "${targetname}")

  OPENPASS_ADJUST_OUTPUT_DIR(${targetname} bin)
  
  # set additional OPENPASS_COMPILE_FLAGS
  SET_TARGET_PROPERTIES(${targetname} PROPERTIES COMPILE_FLAGS "${OPENPASS_COMPILE_FLAGS}")
  # set additional OPENPASS_LINK_FLAGS
  SET_TARGET_PROPERTIES(${targetname} PROPERTIES LINK_FLAGS "${OPENPASS_LINK_FLAGS}")
  
  
  UNSET(SOURCES)
  UNSET(HEADERS)
  UNSET(UIS)

ENDMACRO(ADD_OPENPASS_LIBRARY)

MACRO(OPENPASS_ADD_LIBRARY)
   ADD_OPENPASS_LIBRARY(${ARGN})
ENDMACRO(OPENPASS_ADD_LIBRARY)

# Macro to add OPENPASS executables
MACRO(ADD_OPENPASS_EXECUTABLE targetname)
  ADD_EXECUTABLE(${targetname} ${ARGN} ${SOURCES} ${HEADERS} ${UIS})
  TARGET_LINK_LIBRARIES(${targetname} ${EXTRA_LIBS})
  set_target_properties(${targetname} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
  # SET_TARGET_PROPERTIES(${targetname} PROPERTIES PROJECT_LABEL "${targetname}")
  SET_TARGET_PROPERTIES(${targetname} PROPERTIES OUTPUT_NAME "${targetname}")
	  
  OPENPASS_ADJUST_OUTPUT_DIR(${targetname} "")
  
  # set additional OPENPASS_COMPILE_FLAGS
  SET_TARGET_PROPERTIES(${targetname} PROPERTIES COMPILE_FLAGS "${OPENPASS_COMPILE_FLAGS}")
  # set additional OPENPASS_LINK_FLAGS
  SET_TARGET_PROPERTIES(${targetname} PROPERTIES LINK_FLAGS "${OPENPASS_LINK_FLAGS}")
  
  # SET_TARGET_PROPERTIES(${targetname} PROPERTIES DEBUG_OUTPUT_NAME "${targetname}${CMAKE_DEBUG_POSTFIX}")
  UNSET(SOURCES)
  UNSET(HEADERS)
  UNSET(UIS)
ENDMACRO(ADD_OPENPASS_EXECUTABLE)

MACRO(OPENPASS_add_executable targetname)
   ADD_OPENPASS_EXECUTABLE(${targetname} ${ARGN})
ENDMACRO(OPENPASS_add_executable)




# Macro to add OPENPASS modules (executables with a module-category)
MACRO(ADD_OPENPASS_PLUGIN targetname component)
  ADD_LIBRARY(${targetname} SHARED ${ARGN} ${SOURCES} ${HEADERS} ${UIS})
  TARGET_LINK_LIBRARIES(${targetname} ${EXTRA_LIBS})
  
  # SET_TARGET_PROPERTIES(${targetname} PROPERTIES PROJECT_LABEL "${targetname}")
  SET_TARGET_PROPERTIES(${targetname} PROPERTIES OUTPUT_NAME "${targetname}")
	  
  OPENPASS_ADJUST_OUTPUT_DIR(${targetname} ${component}) 
  
  # set additional OPENPASS_COMPILE_FLAGS
  SET_TARGET_PROPERTIES(${targetname} PROPERTIES COMPILE_FLAGS "${OPENPASS_COMPILE_FLAGS}")
  # set additional OPENPASS_LINK_FLAGS
  SET_TARGET_PROPERTIES(${targetname} PROPERTIES LINK_FLAGS "${OPENPASS_LINK_FLAGS}")
  
  # SET_TARGET_PROPERTIES(${targetname} PROPERTIES DEBUG_OUTPUT_NAME "${targetname}${CMAKE_DEBUG_POSTFIX}")
  UNSET(SOURCES)
  UNSET(HEADERS)
  UNSET(UIS)
ENDMACRO(ADD_OPENPASS_PLUGIN)

MACRO(OPENPASS_GUI_add_plugin targetname category)  
   ADD_OPENPASS_PLUGIN(${targetname} "bin/gui" ${ARGN})
   IF("${category}" STREQUAL "")
       set_target_properties(${targetname} PROPERTIES FOLDER "GUI")
   ELSE("${category}" STREQUAL "")
       set_target_properties(${targetname} PROPERTIES FOLDER "${category}/GUI")
   ENDIF("${category}" STREQUAL "")
   target_link_libraries(${targetname} Common)
   qt5_use_modules(${targetname} Core Gui Xml Widgets)
  
ENDMACRO(OPENPASS_GUI_add_plugin)

MACRO(OPENPASS_add_module targetname category)
   ADD_OPENPASS_PLUGIN(${targetname} lib ${ARGN})
   IF("${category}" STREQUAL "")
       set_target_properties(${targetname} PROPERTIES FOLDER "CoreModules")
   ELSE("${category}" STREQUAL "")
       set_target_properties(${targetname} PROPERTIES FOLDER "${category}/CoreModules")
   ENDIF("${category}" STREQUAL "")
   target_link_libraries(${targetname} Common)
   qt5_use_modules(${targetname} Xml)
  
ENDMACRO(OPENPASS_add_module)

MACRO(OPENPASS_add_component targetname category)
   ADD_OPENPASS_PLUGIN(${targetname} "lib" ${ARGN})
   IF("${category}" STREQUAL "")
       set_target_properties(${targetname} PROPERTIES FOLDER "Component")
   ELSE("${category}" STREQUAL "")
       set_target_properties(${targetname} PROPERTIES FOLDER "${category}/Component")
   ENDIF("${category}" STREQUAL "")
   target_link_libraries(${targetname} Common)
   qt5_use_modules(${targetname} Xml)
  
ENDMACRO(OPENPASS_add_component)

# Macro to install and export
MACRO(OPENPASS_INSTALL_TARGET targetname)
  
install(TARGETS ${targetname}
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" )
		
install(FILES ${HEADERS} DESTINATION "${INSTALL_INC_DIR}")
  
ENDMACRO(OPENPASS_INSTALL_TARGET)

# Macro to install an OpenOPENPASS_GUI plugin
MACRO(OPENPASS_GUI_INSTALL_PLUGIN targetname)
  INSTALL(TARGETS ${ARGV} EXPORT OPENPASS-targets
          RUNTIME DESTINATION ${INSTALL_BIN_DIR}/bin/gui
          LIBRARY DESTINATION ${INSTALL_LIB_DIR}
          ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
          COMPONENT guiplugins
  )
ENDMACRO(OPENPASS_GUI_INSTALL_PLUGIN)

# Macro to install headers
MACRO(OPENPASS_INSTALL_HEADERS dirname)

  INSTALL(FILES ${ARGN} DESTINATION include/OPENPASS/${dirname})
ENDMACRO(OPENPASS_INSTALL_HEADERS)


#
# Per target flag handling
#

FUNCTION(ADD_OPENPASS_COMPILE_FLAGS targetname flags)
  GET_TARGET_PROPERTY(MY_CFLAGS ${targetname} COMPILE_FLAGS)
  IF(NOT MY_CFLAGS)
    SET(MY_CFLAGS "")
  ENDIF()
  FOREACH(cflag ${flags})
    #STRING(REGEX MATCH "${cflag}" flag_matched "${MY_CFLAGS}")
    #IF(NOT flag_matched)
      SET(MY_CFLAGS "${MY_CFLAGS} ${cflag}")
    #ENDIF(NOT flag_matched)
  ENDFOREACH(cflag)
  SET_TARGET_PROPERTIES(${targetname} PROPERTIES COMPILE_FLAGS "${MY_CFLAGS}")
  # MESSAGE("added compile flags ${MY_CFLAGS} to target ${targetname}")
ENDFUNCTION(ADD_OPENPASS_COMPILE_FLAGS)

FUNCTION(REMOVE_OPENPASS_COMPILE_FLAGS targetname flags)
  GET_TARGET_PROPERTY(MY_CFLAGS ${targetname} COMPILE_FLAGS)
  IF(NOT MY_CFLAGS)
    SET(MY_CFLAGS "")
  ENDIF()
  FOREACH(cflag ${flags})
    STRING(REGEX REPLACE "${cflag}[ ]+|${cflag}$" "" MY_CFLAGS "${MY_CFLAGS}")
  ENDFOREACH(cflag)
  SET_TARGET_PROPERTIES(${targetname} PROPERTIES COMPILE_FLAGS "${MY_CFLAGS}")
ENDFUNCTION(REMOVE_OPENPASS_COMPILE_FLAGS)

FUNCTION(ADD_OPENPASS_LINK_FLAGS targetname flags)
  GET_TARGET_PROPERTY(MY_LFLAGS ${targetname} LINK_FLAGS)
  IF(NOT MY_LFLAGS)
    SET(MY_LFLAGS "")
  ENDIF()
  FOREACH(lflag ${flags})
    #STRING(REGEX MATCH "${lflag}" flag_matched "${MY_LFLAGS}")
    #IF(NOT flag_matched)
      SET(MY_LFLAGS "${MY_LFLAGS} ${lflag}")
    #ENDIF(NOT flag_matched)
  ENDFOREACH(lflag)
  SET_TARGET_PROPERTIES(${targetname} PROPERTIES LINK_FLAGS "${MY_LFLAGS}")
  #MESSAGE("added link flags ${MY_LFLAGS} to target ${targetname}")
ENDFUNCTION(ADD_OPENPASS_LINK_FLAGS)

FUNCTION(REMOVE_OPENPASS_LINK_FLAGS targetname flags)
  GET_TARGET_PROPERTY(MY_LFLAGS ${targetname} LINK_FLAGS)
  IF(NOT MY_LFLAGS)
    SET(MY_LFLAGS "")
  ENDIF()
  FOREACH(lflag ${flags})
    STRING(REGEX REPLACE "${lflag}[ ]+|${lflag}$" "" MY_LFLAGS "${MY_LFLAGS}")
  ENDFOREACH(lflag)
  SET_TARGET_PROPERTIES(${targetname} PROPERTIES LINK_FLAGS "${MY_LFLAGS}")
ENDFUNCTION(REMOVE_OPENPASS_LINK_FLAGS)

# small debug helper
FUNCTION(OUTPUT_OPENPASS_COMPILE_FLAGS targetname)
  GET_TARGET_PROPERTY(MY_CFLAGS ${targetname} COMPILE_FLAGS)
  MESSAGE("Target ${targetname} - COMPILE_FLAGS = ${MY_CFLAGS}")
ENDFUNCTION(OUTPUT_OPENPASS_COMPILE_FLAGS)

FUNCTION(OUTPUT_OPENPASS_LINK_FLAGS targetname)
  GET_TARGET_PROPERTY(MY_LFLAGS ${targetname} LINK_FLAGS)
  MESSAGE("Target ${targetname} - LINK_FLAGS = ${MY_LFLAGS}")
ENDFUNCTION(OUTPUT_OPENPASS_LINK_FLAGS)

