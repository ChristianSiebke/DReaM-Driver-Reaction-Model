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
# This script prepares the MSYS2 environment
################################################################################

conan profile new default --detect
conan profile update settings.compiler.libcxx=libstdc++11 default

conan export ../recipe/osi openpass/testing
conan export ../recipe/fmi openpass/testing
conan export ../recipe/openpass

mkdir ../install_file/openpass/build 
cd ../install_file/openpass/build

conan install .. --build=missing
	