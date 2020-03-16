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
# Provides helper macros for build control:
#
#  add_openpass_library(targetname [STATIC | SHARED | MODULE] [EXCLUDE_FROM_ALL]
#              source1 source2 ... sourceN)
#    - OPENPASS specific wrapper macro of add_library. Please use this macro for OPENPASS libraries.
#
#  add_openpass_executable(targetname [WIN32] [MACOSX_BUNDLE] [EXCLUDE_FROM_ALL]
#              source1 source2 ... sourceN)
#    - OPENPASS specific wrapper macro of add_executable. Please use this macro for OPENPASS executables.
#      Note: The variables SOURCES and HEADERS are added automatically.
#
#  add_openpass_module(targetname [WIN32] [MACOSX_BUNDLE] [EXCLUDE_FROM_ALL]
#              source1 source2 ... sourceN)
#    - OPENPASS specific wrapper macro of add_library. Please use this macro for OPENPASS modules.
#      Note: The variables SOURCES and HEADERS are added automatically.
#
#  openpass_install_target(targetname)
#    - OPENPASS specific wrapper macro of install(TARGETS ...) Please use this macro for installing OPENPASS targets.
#      You should use the cmake install() command for anything else but OPENPASS-targets.
#
#  add_openpass_compile_flags(targetname flags)
#    - add additional compile_flags to the given target
#      Example: add_openpass_compile_flags(coJS "-fPIC;-fno-strict-aliasing")
#
#  remove_openpass_compile_flags(targetname flags)
#    - remove compile flags from target
#      Example: remove_openpass_compile_flags(coJS "-fPIC")
#
#  add_openpass_link_flags(targetname flags)
#    - add additional link flags to the given target
#
#  remove_openpass_link_flags(targetname flags)
#    - remove link flags from target
#
#  --------------------------------------------------------------------------------------
#
# @author Uwe Woessner
#

macro(OPENPASS_TEST_FEATURE feature_dest_var feature_test_name my_output)
  message(STATUS "Checking for ${feature_test_name}")
  try_compile(${feature_dest_var}
               ${CMAKE_BINARY_DIR}/CMakeTemp
               ${CM_FEATURE_TESTS_DIR}/${feature_test_name}
               CMAKE_FLAGS
               -DINCLUDE_DIRECTORIES=$ENV{OPENPASS_FT_INC}
               -DLINK_LIBRARIES=$ENV{OPENPASS_FT_LIB}
               OUTPUT_VARIABLE ${my_output}
  )
  # Feature test failed
  if(${feature_dest_var})
     message(STATUS "Checking for ${feature_test_name} - succeeded")
  else(${feature_dest_var})
     message(STATUS "Checking for ${feature_test_name} - feature not available")
  endif(${feature_dest_var})
endmacro(OPENPASS_TEST_FEATURE)


function(OPENPASS_COPY_TARGET_PDB target_name pdb_inst_prefix)
  if(MSVC)
    get_target_property(TARGET_LOC ${target_name} DEBUG_LOCATION)
    if(TARGET_LOC)
      get_filename_component(TARGET_HEAD "${TARGET_LOC}" NAME_WE)
      # get_filename_component(FNABSOLUTE  "${TARGET_LOC}" ABSOLUTE)
      get_filename_component(TARGET_PATH "${TARGET_LOC}" PATH)
      set(TARGET_PDB "${TARGET_PATH}/${TARGET_HEAD}.pdb")
      # message(STATUS "PDB-File of ${target_name} is ${TARGET_PDB}")
      get_target_property(TARGET_TYPE ${target_name} TYPE)
      if(TARGET_TYPE)
        set(pdb_dest )
        if(TARGET_TYPE STREQUAL "STATIC_LIBRARY")
          set(pdb_dest lib)
        else(TARGET_TYPE STREQUAL "STATIC_LIBRARY")
          set(pdb_dest bin)
        endif(TARGET_TYPE STREQUAL "STATIC_LIBRARY")
        # maybe an optional category path given?
        if(NOT "${ARGN}" STREQUAL "")
          set(category_path "${ARGV2}")
        endif(NOT "${ARGN}" STREQUAL "")
        install(FILES ${TARGET_PDB} DESTINATION "${pdb_inst_prefix}/${pdb_dest}${category_path}" CONFIGURATIONS "Debug") 
      endif(TARGET_TYPE)
    endif(TARGET_LOC)
  endif(MSVC)
endfunction(OPENPASS_COPY_TARGET_PDB)

macro(OPENPASS_MSVC_RUNTIME_OPTION)
  if(MSVC)
    option(MSVC_USE_STATIC_RUNTIME "Use static MS-Runtime (/MT, /MTd)" OFF)
    if(MSVC_USE_STATIC_RUNTIME)
      foreach(FLAG_VAR CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
                        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
      if(${FLAG_VAR} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" BCMSVC_${FLAG_VAR} "${${FLAG_VAR}}")
        set(${FLAG_VAR} ${BCMSVC_${FLAG_VAR}} CACHE STRING "" FORCE)
      endif(${FLAG_VAR} MATCHES "/MD")
      endforeach(FLAG_VAR)
    else(MSVC_USE_STATIC_RUNTIME)
      foreach(FLAG_VAR CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
                        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
      if(${FLAG_VAR} MATCHES "/MT")
        string(REGEX REPLACE "/MT" "/MD" BCMSVC_${FLAG_VAR} "${${FLAG_VAR}}")
        set(${FLAG_VAR} ${BCMSVC_${FLAG_VAR}} CACHE STRING "" FORCE)
      endif(${FLAG_VAR} MATCHES "/MT")
      endforeach(FLAG_VAR)
    endif(MSVC_USE_STATIC_RUNTIME)
  endif(MSVC)
endmacro(OPENPASS_MSVC_RUNTIME_OPTION)


# Macro to adjust the output directories of a target
function(openpass_adjust_output_dir targetname)
    #message("openpass_adjust_output_dir(${targetname}) : TARGET_TYPE = ${TARGET_TYPE}, ARGV2=${ARGV2}")

    set(MYPATH_POSTFIX )
    # optional path-postfix specified?
    if(NOT "${ARGV1}" STREQUAL "")
      if("${ARGV1}" MATCHES "^/.*")
        set(MYPATH_POSTFIX "${ARGV1}")
      else()
        set(MYPATH_POSTFIX "/${ARGV1}")
      endif()
    endif()
    
    # adjust
    if(CMAKE_CONFIGURATION_TYPES)
      # generator supports configuration types
      foreach(conf_type ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER "${conf_type}" upper_conf_type_str)
            set_target_properties(${ARGV0} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${upper_conf_type_str} "${OPENPASS_DESTDIR}${MYPATH_POSTFIX}")
            set_target_properties(${ARGV0} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${upper_conf_type_str} "${OPENPASS_DESTDIR}${MYPATH_POSTFIX}")
            set_target_properties(${ARGV0} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${upper_conf_type_str} "${OPENPASS_DESTDIR}${MYPATH_POSTFIX}")
      endforeach(conf_type)
    else(CMAKE_CONFIGURATION_TYPES)
      # no configuration types - probably makefile generator
      string(TOUPPER "${CMAKE_BUILD_TYPE}" upper_build_type_str)
      set_target_properties(${ARGV0} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${upper_build_type_str} "${OPENPASS_DESTDIR}${MYPATH_POSTFIX}")
      set_target_properties(${ARGV0} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${upper_build_type_str} "${OPENPASS_DESTDIR}${MYPATH_POSTFIX}")
      set_target_properties(${ARGV0} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${upper_build_type_str} "${OPENPASS_DESTDIR}${MYPATH_POSTFIX}")
    endif(CMAKE_CONFIGURATION_TYPES)
endfunction(openpass_adjust_output_dir)

# Macro to add OPENPASS libraries
macro(openpass_add_library targetname)
  add_library(${ARGV} ${SOURCES} ${HEADERS} ${UIS})
  target_link_libraries(${targetname} ${EXTRA_LIBS})
  # set_target_properties(${targetname} PROPERTIES PROJECT_LABEL "${targetname}")
  set_target_properties(${targetname} PROPERTIES OUTPUT_NAME "${targetname}")

  openpass_adjust_output_dir(${targetname} bin)
  
  # set additional OPENPASS_COMPILE_FLAGS
  set_target_properties(${targetname} PROPERTIES COMPILE_FLAGS "${OPENPASS_COMPILE_FLAGS}")
  # set additional OPENPASS_LINK_FLAGS
  set_target_properties(${targetname} PROPERTIES LINK_FLAGS "${OPENPASS_LINK_FLAGS}")
  
  
  unset(SOURCES)
  unset(HEADERS)
  unset(UIS)

endmacro(openpass_add_library)

# Macro to add OPENPASS executables
macro(openpass_add_executable targetname)
  add_executable(${targetname} ${ARGN} ${SOURCES} ${HEADERS} ${UIS})
  target_link_libraries(${targetname} ${EXTRA_LIBS})
  set_target_properties(${targetname} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
  # set_target_properties(${targetname} PROPERTIES PROJECT_LABEL "${targetname}")
  set_target_properties(${targetname} PROPERTIES OUTPUT_NAME "${targetname}")
	  
  openpass_adjust_output_dir(${targetname} "")
  
  # set additional OPENPASS_COMPILE_FLAGS
  set_target_properties(${targetname} PROPERTIES COMPILE_FLAGS "${OPENPASS_COMPILE_FLAGS}")
  # set additional OPENPASS_LINK_FLAGS
  set_target_properties(${targetname} PROPERTIES LINK_FLAGS "${OPENPASS_LINK_FLAGS}")
  
  # set_target_properties(${targetname} PROPERTIES DEBUG_OUTPUT_NAME "${targetname}${CMAKE_DEBUG_POSTFIX}")
  unset(SOURCES)
  unset(HEADERS)
  unset(UIS)
endmacro(openpass_add_executable)




# Macro to add OPENPASS modules (executables with a module-category)
macro(openpass_add_plugin targetname component)
  add_library(${targetname} SHARED ${ARGN} ${SOURCES} ${HEADERS} ${UIS})
  target_link_libraries(${targetname} ${EXTRA_LIBS})
  
  # set_target_properties(${targetname} PROPERTIES PROJECT_LABEL "${targetname}")
  set_target_properties(${targetname} PROPERTIES OUTPUT_NAME "${targetname}")
	  
  openpass_adjust_output_dir(${targetname} ${component}) 
  
  # set additional OPENPASS_COMPILE_FLAGS
  set_target_properties(${targetname} PROPERTIES COMPILE_FLAGS "${OPENPASS_COMPILE_FLAGS}")
  # set additional OPENPASS_LINK_FLAGS
  set_target_properties(${targetname} PROPERTIES LINK_FLAGS "${OPENPASS_LINK_FLAGS}")
  
  # set_target_properties(${targetname} PROPERTIES DEBUG_OUTPUT_NAME "${targetname}${CMAKE_DEBUG_POSTFIX}")
  unset(SOURCES)
  unset(HEADERS)
  unset(UIS)
endmacro(openpass_add_plugin)

macro(openpass_gui_add_plugin targetname category)  
   openpass_add_plugin(${targetname} "bin/gui" ${ARGN})
   if("${category}" STREQUAL "")
       set_target_properties(${targetname} PROPERTIES FOLDER "GUI")
   else("${category}" STREQUAL "")
       set_target_properties(${targetname} PROPERTIES FOLDER "${category}/GUI")
   endif("${category}" STREQUAL "")
   target_link_libraries(${targetname} Common)
   qt5_use_modules(${targetname} Core Gui Xml Widgets)
  
endmacro(openpass_gui_add_plugin)

macro(openpass_add_module targetname category)
   openpass_add_plugin(${targetname} lib ${ARGN})
   if("${category}" STREQUAL "")
       set_target_properties(${targetname} PROPERTIES FOLDER "CoreModules")
   else("${category}" STREQUAL "")
       set_target_properties(${targetname} PROPERTIES FOLDER "${category}/CoreModules")
   endif("${category}" STREQUAL "")
   target_link_libraries(${targetname} Common)
   qt5_use_modules(${targetname} Xml)
  
endmacro(openpass_add_module)

macro(openpass_add_component targetname category)
   openpass_add_plugin(${targetname} "lib" ${ARGN})
   if("${category}" STREQUAL "")
       set_target_properties(${targetname} PROPERTIES FOLDER "Component")
   else("${category}" STREQUAL "")
       set_target_properties(${targetname} PROPERTIES FOLDER "${category}/Component")
   endif("${category}" STREQUAL "")
   target_link_libraries(${targetname} Common)
   qt5_use_modules(${targetname} Xml)
  
endmacro(openpass_add_component)

# Macro to install and export
macro(openpass_install_target targetname)
  
install(TARGETS ${targetname}
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" )
		
install(FILES ${HEADERS} DESTINATION "${INSTALL_INC_DIR}")
  
endmacro(openpass_install_target)

# Macro to install an OpenOPENPASS_GUI plugin
macro(openpass_gui_install_plugin targetname)
  install(TARGETS ${ARGV} EXPORT OPENPASS-targets
          RUNTIME DESTINATION ${INSTALL_BIN_DIR}/bin/gui
          LIBRARY DESTINATION ${INSTALL_LIB_DIR}
          ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
          COMPONENT guiplugins
  )
endmacro(openpass_gui_install_plugin)

# Macro to install headers
macro(openpass_install_headers dirname)
  install(FILES ${ARGN} DESTINATION include/OPENPASS/${dirname})
endmacro(openpass_install_headers)


#
# Per target flag handling
#

function(add_openpass_compile_flags targetname flags)
  get_target_property(MY_CFLAGS ${targetname} COMPILE_FLAGS)
  if(NOT MY_CFLAGS)
    set(MY_CFLAGS "")
  endif()
  foreach(cflag ${flags})
    #string(REGEX MATCH "${cflag}" flag_matched "${MY_CFLAGS}")
    #if(NOT flag_matched)
      set(MY_CFLAGS "${MY_CFLAGS} ${cflag}")
    #endif(NOT flag_matched)
  endforeach(cflag)
  set_target_properties(${targetname} PROPERTIES COMPILE_FLAGS "${MY_CFLAGS}")
  # message("added compile flags ${MY_CFLAGS} to target ${targetname}")
endfunction(add_openpass_compile_flags)

function(remove_openpass_compile_flags targetname flags)
  get_target_property(MY_CFLAGS ${targetname} COMPILE_FLAGS)
  if(NOT MY_CFLAGS)
    set(MY_CFLAGS "")
  endif()
  foreach(cflag ${flags})
    string(REGEX REPLACE "${cflag}[ ]+|${cflag}$" "" MY_CFLAGS "${MY_CFLAGS}")
  endforeach(cflag)
  set_target_properties(${targetname} PROPERTIES COMPILE_FLAGS "${MY_CFLAGS}")
endfunction(remove_openpass_compile_flags)

function(add_openpass_link_flags targetname flags)
  get_target_property(MY_LFLAGS ${targetname} LINK_FLAGS)
  if(NOT MY_LFLAGS)
    set(MY_LFLAGS "")
  endif()
  foreach(lflag ${flags})
    #string(REGEX MATCH "${lflag}" flag_matched "${MY_LFLAGS}")
    #if(NOT flag_matched)
      set(MY_LFLAGS "${MY_LFLAGS} ${lflag}")
    #endif(NOT flag_matched)
  endforeach(lflag)
  set_target_properties(${targetname} PROPERTIES LINK_FLAGS "${MY_LFLAGS}")
  #message("added link flags ${MY_LFLAGS} to target ${targetname}")
endfunction(add_openpass_link_flags)

function(remove_openpass_link_flags targetname flags)
  get_target_property(MY_LFLAGS ${targetname} LINK_FLAGS)
  if(NOT MY_LFLAGS)
    set(MY_LFLAGS "")
  endif()
  foreach(lflag ${flags})
    string(REGEX REPLACE "${lflag}[ ]+|${lflag}$" "" MY_LFLAGS "${MY_LFLAGS}")
  endforeach(lflag)
  set_target_properties(${targetname} PROPERTIES LINK_FLAGS "${MY_LFLAGS}")
endfunction(remove_openpass_link_flags)


