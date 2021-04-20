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
# This script fetches and builds fmi-library
################################################################################

MYDIR="$(dirname "$(readlink -f $0)")"
cd "$MYDIR/../../../.."

WORKSPACE_ROOT="$PWD"

if [[ "${OSTYPE}" = "msys" ]]; then
  # set the correct CMake generator
  CMAKE_GENERATOR_ARG="-GMSYS Makefiles"
fi

mkdir -p build-fmi-library && cd build-fmi-library

git clone --branch 2.0.3 --depth=1 -c advice.detachedHead=false https://github.com/modelon-community/fmi-library.git src || exit 1

cd src
dos2unix src/Import/src/FMI1/fmi1_import_capi.c src/Import/src/FMI2/fmi2_import_capi.c src/Util/include/JM/jm_portability.h
patch -l -p1 < "$MYDIR/patches/fmi-library-2.0.3-fixes.patch" || exit 1

mkdir -p build && cd build
cmake "$CMAKE_GENERATOR_ARG" -D CMAKE_BUILD_TYPE=Release -D FMILIB_INSTALL_PREFIX="$WORKSPACE_ROOT/deps/FMILibrary" -D FMILIB_BUILD_STATIC_LIB=OFF -D FMILIB_BUILD_SHARED_LIB=ON ..
make -j4
make install

