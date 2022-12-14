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
cd "$MYDIR/../../../../dist/opSim" || exit 1

mkdir -p ../../artifacts || exit 1

if [[ "${OSTYPE}" = "msys" ]]; then
  $MYDIR/util_zip.sh ../../artifacts/openPASS_SIM.zip doc gui modules schemas opSimulation.exe opSimulationManager.exe openPASS.exe *.dll
  $MYDIR/util_zip.sh ../../artifacts/openPASS_EndToEndTests.zip artifacts
else
  $MYDIR/util_tar.sh ../../artifacts/openPASS_SIM.tar.gz doc gui lib modules schemas opSimulation opSimulationManager openPASS *.so* --ignore-failed-read
  $MYDIR/util_tar.sh ../../artifacts/openPASS_EndToEndTests.tar.gz artifacts --ignore-failed-read
fi
