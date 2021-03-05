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

mkdir -p build-osi && cd build-osi

git clone --branch v3.2.0 --depth=1 https://github.com/OpenSimulationInterface/open-simulation-interface.git src || exit 1

mkdir -p src/build && cd src/build
cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX="$WD/repo/deps/osi" ..
make -j2
make install


