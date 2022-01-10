#!/bin/bash

#************************************************************
# Copyright (c) 2021 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
# ************************************************************

# check
if [ $# -ne 1 ]; then
  ME=$(basename "$0")
  echo "Usage: $ME <BUILD_DIRECTORY>"
  exit 1
fi

# variables
CWD=$(pwd)
BUILD_DIRECTORY="$1"

# actual command
mkdir $BUILD_DIRECTORY
cd $BUILD_DIRECTORY
cmake -DWITH_SIMCORE=OFF -DWITH_TESTS=OFF -DWITH_DOC=ON $CWD/..
make doc