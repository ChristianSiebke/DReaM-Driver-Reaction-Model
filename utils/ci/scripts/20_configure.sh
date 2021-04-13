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

MYDIR="$(dirname "$(readlink -f $0)")"
cd "$MYDIR/../../../../build" || exit 1

cmake \
  -D CMAKE_PREFIX_PATH="$PWD/../deps/FMILibrary;$PWD/../deps/osi" \
  -D CMAKE_INSTALL_PREFIX="$PWD/../dist/Slave" \
  -D CMAKE_BUILD_TYPE=Release \
  -D CMAKE_C_COMPILER=gcc-10 \
  -D CMAKE_CXX_COMPILER=g++-10 \
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

