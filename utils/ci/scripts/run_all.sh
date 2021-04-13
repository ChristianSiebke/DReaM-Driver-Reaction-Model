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
# This script runs all build scripts, in order
#
# NOTE: Files to be executed have to be named like 00_script.sh, where 00 is
# used to specifiy execution order. The executability permission has to be set.
################################################################################

MYDIR="$(dirname "$(readlink -f $0)")"
cd "$MYDIR" || exit 1

export CCACHE_DIR=$MYDIR/../../../../ccache

for SCRIPT in $(find -maxdepth 1 -type f -executable -name '[0-9]*_*.sh' | sort); do
  echo
  echo "======================================================================="
  echo "Executing ${SCRIPT}..."
  echo "======================================================================="
  echo

  $SCRIPT || exit 1
done

