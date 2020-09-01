#!/bin/bash

################################################################################
# Copyright (c) 2020 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

function showUsage()
{
  echo ""
  echo "openPASS config runner"
  echo "======================"
  echo ""
  echo "Tries to execute all configs (subdirectories) of a given directory location"
  echo "and shows the result of the simulations. Return code shows an overall status"
  echo "of the single simulator return codes and of the created logfiles. The files"
  echo "are required to be empty after a simualtion run, in order to generate a"
  echo "successful return value."
  echo ""
  echo "WARNING    Execution of this script deletes 'configs' and 'results_*' directories"
  echo "WARNING    in the simulator directory."
  echo ""
  echo "Usage: $(basename $0) <simulator_directory> <configs_to_tests> [additional_simulator_arguments]"
  echo ""
}

if [ $# -lt 2 ]; then
  showUsage
  exit 1
fi

if [ ! -d "$1" ]; then
  echo "Given simulator directory is not a directory"
  echo
  showUsage
  exit 1
fi

SLAVE_FOUND=0
SLAVE_NAME="OpenPassSlave"

if [ -e "$1/$SLAVE_NAME" ]; then
  SLAVE_FOUND=1
else
  SLAVE_NAME="${SLAVE_NAME}.exe"
  if [ -e "$1/$SLAVE_NAME" ]; then
    SLAVE_FOUND=1
  fi
fi

if [ $SLAVE_FOUND -eq 0 ]; then
  echo "Simulator executable not found"
  echo
  showUsage
  exit 1
fi

if [ ! -d "$2" ]; then
  echo "Invalid config directory"
  echo
  showUsage
  exit 1
fi

CONFIG_DIR=$(readlink -f "$2")

cd "$1" || exit 1

rm -rf configs
rm -rf results_*

OVERALL_SUCCESS=1

while read -r -d $'\0' CONFIG_FULL; do
  CONFIG=$(basename "$CONFIG_FULL")
  echo Executing config: "$CONFIG"

  cp -r "$CONFIG_FULL"/* .

  SUCCESS=0
  ./$SLAVE_NAME "$@" && SUCCESS=1

  if [ $SUCCESS -eq 1 ]; then
    echo "Simulator result: SUCCESS"
  else
    echo "Simulator result: FAILURE"
    OVERALL_SUCCESS=0
  fi

  if [ -s OpenPassSlave.log ]; then
    echo "Logfile not empty! FAILURE"
    OVERALL_SUCCESS=0
  fi

  mv OpenPassSlave.log results
  mv configs/* results
  mv results "results_$CONFIG"

  rm -rf configs

done < <(find "$CONFIG_DIR" -mindepth 1 -maxdepth 1 -type d -print0)

[ $OVERALL_SUCCESS -eq 1 ] && exit 0 || exit 1

