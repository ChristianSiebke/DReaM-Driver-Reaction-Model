################################################################################
# Copyright (c) 2021 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
#
# Find Package Adapter for "Gcov"
#
# - GCOV_EXECUTABLE: The executable
# - GCOV_FOUND: True if gcov was found
#
include(FindPackageHandleStandardArgs)
find_program(GCOV_EXECUTABLE NAMES gcov gcov.py "Path to the gcov executable")
find_package_handle_standard_args(Gcov "Unable to locate gcov" GCOV_EXECUTABLE)