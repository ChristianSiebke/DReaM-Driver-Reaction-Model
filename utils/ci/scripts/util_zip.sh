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
# This script just wraps the call to zip by adding common flags
################################################################################

if [ $# -lt 2 ]; then
  echo "Usage: $0 <target artifact path> <files/dirs to add...>"
  exit 1
fi

ARTIFACT_PATH="$(readlink -f $1)"
shift

SRCFILES=("$@")

# not quoted to allow glob expansion
echo Archiving into $ARTIFACT_PATH: ${SRCFILES[@]}
zip -r -9 "$ARTIFACT_PATH" ${SRCFILES[@]}
