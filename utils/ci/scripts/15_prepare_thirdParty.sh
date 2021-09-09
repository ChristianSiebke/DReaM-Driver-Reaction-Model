#!/bin/bash

################################################################################
# Copyright (c) 2021 ITK Engineering GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

################################################################################
# This script prepares the thirdParty dependencies
################################################################################

MYDIR="$(dirname "$(readlink -f $0)")"
cd "$MYDIR/../../../.."

WORKSPACE_ROOT="$PWD"

# Detect system settings and create a conan profile
${PYTHON_WINDOWS_EXE} -m conans.conan profile new default --detect
${PYTHON_WINDOWS_EXE} -m conans.conan profile update settings.compiler.libcxx=libstdc++11 default

# export conan recipes of thirdParty dependencies to conans local cache
${PYTHON_WINDOWS_EXE} -m conans.conan export $WORKSPACE_ROOT/utils/ci/conan/recipe/osi openpass/testing
${PYTHON_WINDOWS_EXE} -m conans.conan export $WORKSPACE_ROOT/utils/ci/conan/recipe/fmi openpass/testing

# prepare a build directory
mkdir -p build-thirdParty && cd build-thirdParty

# (build and) install thirdParty dependencies to conans local cache and deploy them
${PYTHON_WINDOWS_EXE} -m conans.conan install $WORKSPACE_ROOT/utils/ci/conan/install_file/thirdParty --build=missing --install-folder="$WORKSPACE_ROOT/deps/" -g deploy
