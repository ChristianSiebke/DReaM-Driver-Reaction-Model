#!/bin/bash

################################################################################
# Copyright (c) 2021 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

################################################################################
# This script configures cmake
################################################################################

# joins arguments using the cmake list separator (;)
function join_paths()
{
  local IFS=\;
  echo "$*"
}

MYDIR="$(dirname "$(readlink -f $0)")"
cd "$MYDIR/../../../../build" || exit 1

# dependencies built previously
DEPS=(
  "$PWD/../deps/FMILibrary"
  "$PWD/../deps/osi"
)

# preparations for building on Windows/MSYS
if [[ "${OSTYPE}" = "msys" ]]; then
  # set the correct CMake generator
  CMAKE_GENERATOR_ARG="-GMSYS Makefiles"

  # set python command
  if [[ -n "${PYTHON_WINDOWS_EXE}" ]]; then
    CMAKE_PYTHON_COMMAND_ARG="-DPYTHON_COMMAND=${PYTHON_WINDOWS_EXE}"
  fi

  # prepare dependency paths
  # it seems cmake doesn't like MSYS paths starting with drive letters (e.g. /c/thirdParty/...)
  # cygpath is used here to format the paths in "mixed format" (e.g. c:/thirdparty/...)
  # also, add the mingw64 base path to allow resolving of runtime dependencies during install
  OLD_DEPS=(${DEPS[@]})
  OLD_DEPS+=("/mingw64")
  DEPS=()
  for DEP in "${OLD_DEPS[@]}"; do
    DEPS+=("$(cygpath -a -m ${DEP})")
  done
fi

cmake \
  "$CMAKE_GENERATOR_ARG" \
  "$CMAKE_PYTHON_COMMAND_ARG" \
  -D CMAKE_PREFIX_PATH="$(join_paths ${DEPS[@]})" \
  -D CMAKE_INSTALL_PREFIX="$PWD/../dist/Slave" \
  -D CMAKE_BUILD_TYPE=Release \
  -D INSTALL_BIN_DIR:STRING=. \
  -D INSTALL_EXTRA_RUNTIME_DEPS=ON \
  -D OPENPASS_ADJUST_OUTPUT=OFF \
  -D USE_CCACHE=OFF \
  -D WITH_COVERAGE=OFF \
  -D WITH_DEBUG_POSTFIX=OFF \
  -D WITH_DOC=OFF \
  -D WITH_ENDTOEND_TESTS=ON \
  -D WITH_EXTENDED_OSI=OFF \
  -D WITH_GUI=OFF \
  ../repo
