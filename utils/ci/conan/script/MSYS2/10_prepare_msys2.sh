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

yes | pacman -S mingw-w64-x86_64-cmake \
                mingw-w64-x86_64-gcc \
                mingw-w64-x86_64-gdb \
                mingw-w64-x86_64-make \
                mingw-w64-x86_64-qt5 git \
                mingw-w64-x86_64-boost \
                mingw-w64-x86_64-protobuf \
                mingw-w64-x86_64-gtest \
                mingw-w64-x86_64-ccache \
                mingw-w64-x86_64-doxygen \
                mingw-w64-x86_64-graphviz \
                mingw-w64-x86_64-python-pip \
                pacman -S mingw-w64-x86_64-python-sphinx \
                pacman -S mingw-w64-x86_64-enchant
	