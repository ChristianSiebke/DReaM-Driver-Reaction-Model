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
# This script executes end-to-end tests
################################################################################

MYDIR="$(dirname "$(readlink -f $0)")"
cd "$MYDIR/../../../../build"

make pyOpenPASS

