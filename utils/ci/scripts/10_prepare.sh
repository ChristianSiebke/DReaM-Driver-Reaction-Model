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
# This script prepares building
################################################################################

MYDIR="$(dirname "$(readlink -f $0)")"
cd "$MYDIR/../../../.." || exit 1

if [ ! -d repo ]; then
  echo "repo folder doesn't exist as expected. exiting."
  exit 1
fi

# wipe build directories and pyOpenPASS results
rm -rf artifacts build build-fmi-library build-osi
rm -f repo/sim/tests/endToEndTests/pyOpenPASS/result_*.xml

# prepare
mkdir build

