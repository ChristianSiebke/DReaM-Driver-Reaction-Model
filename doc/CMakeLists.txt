################################################################################
# Copyright (c) 2021 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

if(WITH_DOC)
  if(TARGET doc)
    message(STATUS "Target doc already defined. Skipping.")
  else()
    add_custom_target(doc ALL
        COMMAND ${CMAKE_COMMAND}
            -DSRC=${CMAKE_CURRENT_LIST_DIR}/source
            -DDST=${CMAKE_BINARY_DIR}/doc
            -P ${CMAKE_CURRENT_SOURCE_DIR}/PrepareDoc.cmake
            COMMENT "Copy OS documentation and replace placeholders"
        COMMAND ${SPHINX_EXECUTABLE}            # sphinx-build
            -M html                             # generate HTML
            ${CMAKE_BINARY_DIR}/doc/source      # source path
            ${CMAKE_BINARY_DIR}/doc             # destination path
            -DWITH_API_DOC=${WITH_API_DOC}      # turn exhale ON/OFF
            COMMENT "Build Sphinx documentation"
        COMMAND ${CMAKE_COMMAND} 
          -E cmake_echo_color --green
          "The HTML pages are in ${CMAKE_BINARY_DIR}/doc/html.")

    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
      ${CMAKE_BINARY_DIR}/doc)

    # make HTML doc available on install
    install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/html/
            DESTINATION ${CMAKE_INSTALL_PREFIX}/doc)
  endif()
endif()
