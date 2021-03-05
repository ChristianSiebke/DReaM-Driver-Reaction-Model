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

# !!! WARNING !!! WARNIG !!! WARNING !!!
# For CI usage only!
# Don't run this script locally, it might wipe out your repository directory!
# !!! WARNING !!! WARNIG !!! WARNING !!!


MYDIR="$(dirname "$(readlink -f $0)")"
cd "$MYDIR/../../.." || exit 1

if [ "$(basename "$(readlink -f .)")" = "repo" ] && [ -d ".git" ]; then
  echo "Found 'repo' directory containing '.git' directory. Assuming CI checkout."
  echo "Wiping workspace..."
  cd ..

  # cleanup
  # skipping for now, as it is dangerous when running locally
  #find -mindepth 1 -maxdepth 1 -not -name "repo" -a -not -name "ccache" -a -not -name ".git" -print0 | xargs -0 rm -rf --one-file-system --

  # prepare
  for d in artifacts build dist; do
    mkdir -p $d
  done
fi

