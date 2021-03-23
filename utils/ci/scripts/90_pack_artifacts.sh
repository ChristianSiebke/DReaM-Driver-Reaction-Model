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
# This script packs the artifacts
################################################################################

MYDIR="$(dirname "$(readlink -f $0)")"
cd "$MYDIR/../../../../dist" || exit 1

util_tar.sh ../artifacts/openPASS_SIM.tar.gz lib OpenPassMaster OpenPassSlave *.so* --ignore-failed-read
util_tar.sh ../artifacts/openPASS_EndToEndTests.tar.gz results_* --ignore-failed-read

