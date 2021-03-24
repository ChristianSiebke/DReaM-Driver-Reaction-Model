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
# This script runs the simulation core build and end-to-end tests
################################################################################

MYDIR="$(dirname "$(readlink -f $0)")"
cd "$MYDIR" || exit 1

export CCACHE_DIR=$MYDIR/../../../../ccache

for SCRIPT in 10_prepare.sh 15_dep_osi.sh 16_dep_fmilibrary.sh 20_configure.sh 30_build.sh 55_endtoend.sh 90_pack_artifacts.sh; do
  echo
  echo "======================================================================="
  echo "Executing ${SCRIPT}..."
  echo "======================================================================="
  echo

  ./$SCRIPT || exit 1
done

