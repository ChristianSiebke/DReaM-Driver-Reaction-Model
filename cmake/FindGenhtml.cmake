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
# Find Package Adapter for "Genhtml"
#
# - GENHTML_EXECUTABLE: The executable
# - GENHTML_FOUND: True if genhtml was found
#
include(FindPackageHandleStandardArgs)
find_program(GENHTML_EXECUTABLE NAMES genhtml genhtml.perl genhtml.bat )
find_package_handle_standard_args(Genhtml "Unable to locate genhtml" GENHTML_EXECUTABLE)