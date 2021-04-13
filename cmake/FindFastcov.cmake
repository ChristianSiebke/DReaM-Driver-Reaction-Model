################################################################################
# Copyright (c) 2021 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
#
# Find Package Adapter for "Fastcov"
#
# - FASTCOV_EXECUTABLE: The executable
# - FASTCOV_FOUND: True if fastcov was found
#
include(FindPackageHandleStandardArgs)
find_program(FASTCOV_EXECUTABLE NAMES fastcov fastcov.py "Path to the fastcov executable")
find_package_handle_standard_args(Fastcov "Unable to locate fastcov" FASTCOV_EXECUTABLE)