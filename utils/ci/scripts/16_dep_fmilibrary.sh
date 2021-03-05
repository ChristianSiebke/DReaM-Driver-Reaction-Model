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

MYDIR="$(dirname "$(readlink -f $0)")"
cd "$MYDIR/../../../.."

WD="$PWD"

mkdir -p build-fmi-library && cd build-fmi-library

git clone --branch 2.0.3 --depth=1 https://github.com/modelon-community/fmi-library.git src || exit 1

cd src
patch -p1 < "$MYDIR/patches/fmi-library-2.0.3-fixes.patch" || exit 1

mkdir -p build && cd build
cmake -D CMAKE_BUILD_TYPE=Release -D FMILIB_INSTALL_PREFIX="$WD/repo/deps/FMILibrary" -D FMILIB_BUILD_STATIC_LIB=OFF -D FMILIB_BUILD_SHARED_LIB=ON ..
make -j2
make install

