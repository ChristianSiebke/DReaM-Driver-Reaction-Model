################################################################################
# Copyright (c) 2021 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

if(WITH_DOC)
  add_custom_target(doc
      # Copy documentation and change placeholders
      # (see PrepareDoc for more information)
      COMMAND ${CMAKE_COMMAND}
          -DSRC=${OPENPASS_OS_DIR}/doc/source
          -DDST=${CMAKE_BINARY_DIR}/doc
          -P ${OPENPASS_OS_DIR}/doc/PrepareDoc.cmake
          COMMENT "Copy OS documentation and replace placeholders"
      COMMAND ${CMAKE_COMMAND}
          -DSRC=${CMAKE_CURRENT_LIST_DIR}/source
          -DDST=${CMAKE_BINARY_DIR}/doc
          -P ${CMAKE_CURRENT_LIST_DIR}/PrepareDocCustom.cmake
          COMMENT "Copy custom documentation and replace placeholders (overrides!)"
      COMMAND ${SPHINX_EXECUTABLE}            # sphinx-build
          -M html                             # generate HTML
          ${CMAKE_BINARY_DIR}/doc/source      # source path
          ${CMAKE_BINARY_DIR}/doc             # destination path
          -DWITH_API_DOC=${WITH_API_DOC}      # turn exhale ON/OFF
          COMMENT "Build sphinx documentation"
      COMMAND ${CMAKE_COMMAND} 
          -E cmake_echo_color --green
          "The HTML pages are in ${CMAKE_BINARY_DIR}/doc/html.")

  set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
      ${CMAKE_BINARY_DIR}/doc)

  # make HTML doc available on install
  install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/html/
      DESTINATION ${CMAKE_INSTALL_PREFIX}/doc) 
endif()
