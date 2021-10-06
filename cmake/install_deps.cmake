################################################################################
# Copyright (c) 2020, 2021 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

get_property(EXE_TARGET_LIST GLOBAL PROPERTY exe_target_list)
get_property(LIB_TARGET_LIST GLOBAL PROPERTY lib_target_list)

message(STATUS "Collecting runtime library paths...")

if(WIN32 AND NOT MSVC)
  find_program(CYGPATH NAMES cygpath REQUIRED)
endif()

set(SEARCH_DIRS)
set(RT_LIB_DIRS)

if(INSTALL_EXTRA_RUNTIME_DEPS)
  list(APPEND SEARCH_DIRS "${CMAKE_PREFIX_PATH}")
endif()

# build a list of library directory candidates
foreach(SEARCH_DIR IN LISTS SEARCH_DIRS)
  message(DEBUG "processing ${SEARCH_DIR}")
  if(WIN32 AND NOT MSVC)
    execute_process (
        WORKING_DIRECTORY ${SEARCH_DIR}
        COMMAND $ENV{SHELL} -c "for f in \$(find -iname \'*.dll\'); do cygpath -a -m \$(dirname \$f); done | sort -u"
        OUTPUT_VARIABLE DETECTED_LIBRARY_DIRS
    )
  else()
    execute_process (
        WORKING_DIRECTORY ${SEARCH_DIR}
        COMMAND $ENV{SHELL} -lc "for f in \$(find -iname \'*.so\'); do realpath \$(dirname \$f); done | sort -u"
        OUTPUT_VARIABLE DETECTED_LIBRARY_DIRS
    )
  endif()
  string(REPLACE "\n" ";" DETECTED_LIBRARY_DIRS "${DETECTED_LIBRARY_DIRS}")
  list(APPEND RT_LIB_DIRS ${DETECTED_LIBRARY_DIRS})
endforeach()

# copies runtime dependencies to the installation directory
# this code is executed on "make install"
install(CODE
  "
  file(GET_RUNTIME_DEPENDENCIES
       RESOLVED_DEPENDENCIES_VAR resolved_deps
       UNRESOLVED_DEPENDENCIES_VAR unresolved_deps
       DIRECTORIES ${RT_LIB_DIRS}
       EXECUTABLES ${EXE_TARGET_LIST}
       LIBRARIES ${LIB_TARGET_LIST})

  message(DEBUG \"Resolved runtime dependencies: \${resolved_deps}\")

  # skip system libraries
  if(\"${INSTALL_SYSTEM_RUNTIME_DEPS}\" STREQUAL \"OFF\")
    message(STATUS \"Filtering system runtime dependencies...\")
    if(WIN32)
      list(FILTER resolved_deps EXCLUDE REGEX \"^[A-Za-z]:[/\\\\][A-Za-z0-9_]*[/\\\\]system32|^api-ms-.*|ext-ms-.*\")
    elseif(UNIX)
      list(FILTER resolved_deps EXCLUDE REGEX \"^/lib|^/usr\")
    endif()
  endif()

  message(DEBUG \"RT_LIB_DIRS: ${RT_LIB_DIRS}\")
  message(DEBUG \"EXE list: ${EXE_TARGET_LIST}\")
  message(DEBUG \"LIB list: ${LIB_TARGET_LIST}\")
  message(DEBUG \"Unresolved runtime dependencies: \${unresolved_deps}\")
  message(STATUS \"Installing resolved runtime dependencies: \${resolved_deps}\")

  file(COPY \${resolved_deps}
       DESTINATION ${CMAKE_INSTALL_PREFIX}
       FOLLOW_SYMLINK_CHAIN)
  "
)

# install schema files into install directory
file(TO_NATIVE_PATH ${CMAKE_SOURCE_DIR}/schemas/ SCHEMA_SRC)
file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX}/schemas/ SCHEMA_DEST)
  install(DIRECTORY ${SCHEMA_SRC}
          DESTINATION ${SCHEMA_DEST})
  message("schemas installed from ${SCHEMA_SRC} to ${SCHEMA_DEST}")

