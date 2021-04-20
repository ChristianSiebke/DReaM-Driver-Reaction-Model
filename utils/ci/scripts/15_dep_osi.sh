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
# This script fetches and builds open-simulation-interface
################################################################################

MYDIR="$(dirname "$(readlink -f $0)")"
cd "$MYDIR/../../../.."

WORKSPACE_ROOT="$PWD"

if [[ "${OSTYPE}" = "msys" ]]; then
  # set the correct CMake generator
  CMAKE_GENERATOR_ARG="-GMSYS Makefiles"
fi

mkdir -p build-osi && cd build-osi

git clone --branch v3.2.0 --depth=1 -c advice.detachedHead=false https://github.com/OpenSimulationInterface/open-simulation-interface.git src || exit 1

mkdir -p src/build && cd src/build
cmake "$CMAKE_GENERATOR_ARG" -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX="$WORKSPACE_ROOT/deps/osi" ..
make -j4
make install

